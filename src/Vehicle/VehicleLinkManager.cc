#include "VehicleLinkManager.h"
#include "Vehicle.h"
#include "LinkManager.h"
#include "QUAVApplication.h"

VehicleLinkManager::VehicleLinkManager(Vehicle* vehicle)
                  : QObject(vehicle)
                  , m_vehicle(vehicle)
{
    qDebug() << Q_FUNC_INFO << this;

    (void) connect(this, &VehicleLinkManager::linkNamesChanged, this, &VehicleLinkManager::linkStatusesChanged);
}

VehicleLinkManager::~VehicleLinkManager()
{
    qDebug() << Q_FUNC_INFO << this;
}

void VehicleLinkManager::mavlinkMessageReceived(LinkInterface *link, const mavlink_message_t &message)
{
    // Radio status messages come from Sik Radios directly. It doesn't indicate there is any life on the other end.
    if (message.msgid == MAVLINK_MSG_ID_RADIO_STATUS) {
        return;
    }

    const int linkIndex = m_containsLinkIndex(link);
    if (linkIndex == -1) {
        m_addLink(link);
        return;
    }
}

int VehicleLinkManager::m_containsLinkIndex(const LinkInterface* link)
{
    for (int i = 0; i < m_rgLinkInfo.count(); i++) {
        if (m_rgLinkInfo[i].link.get() == link) {
            return i;
        }
    }
    return -1;
}

void VehicleLinkManager::m_addLink(LinkInterface* link)
{
    if (m_containsLinkIndex(link) != -1) {
        qWarning() << "m_addLink call with link which is already in the list";
        return;
    }

    SharedLinkInterfacePtr sharedLink = LinkManager::instance()->sharedLinkInterfacePointerForLink(link);
    if (!sharedLink) {
        qDebug() << "m_addLink stale link" << (void*)link;
        return;
    }

    qDebug() << "m_addLink:" << link->linkConfiguration()->name()<< QString("%1").arg((qulonglong)link, 0, 16);

    link->addVehicleReference();

    LinkInfo_t linkInfo;
    linkInfo.link = sharedLink;
    m_rgLinkInfo.append(linkInfo);

    m_updatePrimaryLink();

    (void) connect(link , &LinkInterface::disconnected, this, &VehicleLinkManager::m_linkDisconnected);

    emit linkNamesChanged();
}

void VehicleLinkManager::m_removeLink(LinkInterface *link)
{
    const int linkIndex = m_containsLinkIndex(link);
    if (linkIndex == -1) {
        qWarning() << "_removeLink call with link which is already in the list";
        return;
    }

    qDebug() << "m_removeLink:" << QString("%1").arg((qulonglong)link, 0, 16);

    if (link == m_primaryLink.lock().get()) {
        m_primaryLink.reset();
        emit primaryLinkChanged();
    }

    disconnect(link, &LinkInterface::disconnected, this, &VehicleLinkManager::m_linkDisconnected);
    link->removeVehicleReference();
    emit linkNamesChanged();
    m_rgLinkInfo.removeAt(linkIndex);
}

void VehicleLinkManager::m_linkDisconnected()
{
    qDebug() << Q_FUNC_INFO << "linkCount" << m_rgLinkInfo.count();

    LinkInterface* link = qobject_cast<LinkInterface*>(sender());
    if (!link) {return;}

    m_removeLink(link);
    m_updatePrimaryLink();

    if (m_rgLinkInfo.isEmpty() && !m_allLinksRemovedSignalledByCloseVehicle) {
        qDebug() << "signalling allLinksRemoved";
        emit allLinksRemoved(m_vehicle);
    }
}

SharedLinkInterfacePtr VehicleLinkManager::m_bestActivePrimaryLink()
{
    // Next best is normal latency link
    for (const LinkInfo_t& linkInfo: m_rgLinkInfo) {
        SharedLinkInterfacePtr link = linkInfo.link;
        const SharedLinkConfigurationPtr config = link->linkConfiguration();
        if (config && !config->isHighLatency()) {  return link; }
    }

    // Last possible choice is a high latency link
    SharedLinkInterfacePtr link = m_primaryLink.lock();
    if (link && link->linkConfiguration()->isHighLatency()) {
        // Best choice continues to be the current high latency link
        return link;
    }

    // Pick any high latency link if one exists
    for (const LinkInfo_t& linkInfo: m_rgLinkInfo) {
        SharedLinkInterfacePtr link = linkInfo.link;
        const SharedLinkConfigurationPtr config = link->linkConfiguration();
        if (config && config->isHighLatency()) {  return link; }
    }

    return {};
}

bool VehicleLinkManager::m_updatePrimaryLink()
{
    SharedLinkInterfacePtr primaryLink = m_primaryLink.lock();
    const int linkIndex = m_containsLinkIndex(primaryLink.get());

    if ((linkIndex != -1) && !primaryLink->linkConfiguration()->isHighLatency()) {
        // Current priority link is still valid
        return false;
    }

    SharedLinkInterfacePtr bestActivePrimaryLink = m_bestActivePrimaryLink();
    if ((linkIndex != -1) && !bestActivePrimaryLink) {
        // Nothing better available, leave things set to current primary link
        return false;
    }

    if (bestActivePrimaryLink == primaryLink) {
        return false;
    }

    if (primaryLink && primaryLink->linkConfiguration()->isHighLatency()) {
        qDebug()<<"m_vehicle->sendMavCommand() 0";
    }

    m_primaryLink = bestActivePrimaryLink;

    if (bestActivePrimaryLink && bestActivePrimaryLink->linkConfiguration()->isHighLatency()) {
        qDebug()<<"m_vehicle->sendMavCommand() 1";
    }

    return true;
}

bool VehicleLinkManager::containsLink(LinkInterface *link)
{
    return (m_containsLinkIndex(link) != -1);
}

QString VehicleLinkManager::primaryLinkName() const
{
    if (!m_primaryLink.expired()) { return m_primaryLink.lock()->linkConfiguration()->name();}
    return QString();
}

void VehicleLinkManager::setPrimaryLinkByName(const QString &name)
{
    for (const LinkInfo_t& linkInfo: m_rgLinkInfo) {
        if (linkInfo.link->linkConfiguration()->name() == name) {
            m_primaryLink = linkInfo.link;
            emit primaryLinkChanged();
        }
    }
}

QStringList VehicleLinkManager::linkNames() const
{
    QStringList rgNames;
    for (const LinkInfo_t& linkInfo: m_rgLinkInfo) {rgNames.append(linkInfo.link->linkConfiguration()->name());}
    return rgNames;
}

QStringList VehicleLinkManager::linkStatuses() const
{
    QStringList rgStatuses;
    for (const LinkInfo_t& linkInfo: m_rgLinkInfo) {rgStatuses.append("");}
    return rgStatuses;
}