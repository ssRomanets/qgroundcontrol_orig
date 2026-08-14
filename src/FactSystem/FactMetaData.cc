
#include "FactMetaData.h"

#include "JsonHelper.h"
#include "MAVLinkLib.h"
#include "SettingsManager.h"
#include "UnitsSettings.h"

#include <QtCore/QtMath>

// Built in translations for all Facts
const FactMetaData::BuiltInTranslation_s FactMetaData::m_rgBuiltInTranslations[] = {
    { "centi-degrees",  "deg",  FactMetaData::m_centiDegreesToDegrees,                   FactMetaData::m_degreesToCentiDegrees },
    { "radians",        "deg",  FactMetaData::m_radiansToDegrees,                        FactMetaData::m_degreesToRadians },
    { "rad",            "deg",  FactMetaData::m_radiansToDegrees,                        FactMetaData::m_degreesToRadians },
    { "gimbal-degrees", "deg",  FactMetaData::m_mavlinkGimbalDegreesToUserGimbalDegrees, FactMetaData::m_userGimbalDegreesToMavlinkGimbalDegrees },
    { "norm",           "%",    FactMetaData::m_normToPercent,                           FactMetaData::m_percentToNorm },
};

// Translations driven by app settings
const FactMetaData::AppSettingsTranslation_s FactMetaData::m_rgAppSettingsTranslations[] = {
    { "m",      "m",        FactMetaData::UnitHorizontalDistance,    UnitsSettings::HorizontalDistanceUnitsMeters, FactMetaData::m_defaultTranslator,                   FactMetaData::m_defaultTranslator },
    { "meter",  "meter",    FactMetaData::UnitHorizontalDistance,    UnitsSettings::HorizontalDistanceUnitsMeters, FactMetaData::m_defaultTranslator,                   FactMetaData::m_defaultTranslator },
    { "meters", "meters",   FactMetaData::UnitHorizontalDistance,    UnitsSettings::HorizontalDistanceUnitsMeters, FactMetaData::m_defaultTranslator,                   FactMetaData::m_defaultTranslator },
    // NOTE: we've coined an artificial "raw unit" of "vertical metre" to separate it from the horizontal metre - a bit awkward but this is all the design permits
    { "vertical m",  "m",   FactMetaData::UnitVerticalDistance,      UnitsSettings::VerticalDistanceUnitsMeters,   FactMetaData::m_defaultTranslator,                   FactMetaData::m_defaultTranslator },
    { "cm/px",  "cm/px",    FactMetaData::UnitHorizontalDistance,    UnitsSettings::HorizontalDistanceUnitsMeters, FactMetaData::m_defaultTranslator,                   FactMetaData::m_defaultTranslator },
    { "m/s",    "m/s",      FactMetaData::UnitSpeed,                 UnitsSettings::SpeedUnitsMetersPerSecond,     FactMetaData::m_defaultTranslator,                   FactMetaData::m_defaultTranslator },
    { "C",      "C",        FactMetaData::UnitTemperature,           UnitsSettings::TemperatureUnitsCelsius,       FactMetaData::m_defaultTranslator,                   FactMetaData::m_defaultTranslator },
    { "m^2",    "m^2",      FactMetaData::UnitArea,                  UnitsSettings::AreaUnitsSquareMeters,         FactMetaData::m_defaultTranslator,                   FactMetaData::m_defaultTranslator },
    { "m",      "ft",       FactMetaData::UnitHorizontalDistance,    UnitsSettings::HorizontalDistanceUnitsFeet,   FactMetaData::m_metersToFeet,                        FactMetaData::m_feetToMeters },
    { "meter",  "ft",       FactMetaData::UnitHorizontalDistance,    UnitsSettings::HorizontalDistanceUnitsFeet,   FactMetaData::m_metersToFeet,                        FactMetaData::m_feetToMeters },
    { "meters", "ft",       FactMetaData::UnitHorizontalDistance,    UnitsSettings::HorizontalDistanceUnitsFeet,   FactMetaData::m_metersToFeet,                        FactMetaData::m_feetToMeters },
    { "vertical m",  "ft",  FactMetaData::UnitVerticalDistance,      UnitsSettings::VerticalDistanceUnitsFeet,     FactMetaData::m_metersToFeet,                        FactMetaData::m_feetToMeters },
    { "cm/px",  "in/px",    FactMetaData::UnitHorizontalDistance,    UnitsSettings::HorizontalDistanceUnitsFeet,   FactMetaData::m_centimetersToInches,                 FactMetaData::m_inchesToCentimeters },
    { "m^2",    "km^2",     FactMetaData::UnitArea,                  UnitsSettings::AreaUnitsSquareKilometers,     FactMetaData::m_squareMetersToSquareKilometers,      FactMetaData::m_squareKilometersToSquareMeters },
    { "m^2",    "ha",       FactMetaData::UnitArea,                  UnitsSettings::AreaUnitsHectares,             FactMetaData::m_squareMetersToHectares,              FactMetaData::m_hectaresToSquareMeters },
    { "m^2",    "ft^2",     FactMetaData::UnitArea,                  UnitsSettings::AreaUnitsSquareFeet,           FactMetaData::m_squareMetersToSquareFeet,            FactMetaData::m_squareFeetToSquareMeters },
    { "m^2",    "ac",       FactMetaData::UnitArea,                  UnitsSettings::AreaUnitsAcres,                FactMetaData::m_squareMetersToAcres,                 FactMetaData::m_acresToSquareMeters },
    { "m^2",    "mi^2",     FactMetaData::UnitArea,                  UnitsSettings::AreaUnitsSquareMiles,          FactMetaData::m_squareMetersToSquareMiles,           FactMetaData::m_squareMilesToSquareMeters },
    { "m/s",    "ft/s",     FactMetaData::UnitSpeed,                 UnitsSettings::SpeedUnitsFeetPerSecond,       FactMetaData::m_metersToFeet,                        FactMetaData::m_feetToMeters },
    { "m/s",    "mph",      FactMetaData::UnitSpeed,                 UnitsSettings::SpeedUnitsMilesPerHour,        FactMetaData::m_metersPerSecondToMilesPerHour,       FactMetaData::m_milesPerHourToMetersPerSecond },
    { "m/s",    "km/h",     FactMetaData::UnitSpeed,                 UnitsSettings::SpeedUnitsKilometersPerHour,   FactMetaData::m_metersPerSecondToKilometersPerHour,  FactMetaData::m_kilometersPerHourToMetersPerSecond },
    { "m/s",    "kn",       FactMetaData::UnitSpeed,                 UnitsSettings::SpeedUnitsKnots,               FactMetaData::m_metersPerSecondToKnots,              FactMetaData::m_knotsToMetersPerSecond },
    { "C",      "F",        FactMetaData::UnitTemperature,           UnitsSettings::TemperatureUnitsFarenheit,     FactMetaData::m_celsiusToFarenheit,                  FactMetaData::m_farenheitToCelsius },
    { "g",      "g",        FactMetaData::UnitWeight,                UnitsSettings::WeightUnitsGrams,              FactMetaData::m_defaultTranslator,                   FactMetaData::m_defaultTranslator },
    { "g",      "kg",       FactMetaData::UnitWeight,                UnitsSettings::WeightUnitsKg,                 FactMetaData::m_gramsToKilograms,                    FactMetaData::m_kilogramsToGrams },
    { "g",      "oz",       FactMetaData::UnitWeight,                UnitsSettings::WeightUnitsOz,                 FactMetaData::m_gramsToOunces,                       FactMetaData::m_ouncesToGrams },
    { "g",      "lbs",      FactMetaData::UnitWeight,                UnitsSettings::WeightUnitsLbs,                FactMetaData::m_gramsToPunds,                        FactMetaData::m_poundsToGrams },
};

