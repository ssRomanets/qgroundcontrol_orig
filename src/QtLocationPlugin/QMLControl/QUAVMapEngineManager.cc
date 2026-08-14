

/// @file
///     @author Gus Grubba <gus@auterion.com>

#include "QUAVMapEngineManager.h"
#include "QUAVCachedTileSet.h"
#include "QUAVMapUrlEngine.h"
#include "QUAVMapEngine.h"
#include "QGeoFileTileCacheQUAV.h"
#include "Providers/ElevationMapProvider.h"
#include "QmlObjectListModel.h"
#include "QUAVApplication.h"
#include "SettingsManager.h"
#include "FlightMapSettings.h"


#include <QtCore/qapplicationstatic.h>
#include <QtCore/QRegularExpression>
#include <QtCore/QSettings>
#include <QtCore/QStorageInfo>
#include <QtQml/QQmlEngine>

Q_APPLICATION_STATIC(QUAVMapEngineManager, m_mapEngineManager);

QUAVMapEngineManager* QUAVMapEngineManager::instance()
{
    return m_mapEngineManager();
}

QUAVMapEngineManager::QUAVMapEngineManager(QObject *parent)
    : QObject(parent)
    , m_tileSets(new QmlObjectListModel(this))
{
    (void) qmlRegisterUncreatableType<QUAVMapEngineManager>("QGroundControl.QUAVMapEngineManager", 1, 0, "QUAVMapEngineManager", "Reference only");

    (void) connect(getQUAVMapEngine(), &QUAVMapEngine::updateTotals, this, &QUAVMapEngineManager::m_updateTotals);

    // qDebug() << Q_FUNC_INFO << this;
}

QUAVMapEngineManager::~QUAVMapEngineManager()
{
    m_tileSets->clear();

    // qDebug() << Q_FUNC_INFO << this;
}

void QUAVMapEngineManager::updateForCurrentView(double lon0, double lat0, double lon1, double lat1, int minZoom, int maxZoom, const QString &mapName)
{
    m_topleftLat = lat0;
    m_topleftLon = lon0;
    m_bottomRightLat = lat1;
    m_bottomRightLon = lon1;
    m_minZoom = minZoom;
    m_maxZoom = maxZoom;

    m_imageSet.clear();
    m_elevationSet.clear();

    for (int z = minZoom; z <= maxZoom; z++) {
        const QUAVTileSet set = UrlFactory::getTileCount(z, lon0, lat0, lon1, lat1, mapName);
        m_imageSet += set;
    }

    if (m_fetchElevation) {
        const QString elevationProviderName = SettingsManager::instance()->flightMapSettings()->elevationMapProvider()->rawValue().toString();
        const QUAVTileSet set = UrlFactory::getTileCount(1, lon0, lat0, lon1, lat1, elevationProviderName);
        m_elevationSet += set;
    }

    emit tileCountChanged();
    emit tileSizeChanged();

//    qDebug() << Q_FUNC_INFO << lat0 << lon0 << lat1 << lon1 << minZoom << maxZoom;
}

QString QUAVMapEngineManager::tileCountStr() const
{
    return quavApp()->numberToString(m_imageSet.tileCount + m_elevationSet.tileCount);
}

QString QUAVMapEngineManager::tileSizeStr() const
{
    return quavApp()->bigSizeToString(m_imageSet.tileSize + m_elevationSet.tileSize);
}

void QUAVMapEngineManager::loadTileSets()
{
    if (m_tileSets->count() > 0) {
        m_tileSets->clear();
        emit tileSetsChanged();
    }

    QUAVFetchTileSetTask* const task = new QUAVFetchTileSetTask(nullptr);
    (void) connect(task, &QUAVFetchTileSetTask::tileSetFetched, this, &QUAVMapEngineManager::m_tileSetFetched);
    (void) connect(task, &QUAVMapTask::error, this, &QUAVMapEngineManager::taskError);
    (void) getQUAVMapEngine()->addTask(task);
}

void QUAVMapEngineManager::m_tileSetFetched(QUAVCachedTileSet *tileSet)
{
    if (tileSet->type() == QStringLiteral("Invalid")) {
        tileSet->setMapTypeStr(QStringLiteral("Various"));
    }

    tileSet->setManager(this);
    m_tileSets->append(tileSet);
    emit tileSetsChanged();
}

