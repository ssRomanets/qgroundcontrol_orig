
#include "JsonHelper.h"
#include "FactMetaData.h"
#include "MissionCommandList.h"

#include <QtCore/qapplicationstatic.h>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonParseError>
#include <QtCore/QObject>
#include <QtCore/QTranslator>

namespace JsonHelper
{
    QString m_jsonValueTypeToString(QJsonValue::Type type);
    bool m_loadGeoCoordinate(const QJsonValue &jsonValue, bool altitudeRequired, QGeoCoordinate &coordinate, QString &errorString, bool geoJsonFormat);
    void m_saveGeoCoordinate(const QGeoCoordinate &coordinate, bool writeAltitude, QJsonValue &jsonValue, bool geoJsonFormat);
    QStringList m_addDefaultLocKeys(QJsonObject &jsonObject);
    QJsonObject m_translateRoot(QJsonObject &jsonObject, const QString &translateContext, const QStringList &translateKeys);
    QJsonObject m_translateObject(QJsonObject &jsonObject, const QString &translateContext, const QStringList &translateKeys);
    QJsonArray m_translateArray(QJsonArray &jsonArray, const QString &translateContext, const QStringList &translateKeys);

    constexpr const char* m_translateKeysKey = "translateKeys";
    constexpr const char* m_arrayIDKeysKey = "_arrayIDKeys";
    constexpr const char* m_jsonGroundStationKey = "groundStation";
    constexpr const char* m_jsonGroundStationValue = "QGroundControl";
}

Q_APPLICATION_STATIC(QTranslator, s_jsonTranslator);

QTranslator *JsonHelper::translator()
{
    return s_jsonTranslator();
}

bool JsonHelper::validateRequiredKeys(const QJsonObject &jsonObject, const QStringList &keys, QString &errorString)
{
    QString missingKeys;

    for (const QString &key : keys) {
        if (!jsonObject.contains(key)) {
            if (!missingKeys.isEmpty()) {
                missingKeys += QStringLiteral(", ");
            }
            missingKeys += key;
        }
    }

    if (!missingKeys.isEmpty()) {
        errorString = QObject::tr("The following required keys are missing: %1").arg(missingKeys);
        return false;
    }

    return true;
}

bool JsonHelper::m_loadGeoCoordinate(const QJsonValue &jsonValue, bool altitudeRequired, QGeoCoordinate &coordinate, QString &errorString, bool geoJsonFormat)
{
    if (!jsonValue.isArray()) {
        errorString = QObject::tr("value for coordinate is not array");
        return false;
    }

    const QJsonArray coordinateArray = jsonValue.toArray();
    const int requiredCount = altitudeRequired ? 3 : 2;
    if (coordinateArray.count() != requiredCount) {
        errorString = QObject::tr("Coordinate array must contain %1 values").arg(requiredCount);
        return false;
    }

    for (const QJsonValue &jsonValue: coordinateArray) {
        if ((jsonValue.type() != QJsonValue::Double) && (jsonValue.type() != QJsonValue::Null)) {
            errorString = QObject::tr("Coordinate array may only contain double values, found: %1").arg(jsonValue.type());
            return false;
        }
    }

    if (geoJsonFormat) {
        coordinate = QGeoCoordinate(coordinateArray[1].toDouble(), coordinateArray[0].toDouble());
    } else {
        coordinate = QGeoCoordinate(possibleNaNJsonValue(coordinateArray[0]), possibleNaNJsonValue(coordinateArray[1]));
    }

    if (altitudeRequired) {
        coordinate.setAltitude(possibleNaNJsonValue(coordinateArray[2]));
    }

    return true;
}

void JsonHelper::m_saveGeoCoordinate(const QGeoCoordinate &coordinate, bool writeAltitude, QJsonValue &jsonValue, bool geoJsonFormat)
{
    QJsonArray coordinateArray;

    if (geoJsonFormat) {
        coordinateArray << coordinate.longitude() << coordinate.latitude();
    } else {
        coordinateArray << coordinate.latitude() << coordinate.longitude();
    }

    if (writeAltitude) {
        coordinateArray << coordinate.altitude();
    }

    jsonValue = QJsonValue(coordinateArray);
}

bool JsonHelper::loadGeoCoordinate(const QJsonValue &jsonValue, bool altitudeRequired, QGeoCoordinate &coordinate, QString &errorString, bool geoJsonFormat)
{
    return m_loadGeoCoordinate(jsonValue, altitudeRequired, coordinate, errorString, geoJsonFormat);
}

