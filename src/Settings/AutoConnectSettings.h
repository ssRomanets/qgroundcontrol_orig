#pragma once

#include "SettingsGroup.h"

/// Auto connect settings
/// Defines which links should be automatically created and started at runtime
class AutoConnectSettings : public SettingsGroup
{
    Q_OBJECT

public:
    AutoConnectSettings(QObject* parent = nullptr);

    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(autoConnectPixhawk)
    DEFINE_SETTINGFACT(autoConnectNmeaPort)

};