FactMetaData::FactMetaData(QObject *parent)
    : QObject(parent)
{
    // qDebug() << Q_FUNC_INFO << this;
}

FactMetaData::FactMetaData(ValueType_t type, QObject *parent)
    : QObject(parent)
    , m_type(type)
{
    // qDebug() << Q_FUNC_INFO << this;
}

FactMetaData::FactMetaData(const FactMetaData &other, QObject *parent)
    : QObject(parent)
{
    // qDebug() << Q_FUNC_INFO << this;
    *this = other;
}

FactMetaData::FactMetaData(ValueType_t type, const QString &name, QObject *parent)
    : QObject(parent)
    , m_type(type)
    , m_name(name)
{
    // qDebug() << Q_FUNC_INFO << this;
}

FactMetaData::~FactMetaData()
{
    // qDebug() << Q_FUNC_INFO << this;
}

const FactMetaData &FactMetaData::operator=(const FactMetaData &other)
{
    m_decimalPlaces = other.m_decimalPlaces;
    m_rawDefaultValue = other.m_rawDefaultValue;
    m_defaultValueAvailable = other.m_defaultValueAvailable;
    m_bitmaskStrings = other.m_bitmaskStrings;
    m_bitmaskValues = other.m_bitmaskValues;
    m_enumStrings = other.m_enumStrings;
    m_enumValues = other.m_enumValues;
    m_category = other.m_category;
    m_group = other.m_group;
    m_longDescription = other.m_longDescription;
    m_rawMax = other.m_rawMax;
    m_rawMin = other.m_rawMin;
    m_name = other.m_name;
    m_shortDescription = other.m_shortDescription;
    m_type = other.m_type;
    m_rawUnits = other.m_rawUnits;
    m_cookedUnits = other.m_cookedUnits;
    m_rawTranslator = other.m_rawTranslator;
    m_cookedTranslator = other.m_cookedTranslator;
    m_vehicleRebootRequired = other.m_vehicleRebootRequired;
    m_quavRebootRequired = other.m_quavRebootRequired;
    m_rawIncrement = other.m_rawIncrement;
    m_hasControl = other.m_hasControl;
    m_readOnly = other.m_readOnly;
    m_writeOnly = other.m_writeOnly;
    m_volatile = other.m_volatile;

    return *this;
}

QVariant FactMetaData::rawDefaultValue() const
{
    if (m_defaultValueAvailable) {
        return m_rawDefaultValue;
    } else {
    //    qWarning() << "Attempt to access unavailable default value";
        return QVariant(0);
    }
}

void FactMetaData::setRawDefaultValue(const QVariant &rawDefaultValue)
{
    if ((m_type == valueTypeString) || (isInRawMinLimit(rawDefaultValue) && isInRawMaxLimit(rawDefaultValue))) {
        m_rawDefaultValue = rawDefaultValue;
        m_defaultValueAvailable = true;
    } else {
    //    qWarning() << "Attempt to set default value which is outside min/max range";
    }
}

void FactMetaData::setRawMin(const QVariant &rawMin)
{
    if (isInRawMinLimit(rawMin)) {
        m_rawMin = rawMin;
    } else {
        // qWarning() << "Attempt to set min below allowable value for fact:" << name()
        //                           << ", value attempted:" << rawMin
        //                           << ", type:" << type()
        //                           << ", min for type:" << _minForType();
        m_rawMin = m_minForType();
    }
}

void FactMetaData::setRawMax(const QVariant &rawMax)
{
    if (isInRawMaxLimit(rawMax)) {
        m_rawMax = rawMax;
    } else {
        // qWarning() << "Attempt to set max above allowable value for fact:" << name()
        //                           << ", value attempted:" << rawMax
        //                           << ", type:" << type()
        //                           << ", max for type:" << _maxForType();
        m_rawMax = m_maxForType();
    }
}

bool FactMetaData::isInRawMinLimit(const QVariant &variantValue) const
{
    switch (m_type) {
    case valueTypeUint8:
        return (m_rawMin.value<unsigned char>() <= variantValue.value<unsigned char>());
    case valueTypeInt8:
        return (m_rawMin.value<signed char>() <= variantValue.value<signed char>());
    case valueTypeUint16:
        return (m_rawMin.value<unsigned short int>() <= variantValue.value<unsigned short int>());
    case valueTypeInt16:
        return (m_rawMin.value<short int>() <= variantValue.value<short int>());
    case valueTypeUint32:
        return (m_rawMin.value<uint32_t>() <= variantValue.value<uint32_t>());
    case valueTypeInt32:
        return (m_rawMin.value<int32_t>() <= variantValue.value<int32_t>());
    case valueTypeUint64:
        return (m_rawMin.value<uint64_t>() <= variantValue.value<uint64_t>());
    case valueTypeInt64:
        return (m_rawMin.value<int64_t>() <= variantValue.value<int64_t>());
    case valueTypeFloat:
        return ((qIsNaN(variantValue.toFloat())) || (m_rawMin.value<float>() <= variantValue.value<float>()));
    case valueTypeDouble:
        return ((qIsNaN(variantValue.toDouble())) || (m_rawMin.value<double>() <= variantValue.value<double>()));
    default:
        return true;
    }
}

bool FactMetaData::isInRawMaxLimit(const QVariant &variantValue) const
{
    switch (m_type) {
    case valueTypeUint8:
        return (m_rawMax.value<unsigned char>() >= variantValue.value<unsigned char>());
    case valueTypeInt8:
        return (m_rawMax.value<signed char>() >= variantValue.value<signed char>());
    case valueTypeUint16:
        return (m_rawMax.value<unsigned short int>() >= variantValue.value<unsigned short int>());
    case valueTypeInt16:
        return (m_rawMax.value<short int>() >= variantValue.value<short int>());
    case valueTypeUint32:
        return (m_rawMax.value<uint32_t>() >= variantValue.value<uint32_t>());
    case valueTypeInt32:
        return (m_rawMax.value<int32_t>() >= variantValue.value<int32_t>());
    case valueTypeUint64:
        return (m_rawMax.value<uint64_t>() >= variantValue.value<uint64_t>());
    case valueTypeInt64:
        return (m_rawMax.value<int64_t>() >= variantValue.value<int64_t>());
    case valueTypeFloat:
        return (qIsNaN(variantValue.toFloat()) || (m_rawMax.value<float>() >= variantValue.value<float>()));
    case valueTypeDouble:
        return (qIsNaN(variantValue.toDouble()) || (m_rawMax.value<double>() >= variantValue.value<double>()));
    default:
        return true;
    }
}

