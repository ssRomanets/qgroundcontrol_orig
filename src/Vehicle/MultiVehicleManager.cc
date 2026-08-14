#include "MultiVehicleManager.h"
#include "MAVLinkProtocol.h"
#include "QUAVCorePlugin.h"
#include "QUAVOptions.h"
#include "QUAVApplication.h"
#include "QUAVCameraManager.h"
#include "QmlObjectListModel.h"
#include "MavlinkCameraControl.h"
#include "SimulatedCameraControl.h"
#include "VehicleLinkManager.h"

#include "Vehicle.h"
#include "LinkInterface.h"

#include <QtCore/qapplicationstatic.h>
#include <QtCore/QTimer>
#include <QtQml/QQmlEngine>

Q_APPLICATION_STATIC(MultiVehicleManager, m_multiVehicleManagerInstance);

MultiVehicleManager::MultiVehicleManager(QObject *parent)
                   : QObject(parent)
                   , m_vehicles(new QmlObjectListModel(this))
{
}

MultiVehicleManager::~MultiVehicleManager()
{
}

MultiVehicleManager* MultiVehicleManager::instance()
{
    return m_multiVehicleManagerInstance();
}

void MultiVehicleManager::registerQmlTypes()
{
    (void) qmlRegisterUncreatableType<MultiVehicleManager> ("QUAV.MultiVehicleManager",  1, 0, "MultiVehicleManager",    "Reference only");
}

void MultiVehicleManager::init()
{
    (void) connect(MAVLinkProtocol::instance(), &MAVLinkProtocol::vehicleHeartbeatInfo, this, &MultiVehicleManager::m_vehicleHeartbeatInfo);
}

void MultiVehicleManager::m_vehicleHeartbeatInfo(LinkInterface* link, int vehicleId, int componentId, int vehicleFirmwareType, int vehicleType)
{
    if (componentId != MAV_COMP_ID_AUTOPILOT1) {
        // Don't create vehicles for components other than the autopilot
        qDebug() << "Ignoring heartbeat from unknown component port:vehicleId:componentId:fwType:vehicleType"
                 << link->linkConfiguration()->name()
                 << vehicleId
                 << componentId
                 << vehicleFirmwareType
                 << vehicleType;
        return;
    }

    switch (vehicleType) {
        case MAV_TYPE_GCS:
        case MAV_TYPE_ONBOARD_CONTROLLER:
        case MAV_TYPE_GIMBAL:
        case MAV_TYPE_ADSB:
            // These are not vehicles, so don't create a vehicle for them
            return;
        default:
            break;
    }

    if ((m_vehicles->count()> 0) && !QUAVCorePlugin::instance()->options()->multiVehicleEnabled()) { return;}

    if (getVehicleById(vehicleId) || (vehicleId == 0)) {  return; }

    qDebug() << "Adding new vehicle link:vehicleId:componentId:vehicleFirmwareType:vehicleType "
             << link->linkConfiguration()->name()
             << vehicleId
             << componentId
             << vehicleFirmwareType
             << vehicleType;

    if (vehicleId == MAVLinkProtocol::instance()->getSystemId()) {
        quavApp()->showAppMessage(tr("Warning: A vehicle is using the same system id as %1: %2").arg(QCoreApplication::applicationName()).arg(vehicleId));
    }

    Vehicle* const vehicle = new Vehicle(link, vehicleId, componentId, (MAV_AUTOPILOT)vehicleFirmwareType, (MAV_TYPE)vehicleType, this);
    (void) connect(vehicle->vehicleLinkManager(), &VehicleLinkManager::allLinksRemoved, this, &MultiVehicleManager::m_deleteVehiclePhase1);

    m_vehicles->append(vehicle);

    if (m_vehicles->count() > 1) {
        quavApp()->showAppMessage(tr("Connected to Vehicle %1").arg(vehicleId));
    } else {
        setActiveVehicle(vehicle);
    }
}

void MultiVehicleManager::m_deleteVehiclePhase1(Vehicle* vehicle)
{
    //обрываем запись видео, когда вынимаем контроллер
    if (vehicle->cameraManager()->currentCameraInstance()->videoCaptureStatus() == MavlinkCameraControl::VIDEO_CAPTURE_STATUS_RUNNING)
    {
        (void) vehicle->cameraManager()->currentCameraInstance()->toggleVideoRecording();
    }

    qDebug() << Q_FUNC_INFO << vehicle;

    bool found = false;
    for(int i = 0; i < m_vehicles->count(); i++) {
        if (m_vehicles->get(i) == vehicle) {
            (void) m_vehicles->removeAt(i);
            found = true;
            break;
        }
    }

    if (!found) {
        qWarning() << "Vehicle not found in map!";
        return;
    }

    // We must let the above signals flow through the system as well as get back to the main loop event queue
    // before we can actually delete the Vehicle. The reason is that Qml may be holding on to references to it.
    // Even though the above signals should unload any Qml which has references, that Qml will not be destroyed
    // until we get back to the main loop. So we set a short timer which will then fire after Qt has finished
    // doing all of its internal nastiness to clean up the Qml. This works for both the normal running case
    // as well as the unit testing case which of course has a different signal flow!
    QTimer::singleShot(20, this, [this, vehicle](){ m_deleteVehiclePhase2(vehicle);});
}

void MultiVehicleManager::m_deleteVehiclePhase2(Vehicle* vehicle)
{
    qDebug() << Q_FUNC_INFO << vehicle;

    /// Qml has been notified of vehicle about to go away and should be disconnected from it by now.
    /// This means we can now clear the active vehicle property and delete the Vehicle for real.

    Vehicle* newActiveVehicle = nullptr;
    if (m_vehicles->count() > 0) {
        newActiveVehicle = qobject_cast<Vehicle*>(m_vehicles->get(0));
    }

    m_setActiveVehicle(newActiveVehicle);
}

void MultiVehicleManager::setActiveVehicle(Vehicle* vehicle)
{
    qDebug() << Q_FUNC_INFO << vehicle;

    if (vehicle != m_activeVehicle) {
        QTimer::singleShot(20, this, [this, vehicle](){   m_setActiveVehiclePhase2(vehicle);  });
    }
}

void MultiVehicleManager::m_setActiveVehiclePhase2(Vehicle* vehicle)
{
    qDebug() << Q_FUNC_INFO << vehicle;

    m_setActiveVehicle(vehicle);
}

Vehicle* MultiVehicleManager::getVehicleById(int vehicleId) const
{
    for (int i = 0; i < m_vehicles->count(); i++) {
        Vehicle* const vehicle = qobject_cast<Vehicle*>(m_vehicles->get(i));
        if (vehicle->id() == vehicleId) {
            return vehicle;
        }
    }
    return nullptr;
}

void MultiVehicleManager::m_setActiveVehicle(Vehicle* vehicle)
{
    if (vehicle != m_activeVehicle) {
        m_activeVehicle = vehicle;
        emit activeVehicleChanged(vehicle);
    }
}

