#include "MAVLinkInspectorController.h"
#include "MAVLinkMessage.h"
#include "MAVLinkProtocol.h"
#include "MAVLinkSystem.h"
#include "QmlObjectListModel.h"

#include <QtQml/QQmlEngine>
#include <QTimer>

MAVLinkInspectorController::MAVLinkInspectorController(QObject *parent)
    : QObject(parent)
    , m_updateFrequencyTimer(new QTimer(this))
    , m_systems(new QmlObjectListModel(this))
{
    MAVLinkProtocol *const mavlinkProtocol = MAVLinkProtocol::instance();
    (void) connect(mavlinkProtocol, &MAVLinkProtocol::messageReceived, this, &MAVLinkInspectorController::m_receiveMessage);
    (void) connect(m_updateFrequencyTimer, &QTimer::timeout, this, &MAVLinkInspectorController::m_refreshFrequency);

    m_updateFrequencyTimer->setInterval(1000);
    m_updateFrequencyTimer->setSingleShot(false);
    m_updateFrequencyTimer->start();
}

MAVLinkInspectorController::~MAVLinkInspectorController()
{

}

QUAVMAVLinkSystem *MAVLinkInspectorController::m_findVehicle(uint8_t id)
{
    for (int i = 0; i < m_systems->count(); i++) {
        QUAVMAVLinkSystem *const system = qobject_cast<QUAVMAVLinkSystem*>(m_systems->get(i));
        if (system && (system->id() == id)) {
            return system;
        }
    }
    return nullptr;
}

void MAVLinkInspectorController::m_refreshFrequency()
{
    for (int i = 0; i < m_systems->count(); i++) {
        QUAVMAVLinkSystem* const system = qobject_cast<QUAVMAVLinkSystem*>(m_systems->get(i));
        if (!system) {
            continue;
        }

        for (int i = 0; i < system->messages()->count(); i++) {
            QUAVMAVLinkMessage *const msg = qobject_cast<QUAVMAVLinkMessage*>(system->messages()->get(i));
            if (msg) {
                msg->updateFreq();
            }
        }
    }
}

void MAVLinkInspectorController::m_receiveMessage(LinkInterface *link, const mavlink_message_t &message)
{
    Q_UNUSED(link);

    QUAVMAVLinkMessage* msg = nullptr;
    QUAVMAVLinkSystem* system = m_findVehicle(message.sysid);

    if (!system) {
        system = new QUAVMAVLinkSystem(message.sysid, this);
        m_systems->append(system);
        m_systemNames.append(tr("System %1").arg(message.sysid));
        emit systemsChanged();

        if (!m_activeSystem) {
            m_activeSystem = system;
            emit activeSystemChanged();
        }
    } else {
        msg = system->findMessage(message.msgid, message.compid);
    }

    if (!msg) {
        msg = new QUAVMAVLinkMessage(message, this);
        system->append(msg);
    } else {
        msg->update(message);
    }
}

void MAVLinkInspectorController::setActiveSystem(int systemId)
{
    QUAVMAVLinkSystem* const system = m_findVehicle(systemId);
    if (system != m_activeSystem) {
        m_activeSystem = system;
        emit activeSystemChanged();
    }
}