#include "VideoManager.h"
#include "UVCReceiver.h"
#include "QUAVCorePlugin.h"
#include "SettingsManager.h"
#include "QUAVApplication.h"
#include "VideoSettings.h"
#include "VideoReceiver.h"
#include "MultiVehicleManager.h"
#include "AppSettings.h"

#include <QtCore/qapplicationstatic.h>
#include <QtCore/QDir>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtCore/QTimer>

static constexpr const char* kFileExtension[VideoReceiver::FILE_FORMAT_MAX + 1] = {"mkv","mov","mp4"};

Q_APPLICATION_STATIC(VideoManager, m_videoManagerInstance);

VideoManager::VideoManager(QObject *parent)
    : QObject(parent)
    , m_videoSettings(SettingsManager::instance()->videoSettings())
{

}

VideoManager::~VideoManager()
{

}

VideoManager* VideoManager::instance()
{
    return m_videoManagerInstance();
}


void VideoManager::setfullScreen(bool on)
{
    if (on != m_fullScreen) {
        m_fullScreen = on;
        emit fullScreenChanged();
    }
}

bool VideoManager::uvcEnabled()
{
    return UVCReceiver::enabled();
}

void VideoManager::m_cleanupOldVideos()
{
    if (!SettingsManager::instance()->videoSettings()->enableStorageLimit()->rawValue().toBool()) {
        return;
    }

    const QString savePath = SettingsManager::instance()->appSettings()->videoSavePath();
    QDir videoDir = QDir(savePath);
    videoDir.setFilter(QDir::Files | QDir::Readable | QDir::NoSymLinks | QDir::Writable);
    videoDir.setSorting(QDir::Time);

    QStringList nameFilters;
    for (size_t i = 0; i < std::size(kFileExtension); i++) {
        nameFilters<<QStringLiteral("*.") + kFileExtension[i];
    }

    videoDir.setNameFilters(nameFilters);
    QFileInfoList vidList = videoDir.entryInfoList();
    if (vidList.isEmpty()) {
        return;
    }

    uint64_t total = 0;
    for (const QFileInfo& video : std::as_const(vidList)) {
        total += video.size();
    }

    const uint64_t maxSize = SettingsManager::instance()->videoSettings()->maxVideoSize()->rawValue().toUInt()*qPow(1024, 2);
    while ((total >= maxSize) && !vidList.isEmpty()) {
        const QFileInfo info = vidList.takeLast();
        total -= info.size();
        const QString path = info.filePath();
        qDebug() << "Removing old video file:" << path;
        (void)QFile::remove(path);
    }
}

void VideoManager::capturePhoto (QMediaCaptureSession* captureSession)
{
    const QString savePath = SettingsManager::instance()->appSettings()->photoSavePath();
    if (savePath.isEmpty()) {
        quavApp()->showAppMessage(tr("Unabled to captureImage. Image save path must be specified in Settings."));
        return;
    }

    const QString photoFileUrl = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh.mm.ss");
    const QString ext = "jpg";

    const QString photoFileNameTemplate = savePath + "/" + photoFileUrl + ".%1" + ext;

    for (VideoReceiver* receiver : std::as_const(m_videoReceivers)) {
        if (!receiver->started()) {
            qDebug() << "Video receiver is not ready.";
            continue;
        }

        const QString streamName = (receiver->name() == QStringLiteral("videoContent")) ? "" : (receiver->name() + ".");
        const QString photoFileName = photoFileNameTemplate.arg(streamName);
        receiver->capturePhoto(captureSession, photoFileName);
    }
}

void  VideoManager::startRecording(QMediaCaptureSession* captureSession)
{
    const VideoReceiver::FILE_FORMAT fileFormat =
        static_cast<VideoReceiver::FILE_FORMAT>(m_videoSettings->recordingFormat()->rawValue().toInt());

    if (!VideoReceiver::isValidFileFormat(fileFormat)) {
        quavApp()->showAppMessage(tr("Invalid video format defined."));
        return;
    }

    m_cleanupOldVideos();

    const QString savePath = SettingsManager::instance()->appSettings()->videoSavePath();
    if (savePath.isEmpty()) {
        quavApp()->showAppMessage(tr("Unabled to record video. Video save path must be specified in Settings."));
        return;
    }

    const QString videoFileUrl = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh.mm.ss");
    const QString ext = kFileExtension[fileFormat];

    const QString videoFileNameTemplate = savePath + "/" + videoFileUrl + ".%1" + ext;

    for (VideoReceiver* receiver : std::as_const(m_videoReceivers)) {
        if (!receiver->started()) {
            qDebug() << "Video receiver is not ready.";
            continue;
        }
        const QString streamName = (receiver->name() == QStringLiteral("videoContent")) ? "" : (receiver->name() + ".");
        const QString videoFileName = videoFileNameTemplate.arg(streamName);
        receiver->startRecording(captureSession, videoFileName, fileFormat);
    }
}

void  VideoManager::stopRecording()
{
    for (VideoReceiver* receiver : std::as_const(m_videoReceivers)) {
        receiver->stopRecording();
    }
}

bool VideoManager::hasVideo() const
{
    return (m_videoSettings->streamEnabled()->rawValue().toBool() && m_videoSettings->streamConfigured());
}

bool VideoManager::isUvc() const
{
    return (!m_uvcVideoSourceID.isEmpty() && uvcEnabled() && hasVideo());
}

