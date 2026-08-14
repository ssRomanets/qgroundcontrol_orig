
/**
 * @file
 *   @brief Map Tile Set
 *
 *   @author Gus Grubba <gus@auterion.com>
 *
 */

#include "QUAVCachedTileSet.h"

#include "ElevationMapProvider.h"
#include "QUAVMapEngine.h"
#include "QMLControl/QUAVMapEngineManager.h"
#include "QUAVMapTasks.h"
#include "QUAVMapUrlEngine.h"
#include "QGeoFileTileCacheQUAV.h"
#include "QGeoTileFetcherQUAV.h"

#include <QUAVApplication.h>

#include <QtNetwork/QNetworkProxy>

QUAVCachedTileSet::QUAVCachedTileSet(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    // qDebug() << Q_FUNC_INFO << this;
}

QUAVCachedTileSet::~QUAVCachedTileSet()
{
    // qDebug() << Q_FUNC_INFO << this;
}

QString QUAVCachedTileSet::downloadStatus() const
{
    if (m_defaultSet) {
        return totalTilesSizeStr();
    }

    if (m_totalTileCount <= m_savedTileCount) {
        return savedTileSizeStr();
    }

    return (savedTileSizeStr() + " / " + totalTilesSizeStr());
}

void QUAVCachedTileSet::createDownloadTask()
{
    if (m_cancelPending) {
        setDownloading(false);
        return;
    }

    if (!m_downloading) {
        setErrorCount(0);
        setDownloading(true);
        m_noMoreTiles = false;
    }

    QUAVGetTileDownloadListTask* const task = new QUAVGetTileDownloadListTask(m_id, kTileBatchSize);
    (void) connect(task, &QUAVGetTileDownloadListTask::tileListFetched, this, &QUAVCachedTileSet::m_tileListFetched);
    if (m_manager) {
        (void) connect(task, &QUAVMapTask::error, m_manager, &QUAVMapEngineManager::taskError);
    }
    getQUAVMapEngine()->addTask(task);

    emit totalTileCountChanged();
    emit totalTilesSizeChanged();

    m_batchRequested = true;
}

void QUAVCachedTileSet::resumeDownloadTask()
{
    m_cancelPending = false;
    QUAVUpdateTileDownloadStateTask* const task = new QUAVUpdateTileDownloadStateTask(m_id, QUAVTile::StatePending, "*");
    getQUAVMapEngine()->addTask(task);
    createDownloadTask();
}

void QUAVCachedTileSet::cancelDownloadTask()
{
    m_cancelPending = true;
}

void QUAVCachedTileSet::m_tileListFetched(const QQueue<QUAVTile*> &tiles)
{
    m_batchRequested = false;
    if (tiles.size() < kTileBatchSize) {
        m_noMoreTiles = true;
    }

    if (tiles.isEmpty()) {
        m_doneWithDownload();
        return;
    }

    if (!m_networkManager) {
        m_networkManager = new QNetworkAccessManager(this);
        QNetworkProxy proxy = m_networkManager->proxy();
        proxy.setType(QNetworkProxy::DefaultProxy);
        m_networkManager->setProxy(proxy);
    }

    (void) m_tilesToDownload.append(tiles);
    m_prepareDownload();
}

void QUAVCachedTileSet::m_doneWithDownload()
{
    if (m_errorCount == 0) {
        setTotalTileCount(m_savedTileCount);
        setTotalTileSize(m_savedTileSize);

        quint32 avg = 0;
        if (m_savedTileSize != 0) {
            avg = m_savedTileSize / m_savedTileCount;
        } else {
//            qWarning() << Q_FUNC_INFO << "_savedTileSize=0";
        }

        setUniqueTileSize(m_uniqueTileCount * avg);
    }

    setDownloading(false);

    emit completeChanged();
}

void QUAVCachedTileSet::m_prepareDownload()
{
    if (m_tilesToDownload.isEmpty()) {
        if (m_noMoreTiles) {
            m_doneWithDownload();
        } else if (!m_batchRequested) {
            createDownloadTask();
        }
        return;
    }

    for (qsizetype i = m_replies.count(); i < QGeoTileFetcherQUAV::concurrentDownloads(m_type); i++) {
        if (m_tilesToDownload.isEmpty()) {
            break;
        }

        QUAVTile* const tile = m_tilesToDownload.dequeue();
        const int mapId = UrlFactory::getQtMapIdFromProviderType(tile->type());
        QNetworkRequest request = QGeoTileFetcherQUAV::getNetworkRequest(mapId, tile->x(), tile->y(), tile->z());
        request.setOriginatingObject(this);
        request.setAttribute(QNetworkRequest::User, tile->hash());

        QNetworkReply* const reply = m_networkManager->get(request);
        reply->setParent(this);

        (void) connect(reply, &QNetworkReply::finished, this, &QUAVCachedTileSet::m_networkReplyFinished);
        (void) connect(reply, &QNetworkReply::errorOccurred, this, &QUAVCachedTileSet::m_networkReplyError);
        (void) m_replies.insert(tile->hash(), reply);

        delete tile;
        if (!m_batchRequested && !m_noMoreTiles && (m_tilesToDownload.count() < (QGeoTileFetcherQUAV::concurrentDownloads(m_type) * 10))) {
            createDownloadTask();
        }
    }
}