void QUAVMapEngineManager::startDownload(const QString &name, const QString &mapType)
{
    if (m_imageSet.tileSize > 0) {
        QUAVCachedTileSet* const set = new QUAVCachedTileSet(name);
        set->setMapTypeStr(mapType);
        set->setTopleftLat(m_topleftLat);
        set->setTopleftLon(m_topleftLon);
        set->setBottomRightLat(m_bottomRightLat);
        set->setBottomRightLon(m_bottomRightLon);
        set->setMinZoom(m_minZoom);
        set->setMaxZoom(m_maxZoom);
        set->setTotalTileSize(m_imageSet.tileSize);
        set->setTotalTileCount(static_cast<quint32>(m_imageSet.tileCount));
        set->setType(mapType);

        QUAVCreateTileSetTask* const task = new QUAVCreateTileSetTask(set);
        (void) connect(task, &QUAVCreateTileSetTask::tileSetSaved, this, &QUAVMapEngineManager::m_tileSetSaved);
        (void) connect(task, &QUAVMapTask::error, this, &QUAVMapEngineManager::taskError);
        (void) getQUAVMapEngine()->addTask(task);
    } else {
//        qWarning() << Q_FUNC_INFO << "No Tiles to save";
    }

    const int mapid = UrlFactory::getQtMapIdFromProviderType(mapType);
    if (m_fetchElevation && !UrlFactory::isElevation(mapid)) {
        QUAVCachedTileSet* const set = new QUAVCachedTileSet(name + QStringLiteral(" Elevation"));
        const QString elevationProviderName = SettingsManager::instance()->flightMapSettings()->elevationMapProvider()->rawValue().toString();
        set->setMapTypeStr(elevationProviderName);
        set->setTopleftLat(m_topleftLat);
        set->setTopleftLon(m_topleftLon);
        set->setBottomRightLat(m_bottomRightLat);
        set->setBottomRightLon(m_bottomRightLon);
        set->setMinZoom(1);
        set->setMaxZoom(1);
        set->setTotalTileSize(m_elevationSet.tileSize);
        set->setTotalTileCount(static_cast<quint32>(m_elevationSet.tileCount));
        set->setType(elevationProviderName);

        QUAVCreateTileSetTask* const task = new QUAVCreateTileSetTask(set);
        (void) connect(task, &QUAVCreateTileSetTask::tileSetSaved, this, &QUAVMapEngineManager::m_tileSetSaved);
        (void) connect(task, &QUAVMapTask::error, this, &QUAVMapEngineManager::taskError);
        (void) getQUAVMapEngine()->addTask(task);
    } else {
//        qWarning() << Q_FUNC_INFO << "No Tiles to save";
    }
}

void QUAVMapEngineManager::m_tileSetSaved(QUAVCachedTileSet *set)
{
//    qDebug() << "New tile set saved (" << set->name() << "). Starting download...";

    m_tileSets->append(set);
    emit tileSetsChanged();
    set->createDownloadTask();
}

void QUAVMapEngineManager::saveSetting(const QString &key, const QString &value)
{
    QSettings settings;
    settings.beginGroup(kQmlOfflineMapKeyName);
    settings.setValue(key, value);
}

QString QUAVMapEngineManager::loadSetting(const QString &key, const QString &defaultValue)
{
    QSettings settings;
    settings.beginGroup(kQmlOfflineMapKeyName);
    return settings.value(key, defaultValue).toString();
}

QStringList QUAVMapEngineManager::mapTypeList(const QString &provider)
{
    QStringList mapStringList = mapList();
    mapStringList = mapStringList.filter(QRegularExpression(provider));

    static const QRegularExpression providerType = QRegularExpression(QStringLiteral("^([^\\ ]*) (.*)$"));
    (void) mapStringList.replaceInStrings(providerType,"\\2");
    (void) mapStringList.removeDuplicates();

    return mapStringList;
}

