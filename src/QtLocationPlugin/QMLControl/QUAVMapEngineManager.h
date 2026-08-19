

/// @file
///     @author Gus Grubba <gus@auterion.com>

#pragma once

#include "QUAVTileSet.h"
#include "QUAVMapTasks.h"

class QUAVCachedTileSet;
class QmlObjectListModel;

class QUAVMapEngineManager : public QObject
{
    Q_OBJECT
    // QML_ELEMENT
    // QML_SINGLETON
    Q_MOC_INCLUDE("QmlObjectListModel.h")
    Q_MOC_INCLUDE("QUAVCachedTileSet.h")
    Q_PROPERTY(bool                 fetchElevation  MEMBER m_fetchElevation                          NOTIFY fetchElevationChanged)
    Q_PROPERTY(bool                 importReplace   MEMBER m_importReplace                           NOTIFY importReplaceChanged)
    Q_PROPERTY(ImportAction         importAction    READ importAction       WRITE setImportAction   NOTIFY importActionChanged)
    Q_PROPERTY(int                  actionProgress  READ actionProgress                             NOTIFY actionProgressChanged)
    Q_PROPERTY(int                  selectedCount   READ selectedCount                              NOTIFY selectedCountChanged)
    Q_PROPERTY(QmlObjectListModel   *tileSets       READ tileSets                                   NOTIFY tileSetsChanged)
    Q_PROPERTY(QString              errorMessage    READ errorMessage                               NOTIFY errorMessageChanged)
    Q_PROPERTY(QString              tileCountStr    READ tileCountStr                               NOTIFY tileCountChanged)
    Q_PROPERTY(QString              tileSizeStr     READ tileSizeStr                                NOTIFY tileSizeChanged)
    Q_PROPERTY(QStringList          mapList         READ mapList                                    CONSTANT)
    Q_PROPERTY(QStringList          mapProviderList READ mapProviderList                            CONSTANT)
    Q_PROPERTY(QStringList          elevationProviderList   READ elevationProviderList              CONSTANT)
    Q_PROPERTY(quint64              tileCount       READ tileCount                                  NOTIFY tileCountChanged)
    Q_PROPERTY(quint64              tileSize        READ tileSize                                   NOTIFY tileSizeChanged)

public:
    QUAVMapEngineManager(QObject *parent = nullptr);
    ~QUAVMapEngineManager();
    static QUAVMapEngineManager *instance();

    enum ImportAction {
        ActionNone,
        ActionImporting,
        ActionExporting,
        ActionDone,
    };
    Q_ENUM(ImportAction)

    Q_INVOKABLE bool exportSets(const QString &path = QString());
    Q_INVOKABLE bool findName(const QString &name) const;
    Q_INVOKABLE bool importSets(const QString &path = QString());
    Q_INVOKABLE QString getUniqueName() const;
    Q_INVOKABLE void deleteTileSet(QUAVCachedTileSet *tileSet);
    Q_INVOKABLE void loadTileSets();
    Q_INVOKABLE void renameTileSet(QUAVCachedTileSet *tileSet, const QString &newName);
    Q_INVOKABLE void resetAction() { setImportAction(ActionNone); }
    Q_INVOKABLE void selectAll();
    Q_INVOKABLE void selectNone();
    Q_INVOKABLE void startDownload(const QString &name, const QString &mapType);
    Q_INVOKABLE void updateForCurrentView(double lon0, double lat0, double lon1, double lat1, int minZoom, int maxZoom, const QString &mapName);

    Q_INVOKABLE static QString loadSetting(const QString &key, const QString &defaultValue);
    Q_INVOKABLE static QStringList mapTypeList(const QString &provider);
    Q_INVOKABLE static void saveSetting(const QString &key, const QString &value);

    ImportAction importAction() const { return m_importAction; }
    int actionProgress() const { return m_actionProgress; }
    int selectedCount() const;
    QmlObjectListModel *tileSets() { return m_tileSets; }
    QString errorMessage() const { return m_errorMessage; }
    QString tileCountStr() const;
    QString tileSizeStr() const;
    quint64 tileCount() const { return (m_imageSet.tileCount + m_elevationSet.tileCount); }
    quint64 tileSize() const { return (m_imageSet.tileSize + m_elevationSet.tileSize); }

    void setActionProgress(int percentage) { if (percentage != m_actionProgress) { m_actionProgress = percentage; emit actionProgressChanged(); } }
    void setErrorMessage(const QString &error) { if (error != m_errorMessage) { m_errorMessage = error; emit errorMessageChanged(); } }
    void setImportAction(ImportAction action) { if (action != m_importAction) { m_importAction = action; emit importActionChanged(); } }

    static QStringList mapList();
    static QStringList mapProviderList();
    static QStringList elevationProviderList();

signals:
    void actionProgressChanged();
    void errorMessageChanged();
    void fetchElevationChanged();
    void freeDiskSpaceChanged();
    void importActionChanged();
    void importReplaceChanged();
    void selectedCountChanged();
    void tileCountChanged();
    void tileSetsChanged();
    void tileSizeChanged();

public slots:
    void taskError(QUAVMapTask::TaskType type, const QString &error);

private slots:
    void m_actionCompleted();
    void m_actionProgressHandler(int percentage) { setActionProgress(percentage); }
    void m_resetCompleted() { loadTileSets(); }
    void m_tileSetDeleted(quint64 setID);
    void m_tileSetFetched(QUAVCachedTileSet *tileSets);
    void m_tileSetSaved(QUAVCachedTileSet *set);
    void m_updateTotals(quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize);

private:
    QmlObjectListModel* m_tileSets = nullptr;
    QUAVTileSet m_imageSet;
    QUAVTileSet m_elevationSet;
    ImportAction m_importAction = ActionNone;
    double m_topleftLat = 0.;
    double m_topleftLon = 0.;
    double m_bottomRightLat = 0.;
    double m_bottomRightLon = 0.;
    int m_minZoom = 0;
    int m_maxZoom = 0;
    int m_actionProgress = 0;
    quint64 m_setID = UINT64_MAX;
    QString m_errorMessage;
    bool m_fetchElevation = true;
    bool m_importReplace = false;

    static constexpr const char* kQmlOfflineMapKeyName = "QGCOfflineMap";
};
