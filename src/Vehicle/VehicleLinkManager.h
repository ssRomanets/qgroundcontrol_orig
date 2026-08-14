#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QElapsedTimer>

#include "LinkInterface.h"
#include "MAVLinkLib.h"

class Vehicle;

class VehicleLinkManager : public QObject
{
    Q_OBJECT
    Q_MOC_INCLUDE("Vehicle.h")
    Q_PROPERTY(QString primaryLinkName       READ primaryLinkName          WRITE setPrimaryLinkByName        NOTIFY primaryLinkChanged     )
    Q_PROPERTY(QStringList linkNames         READ linkNames                                                  NOTIFY linkNamesChanged)
    Q_PROPERTY(QStringList linkStatuses      READ linkStatuses                                               NOTIFY linkStatusesChanged)

    friend class Vehicle;

public:
    VehicleLinkManager(Vehicle* vehicle);
    ~VehicleLinkManager();

    void                 mavlinkMessageReceived(LinkInterface* link, const mavlink_message_t& message);
    bool                 containsLink(LinkInterface* link);
    WeakLinkInterfacePtr primaryLink()              const {return m_primaryLink;}
    QString              primaryLinkName() const;
    QStringList          linkNames() const;
    QStringList          linkStatuses() const;

    void                 setPrimaryLinkByName(const QString& name);
    
signals:
    void primaryLinkChanged();
    void allLinksRemoved(Vehicle* vehicle);
    void linkNamesChanged();
    void linkStatusesChanged();
    
private:
    int                     m_containsLinkIndex(const LinkInterface* link);
    void                    m_addLink(LinkInterface* link);
    void                    m_removeLink(LinkInterface* link);
    void                    m_linkDisconnected();
    bool                    m_updatePrimaryLink();
    SharedLinkInterfacePtr  m_bestActivePrimaryLink();

    struct LinkInfo_t {
        SharedLinkInterfacePtr link;
    };

    Vehicle*                m_vehicle = nullptr;
    QList<LinkInfo_t>       m_rgLinkInfo;
    WeakLinkInterfacePtr    m_primaryLink;

    bool                    m_allLinksRemovedSignalledByCloseVehicle = false;
};