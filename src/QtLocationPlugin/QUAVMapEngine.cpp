
/**
 * @file
 *   @brief Map Tile Cache
 *
 *   @author Gus Grubba <gus@auterion.com>
 *
 */

#include "QUAVMapEngine.h"
#include "QUAVCachedTileSet.h"
#include "QUAVTileCacheWorker.h"
#include "QGeoFileTileCacheQUAV.h"
#include "QUAVMapTasks.h"
#include "QUAVTileSet.h"
#include "QUAVTile.h"
#include "QUAVCacheTile.h"

#include <QtCore/qapplicationstatic.h>

Q_DECLARE_METATYPE(QList<QUAVTile*>)

Q_APPLICATION_STATIC(QUAVMapEngine, m_mapEngine);

QUAVMapEngine *getQUAVMapEngine()
{
    return QUAVMapEngine::instance();
}

QUAVMapEngine::QUAVMapEngine(QObject *parent)
    : QObject(parent)
    , m_worker(new QUAVCacheWorker(this))
{
    // qDebug() << Q_FUNC_INFO << this;

    (void) qRegisterMetaType<QUAVMapTask::TaskType>("TaskType");
    (void) qRegisterMetaType<QUAVTile>("QUAVTile");
    (void) qRegisterMetaType<QList<QUAVTile*>>("QList<QUAVTile*>");
    (void) qRegisterMetaType<QUAVTileSet>("QUAVTileSet");
    (void) qRegisterMetaType<QUAVCacheTile>("QUAVCacheTile");

    (void) connect(m_worker, &QUAVCacheWorker::updateTotals, this, &QUAVMapEngine::m_updateTotals);
}

QUAVMapEngine::~QUAVMapEngine()
{
    (void) disconnect(m_worker);
    m_worker->stop();
    m_worker->wait();

    // qDebug() << Q_FUNC_INFO << this;
}

QUAVMapEngine *QUAVMapEngine::instance()
{
    return m_mapEngine();
}

void QUAVMapEngine::init(const QString &databasePath)
{
    m_worker->setDatabaseFile(databasePath);

    QUAVMapTask* const task = new QUAVMapTask(QUAVMapTask::taskInit);
    (void) addTask(task);
}

bool QUAVMapEngine::addTask(QUAVMapTask *task)
{
    return m_worker->enqueueTask(task);
}

void QUAVMapEngine::m_updateTotals(quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize)
{
    emit updateTotals(totaltiles, totalsize, defaulttiles, defaultsize);

    const quint64 maxSize = static_cast<quint64>(QGeoFileTileCacheQUAV::getMaxDiskCacheSetting()) * pow(1024, 2);
    if (!m_prunning && (defaultsize > maxSize)) {
        m_prunning = true;

        const quint64 amountToPrune = defaultsize - maxSize;
        QUAVPruneCacheTask* const task = new QUAVPruneCacheTask(amountToPrune);
        (void) connect(task, &QUAVPruneCacheTask::pruned, this, &QUAVMapEngine::m_pruned);
        (void) addTask(task);
    }
}
