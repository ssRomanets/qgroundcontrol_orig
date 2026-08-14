#pragma once

class LinkInterface;

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtQmlIntegration/QtQmlIntegration>

#include "MAVLinkLib.h"

class QUAVMAVLinkSystem;
class QmlObjectListModel;
class QTimer;

/// MAVLink message inspector controller (provides the logic for UI display)
class MAVLinkInspectorController : public QObject
{
    Q_OBJECT

    Q_MOC_INCLUDE("MAVLinkSystem.h")
    Q_MOC_INCLUDE("QmlObjectListModel.h")

    Q_PROPERTY(QmlObjectListModel* systems       READ systems        NOTIFY systemsChanged)
    Q_PROPERTY(QUAVMAVLinkSystem*  activeSystem  READ activeSystem   NOTIFY activeSystemChanged)
    Q_PROPERTY(QStringList         systemNames   READ systemNames    NOTIFY systemsChanged)

public:
    explicit MAVLinkInspectorController(QObject *parent = nullptr);
    ~MAVLinkInspectorController();

    Q_INVOKABLE void setActiveSystem(int systemId);

    QmlObjectListModel* systems     () const { return m_systems; }
    QUAVMAVLinkSystem*  activeSystem() const { return m_activeSystem; }
    QStringList         systemNames () const { return m_systemNames; }

signals:
    void activeSystemChanged();
    void systemsChanged();

private slots:
    void m_receiveMessage(LinkInterface *link, const mavlink_message_t& message);
    void m_refreshFrequency();

private:
    QUAVMAVLinkSystem* m_findVehicle(uint8_t id);

    QStringList         m_systemNames;
    QUAVMAVLinkSystem*  m_activeSystem = nullptr;
    QTimer*             m_updateFrequencyTimer = nullptr;
    QmlObjectListModel* m_systems = nullptr;     ///< List of QUAVMAVLinkSystem
};