void QUAVMapEngineManager::deleteTileSet(QUAVCachedTileSet *tileSet)
{
//    qDebug() << "Deleting tile set" << tileSet->name();

    if (tileSet->defaultSet()) {
        for (qsizetype i = 0; i < m_tileSets->count(); i++ ) {
            QUAVCachedTileSet* const set = qobject_cast<QUAVCachedTileSet*>(m_tileSets->get(i));
            if (set) {
                set->setDeleting(true);
            }
        }

        QUAVResetTask* const task = new QUAVResetTask(nullptr);
        (void) connect(task, &QUAVResetTask::resetCompleted, this, &QUAVMapEngineManager::m_resetCompleted);
        (void) connect(task, &QUAVMapTask::error, this, &QUAVMapEngineManager::taskError);
        (void) getQUAVMapEngine()->addTask(task);
    } else {
        tileSet->setDeleting(true);

        QUAVDeleteTileSetTask* const task = new QUAVDeleteTileSetTask(tileSet->id());
        (void) connect(task, &QUAVDeleteTileSetTask::tileSetDeleted, this, &QUAVMapEngineManager::m_tileSetDeleted);
        (void) connect(task, &QUAVMapTask::error, this, &QUAVMapEngineManager::taskError);
        (void) getQUAVMapEngine()->addTask(task);
    }
}

void QUAVMapEngineManager::renameTileSet(QUAVCachedTileSet *tileSet, const QString &newName)
{
    int idx = 1;
    QString name = newName;
    while (findName(name)) {
        name = QString("%1 (%2)").arg(newName).arg(idx++);
    }

//    qDebug() << "Renaming tile set" << tileSet->name() << "to" << name;
    tileSet->setName(name);
    emit tileSet->nameChanged();

    QUAVRenameTileSetTask* const task = new QUAVRenameTileSetTask(tileSet->id(), name);
    (void) connect(task, &QUAVMapTask::error, this, &QUAVMapEngineManager::taskError);
    (void) getQUAVMapEngine()->addTask(task);
}

void QUAVMapEngineManager::m_tileSetDeleted(quint64 setID)
{
    for (qsizetype i = 0; i < m_tileSets->count(); i++ ) {
        QUAVCachedTileSet* const set = qobject_cast<QUAVCachedTileSet*>(m_tileSets->get(i));
        if (set && (set->id() == setID)) {
            m_tileSets->removeAt(i);
            delete set;
            emit tileSetsChanged();
            return;
        }
    }
}

void QUAVMapEngineManager::taskError(QUAVMapTask::TaskType type, const QString &error)
{
    QString task;
    switch (type) {
    case QUAVMapTask::taskFetchTileSets:
        task = QStringLiteral("Fetch Tile Set");
        break;
    case QUAVMapTask::taskCreateTileSet:
        task = QStringLiteral("Create Tile Set");
        break;
    case QUAVMapTask::taskGetTileDownloadList:
        task = QStringLiteral("Get Tile Download List");
        break;
    case QUAVMapTask::taskUpdateTileDownloadState:
        task = QStringLiteral("Update Tile Download Status");
        break;
    case QUAVMapTask::taskDeleteTileSet:
        task = QStringLiteral("Delete Tile Set");
        break;
    case QUAVMapTask::taskReset:
        task = QStringLiteral("Reset Tile Sets");
        break;
    case QUAVMapTask::taskExport:
        task = QStringLiteral("Export Tile Sets");
        break;
    default:
        task = QStringLiteral("Database Error");
        break;
    }

    QString serror = "Error in task: " + task;
    serror += "\nError description:\n";
    serror += error;

    setErrorMessage(serror);

//    qWarning() << serror;
}

void QUAVMapEngineManager::m_updateTotals(quint32 totaltiles, quint64 totalsize, quint32 defaulttiles, quint64 defaultsize)
{
    for (qsizetype i = 0; i < m_tileSets->count(); i++) {
        QUAVCachedTileSet* const set = qobject_cast<QUAVCachedTileSet*>(m_tileSets->get(i));
        if (set && set->defaultSet()) {
            set->setSavedTileSize(totalsize);
            set->setSavedTileCount(totaltiles);
            set->setTotalTileCount(defaulttiles);
            set->setTotalTileSize(defaultsize);
            return;
        }
    }
}

bool QUAVMapEngineManager::findName(const QString &name) const
{
    for (qsizetype i = 0; i < m_tileSets->count(); i++) {
        const QUAVCachedTileSet* const set = qobject_cast<const QUAVCachedTileSet*>(m_tileSets->get(i));
        if (set && (set->name() == name)) {
            return true;
        }
    }

    return false;
}

