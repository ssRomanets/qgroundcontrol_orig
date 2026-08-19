#include "SettingsManager.h"
#include "AppSettings.h"
#include "AutoConnectSettings.h"
#include "BrandImageSettings.h"
#include "MapsSettings.h"
#include "FlightMapSettings.h"
#include "FlyViewSettings.h"
#include "UnitsSettings.h"
#include "VideoSettings.h"
#include "MavlinkSettings.h"

#include <QtCore/qapplicationstatic.h>
#include <QtQml/qqml.h>

Q_APPLICATION_STATIC(SettingsManager, m_settingsManagerInstance);

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
    // qDebug() << Q_FUNC_INFO << this;
}

SettingsManager::~SettingsManager()
{
    // qDebug() << Q_FUNC_INFO << this;
}

SettingsManager *SettingsManager::instance()
{
    return m_settingsManagerInstance();
}

void SettingsManager::registerQmlTypes()
{
    (void) qmlRegisterUncreatableType<SettingsManager>("QUAV.SettingsManager", 1, 0, "SettingsManager", "Reference only");
}

void SettingsManager::init()
{
    m_unitsSettings       = new UnitsSettings(this); // Must be first since AppSettings references it
    m_appSettings         = new AppSettings(this);
    m_autoConnectSettings = new AutoConnectSettings(this);
    m_brandImageSettings  = new BrandImageSettings(this);
    m_flightMapSettings   = new FlightMapSettings(this);
    m_flyViewSettings     = new FlyViewSettings(this);
    m_mapsSettings        = new MapsSettings(this);
    m_videoSettings       = new VideoSettings(this);
    m_mavlinkSettings     = new MavlinkSettings(this);
}

AppSettings*         SettingsManager::appSettings()         const { return m_appSettings; }
AutoConnectSettings* SettingsManager::autoConnectSettings() const { return m_autoConnectSettings; }
BrandImageSettings*  SettingsManager::brandImageSettings()  const { return m_brandImageSettings; }
MapsSettings*        SettingsManager::mapsSettings()        const { return m_mapsSettings; }
FlightMapSettings*   SettingsManager::flightMapSettings()   const { return m_flightMapSettings; }
FlyViewSettings*     SettingsManager::flyViewSettings()     const { return m_flyViewSettings; }
UnitsSettings*       SettingsManager::unitsSettings()       const { return m_unitsSettings; }
VideoSettings*       SettingsManager::videoSettings()       const { return m_videoSettings; }
MavlinkSettings*     SettingsManager::mavlinkSettings()     const { return m_mavlinkSettings; }