QVariant FactMetaData::minForType(ValueType_t type)
{
    switch (type) {
    case valueTypeUint8:
        return QVariant(std::numeric_limits<unsigned char>::min());
    case valueTypeInt8:
        return QVariant(std::numeric_limits<signed char>::min());
    case valueTypeUint16:
        return QVariant(std::numeric_limits<unsigned short int>::min());
    case valueTypeInt16:
        return QVariant(std::numeric_limits<short int>::min());
    case valueTypeUint32:
        return QVariant(std::numeric_limits<uint32_t>::min());
    case valueTypeInt32:
        return QVariant(std::numeric_limits<int32_t>::min());
    case valueTypeUint64:
        return QVariant((qulonglong)std::numeric_limits<uint64_t>::min());
    case valueTypeInt64:
        return QVariant((qlonglong)std::numeric_limits<int64_t>::min());
    case valueTypeFloat:
        return QVariant(-std::numeric_limits<float>::max());
    case valueTypeDouble:
        return QVariant(-std::numeric_limits<double>::max());
    case valueTypeString:
        return QVariant();
    case valueTypeBool:
        return QVariant(0);
    case valueTypeElapsedTimeInSeconds:
        return QVariant(0.0);
    case valueTypeCustom:
    default:
        return QVariant();
    }
}

QVariant FactMetaData::maxForType(ValueType_t type)
{
    switch (type) {
    case valueTypeUint8:
        return QVariant(std::numeric_limits<unsigned char>::max());
    case valueTypeInt8:
        return QVariant(std::numeric_limits<signed char>::max());
    case valueTypeUint16:
        return QVariant(std::numeric_limits<unsigned short int>::max());
    case valueTypeInt16:
        return QVariant(std::numeric_limits<short int>::max());
    case valueTypeUint32:
        return QVariant(std::numeric_limits<uint32_t>::max());
    case valueTypeInt32:
        return QVariant(std::numeric_limits<int32_t>::max());
    case valueTypeUint64:
        return QVariant((qulonglong)std::numeric_limits<uint64_t>::max());
    case valueTypeInt64:
        return QVariant((qlonglong)std::numeric_limits<int64_t>::max());
    case valueTypeFloat:
        return QVariant(std::numeric_limits<float>::max());
    case valueTypeElapsedTimeInSeconds:
    case valueTypeDouble:
        return QVariant(std::numeric_limits<double>::max());
    case valueTypeString:
        return QVariant();
    case valueTypeBool:
        return QVariant(1);
    case valueTypeCustom:
    default:
        return QVariant();
    }
}

bool FactMetaData::convertAndValidateRaw(const QVariant &rawValue, bool convertOnly, QVariant &typedValue, QString &errorString) const
{
    bool convertOk = false;

    errorString.clear();

    switch (type()) {
    case FactMetaData::valueTypeInt8:
    case FactMetaData::valueTypeInt16:
    case FactMetaData::valueTypeInt32:
        typedValue = QVariant(rawValue.toInt(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInRawLimit<int32_t>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(rawMin().toInt()).arg(rawMax().toInt());
            }
        }
        break;
    case FactMetaData::valueTypeInt64:
        typedValue = QVariant(rawValue.toLongLong(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInRawLimit<int64_t>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(rawMin().toInt()).arg(rawMax().toInt());
            }
        }
        break;
    case FactMetaData::valueTypeUint8:
    case FactMetaData::valueTypeUint16:
    case FactMetaData::valueTypeUint32:
        typedValue = QVariant(rawValue.toUInt(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInRawLimit<uint32_t>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(rawMin().toUInt()).arg(rawMax().toUInt());
            }
        }
        break;
    case FactMetaData::valueTypeUint64:
        typedValue = QVariant(rawValue.toULongLong(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInRawLimit<uint64_t>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(rawMin().toUInt()).arg(rawMax().toUInt());
            }
        }
        break;
    case FactMetaData::valueTypeFloat:
        typedValue = QVariant(rawValue.toFloat(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInRawLimit<float>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(rawMin().toDouble()).arg(rawMax().toDouble());
            }
        }
        break;
    case FactMetaData::valueTypeElapsedTimeInSeconds:
    case FactMetaData::valueTypeDouble:
        typedValue = QVariant(rawValue.toDouble(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInRawLimit<double>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(rawMin().toDouble()).arg(rawMax().toDouble());
            }
        }
        break;
    case FactMetaData::valueTypeString:
        convertOk = true;
        typedValue = QVariant(rawValue.toString());
        break;
    case FactMetaData::valueTypeBool:
        convertOk = true;
        typedValue = QVariant(rawValue.toBool());
        break;
    case FactMetaData::valueTypeCustom:
        convertOk = true;
        typedValue = QVariant(rawValue.toByteArray());
        break;
    }

    if (!convertOk) {
        errorString += tr("Invalid number");
    }

    return (convertOk && errorString.isEmpty());
}

bool FactMetaData::convertAndValidateCooked(const QVariant &cookedValue, bool convertOnly, QVariant &typedValue, QString &errorString) const
{
    bool convertOk = false;

    errorString.clear();

    if (!convertOnly && m_customCookedValidator) {
        errorString = m_customCookedValidator(cookedValue);
        if (!errorString.isEmpty()) {
            return false;
        }
    }

    switch (type()) {
    case FactMetaData::valueTypeInt8:
    case FactMetaData::valueTypeInt16:
    case FactMetaData::valueTypeInt32:
        typedValue = QVariant(cookedValue.toInt(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInCookedLimit<int32_t>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(cookedMin().toInt()).arg(cookedMax().toInt());
            }
        }
        break;
    case FactMetaData::valueTypeInt64:
        typedValue = QVariant(cookedValue.toLongLong(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInCookedLimit<int64_t>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(cookedMin().toInt()).arg(cookedMax().toInt());
            }
        }
        break;
    case FactMetaData::valueTypeUint8:
    case FactMetaData::valueTypeUint16:
    case FactMetaData::valueTypeUint32:
        typedValue = QVariant(cookedValue.toUInt(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInCookedLimit<uint32_t>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(cookedMin().toUInt()).arg(cookedMax().toUInt());
            }
        }
        break;
    case FactMetaData::valueTypeUint64:
        typedValue = QVariant(cookedValue.toULongLong(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInCookedLimit<uint64_t>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(cookedMin().toUInt()).arg(cookedMax().toUInt());
            }
        }
        break;
    case FactMetaData::valueTypeFloat:
        typedValue = QVariant(cookedValue.toFloat(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInCookedLimit<float>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(cookedMin().toFloat()).arg(cookedMax().toFloat());
            }
        }
        break;
    case FactMetaData::valueTypeElapsedTimeInSeconds:
    case FactMetaData::valueTypeDouble:
        typedValue = QVariant(cookedValue.toDouble(&convertOk));
        if (!convertOnly && convertOk) {
            if (!isInCookedLimit<double>(typedValue)) {
                errorString = tr("Value must be within %1 and %2").arg(cookedMin().toDouble()).arg(cookedMax().toDouble());
            }
        }
        break;
    case FactMetaData::valueTypeString:
        convertOk = true;
        typedValue = QVariant(cookedValue.toString());
        break;
    case FactMetaData::valueTypeBool:
        convertOk = true;
        typedValue = QVariant(cookedValue.toBool());
        break;
    case FactMetaData::valueTypeCustom:
        convertOk = true;
        typedValue = QVariant(cookedValue.toByteArray());
        break;
    }

    if (!convertOk) {
        errorString += tr("Invalid number");
    }

    return (convertOk && errorString.isEmpty());
}

