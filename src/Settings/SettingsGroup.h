#pragma once

#include "SettingsFact.h"

#define DEFINE_SETTING_NAME_GROUP() \
    static const char* name; \
    static const char* settingsGroup;

#define DECLARE_SETTINGGROUP(NAME, GROUP) \
    const char* NAME ## Settings::name = #NAME; \
    const char* NAME ## Settings::settingsGroup = GROUP; \
    NAME ## Settings::NAME ## Settings(QObject* parent) \
        : SettingsGroup(name, settingsGroup, parent)

#define DECLARE_SETTINGSFACT(CLASS, NAME) \
    const char* CLASS::NAME ## Name = #NAME; \
    Fact* CLASS::NAME() \
    { \
        if (!m_ ## NAME ## Fact) { \
            m_ ## NAME ## Fact = m_createSettingsFact(NAME ## Name); \
        } \
        return m_ ## NAME ## Fact; \
    }

#define DECLARE_SETTINGSFACT_NO_FUNC(CLASS, NAME) \
    const char* CLASS::NAME ## Name = #NAME; \
    Fact* CLASS::NAME()

#define DEFINE_SETTINGFACT(NAME) \
    private: \
    SettingsFact* m_ ## NAME ## Fact = nullptr; \
    public: \
    Q_PROPERTY(Fact* NAME READ NAME CONSTANT) \
    Fact* NAME(); \
    static const char* NAME ## Name;

/// Provides access to group of settings. The group is named and has a visible property associated with which can control whether the group
/// is shows in the ui.
class SettingsGroup : public QObject
{
    Q_OBJECT

public:
    /// @param name Name for this Settings group
    /// @param settingsGroup Group to place settings in for QSettings::setGroup
    SettingsGroup(const QString &name, const QString &settingsGroup, QObject* parent = nullptr);

    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

    virtual bool    visible             () { return m_visible; }
    virtual void    setVisible          (bool vis) { m_visible = vis; emit visibleChanged(); }

signals:
    void            visibleChanged      ();

protected:
    SettingsFact*   m_createSettingsFact(const QString& factName);
    bool            m_visible;
    QString         m_name;
    QString         m_settingsGroup;

    QMap<QString, FactMetaData*> m_nameToMetaDataMap;

private:
    static constexpr const char* kJsonFile = ":/json/%1.SettingsGroup.json";
};
