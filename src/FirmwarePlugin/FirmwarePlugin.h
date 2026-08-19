#pragma once

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVariantList>
#include <QtPositioning/QGeoCoordinate>

class AutoPilotPlugin;
class Vehicle;
class QUAVCameraManager;

/// The FirmwarePlugin class represents the methods and objects which are specific to a certain Firmware flight stack.
/// This is the only place where flight stack specific code should reside in QGroundControl. The remainder of the
/// QGroundControl source is generic to a common mavlink implementation. The implementation in the base class supports
/// mavlink generic firmware. Override the base clase virtuals to create your own firmware specific plugin.

class FirmwarePlugin : public QObject
{
    Q_OBJECT

public:
    explicit FirmwarePlugin(QObject* parent = nullptr);
    virtual ~FirmwarePlugin();

    /// @return The AutoPilotPlugin associated with this firmware plugin. Must be overridden.
    virtual AutoPilotPlugin* autopilotPlugin(Vehicle* vehicle) const;

    /// Creates vehicle camera manager.
    virtual QUAVCameraManager* createCameraManagers(Vehicle* vehicle) const;

    /// Return the resource file which contains the brand image for the vehicle for Indoor theme.
    virtual QString brandImageIndoor(const Vehicle* ) const { return QString(); }

    /// Return the resource file which contains the brand image for the vehicle for Outdoor theme.
    virtual QString brandImageOutdoor(const Vehicle* ) const { return QString(); }

protected:
    QVariantList m_modeIndicatorList;
};
