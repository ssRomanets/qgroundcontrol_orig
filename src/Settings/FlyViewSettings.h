#pragma once

#include "SettingsGroup.h"

class FlyViewSettings : public SettingsGroup
{
    Q_OBJECT

public:
    FlyViewSettings(QObject* parent = nullptr);

    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(showAdditionalIndicatorsCompass)
    DEFINE_SETTINGFACT(lockNoseUpCompass)
    DEFINE_SETTINGFACT(showSimpleCameraControl)
    DEFINE_SETTINGFACT(instrumentQmlFile2)
};