bool FactMetaData::clampValue(const QVariant &cookedValue, QVariant &typedValue) const
{
    bool convertOk = false;

    switch (type()) {
    case FactMetaData::valueTypeInt8:
    case FactMetaData::valueTypeInt16:
    case FactMetaData::valueTypeInt32:
        typedValue = QVariant(cookedValue.toInt(&convertOk));
        if (convertOk) {
            clamp<int32_t>(typedValue);
        }
        break;
    case FactMetaData::valueTypeInt64:
        typedValue = QVariant(cookedValue.toLongLong(&convertOk));
        if (convertOk) {
            clamp<int64_t>(typedValue);
        }
        break;
    case FactMetaData::valueTypeUint8:
    case FactMetaData::valueTypeUint16:
    case FactMetaData::valueTypeUint32:
        typedValue = QVariant(cookedValue.toUInt(&convertOk));
        if (convertOk) {
            clamp<uint32_t>(typedValue);
        }
        break;
    case FactMetaData::valueTypeUint64:
        typedValue = QVariant(cookedValue.toULongLong(&convertOk));
        if (convertOk) {
            clamp<uint64_t>(typedValue);
        }
        break;
    case FactMetaData::valueTypeFloat:
        typedValue = QVariant(cookedValue.toFloat(&convertOk));
        if (convertOk) {
            clamp<float>(typedValue);
        }
        break;
    case FactMetaData::valueTypeElapsedTimeInSeconds:
    case FactMetaData::valueTypeDouble:
        typedValue = QVariant(cookedValue.toDouble(&convertOk));
        if (convertOk) {
            clamp<double>(typedValue);
        }
        break;
    case FactMetaData::valueTypeString:
        convertOk = true;
        typedValue = QVariant(cookedValue.toString());
        break;
    case FactMetaData::valueTypeBool:
        convertOk = true;
        typedValue = QVariant(cookedValue.toBool());
        break;
    case FactMetaData::valueTypeCustom:
        convertOk = true;
        typedValue = QVariant(cookedValue.toByteArray());
        break;
    }

    return convertOk;
}

void FactMetaData::setBitmaskInfo(const QStringList &strings, const QVariantList &values)
{
    if (strings.count() != values.count()) {
    //    qWarning() << "Count mismatch strings:values" << strings.count() << values.count();
        return;
    }

    m_bitmaskStrings = strings;
    m_bitmaskValues = values;
    setBuiltInTranslator();
}

void FactMetaData::addBitmaskInfo(const QString &name, const QVariant &value)
{
    m_bitmaskStrings << name;
    m_bitmaskValues << value;
}

void FactMetaData::setEnumInfo(const QStringList &strings, const QVariantList &values)
{
    if (strings.count() != values.count()) {
    //    qWarning() << "Count mismatch strings:values" << strings.count() << values.count();
        return;
    }

    m_enumStrings = strings;
    m_enumValues = values;
    setBuiltInTranslator();
}

void FactMetaData::addEnumInfo(const QString &name, const QVariant &value)
{
    m_enumStrings << name;
    m_enumValues << value;
}

void FactMetaData::removeEnumInfo(const QVariant &value)
{
    const int index = m_enumValues.indexOf(value);
    if (index < 0) {
    //    qWarning() << "Value does not exist in fact:" << value;
        return;
    }

    m_enumValues.removeAt(index);
    m_enumStrings.removeAt(index);
}

void FactMetaData::setTranslators(Translator rawTranslator, Translator cookedTranslator)
{
    m_rawTranslator = rawTranslator;
    m_cookedTranslator = cookedTranslator;
}

void FactMetaData::setBuiltInTranslator()
{
    if (m_enumStrings.count() || m_bitmaskStrings.count()) {
        // No translation if enum
        setTranslators(m_defaultTranslator, m_defaultTranslator);
        m_cookedUnits = m_rawUnits;
        return;
    } else {
        for (size_t i = 0; i < std::size(m_rgBuiltInTranslations); i++) {
            const BuiltInTranslation_s *pBuiltInTranslation = &m_rgBuiltInTranslations[i];

            if (pBuiltInTranslation->rawUnits.toLower() == m_rawUnits.toLower()) {
                m_cookedUnits = pBuiltInTranslation->cookedUnits;
                setTranslators(pBuiltInTranslation->rawTranslator, pBuiltInTranslation->cookedTranslator);
                return;
            }
        }
    }

    // Translator not yet set, try app settings translators
    m_setAppSettingsTranslators();
}

QVariant FactMetaData::m_degreesToRadians(const QVariant &degrees)
{
    return QVariant(qDegreesToRadians(degrees.toDouble()));
}

QVariant FactMetaData::m_radiansToDegrees(const QVariant &radians)
{
    return QVariant(qRadiansToDegrees(radians.toDouble()));
}

QVariant FactMetaData::m_centiDegreesToDegrees(const QVariant &centiDegrees)
{
    return QVariant(centiDegrees.toReal() / 100.0);
}

QVariant FactMetaData::m_degreesToCentiDegrees(const QVariant &degrees)
{
    return QVariant(qRound(degrees.toReal() * 100.0));
}

QVariant FactMetaData::m_userGimbalDegreesToMavlinkGimbalDegrees(const QVariant &userGimbalDegrees)
{
    // User facing gimbal degree values are from 0 (level) to 90 (straight down)
    // Mavlink gimbal degree values are from 0 (level) to -90 (straight down)
    return (userGimbalDegrees.toDouble() * -1.0);
}

QVariant FactMetaData::m_mavlinkGimbalDegreesToUserGimbalDegrees(const QVariant& mavlinkGimbalDegrees)
{
    // User facing gimbal degree values are from 0 (level) to 90 (straight down)
    // Mavlink gimbal degree values are from 0 (level) to -90 (straight down)
    return (mavlinkGimbalDegrees.toDouble() * -1.0);
}

QVariant FactMetaData::m_metersToFeet(const QVariant &meters)
{
    return QVariant((meters.toDouble() * 1.0) / constants.feetToMeters);
}

QVariant FactMetaData::m_feetToMeters(const QVariant &feet)
{
    return QVariant(feet.toDouble() * constants.feetToMeters);
}

QVariant FactMetaData::m_squareMetersToSquareKilometers(const QVariant &squareMeters)
{
    return QVariant(squareMeters.toDouble() * 0.000001);
}

QVariant FactMetaData::m_squareKilometersToSquareMeters(const QVariant &squareKilometers)
{
    return QVariant(squareKilometers.toDouble() * 1000000.0);
}

QVariant FactMetaData::m_squareMetersToHectares(const QVariant &squareMeters)
{
    return QVariant(squareMeters.toDouble() * 0.0001);
}

QVariant FactMetaData::m_hectaresToSquareMeters(const QVariant &hectares)
{
    return QVariant(hectares.toDouble() * 1000.0);
}

QVariant FactMetaData::m_squareMetersToSquareFeet(const QVariant &squareMeters)
{
    return QVariant(squareMeters.toDouble() * constants.squareMetersToSquareFeet);
}

QVariant FactMetaData::m_squareFeetToSquareMeters(const QVariant &squareFeet)
{
    return QVariant(squareFeet.toDouble() * constants.feetToSquareMeters);
}

QVariant FactMetaData::m_squareMetersToAcres(const QVariant &squareMeters)
{
    return QVariant(squareMeters.toDouble() * constants.squareMetersToAcres);
}

QVariant FactMetaData::m_acresToSquareMeters(const QVariant &acres)
{
    return QVariant(acres.toDouble() * constants.acresToSquareMeters);
}

QVariant FactMetaData::m_squareMetersToSquareMiles(const QVariant &squareMeters)
{
    return QVariant(squareMeters.toDouble() * constants.squareMetersToSquareMiles);
}

QVariant FactMetaData::m_squareMilesToSquareMeters(const QVariant &squareMiles)
{
    return QVariant(squareMiles.toDouble() * constants.squareMilesToSquareMeters);
}

