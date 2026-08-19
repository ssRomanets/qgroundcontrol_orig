#pragma once

#include <QtCore/QObject>
#include <QtQmlIntegration/QtQmlIntegration>

class AppSettings;
class AutoConnectSettings;
class BrandImageSettings;
class FlightMapSettings;
class FlyViewSettings;
class MapsSettings;
class UnitsSettings;
class VideoSettings;
class MavlinkSettings;

/// Provides access to all app settings
class SettingsManager : public QObject
{
    Q_OBJECT
    Q_MOC_INCLUDE("AppSettings.h")
    Q_MOC_INCLUDE("AutoConnectSettings.h")
    Q_MOC_INCLUDE("BrandImageSettings.h")
    Q_MOC_INCLUDE("FlightMapSettings.h")
    Q_MOC_INCLUDE("FlyViewSettings.h")
    Q_MOC_INCLUDE("MapsSettings.h")
    Q_MOC_INCLUDE("UnitsSettings.h")
    Q_MOC_INCLUDE("VideoSettings.h")
    Q_MOC_INCLUDE("MavlinkSettings.h")

    Q_PROPERTY(QObject* appSettings                   READ appSettings                  CONSTANT)
    Q_PROPERTY(QObject* autoConnectSettings           READ autoConnectSettings          CONSTANT)
    Q_PROPERTY(QObject* brandImageSettings            READ brandImageSettings           CONSTANT)
    Q_PROPERTY(QObject* flightMapSettings             READ flightMapSettings            CONSTANT)
    Q_PROPERTY(QObject* flyViewSettings               READ flyViewSettings              CONSTANT)
    Q_PROPERTY(QObject* mapsSettings                  READ mapsSettings                 CONSTANT)
    Q_PROPERTY(QObject* unitsSettings                 READ unitsSettings                CONSTANT)
    Q_PROPERTY(QObject* videoSettings                 READ videoSettings                CONSTANT)
    Q_PROPERTY(QObject* mavlinkSettings               READ mavlinkSettings              CONSTANT)

public:
    SettingsManager(QObject *parent = nullptr);
    ~SettingsManager();

    static SettingsManager *instance();
    static void registerQmlTypes();

    void init();

    AppSettings*         appSettings() const;
    AutoConnectSettings* autoConnectSettings() const;
    BrandImageSettings*  brandImageSettings() const;
    FlightMapSettings*   flightMapSettings() const;
    FlyViewSettings*     flyViewSettings() const;
    MapsSettings*        mapsSettings() const;
    UnitsSettings*       unitsSettings() const;
    VideoSettings*       videoSettings() const;
    MavlinkSettings*     mavlinkSettings() const;

private:
    AppSettings*         m_appSettings = nullptr;
    AutoConnectSettings* m_autoConnectSettings = nullptr;
    BrandImageSettings*  m_brandImageSettings = nullptr;
    FlightMapSettings*   m_flightMapSettings = nullptr;
    FlyViewSettings*     m_flyViewSettings = nullptr;
    MapsSettings*        m_mapsSettings = nullptr;
    UnitsSettings*       m_unitsSettings = nullptr;
    VideoSettings*       m_videoSettings = nullptr;
    MavlinkSettings*     m_mavlinkSettings = nullptr;
};



































