#pragma once

#include <QObject>
#include <QTimer>

#include "LinkConfiguration.h"
#include "LinkInterface.h"
#include "QUAVSerialPortInfo.h"

class AutoConnectSettings;

/// @brief Manage communication links
///        The Link Manager organizes the physical Links. It can manage arbitrary
///        links and takes care of connecting them as well assigning the correct
///        protocol instance to transport the link data into the application.
class LinkManager : public QObject
{
    Q_OBJECT

public:
    explicit LinkManager(QObject* parent = nullptr);
    ~LinkManager();

    static LinkManager* instance();

    void init();

    /// Sets the flag to suspend the all new connections
    ///     @param reason User visible reason to suspend connections
    void setConnectionsSuspended(const QString& reason) {
        m_connectionsSuspended       = true;
        m_connectionsSuspendedReason = reason;
    }

    /// Creates, connects (and adds) a link  based on the given configuration instance.
    bool createConnectedLink(SharedLinkConfigurationPtr& config);

    /// Called to signal app shutdown. Disconnects all links while turning off auto-connect.
    Q_INVOKABLE void shutdown();

    QList<SharedLinkInterfacePtr> links() { return m_rgLinks; }

    void disconnectAll();

    /// Allocates a mavlink channel for use
    ///     @return Mavlink channel index, invalidMavlinkChannel() for no channels available
    uint8_t allocateMavlinkChannel();
    void freeMavlinkChannel(uint8_t channel);

    /// If you are going to hold a reference to a LinkInterface* in your object you must reference count it
    /// by using this method to get access to the shared pointer.
    SharedLinkInterfacePtr sharedLinkInterfacePointerForLink(const LinkInterface* link);

    bool containslink(const LinkInterface* link) const;

    static constexpr uint8_t invalidMavlinkChannel() {return std::numeric_limits<uint8_t>::max();}

private slots:
    void m_linkDisconnected();
    void m_communicationError(const QString& title, const QString& error);

private:
    void m_updateAutoConnectLinks();

    QTimer* m_portListTimer = nullptr;
    AutoConnectSettings* m_autoConnectSettings = nullptr;

    bool        m_connectionsSuspended           = false;
    uint32_t    m_mavlinkChannelsUsedBitMask     = 1;
    QString     m_connectionsSuspendedReason;

    QList<SharedLinkInterfacePtr> m_rgLinks;

    static constexpr int m_autoconnectUpdateTimerMSecs  = 1000;
    static constexpr int m_autoconnectConnectDelayMSecs = 1000;

private:
    bool m_allowAutoConnectToBoard(QUAVSerialPortInfo::BoardType_t boardType) const;
    void m_addSerialAutoConnectLink();
    bool m_portAlreadyConnected(const QString &portName) const;
    void m_filterCompositePorts(QList<QUAVSerialPortInfo>& portList);

    QMap<QString, int>  m_autoconnectPortWaitList;
    QString             m_autoConnectRTKPort;

};




















