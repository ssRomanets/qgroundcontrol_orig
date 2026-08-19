#include "QUAVApplication.h"

#include "LinkManager.h"
#include "MAVLinkProtocol.h"
#include "QUAVSerialPortInfo.h"
#include "SettingsManager.h"
#include "AutoConnectSettings.h"
#include "MultiVehicleManager.h"
#include "QmlObjectListModel.h"
#include "LinkInterface.h"
#include "SerialLink.h"

#include <QtCore/qapplicationstatic.h>
#include <QtCore/QTimer>
#include <QtQml/qqml.h>

Q_APPLICATION_STATIC(LinkManager, m_linkManagerInstance);

LinkManager::LinkManager(QObject *parent)
    : QObject(parent)
    , m_portListTimer(new QTimer(this))

{
    qDebug() << Q_FUNC_INFO << this;
}

LinkManager::~LinkManager()
{
    qDebug() << Q_FUNC_INFO << this;
}

LinkManager* LinkManager::instance()
{
    return m_linkManagerInstance();
}

void LinkManager::init()
{
    m_autoConnectSettings = SettingsManager::instance()->autoConnectSettings();

    if (!quavApp()->runningUnitTests()) {
        (void) connect(m_portListTimer, &QTimer::timeout, this, &LinkManager::m_updateAutoConnectLinks);
        m_portListTimer->start(m_autoconnectUpdateTimerMSecs); // timeout must be long enough to get past bootloader on second pass
    }
}

void LinkManager::m_updateAutoConnectLinks()
{
    if (m_connectionsSuspended) { return; }

    m_addSerialAutoConnectLink();
}

bool LinkManager::createConnectedLink(SharedLinkConfigurationPtr& config)
{
    SharedLinkInterfacePtr link = nullptr;

    switch(config->type()) {

        case LinkConfiguration::TypeSerial:
        {
            link = std::make_shared<SerialLink>(config);
            break;
        }

        default:      break;
    }

    if (!link) {
        return false;
    }

    if (!link->m_allocateMavlinkChannel()) {
         qWarning() << "Link failed to setup mavlink channels";
         return false;
    }

    m_rgLinks.append(link);
    config->setLink(link);

    (void) connect(link.get(), &LinkInterface::communicationError, this,                        &LinkManager::m_communicationError);
    (void) connect(link.get(), &LinkInterface::bytesReceived,      MAVLinkProtocol::instance(), &MAVLinkProtocol::receiveBytes);
    (void) connect(link.get(), &LinkInterface::disconnected,       this,                        &LinkManager::m_linkDisconnected);

    MAVLinkProtocol::instance()->resetMetadataForLink(link.get());
    MAVLinkProtocol::instance()->setVersion(MAVLinkProtocol::instance()->getCurrentVersion());

    if (!link->m_connect()) {
        link->m_freeMavlinkChannel();
        m_rgLinks.removeAt(m_rgLinks.indexOf(link));
        config->setLink(nullptr);
        return false;
    }

    return true;
}

void LinkManager::disconnectAll()
{
    const QList<SharedLinkInterfacePtr> links = m_rgLinks;
    for (const SharedLinkInterfacePtr& sharedLink: links) { sharedLink->disconnect();}
}

void LinkManager::m_linkDisconnected()
{
    qDebug() <<"void LinkManager::m_linkDisconnected()";

    LinkInterface* const link = qobject_cast<LinkInterface*>(sender());

    if (!link || !containslink(link)) { return;}

    (void) disconnect(link, &LinkInterface::communicationError, quavApp(),                   &QUAVApplication::showAppMessage);
    (void) disconnect(link, &LinkInterface::bytesReceived,      MAVLinkProtocol::instance(), &MAVLinkProtocol::receiveBytes);
    (void) disconnect(link, &LinkInterface::disconnected,       this,                        &LinkManager::m_linkDisconnected);

    link->m_freeMavlinkChannel();

    for (auto it = m_rgLinks.begin(); it != m_rgLinks.end(); ++it) {
        if (it->get() == link) {
            qDebug() << Q_FUNC_INFO << it->get()->linkConfiguration()->name() << it->use_count();
            (void) m_rgLinks.erase(it);
            return;
        }
    }
}