QVariant FactMetaData::m_metersPerSecondToMilesPerHour(const QVariant &metersPerSecond)
{
    return QVariant(((metersPerSecond.toDouble() * 1.0) / constants.milesToMeters) * constants.secondsPerHour);
}

QVariant FactMetaData::m_milesPerHourToMetersPerSecond(const QVariant &milesPerHour)
{
    return QVariant((milesPerHour.toDouble() * constants.milesToMeters) / constants.secondsPerHour);
}

QVariant FactMetaData::m_metersPerSecondToKilometersPerHour(const QVariant &metersPerSecond)
{
    return QVariant((metersPerSecond.toDouble() / 1000.0) * constants.secondsPerHour);
}

QVariant FactMetaData::m_kilometersPerHourToMetersPerSecond(const QVariant &kilometersPerHour)
{
    return QVariant((kilometersPerHour.toDouble() * 1000.0) / constants.secondsPerHour);
}

QVariant FactMetaData::m_metersPerSecondToKnots(const QVariant &metersPerSecond)
{
    return QVariant((metersPerSecond.toDouble() * constants.secondsPerHour) / (1000.0 * constants.knotsToKPH));
}

QVariant FactMetaData::m_knotsToMetersPerSecond(const QVariant& knots)
{
    return QVariant(knots.toDouble() * (1000.0 * constants.knotsToKPH / constants.secondsPerHour));
}

QVariant FactMetaData::m_percentToNorm(const QVariant &percent)
{
    return QVariant(percent.toDouble() / 100.0);
}

QVariant FactMetaData::m_normToPercent(const QVariant &normalized)
{
    return QVariant(normalized.toDouble() * 100.0);
}

QVariant FactMetaData::m_centimetersToInches(const QVariant &centimeters)
{
    return QVariant((centimeters.toDouble() * 1.0) / constants.inchesToCentimeters);
}

QVariant FactMetaData::m_inchesToCentimeters(const QVariant &inches)
{
    return QVariant(inches.toDouble() * constants.inchesToCentimeters);
}

QVariant FactMetaData::m_celsiusToFarenheit(const QVariant &celsius)
{
    return QVariant((celsius.toDouble() * (9.0 / 5.0)) + 32);
}

QVariant FactMetaData::m_farenheitToCelsius(const QVariant &farenheit)
{
    return QVariant((farenheit.toDouble() - 32) * (5.0 / 9.0));
}

QVariant FactMetaData::m_kilogramsToGrams(const QVariant &kg)
{
    return QVariant(kg.toDouble() * 1000);
}

QVariant FactMetaData::m_ouncesToGrams(const QVariant &oz)
{
    return QVariant(oz.toDouble() * constants.ouncesToGrams);
}

QVariant FactMetaData::m_poundsToGrams(const QVariant &lbs)
{
    return QVariant(lbs.toDouble() * constants.poundsToGrams);
}

QVariant FactMetaData::m_gramsToKilograms(const QVariant &g)
{
    return QVariant(g.toDouble() / 1000);
}

QVariant FactMetaData::m_gramsToOunces(const QVariant &g)
{
    return QVariant(g.toDouble() / constants.ouncesToGrams);
}

QVariant FactMetaData::m_gramsToPunds(const QVariant &g)
{
    return QVariant(g.toDouble() / constants.poundsToGrams);
}

void FactMetaData::setRawUnits(const QString &rawUnits)
{
    m_rawUnits = rawUnits;
    m_cookedUnits = rawUnits;

    setBuiltInTranslator();
}

FactMetaData::ValueType_t FactMetaData::stringToType(const QString &typeString, bool &unknownType)
{
    unknownType = false;

    for (size_t i = 0; i < std::size(m_rgKnownTypeStrings); i++) {
        if (typeString.compare(m_rgKnownTypeStrings[i], Qt::CaseInsensitive) == 0) {
            return m_rgKnownValueTypes[i];
        }
    }

    unknownType = true;

    return valueTypeDouble;
}

QString FactMetaData::typeToString(ValueType_t type)
{
    for (size_t i = 0; i < std::size(m_rgKnownTypeStrings); i++) {
        if (type == m_rgKnownValueTypes[i]) {
            return m_rgKnownTypeStrings[i];
        }
    }

    return QStringLiteral("UnknownType%1").arg(type);
}

size_t FactMetaData::typeToSize(ValueType_t type)
{
    switch (type) {
    case valueTypeUint8:
    case valueTypeInt8:
        return 1;
    case valueTypeUint16:
    case valueTypeInt16:
        return 2;
    case valueTypeUint32:
    case valueTypeInt32:
    case valueTypeFloat:
        return 4;
    case valueTypeUint64:
    case valueTypeInt64:
    case valueTypeDouble:
        return 8;
    case valueTypeCustom:
        return MAVLINK_MSG_PARAM_EXT_SET_FIELD_PARAM_VALUE_LEN;
    default:
    //     qWarning() << "Unsupported fact value type" << type;
        return 0;
    }
}

void FactMetaData::m_setAppSettingsTranslators()
{
    // We can only translate between real numbers
    if (m_enumStrings.isEmpty() && ((type() == valueTypeDouble) || (type() == valueTypeFloat))) {
        for (size_t i = 0; i < std::size(m_rgAppSettingsTranslations); i++) {
            const AppSettingsTranslation_s *pAppSettingsTranslation = &m_rgAppSettingsTranslations[i];

            if (m_rawUnits.toLower() != pAppSettingsTranslation->rawUnits.toLower()) {
                continue;
            }

            UnitsSettings *const settings = SettingsManager::instance()->unitsSettings();
            uint settingsUnits = 0;

            switch (pAppSettingsTranslation->unitType) {
            case UnitHorizontalDistance:
                settingsUnits = settings->horizontalDistanceUnits()->rawValue().toUInt();
                break;
            case UnitVerticalDistance:
                settingsUnits = settings->verticalDistanceUnits()->rawValue().toUInt();
                break;
            case UnitSpeed:
                settingsUnits = settings->speedUnits()->rawValue().toUInt();
                break;
            case UnitArea:
                settingsUnits = settings->areaUnits()->rawValue().toUInt();
                break;
            case UnitTemperature:
                settingsUnits = settings->temperatureUnits()->rawValue().toUInt();
                break;
            case UnitWeight:
                settingsUnits = settings->weightUnits()->rawValue().toUInt();
                break;
            default:
                break;
            }

            if (settingsUnits == pAppSettingsTranslation->unitOption) {
                m_cookedUnits = pAppSettingsTranslation->cookedUnits;
                setTranslators(pAppSettingsTranslation->rawTranslator, pAppSettingsTranslation->cookedTranslator);
                return;
            }
        }
    }
}

