
#include "QGeoMapReplyQUAV.h"

#include "ElevationMapProvider.h"
#include "MapProvider.h"
#include "QUAVMapEngine.h"
#include "QUAVMapUrlEngine.h"
#include "QGeoFileTileCacheQUAV.h"

#include <DeviceInfo.h>

#include <QtCore/QFile>
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QSslError>

QByteArray QGeoTiledMapReplyQUAV::m_bingNoTileImage;
QByteArray QGeoTiledMapReplyQUAV::m_badTile;

QGeoTiledMapReplyQUAV::QGeoTiledMapReplyQUAV(QNetworkAccessManager *networkManager, const QNetworkRequest &request, const QGeoTileSpec &spec, QObject *parent)
    : QGeoTiledMapReply(spec, parent)
    , m_networkManager(networkManager)
    , m_request(request)
{
    // qDebug() << Q_FUNC_INFO << this;

    _initDataFromResources();

    (void) connect(this, &QGeoTiledMapReplyQUAV::errorOccurred, this, [this](QGeoTiledMapReply::Error error, const QString &errorString) {
//        qWarning() << error << errorString;
        setMapImageData(m_badTile);
        setMapImageFormat("png");
        setCached(false);
    }, Qt::AutoConnection);

    QUAVFetchTileTask* const task = QGeoFileTileCacheQUAV::createFetchTileTask(UrlFactory::getProviderTypeFromQtMapId(spec.mapId()), spec.x(), spec.y(), spec.zoom());
    (void) connect(task, &QUAVFetchTileTask::tileFetched, this, &QGeoTiledMapReplyQUAV::m_cacheReply);
    (void) connect(task, &QUAVMapTask::error, this, &QGeoTiledMapReplyQUAV::m_cacheError);
    getQUAVMapEngine()->addTask(task);
}

QGeoTiledMapReplyQUAV::~QGeoTiledMapReplyQUAV()
{
    // qDebug() << Q_FUNC_INFO << this;
}

void QGeoTiledMapReplyQUAV::_initDataFromResources()
{
    if (m_bingNoTileImage.isEmpty()) {
        QFile file(":/res/BingNoTileBytes.dat");
        if (file.open(QFile::ReadOnly)) {
            m_bingNoTileImage = file.readAll();
            file.close();
        }
    }

    if (m_badTile.isEmpty()) {
        QFile file(":/res/images/notile.png");
        if (file.open(QFile::ReadOnly)) {
            m_badTile = file.readAll();
            file.close();
        }
    }
}

void QGeoTiledMapReplyQUAV::m_networkReplyFinished()
{
    QNetworkReply* const reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        setError(QGeoTiledMapReply::UnknownError, tr("Unexpected Error"));
        return;
    }
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    if (!reply->isOpen()) {
        setError(QGeoTiledMapReply::ParseError, tr("Empty Reply"));
        return;
    }

    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if ((statusCode < HTTP_Response::SUCCESS_OK) || (statusCode >= HTTP_Response::REDIRECTION_MULTIPLE_CHOICES)) {
        setError(QGeoTiledMapReply::CommunicationError, reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        return;
    }

    QByteArray image = reply->readAll();
    if (image.isEmpty()) {
        setError(QGeoTiledMapReply::ParseError, tr("Image is Empty"));
        return;
    }

    const SharedMapProvider mapProvider = UrlFactory::getMapProviderFromQtMapId(tileSpec().mapId());
    Q_CHECK_PTR(mapProvider);

    if (mapProvider->isBingProvider() && (image == m_bingNoTileImage)) {
        setError(QGeoTiledMapReply::CommunicationError, tr("Bing Tile Above Zoom Level"));
        return;
    }

    if (mapProvider->isElevationProvider()) {
        const SharedElevationProvider elevationProvider = std::dynamic_pointer_cast<const ElevationProvider>(mapProvider);
        image = elevationProvider->serialize(image);
        if (image.isEmpty()) {
            setError(QGeoTiledMapReply::ParseError, tr("Failed to Serialize Terrain Tile"));
            return;
        }
    }
    setMapImageData(image);

    const QString format = mapProvider->getImageFormat(image);
    if (format.isEmpty()) {
        setError(QGeoTiledMapReply::ParseError, tr("Unknown Format"));
        return;
    }
    setMapImageFormat(format);

    QGeoFileTileCacheQUAV::cacheTile(mapProvider->getMapName(), tileSpec().x(), tileSpec().y(), tileSpec().zoom(), image, format);

    setFinished(true);
}

void QGeoTiledMapReplyQUAV::m_networkReplyError(QNetworkReply::NetworkError error)
{
    if (error != QNetworkReply::OperationCanceledError) {
        const QNetworkReply* const reply = qobject_cast<const QNetworkReply*>(sender());
        if (!reply) {
            setError(QGeoTiledMapReply::CommunicationError, tr("Invalid Reply"));
        } else {
            setError(QGeoTiledMapReply::CommunicationError, reply->errorString());
        }
    } else {
        setFinished(true);
    }
}

void QGeoTiledMapReplyQUAV::m_networkReplySslErrors(const QList<QSslError> &errors)
{
    QString errorString;
    for (const QSslError &error : errors) {
        if (!errorString.isEmpty()) {
            (void) errorString.append('\n');
        }
        (void) errorString.append(error.errorString());
    }

    if (!errorString.isEmpty()) {
        setError(QGeoTiledMapReply::CommunicationError, errorString);
    }
}

void QGeoTiledMapReplyQUAV::m_cacheReply(QUAVCacheTile *tile)
{
    if (tile) {
        setMapImageData(tile->img());
        setMapImageFormat(tile->format());
        setCached(true);
        setFinished(true);
        delete tile;
    } else {
        setError(QGeoTiledMapReply::UnknownError, tr("Invalid Cache Tile"));
    }
}

void QGeoTiledMapReplyQUAV::m_cacheError(QUAVMapTask::TaskType type, QStringView errorString)
{
    Q_UNUSED(errorString);

    Q_ASSERT(type == QUAVMapTask::taskFetchTile);

    if (!QUAVDeviceInfo::isInternetAvailable()) {
        setError(QGeoTiledMapReply::CommunicationError, tr("Network Not Available"));
        return;
    }

    m_request.setOriginatingObject(this);

    QNetworkReply* const reply = m_networkManager->get(m_request);
    reply->setParent(this);

    (void) connect(reply, &QNetworkReply::finished, this, &QGeoTiledMapReplyQUAV::m_networkReplyFinished);
    (void) connect(reply, &QNetworkReply::errorOccurred, this, &QGeoTiledMapReplyQUAV::m_networkReplyError);
    (void) connect(reply, &QNetworkReply::sslErrors, this, &QGeoTiledMapReplyQUAV::m_networkReplySslErrors);
    (void) connect(this, &QGeoTiledMapReplyQUAV::aborted, reply, &QNetworkReply::abort);
}

void QGeoTiledMapReplyQUAV::abort()
{
    QGeoTiledMapReply::abort();
}
