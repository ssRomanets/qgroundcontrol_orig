#pragma once

#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>
#include <QtCore/QVariantList>

#include "QUAVPalette.h"

class FactMetaData;
class LinkInterface;
class PlanMasterController;
class QFile;
class QUAVOptions;
class QGeoPositionInfoSource;
class QmlObjectListModel;
class QQmlApplicationEngine;
class QQuickItem;
class Vehicle;
class VideoReceiver;
class VideoSink;
class FactValueGrid;
typedef struct __mavlink_message mavlink_message_t;


class QUAVCorePlugin : public QObject
{
    Q_OBJECT
    Q_MOC_INCLUDE("QUAVOptions.h")
    Q_PROPERTY(bool showAdvancedUI                   READ showAdvancedUI              WRITE m_setShowAdvancedUI    NOTIFY showAdvancedUIChanged)
    Q_PROPERTY(bool showTouchAreas                   READ showTouchAreas              WRITE m_setShowTouchAreas    NOTIFY showTouchAreasChanged)
    Q_PROPERTY(const QUAVOptions *options            READ options                     CONSTANT)
    Q_PROPERTY(QString brandImageIndoor              READ brandImageIndoor            CONSTANT)
    Q_PROPERTY(QString brandImageOutdoor             READ brandImageOutdoor           CONSTANT)
    Q_PROPERTY(QVariantList analyzePages             READ analyzePages                CONSTANT)

public:
    explicit QUAVCorePlugin(QObject *parent = nullptr);
    virtual ~QUAVCorePlugin();

    static QUAVCorePlugin *instance();
    static void registerQmlTypes();

    virtual void init() { }
    virtual void cleanup() { }

    /// The list of pages/buttons under the Analyze Menu
    /// @return A list of QmlPageInfo
    virtual const QVariantList& analyzePages();

    /// Global options
    /// @return An instance of QUAVOptions
    virtual QUAVOptions *options();

    /// Allows the core plugin to override the visibility for a settings group
    ///     @param name - SettingsGroup name
    /// @return true: Show settings ui, false: Hide settings ui
    virtual bool overrideSettingsGroupVisibility(const QString &name) { Q_UNUSED(name); return true; }

    /// Allows the core plugin to override the setting meta data before the setting fact is created.
    ///     @param settingsGroup - QSettings group which contains this item
    ///     @param metaData - MetaData for setting fact
    /// @return true: Setting should be visible in ui, false: Setting should not be shown in ui
    virtual bool adjustSettingMetaData(const QString &settingsGroup, FactMetaData &metaData);

    /// Return the resource file which contains the brand image for for Indoor theme.
    virtual QString brandImageIndoor() const { return QString();}

    /// Return the resource file which contains the brand image for for Outdoor theme.
    virtual QString brandImageOutdoor() const { return QString();}

    /// Allows a plugin to override the specified color name from the palette
    virtual void paletteOverride(const QString &colorName, QUAVPalette::PaletteColorInfo_t &colorInfo) { Q_UNUSED(colorName); Q_UNUSED(colorInfo); };

    /// Allows the plugin to override or get access to the QmlApplicationEngine to do things like add import
    /// path or stuff things into the context prior to window creation.
    virtual QQmlApplicationEngine *createQmlApplicationEngine(QObject *parent);

    /// Allows the plugin to override the creation of the root (native) window.
    virtual void createRootWindow(QQmlApplicationEngine *qmlEngine);

    /// Allows the plugin to override the creation of VideoReceiver.
    virtual VideoReceiver* createVideoReceiver(QObject* parent);

    bool showTouchAreas() const { return m_showTouchAreas; }
    bool showAdvancedUI() const { return m_showAdvancedUI; }

signals:
    void showTouchAreasChanged(bool showTouchAreas);
    void showAdvancedUIChanged(bool showAdvancedUI);

protected:
    bool m_showTouchAreas = false;
    bool m_showAdvancedUI = true;

private:
    void        m_setShowTouchAreas(bool show);
    void        m_setShowAdvancedUI(bool show);

    QUAVOptions* m_defaultOptions = nullptr;
};