const FactMetaData::AppSettingsTranslation_s* FactMetaData::m_findAppSettingsUnitsTranslation(const QString &rawUnits, UnitTypes type)
{
    for (size_t i = 0; i < std::size(m_rgAppSettingsTranslations); i++) {
        const AppSettingsTranslation_s *const pAppSettingsTranslation = &m_rgAppSettingsTranslations[i];

        if (rawUnits.toLower() != pAppSettingsTranslation->rawUnits.toLower()) {
            continue;
        }

        uint unitOption = 0;
        UnitsSettings *unitsSettings = SettingsManager::instance()->unitsSettings();
        switch (type) {
        case UnitHorizontalDistance:
            unitOption = unitsSettings->horizontalDistanceUnits()->rawValue().toUInt();
            break;
        case UnitVerticalDistance:
            unitOption = unitsSettings->verticalDistanceUnits()->rawValue().toUInt();
            break;
        case UnitArea:
            unitOption = unitsSettings->areaUnits()->rawValue().toUInt();
            break;
        case UnitSpeed:
            unitOption = unitsSettings->speedUnits()->rawValue().toUInt();
            break;
        case UnitTemperature:
            unitOption = unitsSettings->temperatureUnits()->rawValue().toUInt();
            break;
        case UnitWeight:
            unitOption = unitsSettings->weightUnits()->rawValue().toUInt();
            break;
        }

        if ((pAppSettingsTranslation->unitType == type) && (pAppSettingsTranslation->unitOption == unitOption)) {
            return pAppSettingsTranslation;
        }
    }

    return nullptr;
}

QVariant FactMetaData::metersToAppSettingsHorizontalDistanceUnits(const QVariant &meters)
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("m", UnitHorizontalDistance);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->rawTranslator(meters);
    } else {
        return meters;
    }
}

QVariant FactMetaData::metersToAppSettingsVerticalDistanceUnits(const QVariant &meters)
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("vertical m", UnitVerticalDistance);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->rawTranslator(meters);
    } else {
        return meters;
    }
}

QVariant FactMetaData::appSettingsHorizontalDistanceUnitsToMeters(const QVariant &distance)
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("m", UnitHorizontalDistance);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedTranslator(distance);
    } else {
        return distance;
    }
}

QVariant FactMetaData::appSettingsVerticalDistanceUnitsToMeters(const QVariant &distance)
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("vertical m", UnitVerticalDistance);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedTranslator(distance);
    } else {
        return distance;
    }
}

QString FactMetaData::appSettingsHorizontalDistanceUnitsString()
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("m", UnitHorizontalDistance);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedUnits;
    } else {
        return QStringLiteral("m");
    }
}

QString FactMetaData::appSettingsVerticalDistanceUnitsString()
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("vertical m", UnitVerticalDistance);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedUnits;
    } else {
        return QStringLiteral("m");
    }
}

QString FactMetaData::appSettingsWeightUnitsString()
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("g", UnitWeight);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedUnits;
    } else {
        return QStringLiteral("g");
    }
}

QVariant FactMetaData::squareMetersToAppSettingsAreaUnits(const QVariant &squareMeters)
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("m^2", UnitArea);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->rawTranslator(squareMeters);
    } else {
        return squareMeters;
    }
}

QVariant FactMetaData::appSettingsAreaUnitsToSquareMeters(const QVariant &area)
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("m^2", UnitArea);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedTranslator(area);
    } else {
        return area;
    }
}

QString FactMetaData::appSettingsAreaUnitsString()
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("m^2", UnitArea);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedUnits;
    } else {
        return QStringLiteral("m^2");
    }
}

QVariant FactMetaData::gramsToAppSettingsWeightUnits(const QVariant &grams) {
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("g", UnitWeight);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->rawTranslator(grams);
    } else {
        return grams;
    }
}

QVariant FactMetaData::appSettingsWeightUnitsToGrams(const QVariant &weight) {
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("g", UnitWeight);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedTranslator(weight);
    } else {
        return weight;
    }
}

QVariant FactMetaData::metersSecondToAppSettingsSpeedUnits(const QVariant &metersSecond)
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("m/s", UnitSpeed);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->rawTranslator(metersSecond);
    } else {
        return metersSecond;
    }
}

QVariant FactMetaData::appSettingsSpeedUnitsToMetersSecond(const QVariant &speed)
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("m/s", UnitSpeed);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedTranslator(speed);
    } else {
        return speed;
    }
}

QString FactMetaData::appSettingsSpeedUnitsString()
{
    const AppSettingsTranslation_s *const pAppSettingsTranslation = m_findAppSettingsUnitsTranslation("m/s", UnitSpeed);
    if (pAppSettingsTranslation) {
        return pAppSettingsTranslation->cookedUnits;
    } else {
        return QStringLiteral("m/s");
    }
}

double FactMetaData::cookedIncrement() const
{
    return m_rawTranslator(this->rawIncrement()).toDouble();
}

int FactMetaData::decimalPlaces() const
{
    int actualDecimalPlaces = kDefaultDecimalPlaces;
    int incrementDecimalPlaces = kUnknownDecimalPlaces;

    // First determine decimal places from increment
    double increment = m_rawTranslator(this->rawIncrement()).toDouble();
    if (!qIsNaN(increment)) {
        double integralPart;

        // Get the fractional part only
        increment = fabs(modf(increment, &integralPart));
        if (increment == 0.0) {
            // No fractional part, so no decimal places
            incrementDecimalPlaces = 0;
        } else {
            incrementDecimalPlaces = -ceil(log10(increment));
        }
    }

    if (m_decimalPlaces == kUnknownDecimalPlaces) {
        if (incrementDecimalPlaces != kUnknownDecimalPlaces) {
            actualDecimalPlaces = incrementDecimalPlaces;
        } else {
            // Adjust decimal places for cooked translation
            int settingsDecimalPlaces = (m_decimalPlaces == kUnknownDecimalPlaces) ? kDefaultDecimalPlaces : m_decimalPlaces;
            const double ctest = m_rawTranslator(1.0).toDouble();

            settingsDecimalPlaces += -log10(ctest);

            settingsDecimalPlaces = qMin(25, settingsDecimalPlaces);
            settingsDecimalPlaces = qMax(0, settingsDecimalPlaces);

            actualDecimalPlaces = settingsDecimalPlaces;
        }
    } else {
        actualDecimalPlaces = m_decimalPlaces;
    }

    return actualDecimalPlaces;
}

