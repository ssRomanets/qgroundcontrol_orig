#pragma once

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>

class QNetworkAccessManager;

class QGeoTiledMappingManagerEngineQUAV : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT

public:
    QGeoTiledMappingManagerEngineQUAV(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString, QNetworkAccessManager *networkManager = nullptr, QObject *parent = nullptr);
    ~QGeoTiledMappingManagerEngineQUAV();

    QGeoMap* createMap() final;
    QNetworkAccessManager* networkManager() const { return m_networkManager; }

private:
    QNetworkAccessManager* m_networkManager = nullptr;

    static constexpr int kTileVersion = 1;
};
