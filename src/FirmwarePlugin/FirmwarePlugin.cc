#include "FirmwarePlugin.h"
#include "AutoPilotPlugin.h"
#include "GenericAutoPilotPlugin.h"
#include "Vehicle.h"
#include "QUAVCameraManager.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QThread>
#include <QVariantList>

FirmwarePlugin::FirmwarePlugin(QObject* parent)
              : QObject(parent)
{

}

FirmwarePlugin::~FirmwarePlugin()
{

}

AutoPilotPlugin* FirmwarePlugin::autopilotPlugin(Vehicle* vehicle) const
{
    return new GenericAutoPilotPlugin(vehicle, vehicle);
}

QUAVCameraManager* FirmwarePlugin::createCameraManagers(Vehicle *vehicle) const
{
    return new QUAVCameraManager(vehicle);
}


