#pragma once

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

/// Holds the meta data associated with a Fact. This is kept in a separate object from the Fact itself
/// since you may have multiple instances of the same Fact. But there is only ever one FactMetaData
/// instance or each Fact.
class FactMetaData : public QObject
{
    Q_OBJECT

public:
    enum ValueType_t {
        valueTypeUint8,
        valueTypeInt8,
        valueTypeUint16,
        valueTypeInt16,
        valueTypeUint32,
        valueTypeInt32,
        valueTypeUint64,
        valueTypeInt64,
        valueTypeFloat,
        valueTypeDouble,
        valueTypeString,
        valueTypeBool,
        valueTypeElapsedTimeInSeconds,  // Internally stored as double, valueString displays as HH:MM:SS
        valueTypeCustom,                // Internally stored as a QByteArray
    };
    Q_ENUM(ValueType_t)

    typedef QVariant (*Translator)(const QVariant &from);

    // Custom function to validate a cooked value.
    //  @return Error string for failed validation explanation to user. Empty string indicates no error.
    typedef QString (*CustomCookedValidator)(const QVariant &cookedValue);

    typedef QMap<QString /* param Name */, FactMetaData*> NameToMetaDataMap_t;

    explicit FactMetaData(QObject *parent = nullptr);
    explicit FactMetaData(ValueType_t type, QObject *parent = nullptr);
    explicit FactMetaData(ValueType_t type, const QString &name, QObject *parent = nullptr);
    explicit FactMetaData(const FactMetaData &other, QObject *parent = nullptr);
    ~FactMetaData();

    typedef QMap<QString, QString> DefineMap_t;

    static QMap<QString, FactMetaData*> createMapFromJsonFile(const QString &jsonFilename, QObject *metaDataParent);
    static QMap<QString, FactMetaData*> createMapFromJsonArray(const QJsonArray &jsonArray, const DefineMap_t &defineMap, QObject *metaDataParent);

    static FactMetaData *createFromJsonObject(const QJsonObject &json, const QMap<QString, QString> &defineMap, QObject *metaDataParent);

    const FactMetaData &operator=(const FactMetaData &other);

    /// Converts from meters to the user specified horizontal distance unit
    static QVariant metersToAppSettingsHorizontalDistanceUnits(const QVariant &meters);

    /// Converts from user specified horizontal distance unit to meters
    static QVariant appSettingsHorizontalDistanceUnitsToMeters(const QVariant &distance);

    /// Returns the string for horizontal distance units which has configued by user
    static QString appSettingsHorizontalDistanceUnitsString();

    /// Converts from meters to the user specified vertical distance unit
    static QVariant metersToAppSettingsVerticalDistanceUnits(const QVariant &meters);

    /// Converts from user specified vertical distance unit to meters
    static QVariant appSettingsVerticalDistanceUnitsToMeters(const QVariant &distance);

    /// Returns the string for vertical distance units which has configued by user
    static QString appSettingsVerticalDistanceUnitsString();

    /// Converts from grams to the user specified weight unit
    static QVariant gramsToAppSettingsWeightUnits(const QVariant &grams);

    /// Converts from user specified weight unit to grams
    static QVariant appSettingsWeightUnitsToGrams(const QVariant &weight);

    /// Returns the string for weight units which has configued by user
    static QString appSettingsWeightUnitsString();

    /// Converts from meters to the user specified distance unit
    static QVariant squareMetersToAppSettingsAreaUnits(const QVariant &squareMeters);

    /// Converts from user specified distance unit to meters
    static QVariant appSettingsAreaUnitsToSquareMeters(const QVariant &area);

    /// Returns the string for distance units which has configued by user
    static QString appSettingsAreaUnitsString();

    /// Converts from meters/second to the user specified speed unit
    static QVariant metersSecondToAppSettingsSpeedUnits(const QVariant &metersSecond);

    /// Converts from user specified speed unit to meters/second
    static QVariant appSettingsSpeedUnitsToMetersSecond(const QVariant &speed);

    /// Returns the string for speed units which has configued by user
    static QString appSettingsSpeedUnitsString();

    static const QString defaultCategory() { return QString(kDefaultCategory); }
    static const QString defaultGroup() { return QString(kDefaultGroup); }

