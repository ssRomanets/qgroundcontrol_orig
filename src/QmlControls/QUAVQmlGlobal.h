#pragma once

#include <QtCore/QLoggingCategory>
#include <QtCore/QPointF>
#include <QtCore/QTimer>
#include <QtPositioning/QGeoCoordinate>

class LinkManager;
class MultiVehicleManager;
class QUAVCorePlugin;
class QUAVMapEngineManager;
class QUAVPalette;
class SettingsManager;
class VideoManager;

Q_MOC_INCLUDE("LinkManager.h")
Q_MOC_INCLUDE("MultiVehicleManager.h")
Q_MOC_INCLUDE("QUAVCorePlugin.h")
Q_MOC_INCLUDE("QUAVMapEngineManager.h")
Q_MOC_INCLUDE("QUAVPalette.h")
Q_MOC_INCLUDE("SettingsManager.h")
Q_MOC_INCLUDE("VideoManager.h")

class QUAVQmlGlobal : public QObject
{
    Q_OBJECT

public:
    QUAVQmlGlobal(QObject *parent = nullptr);
    ~QUAVQmlGlobal();

    static void registerQmlTypes();

    Q_PROPERTY(QString               appName                 READ    appName                 CONSTANT)
    Q_PROPERTY(LinkManager*          linkManager             READ    linkManager             CONSTANT)
    Q_PROPERTY(MultiVehicleManager*  multiVehicleManager     READ    multiVehicleManager     CONSTANT)
    Q_PROPERTY(QUAVMapEngineManager* mapEngineManager        READ    mapEngineManager        CONSTANT)
    Q_PROPERTY(VideoManager*         videoManager            READ    videoManager            CONSTANT)
    Q_PROPERTY(SettingsManager*      settingsManager         READ    settingsManager         CONSTANT)
    Q_PROPERTY(QUAVCorePlugin*       corePlugin              READ    corePlugin              CONSTANT)

    Q_PROPERTY(QUAVPalette*          globalPalette           MEMBER  m_globalPalette          CONSTANT)   ///< This palette will always return enabled colors
    Q_PROPERTY(QGeoCoordinate        flightMapPosition       READ    flightMapPosition       WRITE setFlightMapPosition  NOTIFY flightMapPositionChanged)
    Q_PROPERTY(double                flightMapZoom           READ    flightMapZoom           WRITE setFlightMapZoom      NOTIFY flightMapZoomChanged)

    Q_PROPERTY(QString quavVersion READ quavVersion CONSTANT)

    Q_PROPERTY(qreal zOrderTopMost READ zOrderTopMost CONSTANT)
    Q_PROPERTY(qreal zOrderWidgets READ zOrderWidgets CONSTANT)

    Q_INVOKABLE void saveBoolGlobalSetting (const QString& key, bool value);
    Q_INVOKABLE bool loadBoolGlobalSetting (const QString& key, bool defaultValue);

    // Property accessors

    static QString appName();
    LinkManager*            linkManager         ()  { return m_linkManager; }
    MultiVehicleManager*    multiVehicleManager ()  { return m_multiVehicleManager; }
    QUAVMapEngineManager*   mapEngineManager    ()  { return m_mapEngineManager;}
    VideoManager*           videoManager        ()  { return m_videoManager; }
    QUAVCorePlugin*         corePlugin          ()  { return m_corePlugin; }
    SettingsManager*        settingsManager     ()  { return m_settingsManager; }

    static QGeoCoordinate   flightMapPosition   ()  { return m_coord; }
    static double           flightMapZoom       ()  { return m_zoom; }

    qreal zOrderTopMost                         () { return 1000; }
    qreal zOrderWidgets                         () { return 1000; }

    void    setFlightMapPosition        (QGeoCoordinate& coordinate);
    void    setFlightMapZoom            (double zoom);

    static QString quavVersion();

signals:
    void flightMapPositionChanged       (QGeoCoordinate flightMapPosition);
    void flightMapZoomChanged           (double flightMapZoom);

private:
    QUAVMapEngineManager*   m_mapEngineManager       = nullptr;
    VideoManager*           m_videoManager           = nullptr;
    LinkManager*            m_linkManager            = nullptr;
    MultiVehicleManager*    m_multiVehicleManager    = nullptr;
    QUAVCorePlugin*         m_corePlugin             = nullptr;
    SettingsManager*        m_settingsManager        = nullptr;
    QUAVPalette*            m_globalPalette          = nullptr;

    static QGeoCoordinate   m_coord;
    static double           m_zoom;
    QTimer                  m_flightMapPositionSettledTimer;

    static constexpr const char* kQmlGlobalKeyName = "QGCQml";

    static constexpr const char* m_flightMapPositionSettingsGroup =          "FlightMapPosition";
    static constexpr const char* m_flightMapPositionLatitudeSettingsKey =    "Latitude";
    static constexpr const char* m_flightMapPositionLongitudeSettingsKey =   "Longitude";
    static constexpr const char* m_flightMapZoomSettingsKey =                "FlightMapZoom";
};
