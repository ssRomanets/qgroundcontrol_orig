
#include "QUAVTileCacheWorker.h"
#include "QUAVCachedTileSet.h"
#include "QUAVMapTasks.h"
#include "QUAVMapUrlEngine.h"

#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>


QUAVCacheWorker::QUAVCacheWorker(QObject *parent)
    : QThread(parent)
{
    // qDebug() << Q_FUNC_INFO << this;
}

QUAVCacheWorker::~QUAVCacheWorker()
{
    // qDebug() << Q_FUNC_INFO << this;
}

void QUAVCacheWorker::stop()
{
    QMutexLocker lock(&m_taskQueueMutex);
    qDeleteAll(m_taskQueue);
    lock.unlock();

    if (isRunning()) {
        m_waitc.wakeAll();
    }
}

bool QUAVCacheWorker::enqueueTask(QUAVMapTask *task)
{
    if (!m_valid && (task->type() != QUAVMapTask::taskInit)) {
        task->setError(tr("Database Not Initialized"));
        task->deleteLater();
        return false;
    }

    // TODO: Prepend Stop Task Instead?
    QMutexLocker lock(&m_taskQueueMutex);
    m_taskQueue.enqueue(task);
    lock.unlock();

    if (isRunning()) {
        m_waitc.wakeAll();
    } else {
        start(QThread::HighPriority);
    }

    return true;
}

void QUAVCacheWorker::run()
{
    if (!m_valid && !m_failed) {
        if (!m_init()) {
 //           qWarning() << "Failed To Init Database";
            return;
        }
    }

    if (m_valid) {
        if (m_connectDB()) {
            m_deleteBingNoTileTiles();
        }
    }

    QMutexLocker lock(&m_taskQueueMutex);
    while (true) {
        if (!m_taskQueue.isEmpty()) {
            QUAVMapTask* const task = m_taskQueue.dequeue();
            lock.unlock();
            m_runTask(task);
            lock.relock();
            task->deleteLater();

            const qsizetype count = m_taskQueue.count();
            if (count > 100) {
                m_updateTimeout = kLongTimeout;
            } else if (count < 25) {
                m_updateTimeout = kShortTimeout;
            }

            if ((count == 0) || m_updateTimer.hasExpired(m_updateTimeout)) {
                if (m_valid) {
                    lock.unlock();
                    m_updateTotals();
                    lock.relock();
                }
            }
        } else {
            (void) m_waitc.wait(lock.mutex(), 5000);
            if (m_taskQueue.isEmpty()) {
                break;
            }
        }
    }
    lock.unlock();

    m_disconnectDB();
}

void QUAVCacheWorker::m_runTask(QUAVMapTask *task)
{
    switch (task->type()) {
    case QUAVMapTask::taskInit:
        break;
    case QUAVMapTask::taskCacheTile:
        m_saveTile(task);
        break;
    case QUAVMapTask::taskFetchTile:
        m_getTile(task);
        break;
    case QUAVMapTask::taskFetchTileSets:
        m_getTileSets(task);
        break;
    case QUAVMapTask::taskCreateTileSet:
        m_createTileSet(task);
        break;
    case QUAVMapTask::taskGetTileDownloadList:
        m_getTileDownloadList(task);
        break;
    case QUAVMapTask::taskUpdateTileDownloadState:
        m_updateTileDownloadState(task);
        break;
    case QUAVMapTask::taskDeleteTileSet:
        m_deleteTileSet(task);
        break;
    case QUAVMapTask::taskRenameTileSet:
        m_renameTileSet(task);
        break;
    case QUAVMapTask::taskPruneCache:
        m_pruneCache(task);
        break;
    case QUAVMapTask::taskReset:
        m_resetCacheDatabase(task);
        break;
    case QUAVMapTask::taskExport:
        m_exportSets(task);
        break;
    case QUAVMapTask::taskImport:
        m_importSets(task);
        break;
    default:
//        qWarning() << "given unhandled task type" << task->type();
        break;
    }
}

void QUAVCacheWorker::m_deleteBingNoTileTiles()
{
    static const QString alreadyDoneKey = QStringLiteral("_deleteBingNoTileTilesDone");

    QSettings settings;
    if (settings.value(alreadyDoneKey, false).toBool()) {
        return;
    }
    settings.setValue(alreadyDoneKey, true);

    // Previously we would store these empty tile graphics in the cache. This prevented the ability to zoom beyong the level
    // of available tiles. So we need to remove only of these still hanging around to make higher zoom levels work.
    QFile file(QStringLiteral(":/res/BingNoTileBytes.dat"));
    if (!file.open(QFile::ReadOnly)) {
//        qWarning() << "Failed to Open File" << file.fileName() << ":" << file.errorString();
        return;
    }

    const QByteArray noTileBytes = file.readAll();
    file.close();

    QSqlQuery query(*m_db);
    QList<quint64> idsToDelete;
    // Select tiles in default set only, sorted by oldest.
    QString s = QStringLiteral("SELECT tileID, tile, hash FROM Tiles WHERE LENGTH(tile) = %1").arg(noTileBytes.length());
    if (!query.exec(s)) {
//        qWarning() << "query failed";
        return;
    }

    while (query.next()) {
        if (query.value(1).toByteArray() == noTileBytes) {
            idsToDelete.append(query.value(0).toULongLong());
//            qDebug() << "HASH:" << query.value(2).toString();
        }
    }

    for (const quint64 tileId: idsToDelete) {
        s = QStringLiteral("DELETE FROM Tiles WHERE tileID = %1").arg(tileId);
        if (!query.exec(s)) {
//            qWarning() << "Delete failed";
        }
    }
}

