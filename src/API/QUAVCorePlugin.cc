#include "QUAVCorePlugin.h"
#include "QmlComponentInfo.h"
#include "AppSettings.h"
#include "FactMetaData.h"
#include "QUAVOptions.h"
#include "QtMultimediaReceiver.h"
#include "VideoReceiver.h"

#include <QtCore/qapplicationstatic.h>
#include <QtCore/QFile>
#include <QtQml/qqml.h>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickItem>

#ifndef QGC_CUSTOM_BUILD
Q_APPLICATION_STATIC(QUAVCorePlugin, m_quavCorePluginInstance);
#endif

QUAVCorePlugin::QUAVCorePlugin(QObject *parent)
    : QObject(parent)
    , m_defaultOptions(new QUAVOptions(this))
{
    qDebug() << Q_FUNC_INFO << this;
}

QUAVCorePlugin::~QUAVCorePlugin()
{
    qDebug() << Q_FUNC_INFO << this;
}

QUAVCorePlugin* QUAVCorePlugin::instance()
{
#ifndef QGC_CUSTOM_BUILD
    return m_quavCorePluginInstance();
#else
    return CUSTOMCLASS::instance();
#endif
}

void QUAVCorePlugin::registerQmlTypes()
{
    (void) qmlRegisterUncreatableType<QUAVCorePlugin>("QUAV", 1, 0, "QUAVCorePlugin", QStringLiteral("Reference only"));
    (void) qmlRegisterUncreatableType<QUAVOptions>("QUAV", 1, 0, "QUAVOptions", QStringLiteral("Reference only"));
}

const QVariantList& QUAVCorePlugin::analyzePages()
{
    static const QVariantList analyzeList = {
        QVariant::fromValue(new QmlComponentInfo(
            tr("Log Download"),
            QUrl::fromUserInput(QStringLiteral("qrc:/qml/QUAV/AnalyzeView/LogDownloadPage.qml")),
            QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/LogDownloadIcon.svg")))),

        QVariant::fromValue(new QmlComponentInfo(
            tr("GeoTag Images"),
            QUrl::fromUserInput(QStringLiteral("qrc:/qml/QUAV/AnalyzeView/GeoTagPage.qml")),
            QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/GeoTagIcon.svg")))),

        QVariant::fromValue(new QmlComponentInfo(
            tr("MAVLink Console"),
            QUrl::fromUserInput(QStringLiteral("qrc:/qml/QUAV/AnalyzeView/MAVLinkConsolePage.qml")),
            QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/MAVLinkConsoleIcon.svg")))),

        QVariant::fromValue(new QmlComponentInfo(
            tr("MAVLink Inspector"),
            QUrl::fromUserInput(QStringLiteral("qrc:/qml/QUAV/AnalyzeView/MAVLinkInspectorPage.qml")),
            QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/MAVLinkInspector.svg")))),

        QVariant::fromValue(new QmlComponentInfo(
            tr("Vibration"),
            QUrl::fromUserInput(QStringLiteral("qrc:/qml/QUAV/AnalyzeView/VibrationPage.qml")),
            QUrl::fromUserInput(QStringLiteral("qrc:/qmlimages/VibrationPageIcon")))),
    };
    return analyzeList;
}



QUAVOptions *QUAVCorePlugin::options()
{
    return m_defaultOptions;
}


bool QUAVCorePlugin::adjustSettingMetaData(const QString &settingsGroup, FactMetaData &metaData)
{
    if (settingsGroup == AppSettings::settingsGroup) {
        if (metaData.name() == AppSettings::indoorPaletteName) {
            QVariant outdoorPalette;

            outdoorPalette = 1;

            metaData.setRawDefaultValue(outdoorPalette);
            return true;
        }
    }
    return true;
}

QQmlApplicationEngine *QUAVCorePlugin::createQmlApplicationEngine(QObject *parent)
{
    QQmlApplicationEngine *const qmlEngine = new QQmlApplicationEngine(parent);
    qmlEngine->addImportPath(QStringLiteral("qrc:/qml"));
    return qmlEngine;
}

void QUAVCorePlugin::createRootWindow(QQmlApplicationEngine *qmlEngine)
{
    qmlEngine->load(QUrl(QStringLiteral("qrc:/qml/QGroundControl/MainWindow/MainWindow.qml")));
}

VideoReceiver* QUAVCorePlugin::createVideoReceiver(QObject *parent)
{
    return QtMultimediaReceiver::createVideoReceiver(parent);
}

void QUAVCorePlugin::m_setShowTouchAreas(bool show)
{
    if (show != m_showTouchAreas) {
        m_showTouchAreas = show;
        emit showTouchAreasChanged(show);
    }
}

void QUAVCorePlugin::m_setShowAdvancedUI(bool show)
{
    if (show != m_showAdvancedUI) {
        m_showAdvancedUI = show;
        emit showAdvancedUIChanged(show);
    }
}