FactMetaData *FactMetaData::createFromJsonObject(const QJsonObject &json, const QMap<QString, QString> &defineMap, QObject *metaDataParent)
{
    QString errorString;

    static const QList<JsonHelper::KeyValidateInfo> keyInfoList = {
        { m_nameJsonKey,                 QJsonValue::String, true },
        { m_typeJsonKey,                 QJsonValue::String, true },
        { m_shortDescriptionJsonKey,     QJsonValue::String, false },
        { m_longDescriptionJsonKey,      QJsonValue::String, false },
        { m_unitsJsonKey,                QJsonValue::String, false },
        { m_decimalPlacesJsonKey,        QJsonValue::Double, false },
        { m_minJsonKey,                  QJsonValue::Double, false },
        { m_maxJsonKey,                  QJsonValue::Double, false },
        { m_hasControlJsonKey,           QJsonValue::Bool,   false },
        { m_quavRebootRequiredJsonKey,    QJsonValue::Bool,   false },
        { m_rebootRequiredJsonKey,       QJsonValue::Bool,   false },
        { m_categoryJsonKey,             QJsonValue::String, false },
        { m_groupJsonKey,                QJsonValue::String, false },
        { m_volatileJsonKey,             QJsonValue::Bool,   false },
        { m_enumBitmaskArrayJsonKey,     QJsonValue::Array,  false },
        { m_enumValuesArrayJsonKey,      QJsonValue::Array,  false },
        { m_enumValuesJsonKey,           QJsonValue::String, false },
        { m_enumStringsJsonKey,          QJsonValue::String, false },
    };

    if (!JsonHelper::validateKeys(json, keyInfoList, errorString)) {
    //    qWarning() << errorString;
        return new FactMetaData(valueTypeUint32, metaDataParent);
    }

    bool unknownType;
    const FactMetaData::ValueType_t type = FactMetaData::stringToType(json[m_typeJsonKey].toString(), unknownType);
    if (unknownType) {
    //    qWarning() << "Unknown type" << json[_typeJsonKey].toString();
        return new FactMetaData(valueTypeUint32, metaDataParent);
    }

    FactMetaData *const metaData = new FactMetaData(type, metaDataParent);

    metaData->m_name = json[m_nameJsonKey].toString();

    QStringList rgDescriptions;
    QList<double> rgDoubleValues;
    QList<int> rgIntValues;
    QStringList rgStringValues;

    bool foundBitmask = false;
    if (!m_parseValuesArray(json, rgDescriptions, rgDoubleValues, errorString)) {
    //    qWarning() << QStringLiteral("FactMetaData::createFromJsonObject _parseValueDescriptionArray for '%1' failed. %2").arg(metaData->name(), errorString);
    }
    if (rgDescriptions.isEmpty()) {
        if (!m_parseBitmaskArray(json, rgDescriptions, rgIntValues, errorString)) {
        //    qWarning() << QStringLiteral("FactMetaData::createFromJsonObject _parseBitmaskArray for '%1' failed. %2").arg(metaData->name(), errorString);
        }
        foundBitmask = rgDescriptions.count() != 0;
    }
    if (rgDescriptions.isEmpty()) {
        if (!m_parseEnum(metaData->m_name, json, defineMap, rgDescriptions, rgStringValues, errorString)) {
        //    qWarning() << QStringLiteral("FactMetaData::createFromJsonObject _parseEnum for '%1' failed. %2").arg(metaData->name(), errorString);
        }
    }

    if (errorString.isEmpty() && !rgDescriptions.isEmpty()) {
        for (qsizetype i = 0; i < rgDescriptions.count(); i++) {
            if (foundBitmask) {
                metaData->addBitmaskInfo(rgDescriptions[i], 1 << rgIntValues[i]);
            } else {
                const QVariant rawValueVariant = !rgDoubleValues.isEmpty() ? QVariant(rgDoubleValues[i]) : QVariant(rgStringValues[i]);
                QVariant convertedValueVariant;
                QString errorString;
                if (metaData->convertAndValidateRaw(rawValueVariant, false /* validate */, convertedValueVariant, errorString)) {
                    metaData->addEnumInfo(rgDescriptions[i], convertedValueVariant);
                } else {
                    // qWarning() << QStringLiteral("FactMetaData::createFromJsonObject convertAndValidateRaw on enum value for %1 failed.").arg(metaData->name())
                    //                           << "type:" << metaData->type()
                    //                           << "value:" << rawValueVariant
                    //                           << "error:" << errorString;
                }
            }
        }
    }

    metaData->setDecimalPlaces(json[m_decimalPlacesJsonKey].toInt(kUnknownDecimalPlaces));
    metaData->setShortDescription(json[m_shortDescriptionJsonKey].toString());
    metaData->setLongDescription(json[m_longDescriptionJsonKey].toString());

    if (json.contains(m_unitsJsonKey)) {
        metaData->setRawUnits(json[m_unitsJsonKey].toString());
    }

    QString defaultValueJsonKey = m_defaultValueJsonKey;

    if (json.contains(defaultValueJsonKey)) {
        const QJsonValue jsonValue = json[defaultValueJsonKey];

        if ((jsonValue.type() == QJsonValue::Null) && (type == valueTypeFloat || type == valueTypeDouble)) {
            metaData->setRawDefaultValue((type == valueTypeFloat) ? std::numeric_limits<float>::quiet_NaN() : std::numeric_limits<double>::quiet_NaN());
        } else {
            QVariant typedValue;
            QString errorString;
            const QVariant initialValue = jsonValue.toVariant();

            if (metaData->convertAndValidateRaw(initialValue, true /* convertOnly */, typedValue, errorString)) {
                metaData->setRawDefaultValue(typedValue);
            } else {
                // qWarning() << "Invalid default value,"
                //                           << "name:" << metaData->name()
                //                           << "type:" << metaData->type()
                //                           << "value:" << initialValue
                //                           << "error:" << errorString;
            }
        }
    }

    if (json.contains(m_incrementJsonKey)) {
        QVariant typedValue;
        QString errorString;
        const QVariant initialValue = json[m_incrementJsonKey].toVariant();
        if (metaData->convertAndValidateRaw(initialValue, true /* convertOnly */, typedValue, errorString)) {
            metaData->setRawIncrement(typedValue.toDouble());
        } else {
            // qWarning() << "Invalid increment value,"
            //                           << "name:" << metaData->name()
            //                           << "type:" << metaData->type()
            //                           << "value:" << initialValue
            //                           << "error:" << errorString;
        }
    }

    if (json.contains(m_minJsonKey)) {
        QVariant typedValue;
        QString errorString;
        const QVariant initialValue = json[m_minJsonKey].toVariant();
        if (metaData->convertAndValidateRaw(initialValue, true /* convertOnly */, typedValue, errorString)) {
            metaData->setRawMin(typedValue);
        } else {
            // qWarning() << "Invalid min value,"
            //                           << "name:" << metaData->name()
            //                           << "type:" << metaData->type()
            //                           << "value:" << initialValue
            //                           << "error:" << errorString;
        }
    }

    if (json.contains(m_maxJsonKey)) {
        QVariant typedValue;
        QString errorString;
        const QVariant initialValue = json[m_maxJsonKey].toVariant();
        if (metaData->convertAndValidateRaw(initialValue, true /* convertOnly */, typedValue, errorString)) {
            metaData->setRawMax(typedValue);
        } else {
            // qWarning() << "Invalid max value,"
            //                           << "name:" << metaData->name()
            //                           << "type:" << metaData->type()
            //                           << "value:" << initialValue
            //                           << "error:" << errorString;
        }
    }

    bool hasControlJsonKey = true;
    if (json.contains(m_hasControlJsonKey)) {
        hasControlJsonKey = json[m_hasControlJsonKey].toBool();
    }
    metaData->setHasControl(hasControlJsonKey);

    bool quavRebootRequired = false;
    if (json.contains(m_quavRebootRequiredJsonKey)) {
        quavRebootRequired = json[m_quavRebootRequiredJsonKey].toBool();
    }
    metaData->setQUAVRebootRequired(quavRebootRequired);

    bool rebootRequired = false;
    if (json.contains(m_rebootRequiredJsonKey)) {
        rebootRequired = json[m_rebootRequiredJsonKey].toBool();
    }
    metaData->setVehicleRebootRequired(rebootRequired);

    bool volatileValue = false;
    if (json.contains(m_volatileJsonKey)) {
        volatileValue = json[m_volatileJsonKey].toBool();
    }
    metaData->setVolatileValue(volatileValue);

    if (json.contains(m_groupJsonKey)) {
        metaData->setGroup(json[m_groupJsonKey].toString());
    }

    if (json.contains(m_categoryJsonKey)) {
        metaData->setCategory(json[m_categoryJsonKey].toString());
    }

    return metaData;
}

void FactMetaData::m_loadJsonDefines(const QJsonObject &jsonDefinesObject, QMap<QString, QString> &defineMap)
{
    for (const QString &defineName: jsonDefinesObject.keys()) {
        const QString mapKey = m_jsonMetaDataDefinesName + QStringLiteral(".") + defineName;
        defineMap[mapKey] = jsonDefinesObject[defineName].toString();
    }
}

