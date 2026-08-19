
#pragma once

#include <QtCore/QElapsedTimer>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>

#include <QtWidgets/QApplication>

class QQmlApplicationEngine;
class QQuickWindow;
class QUAVApplication;
class QEvent;
class QPostEventList;
class QMetaMethod;
class QMetaObject;

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<QUAVApplication*>(QApplication::instance()))

#if defined(qGuiApp)
#undef qGuiApp
#endif
#define qGuiApp (static_cast<QUAVApplication*>(QGuiApplication::instance()))

#define quavApp() qApp

/// The main application and management class.
/// Needs QApplication base to support QtCharts module.
/// TODO: Use QtGraphs to convert to QGuiApplication
class QUAVApplication : public QApplication
{
    Q_OBJECT

    /// Unit Test have access to creating and destroying singletons
    friend class UnitTest;
public:
    QUAVApplication(int &argc, char *argv[], bool unitTesting, bool simpleBootTest);
    ~QUAVApplication();

    /// Sets the persistent flag to delete all settings the next time QGroundControl is started.
    static void deleteAllSettingsNextBoot();

    /// Clears the persistent flag to delete all settings the next time QGroundControl is started.
    static void clearDeleteAllSettingsNextBoot();

    bool runningUnitTests() const { return m_runningUnitTests; }
    bool simpleBootTest() const { return m_simpleBootTest; }

    /// Returns true if Qt debug output should be logged to a file
    bool logOutput() const { return m_logOutput; }

    /// Used to report a missing Parameter. Warning will be displayed to user. Method may be called
    /// multiple times.
    void reportMissingParameter(int componentId, const QString &name);

    /// @return true: Fake ui into showing mobile interface
    bool fakeMobile() const { return m_fakeMobile; }

    void setLanguage();
    QQuickWindow* mainRootWindow();
    uint64_t msecsSinceBoot() const { return m_msecsElapsedTime.elapsed(); }
    QString numberToString(quint64 number);
    QString bigSizeToString(quint64 size);
    QString bigSizeMBToString(quint64 size_MB);

public:
    /// Perform initialize which is common to both normal application running and unit tests.
    void init();
    void shutdown();

signals:
    void languageChanged(const QLocale locale);

public slots:
    void showVehicleConfig();

    /// Get current language
    QLocale getCurrentLanguage() const { return m_locale; }

    /// Show modal application message to the user
    void showAppMessage(const QString &message, const QString &title = QString());

    /// Show modal application message to the user about the need for a reboot. Multiple messages will be supressed if they occur
    /// one after the other.
    void showRebootAppMessage(const QString &message, const QString &title = QString());

private slots:
    void m_showDelayedAppMessages();

private:
    void m_initVideo();

    /// Initialize the application for normal application boot. Or in other words we are not going to run unit tests.
    void m_initForNormalAppBoot();

    QObject* m_rootQmlObject();

    bool m_runningUnitTests = false;
    bool m_simpleBootTest = false;

    QQmlApplicationEngine* m_qmlAppEngine = nullptr;
    bool m_logOutput = false;    ///< true: Log Qt debug output to file
    bool m_fakeMobile = false;    ///< true: Fake ui into displaying mobile interface

    QQuickWindow* m_mainRootWindow = nullptr;
    QTranslator m_quavTranslatorSourceCode;           ///< translations for source code C++/Qml
    QTranslator m_quavTranslatorQtLibs;               ///< tranlsations for Qt libraries
    QLocale m_locale;

    QElapsedTimer m_msecsElapsedTime;

    QList<QPair<QString /* title */, QString /* message */>> m_delayedAppMessages;
};