    // Splits a comma separated list of strings into a QStringList. Taking into account the possibility that
    // the commas may have been translated to other characters such as chinese commas.
    static QStringList splitTranslatedList(const QString &translatedList);

    int decimalPlaces() const;
    QVariant rawDefaultValue() const;
    QVariant cookedDefaultValue() const { return m_rawTranslator(rawDefaultValue()); }
    bool defaultValueAvailable() const { return m_defaultValueAvailable; }
    QStringList bitmaskStrings() const { return m_bitmaskStrings; }
    QVariantList bitmaskValues() const { return m_bitmaskValues; }
    QStringList enumStrings() const { return m_enumStrings; }
    QVariantList enumValues() const { return m_enumValues; }
    QString category() const { return m_category; }
    QString group() const { return m_group; }
    QString longDescription() const { return m_longDescription;}
    QVariant rawMax() const { return m_rawMax; }
    QVariant cookedMax() const;
    bool maxIsDefaultForType() const { return (m_rawMax == m_maxForType()); }
    QVariant rawMin() const { return m_rawMin; }
    QVariant cookedMin() const;
    bool minIsDefaultForType() const { return (m_rawMin == m_minForType()); }
    QString name() const { return m_name; }
    QString shortDescription() const { return m_shortDescription; }
    ValueType_t type() const { return m_type; }
    QString rawUnits() const { return m_rawUnits; }
    QString cookedUnits() const { return m_cookedUnits; }
    bool vehicleRebootRequired() const { return m_vehicleRebootRequired; }
    bool quavRebootRequired() const { return m_quavRebootRequired; }
    bool hasControl() const { return m_hasControl; }
    bool readOnly() const { return m_readOnly; }
    bool writeOnly() const { return m_writeOnly; }
    bool volatileValue() const { return m_volatile; }

    /// Amount to increment value when used in controls such as spin button or slider with detents.
    /// NaN for no increment available.
    double rawIncrement() const { return m_rawIncrement; }
    double cookedIncrement() const;

    Translator rawTranslator() const { return m_rawTranslator; }
    Translator cookedTranslator() const { return m_cookedTranslator; }

    /// Used to add new values to the bitmask lists after the meta data has been loaded
    void addBitmaskInfo(const QString &name, const QVariant &value);

    /// Used to add new values to the enum lists after the meta data has been loaded
    void addEnumInfo(const QString &name, const QVariant &value);

    /// Used to remove values from the enum lists after the meta data has been loaded
    void removeEnumInfo(const QVariant &value);

    void setDecimalPlaces(int decimalPlaces) { m_decimalPlaces = decimalPlaces; }
    void setRawDefaultValue(const QVariant &rawDefaultValue);
    void setBitmaskInfo(const QStringList &strings, const QVariantList &values);
    void setEnumInfo(const QStringList &strings, const QVariantList &values);
    void setCategory(const QString &category) { m_category = category; }
    void setGroup(const QString &group) { m_group = group; }
    void setLongDescription(const QString &longDescription) { m_longDescription = longDescription;}
    void setRawMax(const QVariant &rawMax);
    void setRawMin(const QVariant &rawMin);
    void setName(const QString &name) { m_name = name; }
    void setShortDescription(const QString &shortDescription) { m_shortDescription = shortDescription; }
    void setRawUnits(const QString &rawUnits);
    void setVehicleRebootRequired(bool rebootRequired) { m_vehicleRebootRequired = rebootRequired; }
    void setQUAVRebootRequired(bool rebootRequired) { m_quavRebootRequired = rebootRequired; }
    void setRawIncrement(double increment) { m_rawIncrement = increment; }
    void setHasControl(bool bValue) { m_hasControl = bValue; }
    void setReadOnly(bool bValue) { m_readOnly = bValue; }
    void setWriteOnly(bool bValue) { m_writeOnly = bValue; }
    void setVolatileValue(bool bValue);

    void setTranslators(Translator rawTranslator, Translator cookedTranslator);

    /// Set the translators to the standard built in versions
    void setBuiltInTranslator();

