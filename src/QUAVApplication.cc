/**
 * @file
 *   @brief Implementation of class QUAVApplication
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#include "QUAVApplication.h"

#include <QtCore/QEvent>
#include <QtCore/QFile>
#include <QtCore/QMetaMethod>
#include <QtCore/QMetaObject>
#include <QtCore/QRegularExpression>
#include <QtGui/QFontDatabase>
#include <QtGui/QIcon>
#include <QtNetwork/QNetworkProxyFactory>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickImageProvider>
#include <QtQuick/QQuickWindow>
#include <QtQuickControls2/QQuickStyle>

#include <QtCore/private/qthread_p.h>

#include "JsonHelper.h"
#include "MultiVehicleManager.h"
#include "ParameterManager.h"
#include "QUAVCameraManager.h"
#include "QUAVCorePlugin.h"
#include "QUAVQmlGlobal.h"
#include "SettingsManager.h"
#include "AppSettings.h"
#include "LinkManager.h"
#include "VideoManager.h"

#include "MAVLinkInspectorController.h"

QUAVApplication::QUAVApplication(int &argc, char *argv[], bool unitTesting, bool simpleBootTest)
    : QApplication(argc, argv)
    , m_runningUnitTests(unitTesting)
    , m_simpleBootTest(simpleBootTest)
{
    m_msecsElapsedTime.start();

    // Setup for network proxy support
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    // Set application information
    QString applicationName;
    if (m_runningUnitTests || simpleBootTest) {
        // We don't want unit tests to use the same QSettings space as the normal app. So we tweak the app
        // name. Also we want to run unit tests with clean settings every time.
        applicationName = QStringLiteral("%1_unittest").arg(QGC_APP_NAME);
    } else {
#ifdef QGC_DAILY_BUILD
        // This gives daily builds their own separate settings space. Allowing you to use daily and stable builds
        // side by side without daily screwing up your stable settings.
        applicationName = QStringLiteral("%1 Daily").arg(QGC_APP_NAME);
#else
        applicationName = QGC_APP_NAME;
#endif
    }
    setApplicationName(applicationName);
    setOrganizationName(QGC_ORG_NAME);
    setOrganizationDomain(QGC_ORG_DOMAIN);
    setApplicationVersion(QString(QGC_APP_VERSION_STR));
#ifdef Q_OS_LINUX
    setWindowIcon(QIcon(":/res/qgroundcontrol.ico"));
#endif

    // Set settings format
    QSettings::setDefaultFormat(QSettings::IniFormat);

    // We need to set language as early as possible prior to loading on JSON files.
    setLanguage();
}

void QUAVApplication::setLanguage()
{
    m_locale = QLocale::system();
    qDebug() << "System reported locale:" << m_locale << "; Name" << m_locale.name() << "; Preffered (used in maps): " << (QLocale::system().uiLanguages().length() > 0 ? QLocale::system().uiLanguages()[0] : "None");

    QLocale::Language possibleLocale = AppSettings::m_qLocaleLanguageEarlyAccess();
    if (possibleLocale != QLocale::AnyLanguage) {
        m_locale = QLocale(possibleLocale);
    }
    //-- We have specific fonts for Korean
    if (m_locale == QLocale::Korean) {
        qDebug() << "Loading Korean fonts" << m_locale.name();
        if(QFontDatabase::addApplicationFont(":/fonts/NanumGothic-Regular") < 0) {
            qWarning() << "Could not load /fonts/NanumGothic-Regular font";
        }
        if(QFontDatabase::addApplicationFont(":/fonts/NanumGothic-Bold") < 0) {
            qWarning() << "Could not load /fonts/NanumGothic-Bold font";
        }
    }
    qDebug() << "Loading localizations for" << m_locale.name();
    removeTranslator(JsonHelper::translator());
    removeTranslator(&m_quavTranslatorSourceCode);
    removeTranslator(&m_quavTranslatorQtLibs);
    if (m_locale.name() != "en_US") {
        QLocale::setDefault(m_locale);
        if (m_quavTranslatorQtLibs.load("qt_" + m_locale.name(), QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            installTranslator(&m_quavTranslatorQtLibs);
        } else {
            qWarning() << "Qt lib localization for" << m_locale.name() << "is not present";
        }
        if (m_quavTranslatorSourceCode.load(m_locale, QLatin1String("qgc_source_"), "", ":/i18n")) {
            installTranslator(&m_quavTranslatorSourceCode);
        } else {
            qWarning() << "Error loading source localization for" << m_locale.name();
        }
        if (JsonHelper::translator()->load(m_locale, QLatin1String("qgc_json_"), "", ":/i18n")) {
            installTranslator(JsonHelper::translator());
        } else {
            qWarning() << "Error loading json localization for" << m_locale.name();
        }
    }

    if (m_qmlAppEngine) {
        m_qmlAppEngine->retranslate();
    }

    emit languageChanged(m_locale);
}

QUAVApplication::~QUAVApplication()
{

}

void QUAVApplication::init()
{
    SettingsManager::instance()->init();

    ParameterManager::registerQmlTypes();
    QUAVQmlGlobal::registerQmlTypes();
    QUAVCameraManager::registerQmlTypes();
    MultiVehicleManager::registerQmlTypes();
    SettingsManager::registerQmlTypes();
    QUAVCorePlugin::registerQmlTypes();

    qmlRegisterType<MAVLinkInspectorController>("QUAV.Controllers", 1, 0, "MAVLinkInspectorController");

    if (m_simpleBootTest) {
        m_initVideo();
    } else if (!m_runningUnitTests) {
        m_initForNormalAppBoot();
    }
}

void QUAVApplication::m_initVideo()
{
    QUAVCorePlugin::instance();
    VideoManager::instance();
}

void QUAVApplication::m_initForNormalAppBoot()
{
    m_initVideo();

    QQuickStyle::setStyle("Basic");
    QUAVCorePlugin::instance()->init();
    MultiVehicleManager::instance()->init();
    m_qmlAppEngine = QUAVCorePlugin::instance()->createQmlApplicationEngine(this);
    QObject::connect(m_qmlAppEngine, &QQmlApplicationEngine::objectCreationFailed, this, QCoreApplication::quit, Qt::QueuedConnection);
    QUAVCorePlugin::instance()->createRootWindow(m_qmlAppEngine);

    LinkManager::instance()->init();
    VideoManager::instance()->init();

    mainRootWindow();
}

QObject* QUAVApplication::m_rootQmlObject()
{
    if (m_qmlAppEngine && m_qmlAppEngine->rootObjects().size()) {
        return m_qmlAppEngine->rootObjects()[0];
    }

    return nullptr;
}

void QUAVApplication::showAppMessage(const QString &message, const QString &title)
{
    const QString dialogTitle = title.isEmpty() ? applicationName() : title;

    QObject *const rootQmlObject = m_rootQmlObject();
    if (rootQmlObject) {
        QVariant varReturn;
        QVariant varMessage = QVariant::fromValue(message);
        QMetaObject::invokeMethod(rootQmlObject, "m_showMessageDialog", Q_RETURN_ARG(QVariant, varReturn), Q_ARG(QVariant, dialogTitle), Q_ARG(QVariant, varMessage));
    } else if (runningUnitTests()) {
        // Unit tests can run without UI
        qDebug() << "QUAVApplication::showAppMessage unittest title:message" << dialogTitle << message;
    } else {
        // UI isn't ready yet
        m_delayedAppMessages.append(QPair<QString, QString>(dialogTitle, message));
        QTimer::singleShot(200, this, &QUAVApplication::m_showDelayedAppMessages);
    }
}

void QUAVApplication::showRebootAppMessage(const QString &message, const QString &title)
{
    static QTime lastRebootMessage;

    const QTime currentTime = QTime::currentTime();
    const QTime previousTime = lastRebootMessage;
    lastRebootMessage = currentTime;

    if (previousTime.isValid() && (previousTime.msecsTo(currentTime) < (60 * 1000 * 2))) {
        // Debounce reboot messages
        return;
    }

    showAppMessage(message, title);
}

void QUAVApplication::m_showDelayedAppMessages()
{
    if (m_rootQmlObject()) {
        for (const QPair<QString, QString>& appMsg: m_delayedAppMessages) {
            showAppMessage(appMsg.second, appMsg.first);
        }
        m_delayedAppMessages.clear();
    } else {
        QTimer::singleShot(200, this, &QUAVApplication::m_showDelayedAppMessages);
    }
}

QQuickWindow* QUAVApplication::mainRootWindow()
{
    if (!m_mainRootWindow) {
        m_mainRootWindow = qobject_cast<QQuickWindow*>(m_rootQmlObject());
    }

    return m_mainRootWindow;
}

void QUAVApplication::shutdown()
{
    QUAVCorePlugin::instance()->cleanup();
    // This is bad, but currently qobject inheritances are incorrect and cause crashes on exit without
    delete m_qmlAppEngine;

    qDebug() << "Exit";
}

QString QUAVApplication::numberToString(quint64 number)
{
    return getCurrentLanguage().toString(number);
}

void QUAVApplication::showVehicleConfig()
{
    if (m_rootQmlObject()) { QMetaObject::invokeMethod(m_rootQmlObject(), "showVehicleConfig");}
}

QString QUAVApplication::bigSizeToString(quint64 size)
{
    QString result;
    const QLocale kLocale = getCurrentLanguage();
    if (size < 1024) {
        result = kLocale.toString(size) + "B";
    } else if (size < pow(1024, 2)) {
        result = kLocale.toString(static_cast<double>(size) / 1024.0, 'f', 1) + "KB";
    } else if (size < pow(1024, 3)) {
        result = kLocale.toString(static_cast<double>(size) / pow(1024, 2), 'f', 1) + "MB";
    } else if (size < pow(1024, 4)) {
        result = kLocale.toString(static_cast<double>(size) / pow(1024, 3), 'f', 1) + "GB";
    } else {
        result = kLocale.toString(static_cast<double>(size) / pow(1024, 4), 'f', 1) + "TB";
    }
    return result;
}

QString QUAVApplication::bigSizeMBToString(quint64 size_MB)
{
    QString result;
    const QLocale kLocale = getCurrentLanguage();
    if (size_MB < 1024) {
        result = kLocale.toString(static_cast<double>(size_MB) , 'f', 0) + " MB";
    } else if(size_MB < pow(1024, 2)) {
        result = kLocale.toString(static_cast<double>(size_MB) / 1024.0, 'f', 1) + " GB";
    } else {
        result = kLocale.toString(static_cast<double>(size_MB) / pow(1024, 2), 'f', 2) + " TB";
    }
    return result;
}
