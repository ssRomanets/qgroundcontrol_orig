#include "AutoPilotPlugin.h"
#include "QUAVApplication.h"
#include "Vehicle.h"

#include <QtCore/QCoreApplication>

AutoPilotPlugin::AutoPilotPlugin(Vehicle* vehicle, QObject* parent)
    : QObject(parent)
    , m_vehicle(vehicle)
{

}

AutoPilotPlugin::~AutoPilotPlugin()
{

}

void AutoPilotPlugin::parametersReadyPreChecks()
{
    qDebug()<<"void AutoPilotPlugin::parametersReadyPreChecks()";
    if (!m_setupComplete) {
        // Take the user to Vehicle Config Summary
        quavApp()->showVehicleConfig();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        quavApp()->showAppMessage(tr("One or more vehicle components require setup prior to flight."));
    }
}