void JsonHelper::saveGeoCoordinate(const QGeoCoordinate &coordinate, bool writeAltitude, QJsonValue &jsonValue)
{
    m_saveGeoCoordinate(coordinate, writeAltitude, jsonValue, false /* geoJsonFormat */);
}

bool JsonHelper::loadGeoJsonCoordinate(const QJsonValue &jsonValue, bool altitudeRequired, QGeoCoordinate &coordinate, QString &errorString)
{
    return m_loadGeoCoordinate(jsonValue, altitudeRequired, coordinate, errorString, true /* geoJsonFormat */);
}

void JsonHelper::saveGeoJsonCoordinate(const QGeoCoordinate &coordinate, bool writeAltitude, QJsonValue &jsonValue)
{
    m_saveGeoCoordinate(coordinate, writeAltitude, jsonValue, true /* geoJsonFormat */);
}

bool JsonHelper::validateKeyTypes(const QJsonObject &jsonObject, const QStringList &keys, const QList<QJsonValue::Type> &types, QString & errorString)
{
    for (qsizetype i = 0; i < types.count(); i++) {
        const QString valueKey = keys[i];
        if (jsonObject.contains(valueKey)) {
            const QJsonValue &jsonValue = jsonObject[valueKey];
            if ((jsonValue.type() == QJsonValue::Null) && (types[i] == QJsonValue::Double)) {
                // Null type signals a NaN on a double value
                continue;
            }
            if (jsonValue.type() != types[i]) {
                errorString  = QObject::tr("Incorrect value type - key:type:expected %1:%2:%3").arg(valueKey, m_jsonValueTypeToString(jsonValue.type()), m_jsonValueTypeToString(types[i]));
                return false;
            }
        }
    }

    return true;
}

bool JsonHelper::isJsonFile(const QByteArray &bytes, QJsonDocument &jsonDoc, QString &errorString)
{
    QJsonParseError parseError;
    jsonDoc = QJsonDocument::fromJson(bytes, &parseError);

    if (parseError.error == QJsonParseError::NoError) {
        return true;
    }

    const int startPos = qMax(0, parseError.offset - 100);
    const int length = qMin(bytes.length() - startPos, 200);
    qDebug() << "Json read error" << bytes.mid(startPos, length).constData();
    errorString = parseError.errorString();

    return false;
}

bool JsonHelper::isJsonFile(const QString &fileName, QJsonDocument &jsonDoc, QString &errorString)
{
    QFile jsonFile(fileName);
    if (!jsonFile.open(QFile::ReadOnly)) {
        errorString = QObject::tr("File open failed: file:error %1 %2").arg(jsonFile.fileName(), jsonFile.errorString());
        return false;
    }

    const QByteArray jsonBytes = jsonFile.readAll();
    jsonFile.close();

    return isJsonFile(jsonBytes, jsonDoc, errorString);
}

bool JsonHelper::validateInternalQUAVJsonFile(const QJsonObject &jsonObject, const QString &expectedFileType, int minSupportedVersion, int maxSupportedVersion, int &version, QString &errorString)
{
    static const QList<JsonHelper::KeyValidateInfo> requiredKeys = {
        { jsonFileTypeKey, QJsonValue::String, true },
        { jsonVersionKey, QJsonValue::Double, true },
    };

    if (!JsonHelper::validateKeys(jsonObject, requiredKeys, errorString)) {
        return false;
    }

    const QString fileTypeValue = jsonObject[jsonFileTypeKey].toString();
    if (fileTypeValue != expectedFileType) {
        errorString = QObject::tr("Incorrect file type key expected:%1 actual:%2").arg(expectedFileType, fileTypeValue);
        return false;
    }

    version = jsonObject[jsonVersionKey].toInt();
    if (version < minSupportedVersion) {
        errorString = QObject::tr("File version %1 is no longer supported").arg(version);
        return false;
    }

    if (version > maxSupportedVersion) {
        errorString = QObject::tr("File version %1 is newer than current supported version %2").arg(version).arg(maxSupportedVersion);
        return false;
    }

    return true;
}

bool JsonHelper::validateExternalQUAVJsonFile(const QJsonObject &jsonObject, const QString &expectedFileType, int minSupportedVersion, int maxSupportedVersion, int &version, QString &errorString)
{
    static const QList<JsonHelper::KeyValidateInfo> requiredKeys = {
        { m_jsonGroundStationKey, QJsonValue::String, true },
    };

    if (!JsonHelper::validateKeys(jsonObject, requiredKeys, errorString)) {
        return false;
    }

    return validateInternalQUAVJsonFile(jsonObject, expectedFileType, minSupportedVersion, maxSupportedVersion, version, errorString);
}