    /// Converts the specified raw value, validating against meta data
    ///     @param rawValue: Value to convert, can be string
    ///     @param convertOnly: true: convert to correct type only, do not validate against meta data
    ///     @param typeValue: Converted value, correctly typed
    ///     @param errorString: Error string if convert fails, values are cooked values since user visible
    /// @returns false: Convert failed, errorString set
    bool convertAndValidateRaw(const QVariant &rawValue, bool convertOnly, QVariant &typedValue, QString &errorString) const;

    /// Same as convertAndValidateRaw except for cookedValue input
    bool convertAndValidateCooked(const QVariant &cookedValue, bool convertOnly, QVariant &typedValue, QString &errorString) const;

    /// Converts the specified cooked value and clamps it (max/min)
    ///     @param cookedValue: Value to convert, can be string
    ///     @param typeValue: Converted value, correctly typed and clamped
    /// @returns false: Convertion failed
    bool clampValue(const QVariant &cookedValue, QVariant &typedValue) const;

    /// Sets a custom cooked validator function for this metadata. The custom validator will be called
    /// prior to the standard validator when convertAndValidateCooked is called.
    void setCustomCookedValidator(CustomCookedValidator customValidator) { m_customCookedValidator = customValidator; }

    static constexpr int kDefaultDecimalPlaces = 3;  ///< Default value for decimal places if not specified/known
    static constexpr int kUnknownDecimalPlaces = -1; ///< Number of decimal places to specify is not known

    static ValueType_t stringToType(const QString &typeString, bool &unknownType);
    static QString typeToString(ValueType_t type);
    static size_t typeToSize(ValueType_t type);

    static QVariant minForType(ValueType_t type);
    static QVariant maxForType(ValueType_t type);

    static constexpr const char* kDefaultCategory = QT_TRANSLATE_NOOP("FactMetaData", "Other");
    static constexpr const char* kDefaultGroup = QT_TRANSLATE_NOOP("FactMetaData", "Misc");
    static constexpr const char* quavFileType = "FactMetaData";

private:
    QVariant m_minForType() const { return minForType(m_type); };
    QVariant m_maxForType() const { return maxForType(m_type); };
    /// Set translators according to app settings
    void m_setAppSettingsTranslators();

    /// Clamp a value to be within cookedMin and cookedMax
    template<class T>
    void clamp(QVariant& variantValue) const {
        if (cookedMin().value<T>() > variantValue.value<T>()) {
            variantValue = cookedMin();
        } else if(variantValue.value<T>() > cookedMax().value<T>()) {
            variantValue = cookedMax();
        }
    }

    template<class T>
    bool isInCookedLimit(const QVariant &variantValue) const {
        return ((cookedMin().value<T>() <= variantValue.value<T>()) && (variantValue.value<T>() <= cookedMax().value<T>()));
    }

    template<class T>
    bool isInRawLimit(const QVariant &variantValue) const {
        return ((rawMin().value<T>() <= variantValue.value<T>()) && (variantValue.value<T>() <= rawMax().value<T>()));
    }

    bool isInRawMinLimit(const QVariant &variantValue) const;
    bool isInRawMaxLimit(const QVariant &variantValue) const;

    static bool m_parseEnum(const QString& name, const QJsonObject &jsonObject, const DefineMap_t &defineMap, QStringList &rgDescriptions, QStringList &rgValues, QString &errorString);
    static bool m_parseValuesArray(const QJsonObject &jsonObject, QStringList &rgDescriptions, QList<double> &rgValues, QString &errorString);
    static bool m_parseBitmaskArray(const QJsonObject &jsonObject, QStringList &rgDescriptions, QList<int> &rgValues, QString &errorString);

