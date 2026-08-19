
#include "QGeoTiledMapQUAV.h"
#include "QGeoTiledMappingManagerEngineQUAV.h"

QGeoTiledMapQUAV::QGeoTiledMapQUAV(QGeoTiledMappingManagerEngineQUAV *engine, QObject *parent)
    : QGeoTiledMap(engine, parent)
{
        // qDebug() << Q_FUNC_INFO << this;
}

QGeoTiledMapQUAV::~QGeoTiledMapQUAV()
{
    // qDebug() << Q_FUNC_INFO << this;
}

QGeoMap::Capabilities QGeoTiledMapQUAV::capabilities() const
{
    return Capabilities(SupportsVisibleRegion
                        | SupportsAnchoringCoordinate
                        | SupportsVisibleArea);
}