QStringList JsonHelper::m_addDefaultLocKeys(QJsonObject &jsonObject)
{
    QString translateKeys;
    const QString fileType = jsonObject[jsonFileTypeKey].toString();
    if (!fileType.isEmpty()) {
        if (fileType == MissionCommandList::quavFileType) {
            if (jsonObject.contains(m_translateKeysKey)) {
                translateKeys = jsonObject[m_translateKeysKey].toString();
            } else {
                translateKeys = QStringLiteral("label,enumStrings,friendlyName,description,category");
                jsonObject[m_translateKeysKey] = translateKeys;
            }

            if (!jsonObject.contains(m_arrayIDKeysKey)) {
                jsonObject[m_arrayIDKeysKey] = QStringLiteral("rawName,comment");
            }
        } else if (fileType == FactMetaData::quavFileType) {
            if (jsonObject.contains(m_translateKeysKey)) {
                translateKeys = jsonObject[m_translateKeysKey].toString();
            } else {
                translateKeys = QStringLiteral("shortDescription,longDescription,enumStrings");
                jsonObject[m_translateKeysKey] = QStringLiteral("shortDescription,longDescription,enumStrings");
            }

            if (!jsonObject.contains(m_arrayIDKeysKey)) {
                jsonObject[m_arrayIDKeysKey] = QStringLiteral("name");
            }
        }
    }

    return translateKeys.split(",");
}

QJsonObject JsonHelper::m_translateObject(QJsonObject &jsonObject, const QString &translateContext, const QStringList &translateKeys)
{
    for (const QString &key: jsonObject.keys()) {
        if (jsonObject[key].isString()) {
            QString locString = jsonObject[key].toString();
            if (!translateKeys.contains(key)) {
                continue;
            }

            QString disambiguation;
            QString disambiguationPrefix("#loc.disambiguation#");

            if (locString.startsWith(disambiguationPrefix)) {
                locString = locString.right(locString.length() - disambiguationPrefix.length());
                const int commentEndIndex = locString.indexOf("#");
                if (commentEndIndex != -1) {
                    disambiguation = locString.left(commentEndIndex);
                    locString = locString.right(locString.length() - disambiguation.length() - 1);
                }
            }

            const QString xlatString = translator()->translate(translateContext.toUtf8().constData(), locString.toUtf8().constData(), disambiguation.toUtf8().constData());
            if (!xlatString.isNull()) {
                jsonObject[key] = xlatString;
            }
        } else if (jsonObject[key].isArray()) {
            QJsonArray childJsonArray = jsonObject[key].toArray();
            jsonObject[key] = m_translateArray(childJsonArray, translateContext, translateKeys);
        } else if (jsonObject[key].isObject()) {
            QJsonObject childJsonObject = jsonObject[key].toObject();
            jsonObject[key] = m_translateObject(childJsonObject, translateContext, translateKeys);
        }
    }

    return jsonObject;
}

QJsonArray JsonHelper::m_translateArray(QJsonArray &jsonArray, const QString &translateContext, const QStringList &translateKeys)
{
    for (qsizetype i = 0; i < jsonArray.count(); i++) {
        QJsonObject childJsonObject = jsonArray[i].toObject();
        jsonArray[i] = m_translateObject(childJsonObject, translateContext, translateKeys);
    }

    return jsonArray;
}

QJsonObject JsonHelper::m_translateRoot(QJsonObject &jsonObject, const QString &translateContext, const QStringList &translateKeys)
{
    return m_translateObject(jsonObject, translateContext, translateKeys);
}

QJsonObject JsonHelper::openInternalQUAVJsonFile(const QString &jsonFilename, const QString &expectedFileType, int minSupportedVersion, int maxSupportedVersion, int &version, QString &errorString)
{
    QFile jsonFile(jsonFilename);
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorString = QObject::tr("Unable to open file: '%1', error: %2").arg(jsonFilename, jsonFile.errorString());
        return QJsonObject();
    }

    const QByteArray bytes = jsonFile.readAll();
    jsonFile.close();
    QJsonParseError jsonParseError;
    const QJsonDocument doc = QJsonDocument::fromJson(bytes, &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError) {
        errorString = QObject::tr("Unable to parse json file: %1 error: %2 offset: %3").arg(jsonFilename, jsonParseError.errorString()).arg(jsonParseError.offset);
        return QJsonObject();
    }

    if (!doc.isObject()) {
        errorString = QObject::tr("Root of json file is not object: %1").arg(jsonFilename);
        return QJsonObject();
    }

    QJsonObject jsonObject = doc.object();
    const bool success = validateInternalQUAVJsonFile(jsonObject, expectedFileType, minSupportedVersion, maxSupportedVersion, version, errorString);
    if (!success) {
        errorString = QObject::tr("Json file: '%1'. %2").arg(jsonFilename, errorString);
        return QJsonObject();
    }

    const QStringList translateKeys = m_addDefaultLocKeys(jsonObject);
    const QString context = QFileInfo(jsonFile).fileName();
    return m_translateRoot(jsonObject, context, translateKeys);
}