    // Built in translators
    static QVariant m_defaultTranslator(const QVariant &from) { return from; }
    static QVariant m_degreesToRadians(const QVariant &degrees);
    static QVariant m_radiansToDegrees(const QVariant &radians);
    static QVariant m_centiDegreesToDegrees(const QVariant &centiDegrees);
    static QVariant m_degreesToCentiDegrees(const QVariant &degrees);
    static QVariant m_userGimbalDegreesToMavlinkGimbalDegrees(const QVariant &userGimbalDegrees);
    static QVariant m_mavlinkGimbalDegreesToUserGimbalDegrees(const QVariant &mavlinkGimbalDegrees);
    static QVariant m_metersToFeet(const QVariant &meters);
    static QVariant m_feetToMeters(const QVariant &feet);
    static QVariant m_squareMetersToSquareKilometers(const QVariant &squareMeters);
    static QVariant m_squareKilometersToSquareMeters(const QVariant &squareKilometers);
    static QVariant m_squareMetersToHectares(const QVariant &squareMeters);
    static QVariant m_hectaresToSquareMeters(const QVariant &hectares);
    static QVariant m_squareMetersToSquareFeet(const QVariant &squareMeters);
    static QVariant m_squareFeetToSquareMeters(const QVariant &squareFeet);
    static QVariant m_squareMetersToAcres(const QVariant &squareMeters);
    static QVariant m_acresToSquareMeters(const QVariant &acres);
    static QVariant m_squareMetersToSquareMiles(const QVariant &squareMeters);
    static QVariant m_squareMilesToSquareMeters(const QVariant &squareMiles);
    static QVariant m_metersPerSecondToMilesPerHour(const QVariant &metersPerSecond);
    static QVariant m_milesPerHourToMetersPerSecond(const QVariant &milesPerHour);
    static QVariant m_metersPerSecondToKilometersPerHour(const QVariant &metersPerSecond);
    static QVariant m_kilometersPerHourToMetersPerSecond(const QVariant &kilometersPerHour);
    static QVariant m_metersPerSecondToKnots(const QVariant &metersPerSecond);
    static QVariant m_knotsToMetersPerSecond(const QVariant &knots);
    static QVariant m_percentToNorm(const QVariant &percent);
    static QVariant m_normToPercent(const QVariant &normalized);
    static QVariant m_centimetersToInches(const QVariant &centimeters);
    static QVariant m_inchesToCentimeters(const QVariant &inches);
    static QVariant m_celsiusToFarenheit(const QVariant &celsius);
    static QVariant m_farenheitToCelsius(const QVariant &farenheit);
    static QVariant m_kilogramsToGrams(const QVariant &kg);
    static QVariant m_ouncesToGrams(const QVariant &oz);
    static QVariant m_poundsToGrams(const QVariant &lbs);
    static QVariant m_gramsToKilograms(const QVariant &g);
    static QVariant m_gramsToOunces(const QVariant &g);
    static QVariant m_gramsToPunds(const QVariant &g);

    enum UnitTypes {
        UnitHorizontalDistance = 0,
        UnitVerticalDistance,
        UnitArea,
        UnitSpeed,
        UnitTemperature,
        UnitWeight
    };

    struct AppSettingsTranslation_s {
        QString rawUnits;
        const char *cookedUnits = nullptr;
        UnitTypes unitType = UnitHorizontalDistance;
        uint32_t unitOption = 0;
        Translator rawTranslator;
        Translator cookedTranslator;
    };

    static const AppSettingsTranslation_s* m_findAppSettingsUnitsTranslation(const QString &rawUnits, UnitTypes type);

    static void m_loadJsonDefines(const QJsonObject &jsonDefinesObject, QMap<QString, QString> &defineMap);

    ValueType_t m_type = valueTypeInt32; // must be first for correct constructor init
    int m_decimalPlaces = kUnknownDecimalPlaces;
    QVariant m_rawDefaultValue = 0;
    bool m_defaultValueAvailable = false;
    QStringList m_bitmaskStrings;
    QVariantList m_bitmaskValues;
    QStringList m_enumStrings;
    QVariantList m_enumValues;
    QString m_category = kDefaultCategory;
    QString m_group = kDefaultGroup;
    QString m_longDescription;
    QVariant m_rawMax = m_maxForType();
    QVariant m_rawMin = m_minForType();
    QString m_name;
    QString m_shortDescription;
    QString m_rawUnits;
    QString m_cookedUnits;
    Translator m_rawTranslator = m_defaultTranslator;
    Translator m_cookedTranslator = m_defaultTranslator;
    bool m_vehicleRebootRequired = false;
    bool m_quavRebootRequired = false;
    double m_rawIncrement = std::numeric_limits<double>::quiet_NaN();
    bool m_hasControl = true;
    bool m_readOnly = false;
    bool m_writeOnly = false;
    bool m_volatile = false;
    CustomCookedValidator m_customCookedValidator = nullptr;