bool QUAVCacheWorker::m_findTileSetID(const QString &name, quint64 &setID)
{
    QSqlQuery query(*m_db);
    const QString s = QStringLiteral("SELECT setID FROM TileSets WHERE name = \"%1\"").arg(name);
    if (query.exec(s) && query.next()) {
        setID = query.value(0).toULongLong();
        return true;
    }

    return false;
}

quint64 QUAVCacheWorker::m_getDefaultTileSet()
{
    if (m_defaultSet != UINT64_MAX) {
        return m_defaultSet;
    }

    QSqlQuery query(*m_db);
    const QString s = QStringLiteral("SELECT setID FROM TileSets WHERE defaultSet = 1");
    if (query.exec(s) && query.next()) {
        m_defaultSet = query.value(0).toULongLong();
        return m_defaultSet;
    }

    return 1L;
}

void QUAVCacheWorker::m_saveTile(QUAVMapTask *mtask)
{
    if (!m_valid) {
//        qWarning() << "Map Cache SQL error (saveTile() open db):" << _db->lastError();
        return;
    }

    QUAVSaveTileTask* task = static_cast<QUAVSaveTileTask*>(mtask);
    QSqlQuery query(*m_db);
    (void) query.prepare("INSERT INTO Tiles(hash, format, tile, size, type, date) VALUES(?, ?, ?, ?, ?, ?)");
    query.addBindValue(task->tile()->hash());
    query.addBindValue(task->tile()->format());
    query.addBindValue(task->tile()->img());
    query.addBindValue(task->tile()->img().size());
    query.addBindValue(task->tile()->type());
    query.addBindValue(QDateTime::currentSecsSinceEpoch());
    if (!query.exec()) {
        // Tile was already there.
        // QtLocation some times requests the same tile twice in a row. The first is saved, the second is already there.
        return;
    }

    const quint64 tileID = query.lastInsertId().toULongLong();
    const quint64 setID = task->tile()->tileSet() == UINT64_MAX ? m_getDefaultTileSet() : task->tile()->tileSet();
    const QString s = QStringLiteral("INSERT INTO SetTiles(tileID, setID) VALUES(%1, %2)").arg(tileID).arg(setID);
    (void) query.prepare(s);
    if (!query.exec()) {
//        qWarning() << "Map Cache SQL error (add tile into SetTiles):" << query.lastError().text();
    }

//    qDebug() << "HASH:" << task->tile()->hash();
}

void QUAVCacheWorker::m_getTile(QUAVMapTask* mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QUAVFetchTileTask *task = static_cast<QUAVFetchTileTask*>(mtask);
    QSqlQuery query(*m_db);
    const QString s = QStringLiteral("SELECT tile, format, type FROM Tiles WHERE hash = \"%1\"").arg(task->hash());
    if (query.exec(s) && query.next()) {
        const QByteArray &arrray = query.value(0).toByteArray();
        const QString &format = query.value(1).toString();
        const QString &type = query.value(2).toString();
//        qDebug() << "(Found in DB) HASH:" << task->hash();
        QUAVCacheTile *tile = new QUAVCacheTile(task->hash(), arrray, format, type);
        task->setTileFetched(tile);
        return;
    }

//    qDebug() << "(NOT in DB) HASH:" << task->hash();
    task->setError("Tile not in cache database");
}

void QUAVCacheWorker::m_getTileSets(QUAVMapTask *mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QUAVFetchTileSetTask *task = static_cast<QUAVFetchTileSetTask*>(mtask);
    QSqlQuery query(*m_db);
    const QString s = QStringLiteral("SELECT * FROM TileSets ORDER BY defaultSet DESC, name ASC");
//    qDebug() << s;
    if (!query.exec(s)) {
        task->setError("No tile set in database");
        return;
    }

    while (query.next()) {
        const QString name = query.value("name").toString();
        QUAVCachedTileSet *set = new QUAVCachedTileSet(name);
        set->setId(query.value("setID").toULongLong());
        set->setMapTypeStr(query.value("typeStr").toString());
        set->setTopleftLat(query.value("topleftLat").toDouble());
        set->setTopleftLon(query.value("topleftLon").toDouble());
        set->setBottomRightLat(query.value("bottomRightLat").toDouble());
        set->setBottomRightLon(query.value("bottomRightLon").toDouble());
        set->setMinZoom(query.value("minZoom").toInt());
        set->setMaxZoom(query.value("maxZoom").toInt());
        set->setType(UrlFactory::getProviderTypeFromQtMapId(query.value("type").toInt()));
        set->setTotalTileCount(query.value("numTiles").toUInt());
        set->setDefaultSet(query.value("defaultSet").toInt() != 0);
        set->setCreationDate(QDateTime::fromSecsSinceEpoch(query.value("date").toUInt()));
        m_updateSetTotals(set);
        // Object created here must be moved to app thread to be used there
        (void) set->moveToThread(QCoreApplication::instance()->thread());
        task->setTileSetFetched(set);
    }
}

