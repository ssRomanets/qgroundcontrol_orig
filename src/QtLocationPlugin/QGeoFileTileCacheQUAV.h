
#pragma once

#include <QtLocation/private/qgeofiletilecache_p.h>

class QUAVFetchTileTask;

class QGeoFileTileCacheQUAV : public QGeoFileTileCache
{
    Q_OBJECT

public:
    explicit QGeoFileTileCacheQUAV(const QVariantMap &parameters, QObject *parent = nullptr);
    ~QGeoFileTileCacheQUAV();

    static quint32 getMaxDiskCacheSetting();
    static void cacheTile(const QString &type, int x, int y, int z, const QByteArray &image, const QString &format, qulonglong set = UINT64_MAX);
    static void cacheTile(const QString &type, const QString &hash, const QByteArray &image, const QString &format, qulonglong set = UINT64_MAX);
    static QUAVFetchTileTask *createFetchTileTask(const QString &type, int x, int y, int z);
    static QString getDatabaseFilePath() { return m_databaseFilePath; }
    static QString getCachePath()        { return m_cachePath; }

private:
    static void m_initCache();
    static bool m_wipeDirectory(const QString &dirPath);
    static void m_wipeOldCaches();

    static QString  m_getCachePath(const QVariantMap &parameters);
    static uint32_t m_getMemLimit(const QVariantMap &Parameters);

    static uint32_t m_getDefaultMaxMemLimit() { return (3 * pow(1024, 2)); }
    static uint32_t m_getDefaultMaxDiskCache() { return 0; } // (50 * pow(1024, 2));
    static uint32_t m_getDefaultExtraTexture() { return (6 * pow(1024, 2)); }
    static uint32_t m_getDefaultMinTexture() { return 0; }

    static quint32 m_getMaxMemCacheSetting();

    static QString m_databaseFilePath;
    static QString m_cachePath;
    static bool    m_cacheWasReset;

    static constexpr const char *kCachePathVersion = "300";
};
