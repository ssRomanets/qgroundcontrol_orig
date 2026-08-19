
#pragma once

#include <QtCore/QList>

class TerrainTile
{

public:
     /// Constructor from serialized elevation data (either from file or web)
     ///    @param document
     explicit TerrainTile(const QByteArray &byteArray);
     virtual ~TerrainTile();

protected:
    struct TileInfo_t {
        double  swLat, swLon, neLat, neLon;
        int16_t minElevation, maxElevation;
        double  avgElevation;
        int16_t gridSizeLat, gridSizeLon;
    } Q_PACKED;

};
