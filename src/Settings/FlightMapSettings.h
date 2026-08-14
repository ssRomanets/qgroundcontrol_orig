#pragma once

#include "SettingsGroup.h"

class FlightMapSettings : public SettingsGroup
{
    Q_OBJECT

public:
    FlightMapSettings(QObject* parent = nullptr);


    DEFINE_SETTING_NAME_GROUP()
    DEFINE_SETTINGFACT(mapProvider)
    DEFINE_SETTINGFACT(mapType)
    DEFINE_SETTINGFACT(elevationMapProvider)
};
