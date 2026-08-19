#include "QUAVQmlGlobal.h"

#include "QUAVApplication.h"
#include "LinkManager.h"
#include "SettingsManager.h"
#include "QUAVMapEngineManager.h"
#include "QUAVCorePlugin.h"
#include "ScreenToolsController.h"
#include "QUAVMapPalette.h"
#include "QUAVPalette.h"
#include "VideoManager.h"
#include "MultiVehicleManager.h"

#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>
#include <QCoreApplication>

QGeoCoordinate QUAVQmlGlobal::m_coord = QGeoCoordinate(0.0,0.0);
double QUAVQmlGlobal::m_zoom = 2;

static QObject* screenToolsControllerSingletonFactory(QQmlEngine*, QJSEngine*)
{
    ScreenToolsController* screenToolsController = new ScreenToolsController();
    return screenToolsController;
}

static QObject* quavQmlGlobalSingletonFactory(QQmlEngine*, QJSEngine*)
{
    QUAVQmlGlobal *const qmlGlobal = new QUAVQmlGlobal();
    return qmlGlobal;
}

void QUAVQmlGlobal::registerQmlTypes()
{
    qmlRegisterType<QUAVMapPalette>                  ("QUAV.Palette",               1, 0, "QUAVMapPalette");
    qmlRegisterType<QUAVPalette>                     ("QUAV.Palette",               1, 0, "QUAVPalette");
    qmlRegisterSingletonType<QUAVQmlGlobal>          ("QUAV",                       1, 0, "QUAV",                   quavQmlGlobalSingletonFactory);
    qmlRegisterSingletonType<ScreenToolsController>  ("QUAV.ScreenToolsController", 1, 0, "ScreenToolsController",  screenToolsControllerSingletonFactory);
}

QUAVQmlGlobal::QUAVQmlGlobal(QObject *parent)
    : QObject(parent)
    , m_mapEngineManager(QUAVMapEngineManager::instance())
    , m_videoManager(VideoManager::instance())
    , m_linkManager(LinkManager::instance())
    , m_multiVehicleManager(MultiVehicleManager::instance())
    , m_settingsManager(SettingsManager::instance())
    , m_corePlugin(QUAVCorePlugin::instance())
    , m_globalPalette(new QUAVPalette(this))
{
    // We clear the parent on this object since we run into shutdown problems caused by hybrid qml app. Instead we let it leak on shutdown.
    // setParent(nullptr);

    // Load last coordinates and zoom from config file
    QSettings settings;
    settings.beginGroup(m_flightMapPositionSettingsGroup);
    m_coord.setLatitude(settings.value(m_flightMapPositionLatitudeSettingsKey,    m_coord.latitude()).toDouble());
    m_coord.setLongitude(settings.value(m_flightMapPositionLongitudeSettingsKey,  m_coord.longitude()).toDouble());
    m_zoom = settings.value(m_flightMapZoomSettingsKey, m_zoom).toDouble();

    m_flightMapPositionSettledTimer.setSingleShot(true);
    m_flightMapPositionSettledTimer.setInterval(1000);
    (void) connect(&m_flightMapPositionSettledTimer, &QTimer::timeout, this, []() {
        // When they settle, save flightMapPosition and Zoom to the config file
        QSettings settings;
        settings.beginGroup(m_flightMapPositionSettingsGroup);
        settings.setValue(m_flightMapPositionLatitudeSettingsKey, m_coord.latitude());
        settings.setValue(m_flightMapPositionLongitudeSettingsKey, m_coord.longitude());
        settings.setValue(m_flightMapZoomSettingsKey, m_zoom);
    });
    connect(this, &QUAVQmlGlobal::flightMapPositionChanged, this, [this](QGeoCoordinate){
        if (!m_flightMapPositionSettledTimer.isActive()) {
            m_flightMapPositionSettledTimer.start();
        }
    });
    connect(this, &QUAVQmlGlobal::flightMapZoomChanged, this, [this](double){
        if (!m_flightMapPositionSettledTimer.isActive()) {
            m_flightMapPositionSettledTimer.start();
        }
    });
}

QUAVQmlGlobal::~QUAVQmlGlobal()
{
}

void QUAVQmlGlobal::saveBoolGlobalSetting (const QString& key, bool value)
{
    QSettings settings;
    settings.beginGroup(kQmlGlobalKeyName);
    settings.setValue(key, value);
}

bool QUAVQmlGlobal::loadBoolGlobalSetting (const QString& key, bool defaultValue)
{
    QSettings settings;
    settings.beginGroup(kQmlGlobalKeyName);
    return settings.value(key, defaultValue).toBool();
}

void QUAVQmlGlobal::setFlightMapPosition(QGeoCoordinate& coordinate)
{
    if (coordinate != flightMapPosition()) {
        m_coord.setLatitude(coordinate.latitude());
        m_coord.setLongitude(coordinate.longitude());
        emit flightMapPositionChanged(coordinate);
    }
}

void QUAVQmlGlobal::setFlightMapZoom(double zoom)
{
    if (zoom != flightMapZoom()) {
        m_zoom = zoom;
        emit flightMapZoomChanged(zoom);
    }
}

QString QUAVQmlGlobal::quavVersion(void)
{
    QString versionStr = QCoreApplication::applicationVersion();
    if (QSysInfo::buildAbi().contains("32"))
    {
        versionStr += QStringLiteral(" %1").arg(tr("32 bit"));
    }
    else if (QSysInfo::buildAbi().contains("64"))
    {
        versionStr += QStringLiteral(" %1").arg(tr("64 bit"));
    }
    return versionStr;
}

QString QUAVQmlGlobal::appName()
{
    return QCoreApplication::applicationName();
}