

/**
 * @file
 *   @brief Map Tile Set
 *
 *   @author Gus Grubba <gus@auterion.com>
 *
 */

#pragma once

#include <QtCore/QDateTime>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtNetwork/QNetworkReply>

class QUAVTile;
class QUAVMapEngineManager;
class QNetworkAccessManager;

class QUAVCachedTileSet : public QObject
{
    Q_OBJECT
    Q_MOC_INCLUDE("QUAVTile.h")

    Q_PROPERTY(QString      name                READ    name                NOTIFY nameChanged)
    Q_PROPERTY(QString      mapTypeStr          READ    mapTypeStr          CONSTANT)
    Q_PROPERTY(double       topleftLon          READ    topleftLon          CONSTANT)
    Q_PROPERTY(double       topleftLat          READ    topleftLat          CONSTANT)
    Q_PROPERTY(double       bottomRightLon      READ    bottomRightLon      CONSTANT)
    Q_PROPERTY(double       bottomRightLat      READ    bottomRightLat      CONSTANT)
    Q_PROPERTY(int          minZoom             READ    minZoom             CONSTANT)
    Q_PROPERTY(int          maxZoom             READ    maxZoom             CONSTANT)
    Q_PROPERTY(quint32      totalTileCount      READ    totalTileCount      NOTIFY totalTileCountChanged)
    Q_PROPERTY(QString      totalTileCountStr   READ    totalTileCountStr   NOTIFY totalTileCountChanged)
    Q_PROPERTY(quint64      totalTilesSize      READ    totalTilesSize      NOTIFY totalTilesSizeChanged)
    Q_PROPERTY(QString      totalTilesSizeStr   READ    totalTilesSizeStr   NOTIFY totalTilesSizeChanged)
    Q_PROPERTY(quint32      uniqueTileCount     READ    uniqueTileCount     NOTIFY uniqueTileCountChanged)
    Q_PROPERTY(QString      uniqueTileCountStr  READ    uniqueTileCountStr  NOTIFY uniqueTileCountChanged)
    Q_PROPERTY(quint64      uniqueTileSize      READ    uniqueTileSize      NOTIFY uniqueTileSizeChanged)
    Q_PROPERTY(QString      uniqueTileSizeStr   READ    uniqueTileSizeStr   NOTIFY uniqueTileSizeChanged)
    Q_PROPERTY(quint32      savedTileCount      READ    savedTileCount      NOTIFY savedTileCountChanged)
    Q_PROPERTY(QString      savedTileCountStr   READ    savedTileCountStr   NOTIFY savedTileCountChanged)
    Q_PROPERTY(quint64      savedTileSize       READ    savedTileSize       NOTIFY savedTileSizeChanged)
    Q_PROPERTY(QString      savedTileSizeStr    READ    savedTileSizeStr    NOTIFY savedTileSizeChanged)
    Q_PROPERTY(QString      downloadStatus      READ    downloadStatus      NOTIFY savedTileSizeChanged)
    Q_PROPERTY(QDateTime    creationDate        READ    creationDate        CONSTANT)
    Q_PROPERTY(bool         complete            READ    complete            NOTIFY completeChanged)
    Q_PROPERTY(bool         defaultSet          READ    defaultSet          CONSTANT)
    Q_PROPERTY(quint64      id                  READ    id                  CONSTANT)
    Q_PROPERTY(bool         deleting            READ    deleting            NOTIFY deletingChanged)
    Q_PROPERTY(bool         downloading         READ    downloading         NOTIFY downloadingChanged)
    Q_PROPERTY(quint32      errorCount          READ    errorCount          NOTIFY errorCountChanged)
    Q_PROPERTY(QString      errorCountStr       READ    errorCountStr       NOTIFY errorCountChanged)
    Q_PROPERTY(bool         selected            READ    selected            WRITE  setSelected  NOTIFY selectedChanged)

public:
    explicit QUAVCachedTileSet(const QString &name, QObject *parent = nullptr);
    ~QUAVCachedTileSet();

    Q_INVOKABLE void createDownloadTask();
    Q_INVOKABLE void resumeDownloadTask();
    Q_INVOKABLE void cancelDownloadTask();

    const QString &name() const { return m_name; }
    const QString &mapTypeStr() const { return m_mapTypeStr; }

    double topleftLat() const { return m_topleftLat; }
    double topleftLon() const { return m_topleftLon; }
    double bottomRightLat() const { return m_bottomRightLat; }
    double bottomRightLon() const { return m_bottomRightLon; }

    quint32 totalTileCount() const { return m_totalTileCount; }
    QString totalTileCountStr() const;
    quint64 totalTilesSize() const { return m_totalTileSize; }
    QString totalTilesSizeStr() const;
    quint32 uniqueTileCount() const { return m_uniqueTileCount; }
    QString uniqueTileCountStr() const;
    quint64 uniqueTileSize() const { return m_uniqueTileSize; }
    QString uniqueTileSizeStr() const;
    quint32 savedTileCount() const { return m_savedTileCount; }
    QString savedTileCountStr() const;
    quint64 savedTileSize() const { return m_savedTileSize; }
    QString savedTileSizeStr() const;

