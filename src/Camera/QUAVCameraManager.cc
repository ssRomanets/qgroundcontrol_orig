#include "QUAVCameraManager.h"
#include "SimulatedCameraControl.h"
#include "Vehicle.h"

#include <QtQml/QQmlEngine>

QUAVCameraManager::QUAVCameraManager(Vehicle *vehicle)
                 : m_simulatedCameraControl(new SimulatedCameraControl(vehicle, this))
{
    qDebug()<<"QGCCameraManager Created";

    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    m_addCameraControlToLists(m_simulatedCameraControl);
}

QUAVCameraManager::~QUAVCameraManager()
{

}

void QUAVCameraManager::registerQmlTypes()
{
    qmlRegisterUncreatableType<MavlinkCameraControl>("QUAV.Vehicle", 1, 0, "MavlinkCameraControl", "Reference only");
}

MavlinkCameraControl* QUAVCameraManager::currentCameraInstance()
{
    if (m_currentCameraIndex < m_cameras.count() && m_cameras.count()) {
        auto pCamera = qobject_cast<MavlinkCameraControl*>(m_cameras[m_currentCameraIndex]);
        return pCamera;
    }
    return nullptr;
}

void QUAVCameraManager::m_addCameraControlToLists(MavlinkCameraControl *cameraControl)
{
    QQmlEngine::setObjectOwnership(cameraControl, QQmlEngine::CppOwnership);
    m_cameras.append(cameraControl);

    emit camerasChanged();
}















