#pragma once

#include <QtCore/QElapsedTimer>
#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QVariantList>
#include <QtPositioning/QGeoCoordinate>
#include <QtCore/QFile>

#include "HealthAndArmingCheckReport.h"

#include "MAVLinkLib.h"
#include "QUAVMAVLink.h"
#include "VehicleLinkManager.h"

#include "VehicleFactGroup.h"

class AutoPilotPlugin;
class InitialConnectStateMachine;
class LinkInterface;
class FirmwarePlugin;
class ParameterManager;
class QUAVCameraManager;
class StandardModes;

class Vehicle : public VehicleFactGroup
{
    Q_OBJECT
    Q_MOC_INCLUDE("AutoPilotPlugin.h")
    Q_MOC_INCLUDE("ParameterManager.h")
    Q_MOC_INCLUDE("QUAVCameraManager.h")

    friend class InitialConnectStateMachine;
    friend class VehicleLinkManager;

public:
    Vehicle(
        LinkInterface* link,
        int vehicleId,
        int defaultComponentId,
        MAV_AUTOPILOT firmwareType,
        MAV_TYPE vehicleType,
        QObject* parent = nullptr
    );
    ~Vehicle();

    // Pass these into the offline constructor to create an offline vehicle which tracks the offline vehicle settings
    static const MAV_AUTOPILOT MAV_AUTOPILOT_TRACK = static_cast<MAV_AUTOPILOT>(-1);
    static const MAV_TYPE      MAV_TYPE_TRACK = static_cast<MAV_TYPE>(-1);

    Q_PROPERTY(int                  id              READ   id CONSTANT)
    Q_PROPERTY(AutoPilotPlugin*     autoPilotPlugin MEMBER m_autoPilotPlugin CONSTANT)
    Q_PROPERTY(bool                 armed           READ   armed       WRITE setArmedShowError NOTIFY armedChanged)
    Q_PROPERTY(bool                 px4Firmware     READ   px4Firmware NOTIFY firmwareTypeChanged)

    Q_PROPERTY(QString              brandImageIndoor    READ brandImageIndoor   NOTIFY firmwareTypeChanged)
    Q_PROPERTY(QString              brandImageOutdoor   READ brandImageOutdoor  NOTIFY firmwareTypeChanged)

    Q_PROPERTY(QUAVCameraManager*   cameraManager   READ   cameraManager  NOTIFY cameraManagerChanged)

    Q_PROPERTY(ParameterManager*   parameterManager   READ parameterManager CONSTANT)
    Q_PROPERTY(VehicleLinkManager* vehicleLinkManager READ vehicleLinkManager CONSTANT)

    Q_PROPERTY(HealthAndArmingCheckReport* healthAndArmingCheckReport READ healthAndArmingCheckReport CONSTANT)

    // Property accesors
    int id()                     const { return m_id;          }
    int compId()                 const { return m_compID;      }
    MAV_AUTOPILOT firmwareType() const { return m_firmwareType;}
    MAV_TYPE vehicleType()       const { return m_vehicleType; }
    
    /// Sends a message to the specified link
    /// @return true: message sent, false: Link no longer connected
    bool sendMessageOnLinkThreadSafe(LinkInterface* link, mavlink_message_t message);

    /// Provides access to AutoPilotPlugin for this vehicle.
    AutoPilotPlugin* autoPilotPlugin  () {return m_autoPilotPlugin;}
    
    /// Provides access to the Firmware Plugin for this Vehicle
    FirmwarePlugin* firmwarePlugin() {return m_firmwarePlugin;}

    bool armed () const { return m_armed;}
    void setArmed (bool armed, bool showError);
    void setArmedShowError (bool armed) {setArmed(armed, true);}

    bool px4Firmware () const { return m_firmwareType == MAV_AUTOPILOT_PX4; }
    QString brandImageIndoor () const;
    QString brandImageOutdoor () const;
    
    ParameterManager* parameterManger      ()       { return m_parameterManager;}
    ParameterManager* parameterManager      () const { return m_parameterManager;}
    VehicleLinkManager* vehicleLinkManager ()       { return m_vehicleLinkManager;}

    ///
    /// \brief isMavCommandPending
    ///     Query whether the specified MAV_CMD is in queue to be sent or has
    /// already been sent but whose reply has not yet been received and whose
    /// timeout has not yet expired.
    ///
    ///     Or, said another way: if you call `sendMavCommand(compId, command, true, ...)`
    /// will an error be shown because you (or another part of QGC) has already
    /// sent that command?
    ///
    /// \param targetCompId
    /// \param command
    /// \return
    ///
    bool isMavCommandPending(int targetCompId, MAV_CMD command);

    typedef enum {
        MavCmdResultCommandResultOnly,
        MavCmdResultFailureNoResponseToCommand,
        MavCmdResultFailureDuplicateToCommand,
    } MavCmdResultFailureCode_t;

    /// Callback for sendMavCommandWithHandler which handles all acks which are not MAV_RESULT_IN_PROGRESS
    ///     @param resultHandlerData    Opaque data passed in to sendMavCommand call
    ///     @param ack                  Received COMMAND_ACK
    ///     @param failureCode          Failure reason. If not MavCmdResultCommandResultOnly only ack.result == MAV_RESULT_FAILED is valid.
    typedef void (*MavCmdResultHandler) (
        void* resultHandlerdata, int compId, const mavlink_command_ack_t& ack, MavCmdResultFailureCode_t failureCode
    );

