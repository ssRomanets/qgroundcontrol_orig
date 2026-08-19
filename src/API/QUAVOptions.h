
#pragma once

#include <QtGui/QColor>
#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtQmlIntegration/QtQmlIntegration>

class QUAVOptions;

class QUAVFlyViewOptions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool showInstrumentPanel         READ showInstrumentPanel       CONSTANT)
    Q_PROPERTY(bool showMapScale                READ showMapScale              CONSTANT)

public:
    explicit QUAVFlyViewOptions(QUAVOptions *options, QObject *parent = nullptr);
    ~QUAVFlyViewOptions();

protected:
    virtual bool showInstrumentPanel() const { return true; }
    virtual bool showMapScale() const { return true; }

    const QUAVOptions* m_options = nullptr;
};


class QUAVOptions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool   combineSettingsAndSetup       READ combineSettingsAndSetup        CONSTANT)
    Q_PROPERTY(bool   enableSaveMainWindowPosition  READ enableSaveMainWindowPosition   CONSTANT)
    Q_PROPERTY(bool   multiVehicleEnabled           READ multiVehicleEnabled            NOTIFY multiVehicleEnabledChanged)
    Q_PROPERTY(double toolbarHeightMultiplier       READ toolbarHeightMultiplier        CONSTANT)
    Q_PROPERTY(float  devicePixelDensity            READ devicePixelDensity             NOTIFY devicePixelDensityChanged)
    Q_PROPERTY(const  QUAVFlyViewOptions* flyView   READ flyViewOptions                 CONSTANT)

public:
    explicit QUAVOptions(QObject *parent = nullptr);
    ~QUAVOptions();

    /// Should QUAV hide its settings menu and colapse it into one single menu (Settings and Vehicle Setup)?
    /// @return true if QUAV should consolidate both menus into one.
    virtual bool combineSettingsAndSetup() const { return false; }

    /// Main ToolBar Multiplier.
    /// @return Factor to use when computing toolbar height
    virtual double toolbarHeightMultiplier() const { return 1.0; }

    /// Desktop builds save the main application size and position on close (and restore it on open)
    virtual bool enableSaveMainWindowPosition() const { return true; }
    virtual float devicePixelDensity() const { return 0.0f; }

    virtual const QUAVFlyViewOptions* flyViewOptions() const { return m_defaultFlyViewOptions; }

    /// @return false: multi vehicle support is disabled
    virtual bool multiVehicleEnabled() const { return true; }

signals:
    void devicePixelDensityChanged();
    void multiVehicleEnabledChanged(bool multiVehicleEnabled);

protected:
    const QUAVFlyViewOptions* m_defaultFlyViewOptions = nullptr;
};
