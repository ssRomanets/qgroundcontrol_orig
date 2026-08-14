#pragma once

#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>

#include "QUAVMAVLink.h"

class FirmwarePlugin;

/// FirmwarePluginManager is a singleton which is used to return the correct FirmwarePlugin for a MAV_AUTOPILOT type.
class FirmwarePluginManager : public QObject
{
    Q_OBJECT

public:
    /// Constructs an FirmwarePluginManager object.
    ///     @param parent The parent QObject.
    explicit FirmwarePluginManager(QObject* parent = nullptr);

    /// Destructor for the FirmwarePluginManager class.
    ~FirmwarePluginManager();

    /// Gets the singleton instance of FirmwarePluginManager.
    ///     @return The singleton instance.
    static FirmwarePluginManager* instance();

    /// Returns appropriate plugin for autopilot type.
    ///     @param firmwareType Type of firmwware to return plugin for.
    ///     @param vehicleType Vehicle type to return plugin for.
    /// @return Singleton FirmwarePlugin instance for the specified MAV_AUTOPILOT.
    FirmwarePlugin* firmwarePluginForAutopilot(MAV_AUTOPILOT firmwareType, MAV_TYPE vehicleType);

private:
    FirmwarePlugin* m_genericFirmwarePlugin = nullptr;
};









