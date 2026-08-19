
#pragma once

#include <QtLocation/private/qgeotiledmap_p.h>

class QGeoTiledMappingManagerEngineQUAV;

class QGeoTiledMapQUAV : public QGeoTiledMap
{
    Q_OBJECT

public:
    explicit QGeoTiledMapQUAV(QGeoTiledMappingManagerEngineQUAV* engine, QObject *parent = nullptr);
    ~QGeoTiledMapQUAV();

    QGeoMap::Capabilities capabilities() const final;
};
