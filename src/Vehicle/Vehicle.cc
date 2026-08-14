#include "AutoPilotPlugin.h"
#include "Vehicle.h"
#include "FirmwarePlugin.h"
#include "FirmwarePluginManager.h"
#include "LinkManager.h"
#include "MAVLinkProtocol.h"
#include "ParameterManager.h"
#include "QUAVApplication.h"
#include "InitialConnectStateMachine.h"
#include "StateMachine.h"
#include "StandardModes.h"
#include "VehicleLinkManager.h"
#include "QUAVCameraManager.h"

#include <QtCore/QDateTime>

Vehicle::Vehicle(
    LinkInterface*  link,
    int             vehicleId,
    int             defaultComponentId,
    MAV_AUTOPILOT   firmwareType,
    MAV_TYPE        vehicleType,
    QObject*        parent
)   : VehicleFactGroup  (parent)
    , m_id              (vehicleId)
    , m_firmwareType    (firmwareType)
    , m_vehicleType     (vehicleType)
{
    qDebug() << "Link started with Mavlink " << (MAVLinkProtocol::instance()->getCurrentVersion() >= 200 ? "V2" : "V1");

    connect(MAVLinkProtocol::instance(), &MAVLinkProtocol::messageReceived, this, &Vehicle::m_mavlinkMessageReceived);

    m_commonInit();

    m_vehicleLinkManager->m_addLink(link);

    m_autoPilotPlugin = m_firmwarePlugin->autopilotPlugin(this);
    m_autoPilotPlugin->setParent(this);

    // Send MAV_CMD ack timer
    m_mavCommandResponseCheckTimer.setSingleShot(false);
    m_mavCommandResponseCheckTimer.setInterval(m_mavCommandResponseCheckTimeoutMSecs);
    m_mavCommandResponseCheckTimer.start();
    connect(&m_mavCommandResponseCheckTimer, &QTimer::timeout, this, &Vehicle::m_sendMavCommandResponseTimeoutCheck);

    // MAV_TYPE_GENERIC is used by unit test for creating a vehicle which doesn't do the connect sequence. This
    // way we can test the methods that are used within the connect sequence.
    if (!quavApp()->runningUnitTests() || m_vehicleType != MAV_TYPE_GENERIC) {
        m_initialConnectStateMachine->start();
    }

    // Create camera manager instance
    m_cameraManager = m_firmwarePlugin->createCameraManagers(this);
}

void Vehicle::m_commonInit()
{
    m_firmwarePlugin = FirmwarePluginManager::instance()->firmwarePluginForAutopilot(m_firmwareType, m_vehicleType);

    m_standardModes              = new StandardModes(this, this);
    m_initialConnectStateMachine = new InitialConnectStateMachine(this, this);

    m_vehicleLinkManager         = new VehicleLinkManager(this);

    m_parameterManager = new ParameterManager(this);
}

Vehicle::~Vehicle(){
    qDebug()<<"~Vehicle"<<this;

    delete m_autoPilotPlugin;
    m_autoPilotPlugin = nullptr;
}

void Vehicle::m_mavlinkMessageReceived (LinkInterface* link, mavlink_message_t message)
{
    if (message.sysid != m_id && message.sysid != 0) {
        // We allow RADIO_STATUS messages which come from a link the vehicle is using to pass through and be handled
        if (!(message.msgid == MAVLINK_MSG_ID_RADIO_STATUS && m_vehicleLinkManager->containsLink(link))) {
            return;
        }
    }

    // We give the link manager first whack since it it reponsible for adding new links
    m_vehicleLinkManager->mavlinkMessageReceived(link, message);

    if (!m_heardFrom) {
        if (message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
            m_heardFrom = true;
            m_compID    = message.compid;
        }
    }

    m_parameterManager->mavlinkMessageReceived(message);
    
    m_waitForMavLinkMessageMessageReceivedHandler(message);

    this->handleMessage(this, message);
}


bool Vehicle::sendMessageOnLinkThreadSafe(LinkInterface* link, mavlink_message_t message)
{
    if (!link->isConnected()) {
        qDebug() << "sendMessageOnLinkThreadSafe" << link << "not connected!";
        return false;
    }

    // Write message into buffer, prepending start sign
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    int len = mavlink_msg_to_send_buffer(buffer, &message);

    link->writeBytesThreadSafe((const char*)buffer, len);

    return true;
}

bool Vehicle::isMavCommandPending(int targetCompId, MAV_CMD command)
{
    bool pending = ((-1) < m_findMavCommandListEntryIndex(targetCompId, command));
    // qDebug() << "Pending target: " << targetCompId << ", command: " << (int)command << ", pending: " << (pending ? "yes" : "no");
    return pending;
}

int Vehicle::m_findMavCommandListEntryIndex(int targetCompId, MAV_CMD command)
{
    for (int i=0; i<m_mavCommandList.count(); i++) {
        const MavCommandListEntry_t& entry = m_mavCommandList[i];
        if (entry.targetCompId == targetCompId && entry.command == command) {   return i; }
    }
    return -1;
}

