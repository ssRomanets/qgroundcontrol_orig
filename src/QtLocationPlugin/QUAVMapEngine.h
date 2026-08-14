

/**
 * @file
 *   @brief Map Tile Cache
 *
 *   @author Gus Grubba <gus@auterion.com>
 *
 */

#pragma once

#include <QtCore/QString>
#include <QtCore/QObject>

class QUAVMapTask;
class QUAVCacheWorker;

class QUAVMapEngine : public QObject
{
    Q_OBJECT

public:
    explicit QUAVMapEngine(QObject *parent = nullptr);
    ~QUAVMapEngine();

    void init(const QString &databasePath);
    bool addTask(QUAVMapTask *task);

    static QUAVMapEngine *instance();

signals:
    void updateTotals(quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize);

private slots:
    void m_updateTotals(quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize);
    void m_pruned() { m_prunning = false; }

private:
    QUAVCacheWorker* m_worker = nullptr;
    bool m_prunning = false;
};

extern QUAVMapEngine* getQUAVMapEngine();
