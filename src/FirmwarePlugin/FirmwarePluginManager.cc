#include "FirmwarePluginManager.h"
#include "FirmwarePlugin.h"

#include <QtCore/QGlobalStatic>

Q_GLOBAL_STATIC(FirmwarePluginManager, m_firmwarePluginManagerInstance)

FirmwarePluginManager::FirmwarePluginManager(QObject* parent)
    : QObject(parent)
{

}

FirmwarePluginManager::~FirmwarePluginManager()
{

}

FirmwarePluginManager* FirmwarePluginManager::instance()
{
    return m_firmwarePluginManagerInstance();
}

FirmwarePlugin* FirmwarePluginManager::firmwarePluginForAutopilot(MAV_AUTOPILOT firmwareType, MAV_TYPE vehicleType)
{
    FirmwarePlugin* plugin = nullptr;

    if (!plugin) {
        if (!m_genericFirmwarePlugin) {
            m_genericFirmwarePlugin = new FirmwarePlugin(this);
        }
        plugin = m_genericFirmwarePlugin;
    }

    return plugin;
}