void QUAVMapEngineManager::selectAll()
{
    for (qsizetype i = 0; i < m_tileSets->count(); i++) {
        QUAVCachedTileSet* const set = qobject_cast<QUAVCachedTileSet*>(m_tileSets->get(i));
        if (set) {
            set->setSelected(true);
        }
    }
}

void QUAVMapEngineManager::selectNone()
{
    for (qsizetype i = 0; i < m_tileSets->count(); i++) {
        QUAVCachedTileSet* const set = qobject_cast<QUAVCachedTileSet*>(m_tileSets->get(i));
        if (set) {
            set->setSelected(false);
        }
    }
}

int QUAVMapEngineManager::selectedCount() const
{
    int count = 0;

    for (qsizetype i = 0; i < m_tileSets->count(); i++) {
        const QUAVCachedTileSet* const set = qobject_cast<const QUAVCachedTileSet*>(m_tileSets->get(i));
        if (set && set->selected()) {
            count++;
        }
    }

    return count;
}

bool QUAVMapEngineManager::importSets(const QString &path)
{
    setImportAction(ActionNone);

    if (path.isEmpty()) {
        return false;
    }

    setImportAction(ActionImporting);

    QUAVImportTileTask* const task = new QUAVImportTileTask(path, m_importReplace);
    (void) connect(task, &QUAVImportTileTask::actionCompleted, this, &QUAVMapEngineManager::m_actionCompleted);
    (void) connect(task, &QUAVImportTileTask::actionProgress, this, &QUAVMapEngineManager::m_actionProgressHandler);
    (void) connect(task, &QUAVMapTask::error, this, &QUAVMapEngineManager::taskError);
    (void) getQUAVMapEngine()->addTask(task);

    return true;
}

bool QUAVMapEngineManager::exportSets(const QString &path)
{
    setImportAction(ActionNone);

    if (path.isEmpty()) {
        return false;
    }

    QList<QUAVCachedTileSet*> sets;

    for (qsizetype i = 0; i < m_tileSets->count(); i++) {
        QUAVCachedTileSet* const set = qobject_cast<QUAVCachedTileSet*>(m_tileSets->get(i));
        if (set->selected()) {
            sets.append(set);
        }
    }

    if (sets.isEmpty()) {
        return false;
    }

    setImportAction(ActionExporting);

    QUAVExportTileTask* const task = new QUAVExportTileTask(sets, path);
    (void) connect(task, &QUAVExportTileTask::actionCompleted, this, &QUAVMapEngineManager::m_actionCompleted);
    (void) connect(task, &QUAVExportTileTask::actionProgress, this, &QUAVMapEngineManager::m_actionProgressHandler);
    (void) connect(task, &QUAVMapTask::error, this, &QUAVMapEngineManager::taskError);
    (void) getQUAVMapEngine()->addTask(task);

    return true;
}

void QUAVMapEngineManager::m_actionCompleted()
{
    const ImportAction oldState = m_importAction;
    setImportAction(ActionDone);

    if (oldState == ActionImporting) {
        loadTileSets();
    }
}

QString QUAVMapEngineManager::getUniqueName() const
{
    int count = 1;
    while (true) {
        const QString name = QStringLiteral("Tile Set ") + QString::asprintf("%03d", count++);
        if (!findName(name)) {
            return name;
        }
    }

    return QStringLiteral("");
}

QStringList QUAVMapEngineManager::mapList()
{
    return UrlFactory::getProviderTypes();
}

QStringList QUAVMapEngineManager::mapProviderList()
{
    QStringList mapStringList = mapList();
    const QStringList elevationStringList = elevationProviderList();
    for (const QString &elevationProviderName : elevationStringList) {
        (void) mapStringList.removeAll(elevationProviderName);
    }

    static const QRegularExpression providerType = QRegularExpression(QStringLiteral("^([^\\ ]*) (.*)$"));
    (void) mapStringList.replaceInStrings(providerType,"\\1");
    (void) mapStringList.removeDuplicates();

    return mapStringList;
}

QStringList QUAVMapEngineManager::elevationProviderList()
{
    return UrlFactory::getElevationProviderTypes();
}
