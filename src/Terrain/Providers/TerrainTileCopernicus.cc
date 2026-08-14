
#include "TerrainTileCopernicus.h"
#include "JsonHelper.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

TerrainTileCopernicus::TerrainTileCopernicus(const QByteArray &byteArray)
    : TerrainTile(byteArray)
{
    // qDebug() << Q_FUNC_INFO << this;
}

TerrainTileCopernicus::~TerrainTileCopernicus()
{
    // qDebug() << Q_FUNC_INFO << this;
}

QJsonValue TerrainTileCopernicus::getJsonFromData(const QByteArray &input)
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(input, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
    //    qWarning() << Q_FUNC_INFO << "Terrain tile json doc parse error" << parseError.errorString();
        return QJsonValue();
    }

    if (!document.isObject()) {
    //    qWarning() << Q_FUNC_INFO << "Terrain tile json doc is no object";
        return QJsonValue();
    }

    const QJsonObject rootObject = document.object();

    static const QList<JsonHelper::KeyValidateInfo> rootVersionKeyInfoList = {
        { m_jsonStatusKey, QJsonValue::String, true },
        { m_jsonDataKey, QJsonValue::Object, true },
    };
    QString errorString;
    if (!JsonHelper::validateKeys(rootObject, rootVersionKeyInfoList, errorString)) {
    //    qWarning() << Q_FUNC_INFO << "Error in reading json: " << errorString;
        return QJsonValue();
    }

    if (rootObject[m_jsonStatusKey].toString() != "success") {
    //    qWarning() << Q_FUNC_INFO << "Invalid terrain tile.";
        return QJsonValue();
    }

    const QJsonValue &jsonData = rootObject[m_jsonDataKey];
    const QJsonObject &dataObject = jsonData.toObject();

    QList<JsonHelper::KeyValidateInfo> dataVersionKeyInfoList = {
        { m_jsonBoundsKey, QJsonValue::Object, true },
        { m_jsonStatsKey,  QJsonValue::Object, true },
        { m_jsonCarpetKey, QJsonValue::Array, true },
    };

    if (!JsonHelper::validateKeys(dataObject, dataVersionKeyInfoList, errorString)) {
    //    qWarning() << Q_FUNC_INFO << "Error in reading json: " << errorString;
        return QJsonValue();
    }

    return jsonData;
}

QByteArray TerrainTileCopernicus::serializeFromData(const QByteArray &input)
{
    const QJsonValue &jsonData = getJsonFromData(input);
    if (jsonData.isNull()) {
        return QByteArray();
    }

    const QJsonObject &dataObject = jsonData.toObject();

    const QJsonObject &boundsObject = dataObject[m_jsonBoundsKey].toObject();
    static const QList<JsonHelper::KeyValidateInfo> boundsVersionKeyInfoList = {
        { m_jsonSouthWestKey, QJsonValue::Array, true },
        { m_jsonNorthEastKey, QJsonValue::Array, true },
    };
    QString errorString;
    if (!JsonHelper::validateKeys(boundsObject, boundsVersionKeyInfoList, errorString)) {
    //    qWarning() << Q_FUNC_INFO << "Error in reading json: " << errorString;
        return QByteArray();
    }

    const QJsonArray &swArray = boundsObject[m_jsonSouthWestKey].toArray();
    const QJsonArray &neArray = boundsObject[m_jsonNorthEastKey].toArray();
    if ((swArray.count() < 2) || (neArray.count() < 2 )) {
    //    qWarning() << Q_FUNC_INFO << "Incomplete bounding location";
        return QByteArray();
    }

    const QJsonObject &statsObject = dataObject[m_jsonStatsKey].toObject();
    static const QList<JsonHelper::KeyValidateInfo> statsVersionKeyInfoList = {
        { m_jsonMinElevationKey, QJsonValue::Double, true },
        { m_jsonMaxElevationKey, QJsonValue::Double, true },
        { m_jsonAvgElevationKey, QJsonValue::Double, true },
    };
    if (!JsonHelper::validateKeys(statsObject, statsVersionKeyInfoList, errorString)) {
    //    qWarning() << Q_FUNC_INFO << "Error in reading json: " << errorString;
        return QByteArray();
    }

    const QJsonArray &carpetArray = dataObject[m_jsonCarpetKey].toArray();

    TerrainTile::TileInfo_t tileInfo;
    tileInfo.swLat = swArray[0].toDouble();
    tileInfo.swLon = swArray[1].toDouble();
    tileInfo.neLat = neArray[0].toDouble();
    tileInfo.neLon = neArray[1].toDouble();
    tileInfo.minElevation = static_cast<int16_t>(statsObject[m_jsonMinElevationKey].toInt());
    tileInfo.maxElevation = static_cast<int16_t>(statsObject[m_jsonMaxElevationKey].toInt());
    tileInfo.avgElevation = statsObject[m_jsonAvgElevationKey].toDouble();
    tileInfo.gridSizeLat = static_cast<int16_t>(carpetArray.count());
    tileInfo.gridSizeLon = static_cast<int16_t>(carpetArray[0].toArray().count());

    // qDebug() << "Serialize: TileInfo: south west:" << tileInfo.swLat << tileInfo.swLon;
    // qDebug() << "Serialize: TileInfo: north east:" << tileInfo.neLat << tileInfo.neLon;

    constexpr int cTileNumHeaderBytes = static_cast<int>(sizeof(TileInfo_t));
    const int cTileNumDataBytes = static_cast<int>(sizeof(int16_t)) * tileInfo.gridSizeLat * tileInfo.gridSizeLon;

    QByteArray result(cTileNumHeaderBytes + cTileNumDataBytes, Qt::Uninitialized);
    TileInfo_t *pTileInfo = reinterpret_cast<TileInfo_t*>(result.data());
    int16_t* const pTileData = reinterpret_cast<int16_t*>(&reinterpret_cast<uint8_t*>(result.data())[cTileNumHeaderBytes]);

    *pTileInfo = tileInfo;

    int valueIndex = 0;
    for (qsizetype i = 0; i < static_cast<qsizetype>(tileInfo.gridSizeLat); i++) {
        const QJsonArray &row = carpetArray[i].toArray();
        if (row.count() < tileInfo.gridSizeLon) {
        //    qDebug() << "Expected row array of" << tileInfo.gridSizeLon << ", instead got" << row.count();
            return QByteArray();
        }

        for (qsizetype j = 0; j < static_cast<qsizetype>(tileInfo.gridSizeLon); j++) {
            pTileData[valueIndex++] = static_cast<int16_t>(row[j].toDouble());
        }
    }

    return result;
}