QMap<QString, FactMetaData*> FactMetaData::createMapFromJsonFile(const QString &jsonFilename, QObject *metaDataParent)
{
    QMap<QString, FactMetaData*> metaDataMap;

    QString errorString;
    int version;
    const QJsonObject jsonObject = JsonHelper::openInternalQUAVJsonFile(jsonFilename, quavFileType, 1, 1, version, errorString);
    if (!errorString.isEmpty()) {
    //    qWarning() << "Internal Error:" << errorString;
        return metaDataMap;
    }

    static const QList<JsonHelper::KeyValidateInfo> keyInfoList = {
        { FactMetaData::m_jsonMetaDataDefinesName, QJsonValue::Object, false },
        { FactMetaData::m_jsonMetaDataFactsName, QJsonValue::Array, true },
    };
    if (!JsonHelper::validateKeys(jsonObject, keyInfoList, errorString)) {
    //    qWarning() << "Json document incorrect format:" << errorString;
        return metaDataMap;
    }

    QMap<QString /* define name */, QString /* define value */> defineMap;
    m_loadJsonDefines(jsonObject[FactMetaData::m_jsonMetaDataDefinesName].toObject(), defineMap);
    const QJsonArray factArray = jsonObject[FactMetaData::m_jsonMetaDataFactsName].toArray();

    return createMapFromJsonArray(factArray, defineMap, metaDataParent);
}

QMap<QString, FactMetaData*> FactMetaData::createMapFromJsonArray(const QJsonArray &jsonArray, const QMap<QString, QString> &defineMap, QObject *metaDataParent)
{
    QMap<QString, FactMetaData*> metaDataMap;
    for (const QJsonValue &jsonValue : jsonArray) {
        if (!jsonValue.isObject()) {
        //    qWarning() << "JsonValue is not an object";
            continue;
        }

        const QJsonObject jsonObject = jsonValue.toObject();
        FactMetaData *const metaData = createFromJsonObject(jsonObject, defineMap, metaDataParent);
        if (metaDataMap.contains(metaData->name())) {
        //    qWarning() << "Duplicate fact name:" << metaData->name();
            delete metaData;
        } else {
            metaDataMap[metaData->name()] = metaData;
        }
    }

    return metaDataMap;
}

QVariant FactMetaData::cookedMax() const
{
    // We have to be careful with cooked min/max. Running the raw values through the translator could flip min and max.
    return qMax(m_rawTranslator(m_rawMax).toDouble(), m_rawTranslator(m_rawMin).toDouble());
}

QVariant FactMetaData::cookedMin() const
{
    // We have to be careful with cooked min/max. Running the raw values through the translator could flip min and max.
    return qMin(m_rawTranslator(m_rawMax).toDouble(), m_rawTranslator(m_rawMin).toDouble());
}

void FactMetaData::setVolatileValue(bool bValue)
{
    m_volatile = bValue;
    if (m_volatile) {
        m_readOnly = true;
    }
}

QStringList FactMetaData::splitTranslatedList(const QString &translatedList)
{
    const QRegularExpression splitRegex("[,，、]"); // Note chinese commas for translations which have modified the english comma
    QStringList valueList = translatedList.split(splitRegex, Qt::SkipEmptyParts);
    for (QString &value: valueList) {
        value = value.trimmed();
    }
    return valueList;
}

bool FactMetaData::m_parseEnum(const QString& name, const QJsonObject &jsonObject, const DefineMap_t &defineMap, QStringList &rgDescriptions, QStringList &rgValues, QString &errorString)
{
    rgDescriptions.clear();
    rgValues.clear();
    errorString.clear();

    if (!jsonObject.contains(m_enumStringsJsonKey)) {
        return true;
    }

    const QString jsonStrings = jsonObject.value(m_enumStringsJsonKey).toString();
    const QString defineMapStrings = defineMap.value(jsonStrings, jsonStrings);
    rgDescriptions = splitTranslatedList(defineMapStrings);

    const QString jsonValues = jsonObject.value(m_enumValuesJsonKey).toString();
    const QString defineMapValues = defineMap.value(jsonValues, jsonValues);
    rgValues = splitTranslatedList(defineMapValues); // Never translated but still useful to use common string splitting code

    if (rgDescriptions.count() != rgValues.count()) {
        errorString = QStringLiteral("Enum strings/values count mismatch - name: '%1' strings: '%2'[%3] values: '%4'[%5]").arg(name).arg(defineMapStrings).arg(rgDescriptions.count()).arg(defineMapValues).arg(rgValues.count());
        return false;
    }

    return true;
}

bool FactMetaData::m_parseValuesArray(const QJsonObject &jsonObject, QStringList &rgDescriptions, QList<double> &rgValues, QString &errorString)
{
    rgDescriptions.clear();
    rgValues.clear();
    errorString.clear();

    if (!jsonObject.contains(m_enumValuesArrayJsonKey)) {
        return true;
    }

    static const QList<JsonHelper::KeyValidateInfo> keyInfoList = {
        { m_enumValuesArrayDescriptionJsonKey, QJsonValue::String, true },
        { m_enumValuesArrayValueJsonKey, QJsonValue::Double, true },
    };

    const QJsonArray &rgValueDescription = jsonObject[m_enumValuesArrayJsonKey].toArray();
    for (const QJsonValue& jsonValue : rgValueDescription) {
        if (jsonValue.type() != QJsonValue::Object) {
            errorString = QStringLiteral("Value in \"values\" array is not an object.");
            return false;
        }

        const QJsonObject &valueDescriptionObject = jsonValue.toObject();
        if (!JsonHelper::validateKeys(valueDescriptionObject, keyInfoList, errorString)) {
            errorString = QStringLiteral("Object in \"values\" array failed validation '%2'.").arg(errorString);
            return false;
        }

        rgDescriptions.append(valueDescriptionObject[m_enumValuesArrayDescriptionJsonKey].toString());
        rgValues.append(valueDescriptionObject[m_enumValuesArrayValueJsonKey].toDouble());
    }

    return true;
}

bool FactMetaData::m_parseBitmaskArray(const QJsonObject &jsonObject, QStringList &rgDescriptions, QList<int> &rgValues, QString &errorString)
{
    rgDescriptions.clear();
    rgValues.clear();
    errorString.clear();

    if (!jsonObject.contains(m_enumBitmaskArrayJsonKey)) {
        return true;
    }

    static const QList<JsonHelper::KeyValidateInfo> keyInfoList = {
        { m_enumBitmaskArrayDescriptionJsonKey, QJsonValue::String, true },
        { m_enumBitmaskArrayIndexJsonKey, QJsonValue::Double, true },
    };

    const QJsonArray &rgValueDescription = jsonObject[m_enumBitmaskArrayJsonKey].toArray();
    for (const QJsonValue &jsonValue : rgValueDescription) {
        if (jsonValue.type() != QJsonValue::Object) {
            errorString = QStringLiteral("Value in \"values\" array is not an object.");
            return false;
        }

        const QJsonObject &valueDescriptionObject = jsonValue.toObject();
        if (!JsonHelper::validateKeys(valueDescriptionObject, keyInfoList, errorString)) {
            errorString = QStringLiteral("Object in \"values\" array failed validation '%2'.").arg(errorString);
            return false;
        }

        rgDescriptions.append(valueDescriptionObject[m_enumBitmaskArrayDescriptionJsonKey].toString());
        rgValues.append(valueDescriptionObject[m_enumBitmaskArrayIndexJsonKey].toInt());
    }

    return true;
}