    // Callback info for sendMavCommandWithHandler
    typedef struct MavCmdAckHandlerInfo_s {
        MavCmdResultHandler resultHandler;
        void*               resultHandlerData;
    } MavCmdAckHandlerInfo_t;

    typedef enum {
        RequestMessageNoFailure,
        RequestMessageFailureCommandError,
        RequestMessageFailureCommandNotAcked,
        RequestMessageFailureMessageNotReceived,
        RequestMessageFailureDuplicateCommand
    } RequestMessageResultHandlerFailureCode_t;


    /// Callback for requestMessage
    ///     @param resultHandlerData    Opaque data which was passed in to requestMessage call
    ///     @param commandResult        Result from ack to REQUEST_MESSAGE command
    ///     @param failureCode          Failure code
    ///     @param message              Received message which was requested
    typedef void (*RequestMessageResultHandler) (
        void* resultHandlerData, MAV_RESULT commandResult,
        RequestMessageResultHandlerFailureCode_t failureCode, const mavlink_message_t& message
    );

    /// Requests the vehicle to send the specified message. Will retry a number of times.
    ///     @param resultHandler Callback for result
    ///     @param resultHandlerData Opaque data passed back to resultHandler
    void requestMessage(
        RequestMessageResultHandler resultHandler, void* resultHandlerData, int compId, int messageId,
        float param1 = 0.0f, float param2 = 0.0f, float param3 = 0.0f, float param4 = 0.0f, float param5 = 0.0f
    );


    QUAVCameraManager* cameraManager () { return m_cameraManager;}

    HealthAndArmingCheckReport* healthAndArmingCheckReport() {return &m_healthAndArmingCheckReport;}

signals:
    void armedChanged          (bool armed);
    void firmwareTypeChanged   ();
    void cameraManagerChanged  ();


private slots:
    void m_mavlinkMessageReceived (LinkInterface* link, mavlink_message_t message);
    void m_sendMavCommandResponseTimeoutCheck();

private:
    void m_commonInit ();

    int     m_id;
 
    MAV_AUTOPILOT    m_firmwareType;
    MAV_TYPE         m_vehicleType;
    FirmwarePlugin*  m_firmwarePlugin  = nullptr;
    AutoPilotPlugin* m_autoPilotPlugin = nullptr;
    
    QUAVCameraManager* m_cameraManager = nullptr;

    ParameterManager* m_parameterManager = nullptr;

    bool m_armed = false;
    
    uint8_t m_compID = 0;
    bool    m_heardFrom = false;

    HealthAndArmingCheckReport m_healthAndArmingCheckReport;

    void m_waitForMavLinkMessageMessageReceivedHandler(const mavlink_message_t& message);

    typedef struct RequestMessageInfo {
        Vehicle*                    vehicle             = nullptr;
        int                         compId;
        int                         msgId;
        RequestMessageResultHandler resultHandler       = nullptr;
        void*                       resultHandlerdata   = nullptr;
        bool                        commandAckReceived  = false;
        bool                        messageReceived     = false;
        QElapsedTimer               messageWaitElapsedTimer;
        mavlink_message_t           message;
    } RequestMessageInfo_t;

    QMap<int, QMap<int, RequestMessageInfo_t*>> m_requestMessageInfoMap;

    void m_removeRequestMessageInfo(int compId, int msgId);

    static void m_requestMessageCmdResultHandler(
        void* resultHandlerData, int compId, const mavlink_command_ack_t& ack, MavCmdResultFailureCode_t failureCode
    );

    typedef struct MavCommandListEntry {
        int                     targetCompId        = MAV_COMP_ID_AUTOPILOT1;
        bool                    useCommandInt       = false;
        MAV_CMD                 command;
        MAV_FRAME               frame;
        float                   rgParam1            = 0;
        float                   rgParam2            = 0;
        float                   rgParam3            = 0;
        float                   rgParam4            = 0;
        double                  rgParam5            = 0;
        double                  rgParam6            = 0;
        float                   rgParam7            = 0;

        bool                    showError           = true;
        MavCmdAckHandlerInfo_t  ackHandlerInfo;
        int                     maxTries            = m_mavCommandMaxRetryCount;
        int                     tryCount            = 0;
        QElapsedTimer           elapsedTimer;
        int                     ackTimeoutMSecs     = m_mavCommandAckTimeoutMSecs;
    } MavCommandListEntry_t;

    QList<MavCommandListEntry_t>    m_mavCommandList;
    QTimer                          m_mavCommandResponseCheckTimer;
    static const int                m_mavCommandMaxRetryCount                = 3;
    static const int                m_mavCommandResponseCheckTimeoutMSecs    = 500;
    static const int                m_mavCommandAckTimeoutMSecs              = 3000;
    static const int                m_mavCommandAckTimeoutMSecsHighLatency   = 120000;

    void m_sendMavCommandWorker(
        bool commandInt, bool showError,
        const MavCmdAckHandlerInfo_t* ackHandlerInfo,
        int targetCompId, MAV_CMD command, MAV_FRAME frame,
        float param1, float param2, float param3, float param4, float param5, float param6, float param7
    );
    void m_sendMavCommandFromList(int index);
    int  m_findMavCommandListEntryIndex(int targetCompId, MAV_CMD command);
    bool m_sendMavCommandShouldRetry(MAV_CMD command);
    bool m_commandCanBeDuplicated(MAV_CMD command);

    VehicleLinkManager*         m_vehicleLinkManager = nullptr;
    InitialConnectStateMachine* m_initialConnectStateMachine = nullptr;
    StandardModes*              m_standardModes              = nullptr;
};















