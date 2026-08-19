#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QString>

#include "LinkInterface.h"
#include "MAVLinkLib.h"

/// MAVLink micro air vehicle protocol reference implementation.
/// MAVLink is a generic communication protocol for micro air vehicles.
/// for more information, please see the official website: https://mavlink.io
class MAVLinkProtocol : public QObject
{
    Q_OBJECT

public:
    /// Constructs an MAVLinkProtocol object.
    ///     @param parent The parent QObject.
    explicit MAVLinkProtocol(QObject* parent = nullptr);

    /// Destructor for the MAVLinkProtocol class.
    ~MAVLinkProtocol();

    /// Get the system id of this application
    int getSystemId() const;

    /// Get the component id of this application
    static int getComponentId() { return MAV_COMP_ID_MISSIONPLANNER; }

    /// Gets the singleton instance of MAVLinkProtocol.
    ///     @return The singleton instance.
    static MAVLinkProtocol* instance();

    /// Get the currently configured protocol version
    unsigned getCurrentVersion() const { return m_currentVersion;}

    /// Reset the counters for all metadata for this link.
    void resetMetadataForLink(LinkInterface* link);

    /// Set protocol version
    void setVersion(unsigned version);

signals:
    /// Heartbeat received on link
    void vehicleHeartbeatInfo(LinkInterface* link, int vehicleId, int componentId, int vehicleFirmwareType, int vehicleType);

    /// Message received and directly copied via signal
    void messageReceived(LinkInterface *link, const mavlink_message_t &message);

public slots:
    /// Receive bytes from a communication interface and constructs a MAVLink packet
    ///     @param link The interface to read from
    void receiveBytes(LinkInterface* link, const QByteArray& data);

private:
    void m_logData(LinkInterface* link, const mavlink_message_t& message);

    bool m_updateStatus(LinkInterface *link, const SharedLinkInterfacePtr linkPtr, uint8_t mavlinkChannel, const mavlink_message_t& message);

    uint64_t m_totalReceiveCounter[MAVLINK_COMM_NUM_BUFFERS]{};  ///< The total number of successfully received messages
    uint64_t m_totalLossCounter   [MAVLINK_COMM_NUM_BUFFERS]{};     ///< Total messages lost during transmission.
    float    m_runningLossPercent [MAVLINK_COMM_NUM_BUFFERS]{};      ///< Loss rate

    unsigned m_currentVersion = 100;
};




