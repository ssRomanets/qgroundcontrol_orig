
#include "QGeoTileFetcherQUAV.h"
#include "QGeoTiledMappingManagerEngineQUAV.h"
#include "QGeoMapReplyQUAV.h"
#include "QUAVMapUrlEngine.h"
#include "MapProvider.h"

#include <QtNetwork/QNetworkRequest>
#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>
#include <QtLocation/private/qgeotilespec_p.h>

QGeoTileFetcherQUAV::QGeoTileFetcherQUAV(QNetworkAccessManager *networkManager, const QVariantMap &parameters, QGeoTiledMappingManagerEngineQUAV *parent)
    : QGeoTileFetcher(parent)
    , m_networkManager(networkManager)
{
    Q_CHECK_PTR(networkManager);

    // qDebug() << Q_FUNC_INFO << this;
}

QGeoTileFetcherQUAV::~QGeoTileFetcherQUAV()
{
    // qDebug() << Q_FUNC_INFO << this;
}

QGeoTiledMapReply* QGeoTileFetcherQUAV::getTileImage(const QGeoTileSpec &spec)
{
    const SharedMapProvider provider = UrlFactory::getMapProviderFromQtMapId(spec.mapId());
    if (!provider) {
        return nullptr;
    }


    const QNetworkRequest request = getNetworkRequest(spec.mapId(), spec.x(), spec.y(), spec.zoom());
    if (request.url().isEmpty()) {
        return nullptr;
    }

    return new QGeoTiledMapReplyQUAV(m_networkManager, request, spec);
}

bool QGeoTileFetcherQUAV::initialized() const
{
    return (m_networkManager != nullptr);
}

bool QGeoTileFetcherQUAV::fetchingEnabled() const
{
    return initialized();
}

void QGeoTileFetcherQUAV::timerEvent(QTimerEvent *event)
{
    QGeoTileFetcher::timerEvent(event);
}

void QGeoTileFetcherQUAV::handleReply(QGeoTiledMapReply *reply, const QGeoTileSpec &spec)
{
    if (!reply) {
        return;
    }

    reply->deleteLater();

    if (!initialized()) {
        return;
    }

    if (reply->error() == QGeoTiledMapReply::NoError) {
        emit tileFinished(spec, reply->mapImageData(), reply->mapImageFormat());
    } else {
        emit tileError(spec, reply->errorString());
    }
}

QNetworkRequest QGeoTileFetcherQUAV::getNetworkRequest(int mapId, int x, int y, int zoom)
{
    const SharedMapProvider mapProvider = UrlFactory::getMapProviderFromQtMapId(mapId);

    QNetworkRequest request;
    request.setUrl(mapProvider->getTileURL(x, y, zoom));
    request.setRawHeader(QByteArrayLiteral("Accept"), QByteArrayLiteral("*/*"));
    request.setHeader(QNetworkRequest::UserAgentHeader, s_userAgent);
    const QByteArray referrer = mapProvider->getReferrer().toUtf8();
    if (!referrer.isEmpty()) {
        request.setRawHeader(QByteArrayLiteral("Referrer"), referrer);
    }
    const QByteArray token = mapProvider->getToken();
    if (!token.isEmpty()) {
        request.setRawHeader(QByteArrayLiteral("User-Token"), token);
    }
    // request.setOriginatingObject(this);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    request.setAttribute(QNetworkRequest::BackgroundRequestAttribute, true);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
    request.setAttribute(QNetworkRequest::DoNotBufferUploadDataAttribute, false);
    // request.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);
    request.setPriority(QNetworkRequest::NormalPriority);
    request.setTransferTimeout(10000);

    return request;
}