void JsonHelper::saveQUAVJsonFileHeader(QJsonObject &jsonObject, const QString &fileType, int version)
{
    jsonObject[m_jsonGroundStationKey] = m_jsonGroundStationValue;
    jsonObject[jsonFileTypeKey] = fileType;
    jsonObject[jsonVersionKey] = version;
}

bool JsonHelper::loadGeoCoordinateArray(const QJsonValue &jsonValue, bool altitudeRequired, QVariantList &rgVarPoints, QString &errorString)
{
    if (!jsonValue.isArray()) {
        errorString = QObject::tr("value for coordinate array is not array");
        return false;
    }

    const QJsonArray rgJsonPoints = jsonValue.toArray();

    rgVarPoints.clear();
    for (const QJsonValue &point : rgJsonPoints) {
        QGeoCoordinate coordinate;
        if (!JsonHelper::loadGeoCoordinate(point, altitudeRequired, coordinate, errorString)) {
            return false;
        }
        rgVarPoints.append(QVariant::fromValue(coordinate));
    }

    return true;
}

bool JsonHelper::loadGeoCoordinateArray(const QJsonValue &jsonValue, bool altitudeRequired, QList<QGeoCoordinate> &rgPoints, QString &errorString)
{
    QVariantList rgVarPoints;

    if (!loadGeoCoordinateArray(jsonValue, altitudeRequired, rgVarPoints, errorString)) {
        return false;
    }

    rgPoints.clear();
    for (const QVariant &point : rgVarPoints) {
        rgPoints.append(point.value<QGeoCoordinate>());
    }

    return true;
}

void JsonHelper::saveGeoCoordinateArray(const QVariantList &rgVarPoints, bool writeAltitude, QJsonValue &jsonValue)
{
    QJsonArray rgJsonPoints;
    for (const QVariant &point : rgVarPoints) {
        QJsonValue jsonPoint;
        JsonHelper::saveGeoCoordinate(point.value<QGeoCoordinate>(), writeAltitude, jsonPoint);
        rgJsonPoints.append(jsonPoint);
    }

    jsonValue = rgJsonPoints;
}

void JsonHelper::saveGeoCoordinateArray(const QList<QGeoCoordinate> &rgPoints, bool writeAltitude, QJsonValue &jsonValue)
{
    QVariantList rgVarPoints;
    for (const QGeoCoordinate &coord : rgPoints) {
        rgVarPoints.append(QVariant::fromValue(coord));
    }

    return saveGeoCoordinateArray(rgVarPoints, writeAltitude, jsonValue);
}

bool JsonHelper::validateKeys(const QJsonObject &jsonObject, const QList<JsonHelper::KeyValidateInfo> &keyInfo, QString &errorString)
{
    QStringList keyList;
    QList<QJsonValue::Type> typeList;

    for (const JsonHelper::KeyValidateInfo &info : keyInfo) {
        if (info.required) {
            keyList.append(info.key);
        }
    }
    if (!validateRequiredKeys(jsonObject, keyList, errorString)) {
        return false;
    }

    keyList.clear();
    for (const JsonHelper::KeyValidateInfo &info : keyInfo) {
        keyList.append(info.key);
        typeList.append(info.type);
    }

    return validateKeyTypes(jsonObject, keyList, typeList, errorString);
}

QString JsonHelper::m_jsonValueTypeToString(QJsonValue::Type type)
{
    struct TypeToString {
        QJsonValue::Type type;
        const char *string;
    };

    static constexpr const TypeToString rgTypeToString[] = {
        { QJsonValue::Null, "NULL" },
        { QJsonValue::Bool, "Bool" },
        { QJsonValue::Double, "Double" },
        { QJsonValue::String, "String" },
        { QJsonValue::Array, "Array" },
        { QJsonValue::Object, "Object" },
        { QJsonValue::Undefined, "Undefined" },
    };

    for (const TypeToString &conv : rgTypeToString) {
        if (type == conv.type) {
            return conv.string;
        }
    }

    return QObject::tr("Unknown type: %1").arg(type);
}

double JsonHelper::possibleNaNJsonValue(const QJsonValue &value)
{
    if (value.type() == QJsonValue::Null) {
        return std::numeric_limits<double>::quiet_NaN();
    } else {
        return value.toDouble();
    }
}
