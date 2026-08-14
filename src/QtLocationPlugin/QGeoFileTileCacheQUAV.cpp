
#include "QGeoFileTileCacheQUAV.h"
#include "QUAVMapEngine.h"
#include "QUAVApplication.h"
#include "SettingsManager.h"
#include "AppSettings.h"
#include "MapsSettings.h"
#include "QUAVMapUrlEngine.h"
#include "QUAVMapTasks.h"

#include <QtCore/QStandardPaths>
#include <QtCore/QLoggingCategory>
#include <QtCore/QDir>

QString QGeoFileTileCacheQUAV::m_databaseFilePath;
QString QGeoFileTileCacheQUAV::m_cachePath;
bool QGeoFileTileCacheQUAV::m_cacheWasReset = false;

QGeoFileTileCacheQUAV::QGeoFileTileCacheQUAV(const QVariantMap &parameters, QObject *parent)
    : QGeoFileTileCache(baseCacheDirectory(), parent)
{
    // qDebug() << Q_FUNC_INFO << this;

    setCostStrategyDisk(QGeoFileTileCache::ByteSize);
    setMaxDiskUsage(m_getDefaultMaxDiskCache());
    setCostStrategyMemory(QGeoFileTileCache::ByteSize);
    setMaxMemoryUsage(m_getMemLimit(parameters));
    setCostStrategyTexture(QGeoFileTileCache::ByteSize);
    setMinTextureUsage(m_getDefaultMinTexture());
    setExtraTextureUsage(m_getDefaultExtraTexture() - minTextureUsage());

    static std::once_flag cacheInit;
    std::call_once(cacheInit, [this]() {
        m_initCache();
    });

    directory_ = m_getCachePath(parameters);
}

QGeoFileTileCacheQUAV::~QGeoFileTileCacheQUAV()
{
#ifdef QT_DEBUG
    // printStats();
#endif

    // qDebug() << Q_FUNC_INFO << this;
}

uint32_t QGeoFileTileCacheQUAV::m_getMemLimit(const QVariantMap &parameters)
{
    uint32_t memLimit = 0;
    if (parameters.contains(QStringLiteral("mapping.cache.memory.size"))) {
        bool ok = false;
        memLimit = parameters.value(QStringLiteral("mapping.cache.memory.size")).toString().toUInt(&ok);
        if (!ok) {
            memLimit = 0;
        }
    }

    if (memLimit == 0) {
        // Value saved in MB
        memLimit = m_getMaxMemCacheSetting() * pow(1024, 2);
    }
    if (memLimit == 0) {
        memLimit = m_getDefaultMaxMemLimit();
    }
    // 1MB Minimum Memory Cache Required
    if (memLimit < pow(1024, 2)) {
        memLimit = pow(1024, 2);
    }
    // MaxMemoryUsage is 32bit Integer, Round down to 1GB
    if (memLimit > pow(1024, 3)) {
        memLimit = pow(1024, 3);
    }

    return memLimit;
}

quint32 QGeoFileTileCacheQUAV::m_getMaxMemCacheSetting()
{
    return SettingsManager::instance()->mapsSettings()->maxCacheMemorySize()->rawValue().toUInt();
}

quint32 QGeoFileTileCacheQUAV::getMaxDiskCacheSetting()
{
    return SettingsManager::instance()->mapsSettings()->maxCacheDiskSize()->rawValue().toUInt();
}

void QGeoFileTileCacheQUAV::cacheTile(const QString &type, int x, int y, int z, const QByteArray &image, const QString &format, qulonglong set)
{
    const QString hash = UrlFactory::getTileHash(type, x, y, z);
    cacheTile(type, hash, image, format, set);
}

void QGeoFileTileCacheQUAV::cacheTile(const QString &type, const QString &hash, const QByteArray &image, const QString &format, qulonglong set)
{
    AppSettings* const appSettings = SettingsManager::instance()->appSettings();
    if (!appSettings->disableAllPersistence()->rawValue().toBool()) {
        QUAVCacheTile* const tile = new QUAVCacheTile(hash, image, format, type, set);
        QUAVSaveTileTask* const task = new QUAVSaveTileTask(tile);
        (void) getQUAVMapEngine()->addTask(task);
    }
}

QUAVFetchTileTask* QGeoFileTileCacheQUAV::createFetchTileTask(const QString &type, int x, int y, int z)
{
    const QString hash = UrlFactory::getTileHash(type, x, y, z);
    QUAVFetchTileTask* const task = new QUAVFetchTileTask(hash);
    return task;
}

QString QGeoFileTileCacheQUAV::m_getCachePath(const QVariantMap &parameters)
{
    QString cacheDir;
    if (parameters.contains(QStringLiteral("mapping.cache.directory"))) {
        cacheDir = parameters.value(QStringLiteral("mapping.cache.directory")).toString();
    } else {
        cacheDir = m_cachePath + QLatin1String("/providers");
        if (!QFileInfo::exists(cacheDir)) {
            if (!QDir::root().mkpath(cacheDir)) {
//                qWarning() << "Could not create mapping disk cache directory:" << cacheDir;
                cacheDir = QDir::homePath() + QStringLiteral("/.qgcmapscache/");
            }
        }
    }

    if (!QFileInfo::exists(cacheDir)) {
        if (!QDir::root().mkpath(cacheDir)) {
        //    qWarning() << "Could not create mapping disk cache directory:" << cacheDir;
            cacheDir.clear();
        }
    }

    return cacheDir;
}

bool QGeoFileTileCacheQUAV::m_wipeDirectory(const QString &dirPath)
{
    bool result = true;

    const QDir dir(dirPath);
    if (dir.exists(dirPath)) {
        m_cacheWasReset = true;

        const QFileInfoList fileList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
        for (const QFileInfo &info : fileList) {
            if (info.isDir()) {
                result = m_wipeDirectory(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirPath);
    }

    return result;
}

void QGeoFileTileCacheQUAV::m_wipeOldCaches()
{
    const QStringList oldCaches = {"/QGCMapCache55", "/QGCMapCache100"};
    for (const QString &cache : oldCaches) {
        QString oldCacheDir;
        oldCacheDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
        oldCacheDir += cache;
        m_wipeDirectory(oldCacheDir);
    }
}

void QGeoFileTileCacheQUAV::m_initCache()
{
    m_wipeOldCaches();

    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    cacheDir += QStringLiteral("/QGCMapCache") + QString(kCachePathVersion);
    if (!QDir::root().mkpath(cacheDir)) {
//        qWarning() << "Could not create mapping disk cache directory:" << cacheDir;

        cacheDir = QDir::homePath() + QStringLiteral("/.qgcmapscache/");
        if (!QDir::root().mkpath(cacheDir)) {
//            qWarning() << "Could not create mapping disk cache directory:" << cacheDir;
            cacheDir.clear();
        }
    }

    m_cachePath = cacheDir;
    if (!m_cachePath.isEmpty()) {
        m_databaseFilePath = QString(m_cachePath + QStringLiteral("/qgcMapCache.db"));

    //    qDebug() << "Map Cache in:" << _databaseFilePath;
    } else {
    //    qCritical() << "Could not find suitable map cache directory.";
    }

    if (m_cacheWasReset) {
        quavApp()->showAppMessage(tr(
            "The Offline Map Cache database has been upgraded. "
            "Your old map cache sets have been reset."));
    }
}