    QString downloadStatus() const;
    int minZoom() const { return m_minZoom; }
    int maxZoom() const { return m_maxZoom; }
    const QDateTime &creationDate() const { return m_creationDate; }
    quint64 id() const { return m_id; }
    const QString &type() const { return m_type; }
    bool complete() const { return (m_defaultSet || (m_totalTileCount <= m_savedTileCount)); }
    bool defaultSet() const { return m_defaultSet; }
    bool deleting() const { return m_deleting; }
    bool downloading() const { return m_downloading; }
    quint32 errorCount() const { return m_errorCount; }
    QString errorCountStr() const;
    bool selected() const { return m_selected; }

    void setManager(QUAVMapEngineManager* mgr) { m_manager = mgr; }
    void setSelected(bool sel);
    void setName(const QString &name) { if (name != m_name) { m_name = name; emit nameChanged(); } }

    void setMapTypeStr(const QString &typeStr) { m_mapTypeStr = typeStr; }
    void setTopleftLat(double lat) { m_topleftLat = lat; }
    void setTopleftLon(double lon) { m_topleftLon = lon; }
    void setBottomRightLat(double lat) { m_bottomRightLat = lat; }
    void setBottomRightLon(double lon) { m_bottomRightLon = lon; }

    void setUniqueTileCount(quint32 num) { if (num != m_uniqueTileCount) { m_uniqueTileCount = num; emit uniqueTileCountChanged(); } }
    void setTotalTileCount(quint32 num) { if (num != m_totalTileCount) { m_totalTileCount = num; emit totalTileCountChanged(); } }
    void setSavedTileCount(quint32 num) { if (num != m_savedTileCount) { m_savedTileCount = num; emit savedTileCountChanged(); } }
    void setUniqueTileSize(quint64 size) { if (size != m_uniqueTileSize) { m_uniqueTileSize = size; emit uniqueTileSizeChanged(); } }
    void setTotalTileSize(quint64 size) { if (size != m_totalTileSize) { m_totalTileSize = size; emit totalTilesSizeChanged(); } }
    void setSavedTileSize(quint64 size) { if (size != m_savedTileSize) { m_savedTileSize = size; emit savedTileSizeChanged(); }  }

    void setMinZoom(int zoom) { m_minZoom = zoom; }
    void setMaxZoom(int zoom) { m_maxZoom = zoom; }
    void setCreationDate(const QDateTime &date) { m_creationDate = date; }
    void setId(quint64 id) { m_id = id; }
    void setType(const QString &type) { m_type = type; }
    void setDefaultSet(bool def) { m_defaultSet = def; }
    void setDeleting(bool del) { if (del != m_deleting) { m_deleting = del; emit deletingChanged(); } }
    void setDownloading(bool down) { if (down != m_downloading) { m_downloading = down; emit downloadingChanged(); } }
    void setErrorCount(quint32 count) { if (count != m_errorCount) { m_errorCount = count; emit errorCountChanged(); } }

signals:
    void deletingChanged();
    void downloadingChanged();
    void totalTileCountChanged();
    void uniqueTileCountChanged();
    void uniqueTileSizeChanged();
    void totalTilesSizeChanged();
    void savedTileCountChanged();
    void savedTileSizeChanged();
    void completeChanged();
    void errorCountChanged();
    void selectedChanged();
    void nameChanged();

private slots:
    void m_tileListFetched(const QQueue<QUAVTile*> &tiles);
    void m_networkReplyFinished();
    void m_networkReplyError(QNetworkReply::NetworkError error);

private:
    void m_prepareDownload();
    void m_doneWithDownload();

    QString m_name;
    QString m_mapTypeStr;
    QString m_type = QStringLiteral("Invalid");
    quint64 m_id = 0;

    double m_topleftLat = 0.;
    double m_topleftLon = 0.;
    double m_bottomRightLat = 0.;
    double m_bottomRightLon = 0.;

    quint32 m_totalTileCount = 0;
    quint64 m_totalTileSize = 0;
    quint32 m_uniqueTileCount = 0;
    quint64 m_uniqueTileSize = 0;
    quint32 m_savedTileCount = 0;
    quint64 m_savedTileSize = 0;
    quint32 m_errorCount = 0;

    int m_minZoom = 3;
    int m_maxZoom = 3;

    bool m_defaultSet = false;
    bool m_deleting = false;
    bool m_downloading = false;
    bool m_noMoreTiles = false;
    bool m_batchRequested = false;
    bool m_selected = false;
    bool m_cancelPending = false;

    QDateTime m_creationDate;

    QHash<QString, QNetworkReply*> m_replies;
    QQueue<QUAVTile*>               m_tilesToDownload;
    QUAVMapEngineManager*           m_manager = nullptr;
    QNetworkAccessManager*         m_networkManager = nullptr;

    static constexpr uint32_t kTileBatchSize = 256;
};
