
#pragma once

#include <QtLocation/private/qgeotilefetcher_p.h>
#include <QtNetwork/QNetworkRequest>

class QGeoTiledMappingManagerEngineQUAV;
class QGeoTiledMapReplyQUAV;
class QGeoTileSpec;
class QNetworkAccessManager;

class QGeoTileFetcherQUAV : public QGeoTileFetcher
{
    Q_OBJECT

public:
    QGeoTileFetcherQUAV(QNetworkAccessManager *networkManager, const QVariantMap &parameters, QGeoTiledMappingManagerEngineQUAV *parent = nullptr);
    ~QGeoTileFetcherQUAV();

    static QNetworkRequest getNetworkRequest(int mapId, int x, int y, int zoom);
    /* Note: QNetworkAccessManager queues the requests it receives. The number of requests executed in parallel is dependent on the protocol.
     * Currently, for the HTTP protocol on desktop platforms, 6 requests are executed in parallel for one host/port combination. */
    static uint32_t concurrentDownloads(const QString &type) { Q_UNUSED(type); return 6; }

private:
    QGeoTiledMapReply* getTileImage(const QGeoTileSpec &spec) final;
    bool initialized() const final;
    bool fetchingEnabled() const final;
    void timerEvent(QTimerEvent *event) final;
    void handleReply(QGeoTiledMapReply *reply, const QGeoTileSpec &spec) final;

    QNetworkAccessManager* m_networkManager = nullptr;

    // TODO: Detect Wayland vs X11
    static constexpr const char* s_userAgent = "Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/112.0";

};
