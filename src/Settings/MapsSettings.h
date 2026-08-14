#pragma once

#include "SettingsGroup.h"

class MapsSettings : public SettingsGroup
{
    Q_OBJECT
public:
    MapsSettings(QObject* parent = nullptr);

    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(maxCacheDiskSize)
    DEFINE_SETTINGFACT(maxCacheMemorySize)
};
