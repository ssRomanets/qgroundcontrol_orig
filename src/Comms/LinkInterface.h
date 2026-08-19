#pragma once

#include <QtCore/QLoggingCategory>

#include "LinkConfiguration.h"

class LinkManager;

/// The link interface defines the interface for all links used to communicate with the ground station application.
class LinkInterface : public QObject
{
    Q_OBJECT
    friend class LinkManager;

public:
    virtual ~LinkInterface();

    virtual void disconnect()        = 0;
    virtual bool isConnected() const = 0;

    SharedLinkConfigurationPtr       linkConfiguration()       { return m_config; }
    const SharedLinkConfigurationPtr linkConfiguration() const { return m_config; }

    uint8_t mavlinkChannel() const;
    bool mavlinkChannelIsSet() const;
    void setDecodedFirstMavlinkPacket(bool decodedFirstMavlinkPacket) {m_decodedFirstMavlinkPacket = decodedFirstMavlinkPacket;}
    void writeBytesThreadSafe(const char* bytes, int length);
    void addVehicleReference() { ++ m_vehicleReferenceCount;}
    void removeVehicleReference();

signals:
    void bytesReceived(LinkInterface* link, const QByteArray& data);
    void disconnected();
    void communicationError(const QString& title, const QString& error);

protected:
    /// Links are only created by LinkManager so constructor is not public
    explicit LinkInterface(SharedLinkConfigurationPtr& config, QObject *parent = nullptr);

    /// Called by the LinkManager during LinkInterface construction instructing the link to setup channels.
    /// Default implementation allocates a single channel. But some link types (such as MockLink) need more than one.
    virtual bool m_allocateMavlinkChannel();

    virtual void m_freeMavlinkChannel();

    void m_connectionRemoved();

    SharedLinkConfigurationPtr m_config;

private slots:
    /// Not thread safe if called directly, only writeBytesThreadSafe is thread safe
    virtual void m_writeBytes(const QByteArray& bytes) = 0;

private:
    /// connect is private since all links should be created through LinkManager::createConnectedLink calls
    virtual bool m_connect() = 0;

    uint8_t m_mavlinkChannel            = std::numeric_limits<uint8_t>::max();
    bool    m_decodedFirstMavlinkPacket = false;
    int     m_vehicleReferenceCount     = 0;
};

typedef std::shared_ptr<LinkInterface> SharedLinkInterfacePtr;
typedef std::weak_ptr<LinkInterface>   WeakLinkInterfacePtr;