SharedLinkInterfacePtr LinkManager::sharedLinkInterfacePointerForLink(const LinkInterface* link)
{
    for (SharedLinkInterfacePtr& sharedLink: m_rgLinks) {
        if (sharedLink.get() == link) {
            return sharedLink;
        }
    }

    qWarning() << "returning nullptr";
    return SharedLinkInterfacePtr(nullptr);
}

bool LinkManager::containslink(const LinkInterface* link) const
{
    for (const SharedLinkInterfacePtr& sharedLink : m_rgLinks) { if (sharedLink.get() == link) {return true;} }
    return false;
}

void LinkManager::m_communicationError(const QString& title, const QString& error)
{
    quavApp()->showAppMessage(error, title);
}

uint8_t LinkManager::allocateMavlinkChannel()
{
     for (uint8_t mavlinkChannel = 0; mavlinkChannel < MAVLINK_COMM_NUM_BUFFERS; mavlinkChannel++) {
         if (m_mavlinkChannelsUsedBitMask & (1 << mavlinkChannel)) {continue;}

         mavlink_reset_channel_status(mavlinkChannel);
         mavlink_status_t* const mavlinkStatus = mavlink_get_channel_status(mavlinkChannel);
         mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
         m_mavlinkChannelsUsedBitMask |= (1 << mavlinkChannel);
         qDebug() << "allocateMavlinkChannel" << mavlinkChannel;
         return mavlinkChannel;
    }

    qWarning() << "allocateMavlinkChannel: all channels reserved!";
    return invalidMavlinkChannel();
}

void LinkManager::freeMavlinkChannel(uint8_t channel)
{
    qDebug() << "freeMavlinkChannel" << channel;

    if (invalidMavlinkChannel() == channel) { return; }

    m_mavlinkChannelsUsedBitMask &= ~(1 << channel);
}

void LinkManager::m_addSerialAutoConnectLink()
{
    QList<QUAVSerialPortInfo> portList;
    portList = QUAVSerialPortInfo::availablePorts();

    m_filterCompositePorts(portList);

    QStringList currentPorts;
    for (const QUAVSerialPortInfo& portInfo: portList) {
        // qDebug() << "-----------------------------------------------------";
        // qDebug() << "portName:          " << portInfo.portName();
        // qDebug() << "systemLocation:    " << portInfo.systemLocation();
        // qDebug() << "description:       " << portInfo.description();
        // qDebug() << "manufacturer:      " << portInfo.manufacturer();
        // qDebug() << "serialNumber:      " << portInfo.serialNumber();
        // qDebug() << "vendorIdentifier:  " << portInfo.vendorIdentifier();
        // qDebug() << "productIdentifier: " << portInfo.productIdentifier();

        currentPorts << portInfo.systemLocation();

        QUAVSerialPortInfo::BoardType_t boardType;
        QString boardName;

        // check to see if nmea gps is configured for current Serial port, if so, set it up to connect
        if (portInfo.systemLocation().trimmed() == m_autoConnectSettings->autoConnectNmeaPort()->cookedDefaultValue()) {

        } else if (portInfo.getBoardInfo(boardType, boardName)) {
            // Should we be auto-connecting to this board type?
            if (!m_allowAutoConnectToBoard(boardType)) { continue; }

            if (portInfo.isBootloader()) {
                // Don't connect to bootloader
//                qDebug() << "Waiting for bootloader to finish" << portInfo.systemLocation();
                continue;
            }

            if (m_portAlreadyConnected(portInfo.systemLocation()) || (m_autoConnectRTKPort == portInfo.systemLocation())) {
//                qDebug() << "Skipping existing autoconnect" << portInfo.systemLocation();
            } else if (!m_autoconnectPortWaitList.contains(portInfo.systemLocation())) {
                // We don't connect to the port the first time we see it. The ability to correctly detect whether we
                // are in the bootloader is flaky from a cross-platform standpoint. So by putting it on a wait list
                // and only connect on the second pass we leave enough time for the board to boot up.
//                qDebug() << "Waiting for next autoconnect pass" << portInfo.systemLocation() << boardName;
                m_autoconnectPortWaitList[portInfo.systemLocation()] = 1;
            } else if (
                (++m_autoconnectPortWaitList[portInfo.systemLocation()] * m_autoconnectUpdateTimerMSecs) > m_autoconnectConnectDelayMSecs
            ) {
                SerialConfiguration* pSerialConfig = nullptr;
                m_autoconnectPortWaitList.remove(portInfo.systemLocation());

                switch (boardType) {
                    case QUAVSerialPortInfo::BoardTypePixhawk:
                    {
                        pSerialConfig = new SerialConfiguration(tr("%1 on %2 (AutoConnect)").arg(boardName, portInfo.portName().trimmed()));
                        pSerialConfig->setUsbDirect(true);
                        break;
                    }
                    default:
                    {
                        qWarning() << "Internal error: Unknown board type" << boardType;
                        continue;
                    }
                }

                if (pSerialConfig) {
                    qDebug() << "New auto-connect port added: " << pSerialConfig->name() << portInfo.systemLocation();
                    pSerialConfig->setBaud((boardType == QUAVSerialPortInfo::BoardTypeSiKRadio) ? 57600 : 115200);
                    pSerialConfig->setDynamic(true);
                    pSerialConfig->setPortName(portInfo.systemLocation());
                    pSerialConfig->setAutoConnect(true);

                    SharedLinkConfigurationPtr sharedConfig(pSerialConfig);
                    createConnectedLink(sharedConfig);
                }
            }
        }
    }
}

