#include "LinkInterface.h"
#include "LinkManager.h"

#include <QtQml/QQmlEngine>

LinkInterface::LinkInterface(SharedLinkConfigurationPtr& config, QObject *parent)
             : QObject(parent)
             , m_config(config)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

LinkInterface::~LinkInterface()
{
    if (m_vehicleReferenceCount != 0) {
        qWarning() << Q_FUNC_INFO << "still have vehicle references:" << m_vehicleReferenceCount;
    }

    m_config.reset();
}

uint8_t LinkInterface::mavlinkChannel() const
{
    if (!mavlinkChannelIsSet()) {
        qWarning() << Q_FUNC_INFO << "mavlinkChannelIsSet() == false";
    }
    return m_mavlinkChannel;
}

bool LinkInterface::mavlinkChannelIsSet() const
{
    return (LinkManager::invalidMavlinkChannel() != m_mavlinkChannel);
}

bool LinkInterface::m_allocateMavlinkChannel()
{
    Q_ASSERT(!mavlinkChannelIsSet());

    if (mavlinkChannelIsSet()) {
        qWarning() << Q_FUNC_INFO << "already have" << m_mavlinkChannel;
        return true;
    }

    m_mavlinkChannel = LinkManager::instance()->allocateMavlinkChannel();

    if (!mavlinkChannelIsSet()) {
        qWarning() << Q_FUNC_INFO << "failed";
        return false;
    }

    qDebug() << "m_allocateMavlinkChannel" << m_mavlinkChannel;

    return true;
}

void LinkInterface::m_freeMavlinkChannel()
{
    qDebug() << Q_FUNC_INFO << m_mavlinkChannel;

    if (!mavlinkChannelIsSet()) { return; }

    LinkManager::instance()->freeMavlinkChannel(m_mavlinkChannel);
    m_mavlinkChannel = LinkManager::invalidMavlinkChannel();
}

void LinkInterface::writeBytesThreadSafe(const char* bytes, int length)
{
    const QByteArray data(bytes, length);
    (void) QMetaObject::invokeMethod(this, "m_writeBytes", Qt::AutoConnection, data);
}

void LinkInterface::removeVehicleReference()
{
    if (m_vehicleReferenceCount != 0) {
        m_vehicleReferenceCount--;
        m_connectionRemoved();
    } else {
        qWarning() << Q_FUNC_INFO << "called with no vehicle references";
    }
}

void LinkInterface::m_connectionRemoved()
{
    if (m_vehicleReferenceCount == 0) {
        // Since there are no vehicles on the link we can disconnect it right now
        disconnect();
    } else {
        // If there are still vehicles on this link we allow communication lost to trigger and don't automatically disconect until all the vehicles go away
    }
}