void QUAVCachedTileSet::m_networkReplyFinished()
{
    QNetworkReply* const reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (!reply) {
//        qWarning() << Q_FUNC_INFO << "NULL Reply";
        return;
    }
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    if (!reply->isOpen()) {
//        qWarning() << Q_FUNC_INFO << "Empty Reply";
        return;
    }

    const QString hash = reply->request().attribute(QNetworkRequest::User).toString();
    if (hash.isEmpty()) {
//        qWarning() << Q_FUNC_INFO << "Empty Hash";
        return;
    }

    if (m_replies.contains(hash)) {
        (void) m_replies.remove(hash);
    } else {
//        qWarning() << Q_FUNC_INFO << "Reply not in list: " << hash;
    }
//    qDebug() << "Tile fetched:" << hash;

    QByteArray image = reply->readAll();
    if (image.isEmpty()) {
//        qWarning() << Q_FUNC_INFO << "Empty Image";
        return;
    }

    const QString type = UrlFactory::tileHashToType(hash);
    const SharedMapProvider mapProvider = UrlFactory::getMapProviderFromProviderType(type);
    Q_CHECK_PTR(mapProvider);

    if (mapProvider->isElevationProvider()) {
        const SharedElevationProvider elevationProvider = std::dynamic_pointer_cast<const ElevationProvider>(mapProvider);
        image = elevationProvider->serialize(image);
        if (image.isEmpty()) {
 //           qWarning() << Q_FUNC_INFO << "Failed to Serialize Terrain Tile";
            return;
        }
    }

    const QString format = mapProvider->getImageFormat(image);
    if (format.isEmpty()) {
    //    qWarning() << Q_FUNC_INFO << "Empty Format";
        return;
    }

    QGeoFileTileCacheQUAV::cacheTile(type, hash, image, format, m_id);

    QUAVUpdateTileDownloadStateTask* const task = new QUAVUpdateTileDownloadStateTask(m_id, QUAVTile::StateComplete, hash);
    getQUAVMapEngine()->addTask(task);

    setSavedTileSize(m_savedTileSize + image.size());
    setSavedTileCount(m_savedTileCount + 1);

    if (m_savedTileCount % 10 == 0) {
        const quint32 avg = m_savedTileSize / m_savedTileCount;
        setTotalTileSize(avg * m_totalTileCount);
        setUniqueTileSize(avg * m_uniqueTileCount);
    }

    m_prepareDownload();
}

void QUAVCachedTileSet::m_networkReplyError(QNetworkReply::NetworkError error)
{
    QNetworkReply* const reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (!reply) {
        return;
    }
    //qDebug() << Q_FUNC_INFO << "Error fetching tile" << reply->errorString();

    setErrorCount(m_errorCount + 1);

    const QString hash = reply->request().attribute(QNetworkRequest::User).toString();
    if (hash.isEmpty()) {
    //    qWarning() << Q_FUNC_INFO << "Empty Hash";
        return;
    }

    if (m_replies.contains(hash)) {
        (void) m_replies.remove(hash);
    } else {
    //    qWarning() << Q_FUNC_INFO << "Reply not in list:" << hash;
    }

    if (error != QNetworkReply::OperationCanceledError) {
    //    qWarning() << Q_FUNC_INFO << "Error:" << reply->errorString();
    }

    QUAVUpdateTileDownloadStateTask* const task = new QUAVUpdateTileDownloadStateTask(m_id, QUAVTile::StateError, hash);
    getQUAVMapEngine()->addTask(task);

    m_prepareDownload();
}

void QUAVCachedTileSet::setSelected(bool sel)
{
    if (sel != m_selected) {
        m_selected = sel;
        emit selectedChanged();
        if (m_manager) {
            emit m_manager->selectedCountChanged();
        }
    }
}

QString QUAVCachedTileSet::errorCountStr() const
{
    return quavApp()->numberToString(m_errorCount);
}

QString QUAVCachedTileSet::totalTileCountStr() const
{
    return quavApp()->numberToString(m_totalTileCount);
}

QString QUAVCachedTileSet::totalTilesSizeStr() const
{
    return quavApp()->bigSizeToString(m_totalTileSize);
}

QString QUAVCachedTileSet::uniqueTileSizeStr() const
{
    return quavApp()->bigSizeToString(m_uniqueTileSize);
}

QString QUAVCachedTileSet::uniqueTileCountStr() const
{
    return quavApp()->numberToString(m_uniqueTileCount);
}

QString QUAVCachedTileSet::savedTileCountStr() const
{
    return quavApp()->numberToString(m_savedTileCount);
}

QString QUAVCachedTileSet::savedTileSizeStr() const
{
    return quavApp()->bigSizeToString(m_savedTileSize);
}