void QUAVCacheWorker::m_updateSetTotals(QUAVCachedTileSet *set)
{
    if (set->defaultSet()) {
        m_updateTotals();
        set->setSavedTileCount(m_totalCount);
        set->setSavedTileSize(m_totalSize);
        set->setTotalTileCount(m_defaultCount);
        set->setTotalTileSize(m_defaultSize);
        return;
    }

    QSqlQuery subquery(*m_db);
    QString sq = QStringLiteral("SELECT COUNT(size), SUM(size) FROM Tiles A INNER JOIN SetTiles B on A.tileID = B.tileID WHERE B.setID = %1").arg(set->id());
//    qDebug() << sq;
    if (!subquery.exec(sq) || !subquery.next()) {
        return;
    }

    set->setSavedTileCount(subquery.value(0).toUInt());
    set->setSavedTileSize(subquery.value(1).toULongLong());
//    qDebug() << "Set" << set->id() << "Totals:" << set->savedTileCount() << " " << set->savedTileSize() << "Expected: " << set->totalTileCount() << " " << set->totalTilesSize();
    // Update (estimated) size
    quint64 avg = UrlFactory::averageSizeForType(set->type());
    if (set->totalTileCount() <= set->savedTileCount()) {
        // We're done so the saved size is the total size
        set->setTotalTileSize(set->savedTileSize());
    } else {
        // Otherwise we need to estimate it.
        if ((set->savedTileCount() > 10) && set->savedTileSize()) {
            avg = set->savedTileSize() / set->savedTileCount();
        }
        set->setTotalTileSize(avg * set->totalTileCount());
    }

    // Now figure out the count for tiles unique to this set
    quint32 ucount = 0;
    quint64 usize = 0;
    sq = QStringLiteral("SELECT COUNT(size), SUM(size) FROM Tiles WHERE tileID IN (SELECT A.tileID FROM SetTiles A join SetTiles B on A.tileID = B.tileID WHERE B.setID = %1 GROUP by A.tileID HAVING COUNT(A.tileID) = 1)").arg(set->id());
    if (subquery.exec(sq) && subquery.next()) {
        // This is only accurate when all tiles are downloaded
        ucount = subquery.value(0).toUInt();
        usize = subquery.value(1).toULongLong();
    }

    // If we haven't downloaded it all, estimate size of unique tiles
    quint32 expectedUcount = set->totalTileCount() - set->savedTileCount();
    if (ucount == 0) {
        usize = expectedUcount * avg;
    } else {
        expectedUcount = ucount;
    }
    set->setUniqueTileCount(expectedUcount);
    set->setUniqueTileSize(usize);
}

void QUAVCacheWorker::m_updateTotals()
{
    QSqlQuery query(*m_db);
    QString s = QStringLiteral("SELECT COUNT(size), SUM(size) FROM Tiles");
 //   qDebug() << s;
    if (query.exec(s) && query.next()) {
        m_totalCount = query.value(0).toUInt();
        m_totalSize  = query.value(1).toULongLong();
    }

    s = QStringLiteral("SELECT COUNT(size), SUM(size) FROM Tiles WHERE tileID IN (SELECT A.tileID FROM SetTiles A join SetTiles B on A.tileID = B.tileID WHERE B.setID = %1 GROUP by A.tileID HAVING COUNT(A.tileID) = 1)").arg(m_getDefaultTileSet());
//    qDebug() << s;
    if (query.exec(s) && query.next()) {
        m_defaultCount = query.value(0).toUInt();
        m_defaultSize = query.value(1).toULongLong();
    }

    emit updateTotals(m_totalCount, m_totalSize, m_defaultCount, m_defaultSize);
    if (!m_updateTimer.isValid()) {
        m_updateTimer.start();
    } else {
        (void) m_updateTimer.restart();
    }
}

quint64 QUAVCacheWorker::m_findTile(const QString &hash)
{
    quint64 tileID = 0;

    QSqlQuery query(*m_db);
    const QString s = QStringLiteral("SELECT tileID FROM Tiles WHERE hash = \"%1\"").arg(hash);
    if (query.exec(s) && query.next()) {
        tileID = query.value(0).toULongLong();
    }

    return tileID;
}

