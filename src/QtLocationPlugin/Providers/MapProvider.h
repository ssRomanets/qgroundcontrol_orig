
#pragma once

#include <QtLocation/private/qgeomaptype_p.h>
#include <QtCore/QByteArray>
#include <QtCore/QString>

#include "QUAVTileSet.h"

#define MAX_MAP_ZOOM 23.0
static constexpr const quint32 AVERAGE_TILE_SIZE = 13652;

// TODO: Inherit from QGeoMapType
class MapProvider
{
public:
    MapProvider(const QString &mapName, const QString &referrer, const QString &imageFormat, quint32 averageSize = AVERAGE_TILE_SIZE,
                QGeoMapType::MapStyle mapStyle = QGeoMapType::CustomMap);
    virtual ~MapProvider();

    QUrl getTileURL(int x, int y, int zoom) const;

    QString getImageFormat(QByteArrayView image) const;

    // TODO: Download Random Tile And Use That Size Instead?
    quint32 getAverageSize() const { return m_averageSize; }

    QGeoMapType::MapStyle getMapStyle() const { return m_mapStyle; }
    const QString& getMapName() const { return m_mapName; }
    int getMapId() const { return m_mapId; }
    const QString& getReferrer() const { return m_referrer; }
    virtual QByteArray getToken() const { return QByteArray(); }

    virtual int long2tileX(double lon, int z) const;
    virtual int lat2tileY(double lat, int z) const;

    virtual bool isElevationProvider() const { return false; }
    virtual bool isBingProvider() const { return false; }

    virtual QUAVTileSet getTileCount(int zoom, double topleftLon,
                                    double topleftLat, double bottomRightLon,
                                    double bottomRightLat) const;

protected:
    QString m_tileXYToQuadKey(int tileX, int tileY, int levelOfDetail) const;
    int     m_getServerNum(int x, int y, int max) const;

    virtual QString m_getURL(int x, int y, int zoom) const = 0;

    const QString               m_mapName;
    const QString               m_referrer;
    const QString               m_imageFormat;
    const quint32               m_averageSize;
    const QGeoMapType::MapStyle m_mapStyle;
    const QString               m_language;
    const int                   m_mapId;

private:
    static int m_mapIdIndex;
};
