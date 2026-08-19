
#include "UnitsSettings.h"

#include <QtQml/QQmlEngine>

DECLARE_SETTINGGROUP(Units, "Units")
{
    qmlRegisterUncreatableType<UnitsSettings>("QUAV.SettingsManager", 1, 0, "UnitsSettings", "Reference only");
}

DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, horizontalDistanceUnits)
{
    if (!m_horizontalDistanceUnitsFact) {
        // Distance/Area/Speed units settings can't be loaded from json since it creates an infinite loop of meta data loading.
        QStringList     enumStrings;
        QVariantList    enumValues;
        enumStrings << UnitsSettings::tr("Feet") << UnitsSettings::tr("Meters");
        enumValues << QVariant::fromValue(static_cast<uint32_t>(HorizontalDistanceUnitsFeet))
                   << QVariant::fromValue(static_cast<uint32_t>(HorizontalDistanceUnitsMeters));
        FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
        metaData->setName(horizontalDistanceUnitsName);
        metaData->setShortDescription(UnitsSettings::tr("Horizontal Distance"));
        metaData->setEnumInfo(enumStrings, enumValues);

        HorizontalDistanceUnits defaultHorizontalDistanceUnit = HorizontalDistanceUnitsMeters;
        switch(QLocale::system().measurementSystem()) {
            case QLocale::MetricSystem: {
                defaultHorizontalDistanceUnit = HorizontalDistanceUnitsMeters;
            } break;
            case QLocale::ImperialUSSystem:
            case QLocale::ImperialUKSystem:
                defaultHorizontalDistanceUnit = HorizontalDistanceUnitsFeet;
                break;
        }
        metaData->setRawDefaultValue(defaultHorizontalDistanceUnit);
        metaData->setQUAVRebootRequired(true);
        m_horizontalDistanceUnitsFact = new SettingsFact(m_settingsGroup, metaData, this);
    }
    return m_horizontalDistanceUnitsFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, verticalDistanceUnits)
{
    if (!m_verticalDistanceUnitsFact) {
        // Distance/Area/Speed units settings can't be loaded from json since it creates an infinite loop of meta data loading.
        QStringList     enumStrings;
        QVariantList    enumValues;
        enumStrings << UnitsSettings::tr("Feet") << UnitsSettings::tr("Meters");
        enumValues << QVariant::fromValue(static_cast<uint32_t>(VerticalDistanceUnitsFeet))
                   << QVariant::fromValue(static_cast<uint32_t>(VerticalDistanceUnitsMeters));
        FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
        metaData->setName(verticalDistanceUnitsName);
        metaData->setShortDescription(UnitsSettings::tr("Vertical Distance"));
        metaData->setEnumInfo(enumStrings, enumValues);
        VerticalDistanceUnits defaultVerticalAltitudeUnit = VerticalDistanceUnitsMeters;
        switch(QLocale::system().measurementSystem()) {
            case QLocale::MetricSystem: {
                defaultVerticalAltitudeUnit = VerticalDistanceUnitsMeters;
            } break;
            case QLocale::ImperialUSSystem:
            case QLocale::ImperialUKSystem:
                defaultVerticalAltitudeUnit = VerticalDistanceUnitsFeet;
                break;
        }
        metaData->setRawDefaultValue(defaultVerticalAltitudeUnit);
        metaData->setQUAVRebootRequired(true);
        m_verticalDistanceUnitsFact = new SettingsFact(m_settingsGroup, metaData, this);
    }
    return m_verticalDistanceUnitsFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, areaUnits)
{
    if (!m_areaUnitsFact) {
        // Distance/Area/Speed units settings can't be loaded from json since it creates an infinite loop of meta data loading.
        QStringList     enumStrings;
        QVariantList    enumValues;
        enumStrings << UnitsSettings::tr("SquareFeet") << UnitsSettings::tr("SquareMeters") << UnitsSettings::tr("SquareKilometers") << UnitsSettings::tr("Hectares") << UnitsSettings::tr("Acres") << UnitsSettings::tr("SquareMiles");
        enumValues <<
            QVariant::fromValue(static_cast<uint32_t>(AreaUnitsSquareFeet)) <<
            QVariant::fromValue(static_cast<uint32_t>(AreaUnitsSquareMeters)) <<
            QVariant::fromValue(static_cast<uint32_t>(AreaUnitsSquareKilometers)) <<
            QVariant::fromValue(static_cast<uint32_t>(AreaUnitsHectares)) <<
            QVariant::fromValue(static_cast<uint32_t>(AreaUnitsAcres)) <<
            QVariant::fromValue(static_cast<uint32_t>(AreaUnitsSquareMiles));
        FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
        metaData->setName(areaUnitsName);
        metaData->setShortDescription(UnitsSettings::tr("Area"));
        metaData->setEnumInfo(enumStrings, enumValues);

        AreaUnits defaultAreaUnit = AreaUnitsSquareMeters;
        switch(QLocale::system().measurementSystem()) {
            case QLocale::MetricSystem: {
                defaultAreaUnit = AreaUnitsSquareMeters;
            } break;
            case QLocale::ImperialUSSystem:
            case QLocale::ImperialUKSystem:
                defaultAreaUnit = AreaUnitsSquareMiles;
                break;
        }
        metaData->setRawDefaultValue(defaultAreaUnit);
        metaData->setQUAVRebootRequired(true);
        m_areaUnitsFact = new SettingsFact(m_settingsGroup, metaData, this);
    }
    return m_areaUnitsFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, speedUnits)
{
    if (!m_speedUnitsFact) {
        // Distance/Area/Speed units settings can't be loaded from json since it creates an infinite loop of meta data loading.
        QStringList     enumStrings;
        QVariantList    enumValues;
        enumStrings << "Feet/second" << "Meters/second" << "Miles/hour" << "Kilometers/hour" << UnitsSettings::tr("Knots");
        enumValues <<
            QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsFeetPerSecond)) <<
            QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsMetersPerSecond)) <<
            QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsMilesPerHour)) <<
            QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsKilometersPerHour)) <<
            QVariant::fromValue(static_cast<uint32_t>(SpeedUnitsKnots));
        FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
        metaData->setName(speedUnitsName);
        metaData->setShortDescription(UnitsSettings::tr("Speed"));
        metaData->setEnumInfo(enumStrings, enumValues);

        SpeedUnits defaultSpeedUnit = SpeedUnitsMetersPerSecond;
        switch(QLocale::system().measurementSystem()) {
            case QLocale::MetricSystem: {
                defaultSpeedUnit = SpeedUnitsMetersPerSecond;
            } break;
            case QLocale::ImperialUSSystem:
            case QLocale::ImperialUKSystem:
                defaultSpeedUnit = SpeedUnitsMilesPerHour;
                break;
        }
        metaData->setRawDefaultValue(defaultSpeedUnit);
        metaData->setQUAVRebootRequired(true);
        m_speedUnitsFact = new SettingsFact(m_settingsGroup, metaData, this);
    }
    return m_speedUnitsFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, temperatureUnits)
{
    if (!m_temperatureUnitsFact) {
        // Units settings can't be loaded from json since it creates an infinite loop of meta data loading.
        QStringList     enumStrings;
        QVariantList    enumValues;
        enumStrings << UnitsSettings::tr("Celsius") << UnitsSettings::tr("Fahrenheit");
        enumValues << QVariant::fromValue(static_cast<uint32_t>(TemperatureUnitsCelsius)) << QVariant::fromValue(static_cast<uint32_t>(TemperatureUnitsFarenheit));
        FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
        metaData->setName(temperatureUnitsName);
        metaData->setShortDescription(UnitsSettings::tr("Temperature"));
        metaData->setEnumInfo(enumStrings, enumValues);

        TemperatureUnits defaultTemperatureUnit = TemperatureUnitsCelsius;
        switch(QLocale::system().measurementSystem()) {
            case QLocale::MetricSystem: {
                defaultTemperatureUnit = TemperatureUnitsCelsius;
            } break;
            case QLocale::ImperialUSSystem:
            case QLocale::ImperialUKSystem:
                defaultTemperatureUnit = TemperatureUnitsFarenheit;
                break;
        }
        metaData->setRawDefaultValue(defaultTemperatureUnit);
        metaData->setQUAVRebootRequired(true);
        m_temperatureUnitsFact = new SettingsFact(m_settingsGroup, metaData, this);
    }
    return m_temperatureUnitsFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(UnitsSettings, weightUnits)
{
    if (!m_weightUnitsFact) {
        // Units settings can't be loaded from json since it creates an infinite loop of meta data loading.
        QStringList     enumStrings;
        QVariantList    enumValues;
        enumStrings << UnitsSettings::tr("Grams") << UnitsSettings::tr("Kilograms") << UnitsSettings::tr("Ounces") << UnitsSettings::tr("Pounds");
        enumValues
            << QVariant::fromValue(static_cast<uint32_t>(WeightUnitsGrams))
            << QVariant::fromValue(static_cast<uint32_t>(WeightUnitsKg))
            << QVariant::fromValue(static_cast<uint32_t>(WeightUnitsOz))
            << QVariant::fromValue(static_cast<uint32_t>(WeightUnitsLbs));
        FactMetaData* metaData = new FactMetaData(FactMetaData::valueTypeUint32, this);
        metaData->setName(weightUnitsName);
        metaData->setShortDescription(UnitsSettings::tr("Weight"));
        metaData->setEnumInfo(enumStrings, enumValues);
        WeightUnits defaultWeightUnit = WeightUnitsGrams;
        switch(QLocale::system().measurementSystem()) {
            case QLocale::MetricSystem:
            case QLocale::ImperialUKSystem: {
                defaultWeightUnit = WeightUnitsGrams;
            } break;
            case QLocale::ImperialUSSystem:
                defaultWeightUnit = WeightUnitsOz;
                break;
        }
        metaData->setRawDefaultValue(defaultWeightUnit);
        metaData->setQUAVRebootRequired(true);
        m_weightUnitsFact = new SettingsFact(m_settingsGroup, metaData, this);
    }
    return m_weightUnitsFact;
}