bool Vehicle::m_sendMavCommandShouldRetry(MAV_CMD command)
{
    switch (command) {
        // In general we should not retry any commands. This is for safety reasons. For example you don't want an ARM command
        // to timeout with no response over a noisy link twice and then suddenly have the third try work 6 seconds later. At that
        // point the user could have walked up to the vehicle to see what is going wrong.
        //
        // We do retry commands which are part of the initial vehicle connect sequence. This makes this process work better over noisy
        // links where commands could be lost. Also these commands tend to just be requesting status so if they end up being delayed
        // there are no safety concerns that could occur.
        case MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES:
        case MAV_CMD_REQUEST_PROTOCOL_VERSION:
        case MAV_CMD_REQUEST_MESSAGE:
        case MAV_CMD_PREFLIGHT_STORAGE:
        case MAV_CMD_RUN_PREARM_CHECKS:
            return true;

        default:
            return false;
    }
}

void Vehicle::setArmed(bool armed, bool showError)
{

}

bool Vehicle::m_commandCanBeDuplicated(MAV_CMD command)
{
    // For some commands we don't care about response as much as we care about sending them regularly.
    // This test avoids commands not being sent due to an ACK not being received yet.
    // MOTOR_TEST in ardusub is a case where we need a constant stream of commands so it doesn't time out.
    switch (command) {
        case MAV_CMD_DO_MOTOR_TEST:
            return true;
        case MAV_CMD_SET_MESSAGE_INTERVAL:
            return true;
        default:
            return false;
    }
}


void Vehicle::m_sendMavCommandWorker(
    bool commandInt, bool showError,
    const MavCmdAckHandlerInfo_t* ackHandlerInfo,
    int targetCompId, MAV_CMD command, MAV_FRAME frame,
    float param1, float param2, float param3, float param4, float param5, float param6, float param7
) {
    // We can't send commands to compIdAll using this method. The reason being that we would get responses back possibly from multiple components
    // which this code can't handle.
    // We also can't send the majority of commands again if we are already waiting for a response from that same command. If we did that we would not be able to discern
    // which ack was associated with which command.
    if (
        (targetCompId == MAV_COMP_ID_ALL) ||
        (isMavCommandPending(targetCompId, command) && !m_commandCanBeDuplicated(command))
    ) {
        return;
    }

    SharedLinkInterfacePtr sharedLink = vehicleLinkManager()->primaryLink().lock();
    if (!sharedLink) { return;}

    MavCommandListEntry_t entry;

    entry.useCommandInt  = commandInt;
    entry.targetCompId   = targetCompId;
    entry.command        = command;
    entry.frame          = frame;
    entry.showError      = showError;
    entry.ackHandlerInfo = {};

    if (ackHandlerInfo) {entry.ackHandlerInfo = *ackHandlerInfo;}
    entry.rgParam1 = param1;
    entry.rgParam2 = param2;
    entry.rgParam3 = param3;
    entry.rgParam4 = param4;
    entry.rgParam5 = param5;
    entry.rgParam6 = param6;
    entry.rgParam7 = param7;
    entry.maxTries = m_sendMavCommandShouldRetry(command) ? m_mavCommandMaxRetryCount : 1;
    entry.ackTimeoutMSecs = sharedLink->linkConfiguration()->isHighLatency() ?
                                m_mavCommandAckTimeoutMSecsHighLatency : m_mavCommandAckTimeoutMSecs;
    entry.elapsedTimer.start();

    m_mavCommandList.append(entry);

    m_sendMavCommandFromList(m_mavCommandList.count()-1);
}

void Vehicle::m_sendMavCommandFromList(int index)
{
    MavCommandListEntry_t commandEntry = m_mavCommandList[index];

    if (++m_mavCommandList[index].tryCount > commandEntry.maxTries) {
        return;
    }

    if (commandEntry.tryCount > 1 && !px4Firmware() && commandEntry.command == MAV_CMD_START_RX_PAIR) {
        // The implementation of this command comes from the IO layer and is shared across stacks. So for other firmwares
        // we aren't really sure whether they are correct or not.
        return;
    }

    SharedLinkInterfacePtr sharedLink = vehicleLinkManager()->primaryLink().lock();
    if (!sharedLink){  return; }

    mavlink_message_t msg;

    if (commandEntry.useCommandInt) {
        mavlink_command_int_t cmd;

        memset(&cmd, 0, sizeof(cmd));
        cmd.target_system       = m_id;
        cmd.target_component    = commandEntry.targetCompId;
        cmd.command             = commandEntry.command;
        cmd.frame               = commandEntry.frame;
        cmd.param1              = commandEntry.rgParam1;
        cmd.param2              = commandEntry.rgParam2;
        cmd.param3              = commandEntry.rgParam3;
        cmd.param4              = commandEntry.rgParam4;
        cmd.x                   = commandEntry.frame == MAV_FRAME_MISSION ?
                                  commandEntry.rgParam5 : commandEntry.rgParam5*1e7;
        cmd.y                   = commandEntry.frame == MAV_FRAME_MISSION ?
                                  commandEntry.rgParam6 : commandEntry.rgParam6*1e7;
        cmd.z                   = commandEntry.rgParam7;

        mavlink_msg_command_int_encode_chan(
            MAVLinkProtocol::instance()->getSystemId(),
            MAVLinkProtocol::getComponentId(),
            sharedLink->mavlinkChannel(),
            &msg, &cmd
        );

    } else {

        mavlink_command_long_t cmd;

        memset(&cmd, 0, sizeof(cmd));
        cmd.target_system = m_id;
        cmd.target_component = commandEntry.targetCompId;
        cmd.command = commandEntry.command;
        cmd.confirmation = 0;
        cmd.param1 = commandEntry.rgParam1;
        cmd.param2 = commandEntry.rgParam2;
        cmd.param3 = commandEntry.rgParam3;
        cmd.param4 = commandEntry.rgParam4;
        cmd.param5 = static_cast<float>(commandEntry.rgParam5);
        cmd.param6 = static_cast<float>(commandEntry.rgParam6);
        cmd.param7 = commandEntry.rgParam7;

        mavlink_msg_command_long_encode_chan(
            MAVLinkProtocol::instance()->getSystemId(),
            MAVLinkProtocol::getComponentId(),
            sharedLink->mavlinkChannel(),
            &msg, &cmd
        );
    }

    sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
}