void VideoManager::init()
{
    if (m_initialized) { return; }

    // TODO: VideoSettings m_configChanged/streamConfiguredChanged
    (void) connect(m_videoSettings->videoSource(), &Fact::rawValueChanged, this, &VideoManager::m_videoSourceChanged);
    (void) connect(MultiVehicleManager::instance(), &MultiVehicleManager::activeVehicleChanged,
                  this, &VideoManager::m_setActiveVehicle);

    static const QStringList videoStreamList = {"videoContent"};
    for (const QString& streamName : videoStreamList) {
        VideoReceiver* receiver = QUAVCorePlugin::instance()->createVideoReceiver(this);
        if (!receiver) { continue;}
        receiver->setName(streamName);
        m_initVideoReceiver(receiver);
    }

    m_initialized = true;
}

bool VideoManager::m_updateVideoUri(VideoReceiver *receiver, const QString &uri)
{
    if (!receiver) {
        qDebug()<<"VideoReceiver is NULL";
        return false;
    }

    if ((uri == receiver->uri()) && !receiver->uri().isNull()) {
        return false;
    }

    qDebug()<<"New Video URI"<<uri;

    receiver->setUri(uri);

    return true;
}

bool VideoManager::m_updateSettings(VideoReceiver* receiver)
{
       if (!receiver) {
        qDebug() << "VideoReceiver is NULL";
        return false;
    }

    bool settingsChanged = false;

    settingsChanged |= m_updateUVC(receiver);

    const QString source = m_videoSettings->videoSource()->rawValue().toString();

    settingsChanged |= m_updateVideoUri(receiver, QString());
    if (!isUvc()) {
        qDebug() << "Video source URI \"" << source << "\" is not supported. Please add support!";
    }

    return settingsChanged;
}

void VideoManager::m_startReceiver(VideoReceiver* receiver)
{
    if (!receiver) {
        qDebug()<<"VideoReceiver is NULL";
    }

    if (receiver->started()) {
        qDebug() << "VideoReceiver is already started" << receiver->name();
        return;
    }

    receiver->start();
}

void VideoManager::m_initVideoReceiver(VideoReceiver *receiver)
{
    if (m_videoReceivers.contains(receiver)) {
        qWarning() << "Receiver already initialized";
    }

    (void) connect(receiver, &VideoReceiver::onStartComplete, this, [this, receiver](VideoReceiver::STATUS status) {
        if (!receiver) { return; }

        qDebug()<<"Video"<<receiver->name()<<"Start complete, status"<<status;
        switch (status) {
            case VideoReceiver::STATUS_OK:
            {
                receiver->setStarted(true);
                break;
            }
            case VideoReceiver::STATUS_INVALID_URL:
            case VideoReceiver::STATUS_INVALID_STATE:
            default:
                break;
        }
    });

    (void) connect(receiver, &VideoReceiver::recordingChanged, this, [this, receiver](bool active) {
        qDebug() << "Video" << receiver->name() << "recording changed, active:" << (active ? "yes" : "no");
        if (!receiver->isThermal()) {
            m_recording = active;
            emit recordingChanged();
        }
    });

    qDebug()<<"VideoManager::m_initVideoReceiver -> (void) m_updateSettings(receiver);";

    (void) m_updateSettings(receiver);
    m_videoReceivers.append(receiver);

    m_startReceiver(receiver);
}

void VideoManager::m_setActiveVehicle(Vehicle* vehicle)
{
    qDebug() << Q_FUNC_INFO << "new vehicle" << vehicle << "old active vehicle" << m_activeVehicle;

    m_activeVehicle = vehicle;
}

void VideoManager::m_videoSourceChanged()
{
    bool changed = false;
    if (m_activeVehicle) {
        for (VideoReceiver* receiver : std::as_const(m_videoReceivers)) {
            changed |= m_updateSettings(receiver);
        }
    } else {
        for (VideoReceiver* receiver : std::as_const(m_videoReceivers)) {
            changed |= m_updateSettings(receiver);
        }
    }

    if (changed) {
        emit hasVideoChanged();

        qDebug() << "New Video Source:" << m_videoSettings->videoSource()->rawValue().toString();
    }
}

bool VideoManager::m_updateUVC(VideoReceiver* receiver)
{
    bool result = false;

    const QString oldUvcVideoSrcID = m_uvcVideoSourceID;

    qDebug()<<"!uvcEnabled()"<<" "<<(bool)(!uvcEnabled())<<" "<<"!hasVideo()"<<" "<<(bool)(!hasVideo());

    if (!uvcEnabled() || !hasVideo()) {
        m_uvcVideoSourceID = QString();
    } else {
        m_uvcVideoSourceID = UVCReceiver::getSourceId();
    }

    qDebug() << "oldUvcVideoSrcID" << " " << oldUvcVideoSrcID<<" "<< "m_uvcVideoSourceID" << " " << m_uvcVideoSourceID;

    if (oldUvcVideoSrcID != m_uvcVideoSourceID) {
        qDebug()<<"UVC changed from ["<<oldUvcVideoSrcID<<"] to ["<<m_uvcVideoSourceID<<"]";

        if (!m_uvcVideoSourceID.isEmpty()) {UVCReceiver::checkPermission();}
        result = true;
        emit uvcVideoSourceIDChanged();
        emit isUvcChanged();
    }
    return result;
}