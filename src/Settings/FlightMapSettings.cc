#include "FlightMapSettings.h"

#include <QtQml/QQmlEngine>

DECLARE_SETTINGGROUP(FlightMap, "FlightMap")
{
    qmlRegisterUncreatableType<FlightMapSettings>("QGroundControl.SettingsManager", 1, 0, "FlightMapSettings", "Reference only");
}

DECLARE_SETTINGSFACT(FlightMapSettings, mapProvider)
DECLARE_SETTINGSFACT(FlightMapSettings, mapType)
DECLARE_SETTINGSFACT(FlightMapSettings, elevationMapProvider)