    // Exact conversion constants
    static constexpr struct UnitConsts_s {
        static constexpr const qreal secondsPerHour = 3600.0;
        static constexpr const qreal knotsToKPH = 1.852;
        static constexpr const qreal milesToMeters = 1609.344;
        static constexpr const qreal feetToMeters = 0.3048;
        static constexpr const qreal inchesToCentimeters = 2.54;
        static constexpr const qreal ouncesToGrams = 28.3495;
        static constexpr const qreal poundsToGrams = 453.592;
        static constexpr const qreal acresToSquareMeters = 4046.86;
        static constexpr const qreal squareMetersToAcres = 0.000247105;
        static constexpr const qreal feetToSquareMeters = 0.0929;
        static constexpr const qreal squareMetersToSquareFeet = 10.7639;
        static constexpr const qreal squareMetersToSquareMiles = 3.86102e-7;
        static constexpr const qreal squareMilesToSquareMeters = 2589988.11;
    } constants{};

    struct BuiltInTranslation_s {
        QString rawUnits;
        const char *cookedUnits;
        Translator rawTranslator;
        Translator cookedTranslator;
    };

    static const BuiltInTranslation_s m_rgBuiltInTranslations[];
    static const AppSettingsTranslation_s m_rgAppSettingsTranslations[];

    static constexpr const char* m_jsonMetaDataDefinesName = "QGC.MetaData.Defines";
    static constexpr const char* m_jsonMetaDataFactsName = "QGC.MetaData.Facts";
    static constexpr const char* m_enumStringsJsonKey = "enumStrings";
    static constexpr const char* m_enumValuesJsonKey = "enumValues";

    // This is the newer json format for enums and bitmasks. They are used by the new COMPONENT_METADATA parameter metadata for example.
    static constexpr const char* m_enumValuesArrayJsonKey = "values";
    static constexpr const char* m_enumBitmaskArrayJsonKey = "bitmask";
    static constexpr const char* m_enumValuesArrayValueJsonKey = "value";
    static constexpr const char* m_enumValuesArrayDescriptionJsonKey = "description";
    static constexpr const char* m_enumBitmaskArrayIndexJsonKey = "index";
    static constexpr const char* m_enumBitmaskArrayDescriptionJsonKey = "description";

    static constexpr const char* m_rgKnownTypeStrings[] = {
        "Uint8",
        "Int8",
        "Uint16",
        "Int16",
        "Uint32",
        "Int32",
        "Uint64",
        "Int64",
        "Float",
        "Double",
        "String",
        "Bool",
        "ElapsedSeconds",
        "Custom",
    };

    static constexpr const ValueType_t m_rgKnownValueTypes[] = {
        valueTypeUint8,
        valueTypeInt8,
        valueTypeUint16,
        valueTypeInt16,
        valueTypeUint32,
        valueTypeInt32,
        valueTypeUint64,
        valueTypeInt64,
        valueTypeFloat,
        valueTypeDouble,
        valueTypeString,
        valueTypeBool,
        valueTypeElapsedTimeInSeconds,
        valueTypeCustom,
    };

    static constexpr const char* m_decimalPlacesJsonKey = "decimalPlaces";
    static constexpr const char* m_nameJsonKey = "name";
    static constexpr const char* m_typeJsonKey = "type";
    static constexpr const char* m_shortDescriptionJsonKey = "shortDesc";
    static constexpr const char* m_longDescriptionJsonKey = "longDesc";
    static constexpr const char* m_unitsJsonKey = "units";
    static constexpr const char* m_defaultValueJsonKey = "default";
    static constexpr const char* m_mobileDefaultValueJsonKey = "mobileDefault";
    static constexpr const char* m_minJsonKey = "min";
    static constexpr const char* m_maxJsonKey = "max";
    static constexpr const char* m_incrementJsonKey = "increment";
    static constexpr const char* m_hasControlJsonKey = "control";
    static constexpr const char* m_quavRebootRequiredJsonKey = "qgcRebootRequired";
    static constexpr const char* m_rebootRequiredJsonKey = "rebootRequired";
    static constexpr const char* m_categoryJsonKey = "category";
    static constexpr const char* m_groupJsonKey = "group";
    static constexpr const char* m_volatileJsonKey = "volatile";
};