bool LinkManager::m_allowAutoConnectToBoard(QUAVSerialPortInfo::BoardType_t boardType) const
{
    switch (boardType) {
        case QUAVSerialPortInfo::BoardTypePixhawk:
        {
            if (m_autoConnectSettings->autoConnectPixhawk()->rawValue().toBool()) {
                return true;
            }
            break;
        }
        default:
        {
            qWarning() << "Internal error: Unknown board type" << boardType;
            return false;
        }
    }
    return false;
}

bool LinkManager::m_portAlreadyConnected(const QString &portName) const
{
    const QString searchPort = portName.trimmed();
    for (const SharedLinkInterfacePtr& linkInterface : m_rgLinks) {
        const SharedLinkConfigurationPtr linkConfig = linkInterface->linkConfiguration();
        const SerialConfiguration* const serialConfig = qobject_cast<const SerialConfiguration*>(linkConfig.get());
        if (serialConfig && (serialConfig->portName() == searchPort)) {
            return true;
        }
    }
    return false;
}

void LinkManager::m_filterCompositePorts(QList<QUAVSerialPortInfo> &portList)
{
    typedef QPair<quint16, quint16> VidPidPair_t;

    QMap<VidPidPair_t, QStringList> seenSerialNumbers;

    for (auto it = portList.begin(); it != portList.end();) {
        const QUAVSerialPortInfo& portInfo = *it;
        if (
            portInfo.hasVendorIdentifier() && portInfo.hasProductIdentifier() &&
            !portInfo.serialNumber().isEmpty() && portInfo.serialNumber() != "0"
        ) {
            VidPidPair_t vidPid(portInfo.vendorIdentifier(), portInfo.productIdentifier());
            if (seenSerialNumbers.contains(vidPid) && seenSerialNumbers[vidPid].contains(portInfo.serialNumber())) {
                // Some boards are a composite USB device, with the first port being mavlink and the second something else. We only expose to first mavlink port.
                // However internal NMEA devices can present like this, so dont skip anything with NMEA in description
                if (!portInfo.description().contains("NMEA")) {
                    // qDebug()
                    // << QStringLiteral("Removing secondary port on same device - port:%1 vid:%2 pid%3 sn:%4")
                    //         .arg(portInfo.portName()).arg(portInfo.vendorIdentifier())
                    //         .arg(portInfo.productIdentifier()).arg(portInfo.serialNumber()) << Q_FUNC_INFO;
                    it = portList.erase(it);
                    continue;
                }
            }
            seenSerialNumbers[vidPid].append(portInfo.serialNumber());
        }
        it++;
    }
}

void LinkManager::shutdown()
{
    setConnectionsSuspended(tr("Shutdown"));
    disconnectAll();

    // Wait for all the vehicles to go away to ensure an orderly shutdown and deletion of all objects
    while (MultiVehicleManager::instance()->vehicles()->count()) {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}