void Vehicle::m_sendMavCommandResponseTimeoutCheck(void)
{
    if (m_mavCommandList.isEmpty()) { return; }

    // Walk the list backwards since _sendMavCommandFromList can remove entries
    for (int i = m_mavCommandList.count()-1; i >= 0; i--) {
        MavCommandListEntry_t& commandEntry = m_mavCommandList[i];
        if (commandEntry.elapsedTimer.elapsed() > commandEntry.ackTimeoutMSecs) {
            // Try sending command again
            m_sendMavCommandFromList(i);
        }
    }
}

void Vehicle::m_removeRequestMessageInfo(int compId, int msgId)
{
    if (m_requestMessageInfoMap.contains(compId) && m_requestMessageInfoMap[compId].contains(msgId)) {
        delete m_requestMessageInfoMap[compId][msgId];
        m_requestMessageInfoMap[compId].remove(msgId);
    } else {
        qWarning() << Q_FUNC_INFO << "compId:msgId not found" << compId << msgId;
    }
}

void Vehicle::m_waitForMavLinkMessageMessageReceivedHandler(const mavlink_message_t &message)
{
    if (m_requestMessageInfoMap.contains(message.compid) && m_requestMessageInfoMap[message.compid].contains(message.msgid)) {
        auto pInfo           = m_requestMessageInfoMap[message.compid][message.msgid];
        auto resultHandler     = pInfo->resultHandler;
        auto resultHandlerData = pInfo->resultHandlerdata;

        if (!pInfo->commandAckReceived) {
            int entryIndex = m_findMavCommandListEntryIndex(message.compid, MAV_CMD_REQUEST_MESSAGE);
            if (entryIndex != -1) {
                m_mavCommandList.takeAt(entryIndex);
            } else {
                 qWarning() << Q_FUNC_INFO << "Removing request message command from list failed - not found in list";
            }
        }

        m_removeRequestMessageInfo(message.compid, message.msgid);
        (*resultHandler)(resultHandlerData, MAV_RESULT_ACCEPTED, RequestMessageNoFailure, message);
    }
}

void Vehicle::m_requestMessageCmdResultHandler(
    void* resultHandlerData, int compId, const mavlink_command_ack_t& ack, MavCmdResultFailureCode_t failureCode
) {
    
}

void Vehicle::requestMessage(
    RequestMessageResultHandler resultHandler, void* resultHandlerData, int compId, int messageId,
    float param1, float param2, float param3, float param4, float param5
) {
    auto requestMessageInfo = new RequestMessageInfo_t;
    requestMessageInfo->vehicle           = this;
    requestMessageInfo->compId            = compId;
    requestMessageInfo->msgId             = messageId;
    requestMessageInfo->resultHandler     = resultHandler;
    requestMessageInfo->resultHandlerdata = resultHandlerData;
    
    m_requestMessageInfoMap[compId][messageId] = requestMessageInfo;
    
    Vehicle::MavCmdAckHandlerInfo_t handlerInfo;
    handlerInfo.resultHandler     = m_requestMessageCmdResultHandler;
    handlerInfo.resultHandlerData = requestMessageInfo;
    
    m_sendMavCommandWorker(
        false, false, &handlerInfo, compId, MAV_CMD_REQUEST_MESSAGE, MAV_FRAME_GLOBAL,
        messageId, param1,  param2,  param3,  param4,  param5,  0
    );
}

QString Vehicle::brandImageIndoor() const
{
    return m_firmwarePlugin->brandImageIndoor(this);
}

QString Vehicle::brandImageOutdoor() const
{
    return m_firmwarePlugin->brandImageOutdoor(this);
}
