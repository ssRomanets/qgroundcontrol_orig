#include "MAVLinkProtocol.h"
#include "LinkManager.h"
#include "SettingsManager.h"
#include "MavlinkSettings.h"

#include <QtCore/qapplicationstatic.h>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaType>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>

Q_APPLICATION_STATIC(MAVLinkProtocol, m_mavlinkProtocolInstance);

MAVLinkProtocol::MAVLinkProtocol(QObject *parent)
    : QObject(parent)

{
    qDebug() << Q_FUNC_INFO << this;
}

MAVLinkProtocol::~MAVLinkProtocol()
{
    qDebug() << Q_FUNC_INFO << this;
}

MAVLinkProtocol *MAVLinkProtocol::instance() { return m_mavlinkProtocolInstance(); }

void MAVLinkProtocol::setVersion(unsigned version)
{
    const QList<SharedLinkInterfacePtr> sharedLinks = LinkManager::instance()->links();
    for (const SharedLinkInterfacePtr& interface : sharedLinks) {
        mavlink_set_proto_version(interface.get()->mavlinkChannel(), version / 100);
    }
    m_currentVersion = version;
}

void MAVLinkProtocol::resetMetadataForLink(LinkInterface* link)
{
    const uint8_t channel = link->mavlinkChannel();
    m_totalReceiveCounter[channel] = 0;
    m_totalLossCounter[channel] = 0;
    m_runningLossPercent[channel] = 0.f;

    link->setDecodedFirstMavlinkPacket(false);
}

void MAVLinkProtocol::receiveBytes(LinkInterface* link, const QByteArray& data)
{
    const SharedLinkInterfacePtr linkPtr = LinkManager::instance()->sharedLinkInterfacePointerForLink(link);
    if (!linkPtr) {
        qDebug() << "receiveBytes: link gone!" << data.size() << "bytes arrived too late";
        return;
    }

    for (const uint8_t &byte: data) {
        const uint8_t mavlinkChannel = link->mavlinkChannel();
        mavlink_message_t message{};
        mavlink_status_t status{};

        if (mavlink_parse_char(mavlinkChannel, byte, &message, &status) != MAVLINK_FRAMING_OK) {
            continue;
        }

        m_logData(link, message);

        if (!m_updateStatus(link, linkPtr, mavlinkChannel, message)) {
            break;
        }
    }
}

void MAVLinkProtocol::m_logData(LinkInterface* link, const mavlink_message_t& message)
{
    switch (message.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT: {
            mavlink_heartbeat_t heartbeat{};
            mavlink_msg_heartbeat_decode(&message, &heartbeat);
            emit vehicleHeartbeatInfo(link, message.sysid, message.compid, heartbeat.autopilot, heartbeat.type);
            break;
        }
        case MAVLINK_MSG_ID_HIGH_LATENCY: {
            mavlink_high_latency_t highLatency{};
            mavlink_msg_high_latency_decode(&message, &highLatency);
            // HIGH_LATENCY does not provide autopilot or type information, generic is our safest bet
            emit vehicleHeartbeatInfo(link, message.sysid, message.compid, MAV_AUTOPILOT_GENERIC, MAV_TYPE_GENERIC);
            break;
        }
        case MAVLINK_MSG_ID_HIGH_LATENCY2: {
            mavlink_high_latency2_t highLatency2 {};
            mavlink_msg_high_latency2_decode(&message, &highLatency2);
            emit vehicleHeartbeatInfo(link, message.sysid, message.compid, highLatency2.autopilot, highLatency2.type);
            break;
        }
        default: break;
    }
}

bool MAVLinkProtocol::m_updateStatus(
    LinkInterface *link, const SharedLinkInterfacePtr linkPtr, uint8_t mavlinkChannel, const mavlink_message_t& message
)
{
    emit messageReceived(link, message);

    if (linkPtr.use_count() == 1) {
        return false;
    }

    return true;
}

int MAVLinkProtocol::getSystemId() const
{
    return SettingsManager::instance()->mavlinkSettings()->gcsMavlinkSystemID()->rawValue().toInt();
}