void QUAVCacheWorker::m_createTileSet(QUAVMapTask *mtask)
{
    if (!m_valid) {
        mtask->setError("Error saving tile set");
        return;
    }

    // Create Tile Set
    QUAVCreateTileSetTask *task = static_cast<QUAVCreateTileSetTask*>(mtask);
    QSqlQuery query(*m_db);
    (void) query.prepare("INSERT INTO TileSets("
        "name, typeStr, topleftLat, topleftLon, bottomRightLat, bottomRightLon, minZoom, maxZoom, type, numTiles, date"
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(task->tileSet()->name());
    query.addBindValue(task->tileSet()->mapTypeStr());
    query.addBindValue(task->tileSet()->topleftLat());
    query.addBindValue(task->tileSet()->topleftLon());
    query.addBindValue(task->tileSet()->bottomRightLat());
    query.addBindValue(task->tileSet()->bottomRightLon());
    query.addBindValue(task->tileSet()->minZoom());
    query.addBindValue(task->tileSet()->maxZoom());
    query.addBindValue(UrlFactory::getQtMapIdFromProviderType(task->tileSet()->type()));
    query.addBindValue(task->tileSet()->totalTileCount());
    query.addBindValue(QDateTime::currentSecsSinceEpoch());
    if (!query.exec()) {
//        qWarning() << "Map Cache SQL error (add tileSet into TileSets):" << query.lastError().text();
        mtask->setError("Error saving tile set");
        return;
    }

    // Get just created (auto-incremented) setID
    const quint64 setID = query.lastInsertId().toULongLong();
    task->tileSet()->setId(setID);
    // Prepare Download List
    (void) m_db->transaction();
    for (int z = task->tileSet()->minZoom(); z <= task->tileSet()->maxZoom(); z++) {
        const QUAVTileSet set = UrlFactory::getTileCount(z,
            task->tileSet()->topleftLon(), task->tileSet()->topleftLat(),
            task->tileSet()->bottomRightLon(), task->tileSet()->bottomRightLat(), task->tileSet()->type());
        const QString type = task->tileSet()->type();
        for (int x = set.tileX0; x <= set.tileX1; x++) {
            for (int y = set.tileY0; y <= set.tileY1; y++) {
                // See if tile is already downloaded
                const QString hash = UrlFactory::getTileHash(type, x, y, z);
                const quint64 tileID = m_findTile(hash);
                if (tileID == 0) {
                    // Set to download
                    (void) query.prepare("INSERT OR IGNORE INTO TilesDownload(setID, hash, type, x, y, z, state) VALUES(?, ?, ?, ?, ? ,? ,?)");
                    query.addBindValue(setID);
                    query.addBindValue(hash);
                    query.addBindValue(UrlFactory::getQtMapIdFromProviderType(type));
                    query.addBindValue(x);
                    query.addBindValue(y);
                    query.addBindValue(z);
                    query.addBindValue(0);
                    if (!query.exec()) {
//                        qWarning() << "Map Cache SQL error (add tile into TilesDownload):" << query.lastError().text();
                        mtask->setError("Error creating tile set download list");
                        return;
                    }
                } else {
                    // Tile already in the database. No need to dowload.
                    const QString s = QStringLiteral("INSERT OR IGNORE INTO SetTiles(tileID, setID) VALUES(%1, %2)").arg(tileID).arg(setID);
                    (void) query.prepare(s);
                    if (!query.exec()) {
//                        qWarning() << "Map Cache SQL error (add tile into SetTiles):" << query.lastError().text();
                    }
//                    qDebug() << "Already Cached HASH:" << hash;
                }
            }
        }
    }
    (void) m_db->commit();
    m_updateSetTotals(task->tileSet());
    task->setTileSetSaved();
}

void QUAVCacheWorker::m_getTileDownloadList(QUAVMapTask *mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QQueue<QUAVTile*> tiles;
    QUAVGetTileDownloadListTask *task = static_cast<QUAVGetTileDownloadListTask*>(mtask);
    QSqlQuery query(*m_db);
    QString s = QStringLiteral("SELECT hash, type, x, y, z FROM TilesDownload WHERE setID = %1 AND state = 0 LIMIT %2").arg(task->setID()).arg(task->count());
    if (query.exec(s)) {
        while (query.next()) {
            QUAVTile *tile = new QUAVTile;
            // tile->setTileSet(task->setID());
            tile->setHash(query.value("hash").toString());
            tile->setType(UrlFactory::getProviderTypeFromQtMapId(query.value("type").toInt()));
            tile->setX(query.value("x").toInt());
            tile->setY(query.value("y").toInt());
            tile->setZ(query.value("z").toInt());
            tiles.enqueue(tile);
        }

        for (int i = 0; i < tiles.size(); i++) {
            s = QStringLiteral("UPDATE TilesDownload SET state = %1 WHERE setID = %2 and hash = \"%3\"").arg(static_cast<int>(QUAVTile::StateDownloading)).arg(task->setID()).arg(tiles[i]->hash());
            if (!query.exec(s)) {
//                qWarning() << "Map Cache SQL error (set TilesDownload state):" << query.lastError().text();
            }
        }
    }
    task->setTileListFetched(tiles);
}

void QUAVCacheWorker::m_updateTileDownloadState(QUAVMapTask *mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QUAVUpdateTileDownloadStateTask *task = static_cast<QUAVUpdateTileDownloadStateTask*>(mtask);
    QSqlQuery query(*m_db);
    QString s;
    if (task->state() == QUAVTile::StateComplete) {
        s = QStringLiteral("DELETE FROM TilesDownload WHERE setID = %1 AND hash = \"%2\"").arg(task->setID()).arg(task->hash());
    } else if (task->hash() == "*") {
        s = QStringLiteral("UPDATE TilesDownload SET state = %1 WHERE setID = %2").arg(static_cast<int>(task->state())).arg(task->setID());
    } else {
        s = QStringLiteral("UPDATE TilesDownload SET state = %1 WHERE setID = %2 AND hash = \"%3\"").arg(static_cast<int>(task->state())).arg(task->setID()).arg(task->hash());
    }

    if (!query.exec(s)) {
//        qWarning() << "Error:" << query.lastError().text();
    }
}

void QUAVCacheWorker::m_pruneCache(QUAVMapTask *mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QUAVPruneCacheTask *task = static_cast<QUAVPruneCacheTask*>(mtask);
    QSqlQuery query(*m_db);
    // Select tiles in default set only, sorted by oldest.
    QString s = QStringLiteral("SELECT tileID, size, hash FROM Tiles WHERE tileID IN (SELECT A.tileID FROM SetTiles A join SetTiles B on A.tileID = B.tileID WHERE B.setID = %1 GROUP by A.tileID HAVING COUNT(A.tileID) = 1) ORDER BY DATE ASC LIMIT 128").arg(m_getDefaultTileSet());
    if (!query.exec(s)) {
        return;
    }

    QList<quint64> tlist;
    qint64 amount = static_cast<qint64>(task->amount());
    while (query.next() && (amount >= 0)) {
        tlist << query.value(0).toULongLong();
        amount -= query.value(1).toULongLong();
//        qDebug() << "HASH:" << query.value(2).toString();
    }

    while (!tlist.isEmpty()) {
        s = QStringLiteral("DELETE FROM Tiles WHERE tileID = %1").arg(tlist[0]);
        tlist.removeFirst();
        if (!query.exec(s)) {
            break;
        }
    }

    task->setPruned();
}

void QUAVCacheWorker::m_deleteTileSet(QUAVMapTask *mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QUAVDeleteTileSetTask *task = static_cast<QUAVDeleteTileSetTask*>(mtask);
    m_deleteTileSet(task->setID());
    task->setTileSetDeleted();
}

void QUAVCacheWorker::m_deleteTileSet(qulonglong id)
{
    QSqlQuery query(*m_db);
    // Only delete tiles unique to this set
    QString  s = QStringLiteral("DELETE FROM Tiles WHERE tileID IN (SELECT A.tileID FROM SetTiles A JOIN SetTiles B ON A.tileID = B.tileID WHERE B.setID = %1 GROUP BY A.tileID HAVING COUNT(A.tileID) = 1)").arg(id);
    (void) query.exec(s);
    s = QStringLiteral("DELETE FROM TilesDownload WHERE setID = %1").arg(id);
    (void) query.exec(s);
    s = QStringLiteral("DELETE FROM TileSets WHERE setID = %1").arg(id);
    (void) query.exec(s);
    s = QStringLiteral("DELETE FROM SetTiles WHERE setID = %1").arg(id);
    (void) query.exec(s);
    m_updateTotals();
}

void QUAVCacheWorker::m_renameTileSet(QUAVMapTask *mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QUAVRenameTileSetTask *task = static_cast<QUAVRenameTileSetTask*>(mtask);
    QSqlQuery query(*m_db);
    const QString s = QStringLiteral("UPDATE TileSets SET name = \"%1\" WHERE setID = %2").arg(task->newName()).arg(task->setID());
    if (!query.exec(s)) {
        task->setError("Error renaming tile set");
    }
}

void QUAVCacheWorker::m_resetCacheDatabase(QUAVMapTask *mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QUAVResetTask *task = static_cast<QUAVResetTask*>(mtask);
    QSqlQuery query(*m_db);
    QString s = QStringLiteral("DROP TABLE Tiles");
    (void) query.exec(s);
    s = QStringLiteral("DROP TABLE TileSets");
    (void) query.exec(s);
    s = QStringLiteral("DROP TABLE SetTiles");
    (void) query.exec(s);
    s = QStringLiteral("DROP TABLE TilesDownload");
    (void) query.exec(s);
    m_valid = m_createDB(*m_db);
    task->setResetCompleted();
}

void QUAVCacheWorker::m_importSets(QUAVMapTask *mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QUAVImportTileTask *task = static_cast<QUAVImportTileTask*>(mtask);
    // If replacing, simply copy over it
    if (task->replace()) {
        // Close and delete old database
        m_disconnectDB();
        (void) QFile::remove(m_databasePath);
        // Copy given database
        (void) QFile::copy(task->path(), m_databasePath);
        task->setProgress(25);
        m_init();
        if (m_valid) {
            task->setProgress(50);
            m_connectDB();
        }
        task->setProgress(100);
    } else {
        // Open imported set
        QSqlDatabase *dbImport = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", kExportSession));
        dbImport->setDatabaseName(task->path());
        dbImport->setConnectOptions("QSQLITE_ENABLE_SHARED_CACHE");
        if (dbImport->open()) {
            QSqlQuery query(*dbImport);
            // Prepare progress report
            quint64 tileCount = 0;
            int lastProgress = -1;
            QString s = QStringLiteral("SELECT COUNT(tileID) FROM Tiles");
            if (query.exec(s) && query.next()) {
                // Total number of tiles in imported database
                tileCount  = query.value(0).toULongLong();
            }

            if (tileCount > 0) {
                // Iterate Tile Sets
                s = QStringLiteral("SELECT * FROM TileSets ORDER BY defaultSet DESC, name ASC");
                if (query.exec(s)) {
                    quint64 currentCount = 0;
                    while (query.next()) {
                        QString name = query.value("name").toString();
                        const quint64 setID = query.value("setID").toULongLong();
                        const QString mapType = query.value("typeStr").toString();
                        const double topleftLat = query.value("topleftLat").toDouble();
                        const double topleftLon = query.value("topleftLon").toDouble();
                        const double bottomRightLat = query.value("bottomRightLat").toDouble();
                        const double bottomRightLon = query.value("bottomRightLon").toDouble();
                        const int minZoom = query.value("minZoom").toInt();
                        const int maxZoom = query.value("maxZoom").toInt();
                        const int type = query.value("type").toInt();
                        const quint32 numTiles = query.value("numTiles").toUInt();
                        const int defaultSet = query.value("defaultSet").toInt();
                        quint64 insertSetID = m_getDefaultTileSet();
                        // If not default set, create new one
                        if (defaultSet == 0) {
                            // Check if we have this tile set already
                            if (m_findTileSetID(name, insertSetID)) {
                                int testCount = 0;
                                // Set with this name already exists. Make name unique.
                                while (true) {
                                    const QString testName = QString::asprintf("%s %02d", name.toLatin1().constData(), ++testCount);
                                    if (!m_findTileSetID(testName, insertSetID) || (testCount > 99)) {
                                        name = testName;
                                        break;
                                    }
                                }
                            }
                            // Create new set
                            QSqlQuery cQuery(*m_db);
                            (void) cQuery.prepare("INSERT INTO TileSets("
                                "name, typeStr, topleftLat, topleftLon, bottomRightLat, bottomRightLon, minZoom, maxZoom, type, numTiles, defaultSet, date"
                                ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                            cQuery.addBindValue(name);
                            cQuery.addBindValue(mapType);
                            cQuery.addBindValue(topleftLat);
                            cQuery.addBindValue(topleftLon);
                            cQuery.addBindValue(bottomRightLat);
                            cQuery.addBindValue(bottomRightLon);
                            cQuery.addBindValue(minZoom);
                            cQuery.addBindValue(maxZoom);
                            cQuery.addBindValue(type);
                            cQuery.addBindValue(numTiles);
                            cQuery.addBindValue(defaultSet);
                            cQuery.addBindValue(QDateTime::currentSecsSinceEpoch());
                            if (!cQuery.exec()) {
                                task->setError("Error adding imported tile set to database");
                                break;
                            } else {
                                // Get just created (auto-incremented) setID
                                insertSetID = cQuery.lastInsertId().toULongLong();
                            }
                        }

                        // Find set tiles
                        QSqlQuery cQuery(*m_db);
                        QSqlQuery subQuery(*dbImport);
                        const QString sb = QStringLiteral("SELECT * FROM Tiles WHERE tileID IN (SELECT A.tileID FROM SetTiles A JOIN SetTiles B ON A.tileID = B.tileID WHERE B.setID = %1 GROUP BY A.tileID HAVING COUNT(A.tileID) = 1)").arg(setID);
                        if (subQuery.exec(sb)) {
                            quint64 tilesFound = 0;
                            quint64 tilesSaved = 0;
                            (void) m_db->transaction();
                            while (subQuery.next()) {
                                tilesFound++;
                                const QString hash = subQuery.value("hash").toString();
                                const QString format = subQuery.value("format").toString();
                                const QByteArray img = subQuery.value("tile").toByteArray();
                                const int type = subQuery.value("type").toInt();
                                // Save tile
                                (void) cQuery.prepare("INSERT INTO Tiles(hash, format, tile, size, type, date) VALUES(?, ?, ?, ?, ?, ?)");
                                cQuery.addBindValue(hash);
                                cQuery.addBindValue(format);
                                cQuery.addBindValue(img);
                                cQuery.addBindValue(img.size());
                                cQuery.addBindValue(type);
                                cQuery.addBindValue(QDateTime::currentSecsSinceEpoch());
                                if (cQuery.exec()) {
                                    tilesSaved++;
                                    const quint64 importTileID = cQuery.lastInsertId().toULongLong();
                                    const QString s2 = QStringLiteral("INSERT INTO SetTiles(tileID, setID) VALUES(%1, %2)").arg(importTileID).arg(insertSetID);
                                    (void) cQuery.prepare(s2);
                                    (void) cQuery.exec();
                                    currentCount++;
                                    if (tileCount > 0) {
                                        const int progress = static_cast<int>((static_cast<double>(currentCount) / static_cast<double>(tileCount)) * 100.0);
                                        // Avoid calling this if (int) progress hasn't changed.
                                        if (lastProgress != progress) {
                                            lastProgress = progress;
                                            task->setProgress(progress);
                                        }
                                    }
                                }
                            }

                            (void) m_db->commit();
                            if (tilesSaved > 0) {
                                // Update tile count (if any added)
                                s = QStringLiteral("SELECT COUNT(size) FROM Tiles A INNER JOIN SetTiles B on A.tileID = B.tileID WHERE B.setID = %1").arg(insertSetID);
                                if (cQuery.exec(s) && cQuery.next()) {
                                    const quint64 count = cQuery.value(0).toULongLong();
                                    s = QStringLiteral("UPDATE TileSets SET numTiles = %1 WHERE setID = %2").arg(count).arg(insertSetID);
                                    (void) cQuery.exec(s);
                                }
                            }

                            const qint64 uniqueTiles = tilesFound - tilesSaved;
                            if (static_cast<quint64>(uniqueTiles) < tileCount) {
                                tileCount -= uniqueTiles;
                            } else {
                                tileCount = 0;
                            }

                            // If there was nothing new in this set, remove it.
                            if ((tilesSaved == 0) && (defaultSet == 0)) {
//                                qDebug() << "No unique tiles in" << name << "Removing it.";
                                m_deleteTileSet(insertSetID);
                            }
                        }
                    }
                } else {
                    task->setError("No tile set in database");
                }
            }
            delete dbImport;
            QSqlDatabase::removeDatabase(kExportSession);
            if (tileCount == 0) {
                task->setError("No unique tiles in imported database");
            }
        } else {
            task->setError("Error opening import database");
        }
    }
    task->setImportCompleted();
}

void QUAVCacheWorker::m_exportSets(QUAVMapTask *mtask)
{
    if (!m_testTask(mtask)) {
        return;
    }

    QUAVExportTileTask *task = static_cast<QUAVExportTileTask*>(mtask);
    // Delete target if it exists
    (void) QFile::remove(task->path());
    // Create exported database
    QScopedPointer<QSqlDatabase> dbExport(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", kExportSession)));
    dbExport->setDatabaseName(task->path());
    dbExport->setConnectOptions("QSQLITE_ENABLE_SHARED_CACHE");
    if (dbExport->open()) {
        if (m_createDB(*dbExport, false)) {
            // Prepare progress report
            quint64 tileCount = 0;
            quint64 currentCount = 0;
            for (int i = 0; i < task->sets().count(); i++) {
                const QUAVCachedTileSet *set = task->sets().at(i);
                // Default set has no unique tiles
                if (set->defaultSet()) {
                    tileCount += set->totalTileCount();
                } else {
                    tileCount += set->uniqueTileCount();
                }
            }

            if (tileCount == 0) {
                tileCount = 1;
            }

            // Iterate sets to save
            for (int i = 0; i < task->sets().count(); i++) {
                const QUAVCachedTileSet *set = task->sets().at(i);
                // Create Tile Exported Set
                QSqlQuery exportQuery(*dbExport);
                (void) exportQuery.prepare("INSERT INTO TileSets("
                    "name, typeStr, topleftLat, topleftLon, bottomRightLat, bottomRightLon, minZoom, maxZoom, type, numTiles, defaultSet, date"
                    ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                exportQuery.addBindValue(set->name());
                exportQuery.addBindValue(set->mapTypeStr());
                exportQuery.addBindValue(set->topleftLat());
                exportQuery.addBindValue(set->topleftLon());
                exportQuery.addBindValue(set->bottomRightLat());
                exportQuery.addBindValue(set->bottomRightLon());
                exportQuery.addBindValue(set->minZoom());
                exportQuery.addBindValue(set->maxZoom());
                exportQuery.addBindValue(UrlFactory::getQtMapIdFromProviderType(set->type()));
                exportQuery.addBindValue(set->totalTileCount());
                exportQuery.addBindValue(set->defaultSet());
                exportQuery.addBindValue(QDateTime::currentSecsSinceEpoch());
                if (!exportQuery.exec()) {
                    task->setError("Error adding tile set to exported database");
                    break;
                }

                // Get just created (auto-incremented) setID
                const quint64 exportSetID = exportQuery.lastInsertId().toULongLong();
                // Find set tiles
                QString s = QStringLiteral("SELECT * FROM SetTiles WHERE setID = %1").arg(set->id());
                QSqlQuery query(*m_db);
                if (!query.exec(s)) {
                    continue;
                }

                (void) dbExport->transaction();
                while (query.next()) {
                    const quint64 tileID = query.value("tileID").toULongLong();
                    // Get tile
                    s = QStringLiteral("SELECT * FROM Tiles WHERE tileID = \"%1\"").arg(tileID);
                    QSqlQuery subQuery(*m_db);
                    if (!subQuery.exec(s) || !subQuery.next()) {
                        continue;
                    }

                    const QString hash = subQuery.value("hash").toString();
                    const QString format = subQuery.value("format").toString();
                    const QByteArray img = subQuery.value("tile").toByteArray();
                    const int type = subQuery.value("type").toInt();
                    // Save tile
                    (void) exportQuery.prepare("INSERT INTO Tiles(hash, format, tile, size, type, date) VALUES(?, ?, ?, ?, ?, ?)");
                    exportQuery.addBindValue(hash);
                    exportQuery.addBindValue(format);
                    exportQuery.addBindValue(img);
                    exportQuery.addBindValue(img.size());
                    exportQuery.addBindValue(type);
                    exportQuery.addBindValue(QDateTime::currentSecsSinceEpoch());
                    if (!exportQuery.exec()) {
                        continue;
                    }

                    const quint64 exportTileID = exportQuery.lastInsertId().toULongLong();
                    s = QStringLiteral("INSERT INTO SetTiles(tileID, setID) VALUES(%1, %2)").arg(exportTileID).arg(exportSetID);
                    (void) exportQuery.prepare(s);
                    (void) exportQuery.exec();
                    currentCount++;
                    task->setProgress(static_cast<int>((static_cast<double>(currentCount) / static_cast<double>(tileCount)) * 100.0));
                }
                (void) dbExport->commit();
            }
        } else {
            task->setError("Error creating export database");
        }
    } else {
//        qCCritical(QUAVTileCacheWorkerLog) << "Map Cache SQL error (create export database):" << dbExport->lastError();
        task->setError("Error opening export database");
    }
    dbExport.reset();
    QSqlDatabase::removeDatabase(kExportSession);
    task->setExportCompleted();
}

bool QUAVCacheWorker::m_testTask(QUAVMapTask *mtask)
{
    if (!m_valid) {
        mtask->setError("No Cache Database");
        return false;
    }

    return true;
}

bool QUAVCacheWorker::m_init()
{
    m_failed = false;
    if (!m_databasePath.isEmpty()) {
//        qDebug() << "Mapping cache directory:" << _databasePath;
        // Initialize Database
        if (m_connectDB()) {
            m_valid = m_createDB(*m_db);
            if (!m_valid) {
                m_failed = true;
            }
        } else {
//            qCritical() << "Map Cache SQL error (open db):" << _db->lastError();
            m_failed = true;
        }
        m_disconnectDB();
    } else {
//        qCritical() << "Could not find suitable cache directory.";
        m_failed = true;
    }

    return !m_failed;
}

bool QUAVCacheWorker::m_connectDB()
{
    (void) m_db.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", kSession)));
    m_db->setDatabaseName(m_databasePath);
    m_db->setConnectOptions("QSQLITE_ENABLE_SHARED_CACHE");
    m_valid = m_db->open();
    return m_valid;
}

bool QUAVCacheWorker::m_createDB(QSqlDatabase &db, bool createDefault)
{
    bool res = false;
    QSqlQuery query(db);
    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS Tiles ("
        "tileID INTEGER PRIMARY KEY NOT NULL, "
        "hash TEXT NOT NULL UNIQUE, "
        "format TEXT NOT NULL, "
        "tile BLOB NULL, "
        "size INTEGER, "
        "type INTEGER, "
        "date INTEGER DEFAULT 0)"))
    {
//        qWarning() << "Map Cache SQL error (create Tiles db):" << query.lastError().text();
    } else {
        (void) query.exec("CREATE INDEX IF NOT EXISTS hash ON Tiles ( hash, size, type ) ");
        if (!query.exec(
            "CREATE TABLE IF NOT EXISTS TileSets ("
            "setID INTEGER PRIMARY KEY NOT NULL, "
            "name TEXT NOT NULL UNIQUE, "
            "typeStr TEXT, "
            "topleftLat REAL DEFAULT 0.0, "
            "topleftLon REAL DEFAULT 0.0, "
            "bottomRightLat REAL DEFAULT 0.0, "
            "bottomRightLon REAL DEFAULT 0.0, "
            "minZoom INTEGER DEFAULT 3, "
            "maxZoom INTEGER DEFAULT 3, "
            "type INTEGER DEFAULT -1, "
            "numTiles INTEGER DEFAULT 0, "
            "defaultSet INTEGER DEFAULT 0, "
            "date INTEGER DEFAULT 0)"))
        {
        //    qWarning() << "Map Cache SQL error (create TileSets db):" << query.lastError().text();
        } else if (!query.exec(
            "CREATE TABLE IF NOT EXISTS SetTiles ("
            "setID INTEGER, "
            "tileID INTEGER)")) {
        //    qWarning() << "Map Cache SQL error (create SetTiles db):" << query.lastError().text();
        } else if (!query.exec(
            "CREATE TABLE IF NOT EXISTS TilesDownload ("
            "setID INTEGER, "
            "hash TEXT NOT NULL UNIQUE, "
            "type INTEGER, "
            "x INTEGER, "
            "y INTEGER, "
            "z INTEGER, "
            "state INTEGER DEFAULT 0)")) {
        //    qWarning() << "Map Cache SQL error (create TilesDownload db):" << query.lastError().text();
        } else {
            // Database it ready for use
            res = true;
        }
    }

    // Create default tile set
    if (res && createDefault) {
        const QString s = QString("SELECT name FROM TileSets WHERE name = \"%1\"").arg("Default Tile Set");
        if (query.exec(s)) {
            if (!query.next()) {
                (void) query.prepare("INSERT INTO TileSets(name, defaultSet, date) VALUES(?, ?, ?)");
                query.addBindValue("Default Tile Set");
                query.addBindValue(1);
                query.addBindValue(QDateTime::currentSecsSinceEpoch());
                if (!query.exec()) {
                //    qWarning() << "Map Cache SQL error (Creating default tile set):" << db.lastError();
                    res = false;
                }
            }
        } else {
        //    qWarning() << "Map Cache SQL error (Looking for default tile set):" << db.lastError();
        }
    }

    if (!res) {
        (void) QFile::remove(m_databasePath);
    }

    return res;
}

void QUAVCacheWorker::m_disconnectDB()
{
    if (m_db) {
        m_db.reset();
        QSqlDatabase::removeDatabase(kSession);
    }
}
