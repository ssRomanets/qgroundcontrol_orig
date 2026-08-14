

/**
 * @file
 *   @brief Map Tile Cache Worker Thread
 *
 *   @author Gus Grubba <gus@auterion.com>
 *
 */

#pragma once

#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

class QUAVMapTask;
class QUAVCachedTileSet;
class QSqlDatabase;

class QUAVCacheWorker : public QThread
{
    Q_OBJECT

public:
    explicit QUAVCacheWorker(QObject *parent = nullptr);
    ~QUAVCacheWorker();

    void setDatabaseFile(const QString &path) { m_databasePath = path; }

public slots:
    bool enqueueTask(QUAVMapTask *task);
    void stop();

signals:
    void updateTotals(quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize);

protected:
    void run() final;

private:
    void m_runTask(QUAVMapTask *task);

    void m_saveTile(QUAVMapTask *task);
    void m_getTile(QUAVMapTask *task);
    void m_getTileSets(QUAVMapTask *task);
    void m_createTileSet(QUAVMapTask *task);
    void m_getTileDownloadList(QUAVMapTask *task);
    void m_updateTileDownloadState(QUAVMapTask *task);
    void m_pruneCache(QUAVMapTask *task);
    void m_deleteTileSet(QUAVMapTask *task);
    void m_renameTileSet(QUAVMapTask *task);
    void m_resetCacheDatabase(QUAVMapTask *task);
    void m_importSets(QUAVMapTask *task);
    void m_exportSets(QUAVMapTask *task);
    bool m_testTask(QUAVMapTask *task);

    bool m_connectDB();
    void m_disconnectDB();
    bool m_createDB(QSqlDatabase &db, bool createDefault = true);
    bool m_findTileSetID(const QString &name, quint64 &setID);
    bool m_init();

    quint64 m_findTile(const QString &hash);
    quint64 m_getDefaultTileSet();

    void m_deleteBingNoTileTiles();
    void m_deleteTileSet(quint64 id);
    void m_updateSetTotals(QUAVCachedTileSet *set);
    void m_updateTotals();

    std::shared_ptr<QSqlDatabase> m_db = nullptr;
    QMutex                        m_taskQueueMutex;
    QQueue<QUAVMapTask*>           m_taskQueue;
    QWaitCondition                m_waitc;

    QString m_databasePath;
    quint32 m_defaultCount = 0;
    quint32 m_totalCount = 0;
    quint64 m_defaultSet = UINT64_MAX;
    quint64 m_defaultSize = 0;
    quint64 m_totalSize = 0;

    QElapsedTimer    m_updateTimer;
    int              m_updateTimeout = kShortTimeout;
    std::atomic_bool m_failed = false;
    std::atomic_bool m_valid = false;

    static constexpr const char *kSession = "QGeoTileWorkerSession";
    static constexpr const char *kExportSession = "QGeoTileExportSession";
    static constexpr int kShortTimeout = 2;
    static constexpr int kLongTimeout = 5;
};
