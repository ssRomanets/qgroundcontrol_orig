#pragma once

#include "QmlObjectListModel.h"
#include "MavlinkCameraControl.h"

#include <QtCore/QObject>
#include <QtCore/QVariantList>

class SimulatedCameraControl;
class Vehicle;

/// Camera Manager
class QUAVCameraManager : public QObject
{
    Q_OBJECT

public:
    QUAVCameraManager(Vehicle* vehicle);
    virtual ~QUAVCameraManager();

    static void registerQmlTypes();

    Q_PROPERTY(QmlObjectListModel*   cameras                READ cameras                NOTIFY camerasChanged)
    Q_PROPERTY(MavlinkCameraControl* currentCameraInstance  READ currentCameraInstance  NOTIFY currentCameraChanged)

    virtual QmlObjectListModel*     cameras ()              { return &m_cameras;}
    virtual MavlinkCameraControl*   currentCameraInstance();

signals:
    void camerasChanged ();
    void currentCameraChanged();

protected:
    virtual void m_addCameraControlToLists(MavlinkCameraControl* cameraControl);

    QmlObjectListModel          m_cameras;
    int                         m_currentCameraIndex = 0;
    SimulatedCameraControl*     m_simulatedCameraControl = nullptr;
};












