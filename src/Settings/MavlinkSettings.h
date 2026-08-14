#pragma once

#include "SettingsGroup.h"

/// Application Settings
class MavlinkSettings : public SettingsGroup
{
    Q_OBJECT

public:
    MavlinkSettings(QObject* parent = nullptr);

    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(gcsMavlinkSystemID)

};