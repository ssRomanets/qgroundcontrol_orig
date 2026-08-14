

/**
 * @file
 *   @brief Map Tile Cache Data
 *
 *   @author Gus Grubba <gus@auterion.com>
 *
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QString>

#include "QUAVTile.h"
#include "QUAVCacheTile.h"
#include "QUAVCachedTileSet.h"

class QUAVMapTask : public QObject
{
    Q_OBJECT

public:
    enum TaskType {
        taskInit,
        taskCacheTile,
        taskFetchTile,
        taskFetchTileSets,
        taskCreateTileSet,
        taskGetTileDownloadList,
        taskUpdateTileDownloadState,
        taskDeleteTileSet,
        taskRenameTileSet,
        taskPruneCache,
        taskReset,
        taskExport,
        taskImport
    };
    Q_ENUM(TaskType);

    explicit QUAVMapTask(TaskType type, QObject *parent = nullptr)
        : QObject(parent)
        , m_type(type)
    {}
    virtual ~QUAVMapTask() = default;

    TaskType type() const { return m_type; }

    void setError(const QString &errorString = QString())
    {
        emit error(m_type, errorString);
    }

signals:
    void error(QUAVMapTask::TaskType type, const QString &errorString);

private:
    const TaskType m_type = TaskType::taskInit;
};

//-----------------------------------------------------------------------------

class QUAVFetchTileSetTask : public QUAVMapTask
{
    Q_OBJECT

public:
    QUAVFetchTileSetTask(QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskFetchTileSets, parent)
    {}
    ~QUAVFetchTileSetTask() = default;

    void setTileSetFetched(QUAVCachedTileSet *tileSet)
    {
        emit tileSetFetched(tileSet);
    }

signals:
    void tileSetFetched(QUAVCachedTileSet *tileSet);
};

//-----------------------------------------------------------------------------

class QUAVCreateTileSetTask : public QUAVMapTask
{
    Q_OBJECT

public:
    explicit QUAVCreateTileSetTask(QUAVCachedTileSet *tileSet, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskCreateTileSet, parent)
        , m_tileSet(tileSet)
        , m_saved(false)
    {}
    ~QUAVCreateTileSetTask()
    {
        if (!m_saved) {
            delete m_tileSet;
        }
    }

    QUAVCachedTileSet *tileSet() { return m_tileSet; }

    void setTileSetSaved()
    {
        m_saved = true;
        emit tileSetSaved(m_tileSet);
    }

signals:
    void tileSetSaved(QUAVCachedTileSet *tileSet);

private:
    QUAVCachedTileSet* const m_tileSet = nullptr;
    bool m_saved = false;
};

//-----------------------------------------------------------------------------

class QUAVFetchTileTask : public QUAVMapTask
{
    Q_OBJECT

public:
    explicit QUAVFetchTileTask(const QString &hash, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskFetchTile, parent)
        , m_hash(hash)
    {}
    ~QUAVFetchTileTask() = default;

    void setTileFetched(QUAVCacheTile *tile)
    {
        emit tileFetched(tile);
    }

    QString hash() const { return m_hash; }

signals:
    void tileFetched(QUAVCacheTile *tile);

private:
    const QString m_hash;
};

//-----------------------------------------------------------------------------

class QUAVSaveTileTask : public QUAVMapTask
{
    Q_OBJECT

public:
    explicit QUAVSaveTileTask(QUAVCacheTile *tile, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskCacheTile, parent)
        , m_tile(tile)
    {}
    ~QUAVSaveTileTask()
    {
        delete m_tile;
    }

    QUAVCacheTile *tile() { return m_tile; }

private:
    QUAVCacheTile* const m_tile = nullptr;
};

//-----------------------------------------------------------------------------

class QUAVGetTileDownloadListTask : public QUAVMapTask
{
    Q_OBJECT

public:
    QUAVGetTileDownloadListTask(quint64 setID, int count, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskGetTileDownloadList, parent)
        , m_setID(setID)
        , m_count(count)
    {}
    ~QUAVGetTileDownloadListTask() = default;

    quint64 setID() const { return m_setID; }
    int count() const { return m_count; }

    void setTileListFetched(const QQueue<QUAVTile*> &tiles)
    {
        emit tileListFetched(tiles);
    }

signals:
    void tileListFetched(QQueue<QUAVTile*> tiles);

private:
    const quint64 m_setID = 0;
    const int m_count = 0;
};

//-----------------------------------------------------------------------------

class QUAVUpdateTileDownloadStateTask : public QUAVMapTask
{
    Q_OBJECT

public:
    QUAVUpdateTileDownloadStateTask(quint64 setID, QUAVTile::TileState state, const QString &hash, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskUpdateTileDownloadState, parent)
        , m_setID(setID)
        , m_state(state)
        , m_hash(hash)
    {}
    ~QUAVUpdateTileDownloadStateTask() = default;

    QString hash() const { return m_hash; }
    quint64 setID() const { return m_setID; }
    QUAVTile::TileState state() const { return m_state; }

private:
    const quint64 m_setID = 0;
    const QUAVTile::TileState m_state = QUAVTile::StatePending;
    const QString m_hash;
};

//-----------------------------------------------------------------------------

class QUAVDeleteTileSetTask : public QUAVMapTask
{
    Q_OBJECT

public:
    explicit QUAVDeleteTileSetTask(quint64 setID, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskDeleteTileSet, parent)
        , m_setID(setID)
    {}
    ~QUAVDeleteTileSetTask() = default;

    quint64 setID() const { return m_setID; }

    void setTileSetDeleted()
    {
        emit tileSetDeleted(m_setID);
    }

signals:
    void tileSetDeleted(quint64 setID);

private:
    const quint64 m_setID = 0;
};

//-----------------------------------------------------------------------------

class QUAVRenameTileSetTask : public QUAVMapTask
{
    Q_OBJECT

public:
    QUAVRenameTileSetTask(quint64 setID, const QString &newName, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskRenameTileSet, parent)
        , m_setID(setID)
        , m_newName(newName)
    {}
    ~QUAVRenameTileSetTask() = default;

    quint64 setID() const { return m_setID; }
    QString newName() const { return m_newName; }

private:
    const quint64 m_setID = 0;
    const QString m_newName;
};

//-----------------------------------------------------------------------------

class QUAVPruneCacheTask : public QUAVMapTask
{
    Q_OBJECT

public:
    explicit QUAVPruneCacheTask(quint64 amount, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskPruneCache, parent)
        , m_amount(amount)
    {}
    ~QUAVPruneCacheTask() = default;

    quint64 amount() const { return m_amount; }

    void setPruned()
    {
        emit pruned();
    }

signals:
    void pruned();

private:
    const quint64 m_amount = 0;
};

//-----------------------------------------------------------------------------

class QUAVResetTask : public QUAVMapTask
{
    Q_OBJECT

public:
    QUAVResetTask(QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskReset, parent)
    {}
    ~QUAVResetTask() = default;

    void setResetCompleted()
    {
        emit resetCompleted();
    }

signals:
    void resetCompleted();
};

//-----------------------------------------------------------------------------

class QUAVExportTileTask : public QUAVMapTask
{
    Q_OBJECT

public:
    explicit QUAVExportTileTask(const QVector<QUAVCachedTileSet*> &sets, const QString &path, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskExport, parent)
        , m_sets(sets)
        , m_path(path)
    {}
    ~QUAVExportTileTask() = default;

    QVector<QUAVCachedTileSet*> sets() const { return m_sets; }
    QString path() const { return m_path; }

    void setExportCompleted()
    {
        emit actionCompleted();
    }

    void setProgress(int percentage)
    {
        emit actionProgress(percentage);
    }

signals:
    void actionCompleted();
    void actionProgress(int percentage);

private:
    const QVector<QUAVCachedTileSet*> m_sets;
    const QString m_path;
};

//-----------------------------------------------------------------------------

class QUAVImportTileTask : public QUAVMapTask
{
    Q_OBJECT

public:
    QUAVImportTileTask(const QString &path, bool replace, QObject *parent = nullptr)
        : QUAVMapTask(QUAVMapTask::taskImport, parent)
        , m_path(path)
        , m_replace(replace)
    {}
    ~QUAVImportTileTask() = default;

    QString path() const { return m_path; }
    bool replace() const { return m_replace; }

    void setImportCompleted()
    {
        emit actionCompleted();
    }

    void setProgress(int percentage)
    {
        emit actionProgress(percentage);
    }

signals:
    void actionCompleted();
    void actionProgress(int percentage);

private:
    const QString m_path;
    const bool m_replace = false;
};

//-----------------------------------------------------------------------------
