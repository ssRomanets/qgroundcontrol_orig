#include "FlyViewSettings.h"
#include "SettingsManager.h"
#include "SimulatedCameraControl.h"
#include "MavlinkCameraControl.h"
#include "VideoManager.h"
#include "Vehicle.h"

#include <QtQml/QQmlEngine>

SimulatedCameraControl::SimulatedCameraControl(Vehicle *vehicle, QObject *parent)
                    : MavlinkCameraControl(parent)
                    , m_vehicle(vehicle)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    connect(VideoManager::instance(), &VideoManager::recordingChanged, this, &SimulatedCameraControl::videoCaptureStatusChanged);

    auto flyViewSettings = SettingsManager::instance()->flyViewSettings();
    connect(flyViewSettings->showSimpleCameraControl(), &Fact::rawValueChanged, this, &SimulatedCameraControl::infoChanged);

    m_videoRecordTimeUpdateTimer.setInterval(1000);
    connect(&m_videoRecordTimeUpdateTimer, &QTimer::timeout, this, &SimulatedCameraControl::recordTimeChanged);
}

SimulatedCameraControl::~SimulatedCameraControl()
{

}

QString SimulatedCameraControl::recordTimeStr()
{
    return QTime(0,0).addMSecs(static_cast<int>(recordTime())).toString("hh:mm:ss");
}

SimulatedCameraControl::VideoCaptureStatus SimulatedCameraControl::videoCaptureStatus()
{
    return m_videoCaptureStatus = VideoManager::instance()->recording() ? VIDEO_CAPTURE_STATUS_RUNNING : VIDEO_CAPTURE_STATUS_STOPPED;
}

void SimulatedCameraControl::setCameraMode(CameraMode mode)
{
    qDebug() << "setCameraMode" << cameraModeToStr(mode);

    if (hasModes()) {
        if (mode == CAM_MODE_VIDEO) {
            m_setCameraMode(CAM_MODE_VIDEO);
        } else if (mode == CAM_MODE_PHOTO) {
            m_setCameraMode(CAM_MODE_PHOTO);
        } else {
            qWarning() << "setCameraMode invalid mode" << mode;
        }
    } else {
        qWarning() << "setCameraMode called when camera does not support modes";
    }
}

void SimulatedCameraControl::m_setCameraMode(CameraMode mode)
{
    if (m_cameraMode != mode) {
        m_cameraMode = mode;
        emit cameraModeChanged();
        qDebug()<<"emit cameraModeChanged();";
    }
}

bool SimulatedCameraControl::toggleVideoRecording(QMediaCaptureSession* captureSession)
{
    qDebug()<<"SimulatedCameraControl::toggleVideoRecording";

    if (videoCaptureStatus() == VIDEO_CAPTURE_STATUS_RUNNING) {
        return stopVideoRecording();
    } else {
        //количество снятых фотографий обнуляем
        if (m_photoCount > 0)
        {
            m_photoCount = 0;
            emit photoCountChanged();
        }
        return startVideoRecording(captureSession);
    }
}

void SimulatedCameraControl::setCameraModeVideo()
{
    qDebug()<<"setCameraModeVideo()";

    if (!hasModes()) {
        qWarning() << "setCameraModeVideo: Camera does not support modes";
        return;
    }

    m_setCameraMode(CAM_MODE_VIDEO);
}

void SimulatedCameraControl::setCameraModePhoto()
{
    qDebug() << "setCameraModePhoto()";

    if (!hasModes()) {
        qWarning() << "setCameraModePhoto: Camera does not support modes";
        return;
    }

    m_setCameraMode(CAM_MODE_PHOTO);
}

bool SimulatedCameraControl::takePhoto(QMediaCaptureSession* captureSession)
{
    qDebug() << "takePhoto()";

    if (!capturesPhotos()) {
        qWarning()<<"takePhoto: Camera does not handle image capture";
        return false;
    }

    if (photoCaptureStatus() != PHOTO_CAPTURE_IDLE) {
        qWarning() << "Camera not idle";
        return false;
    }

    if (cameraMode() != CAM_MODE_PHOTO && cameraMode() != CAM_MODE_SURVEY) {
        qWarning() << "takePhoto: Camera not in correct mode:" << cameraModeToStr(cameraMode());
        return false;
    }

    if (photoCaptureMode() == PHOTO_CAPTURE_SINGLE) {
        VideoManager::instance()->capturePhoto(captureSession);

        m_photoCount++;
        emit photoCountChanged();

        m_photoCaptureStatus = PHOTO_CAPTURE_IN_PROGRESS;
        emit photoCaptureStatusChanged();
        QTimer::singleShot(1000, [this](){m_photoCaptureStatus = PHOTO_CAPTURE_IDLE; emit photoCaptureStatusChanged();});
    }

    return true;
}

bool SimulatedCameraControl::startVideoRecording(QMediaCaptureSession* captureSession)
{
    qDebug() << "startVideoRecording()";

    if (!capturesVideo()) {
        qWarning() << "startVideoRecording: Camera does not handle video capture";
        return false;
    }

    if (cameraMode() == CAM_MODE_PHOTO) {
        qWarning() << "startVideoRecording: Camera does not take video in photo mode";
        return false;
    }

    if (videoCaptureStatus() == VIDEO_CAPTURE_STATUS_RUNNING) {
        qWarning() << "startVideoRecording: Camera already recording";
        return false;
    }

    m_videoRecordTimeUpdateTimer.start();
    m_videoRecordTimeElapsedTimer.start();
    VideoManager::instance()->startRecording(captureSession);
    return false;
}

bool SimulatedCameraControl::stopVideoRecording()
{
    qDebug() << "stopVideoRecording()";

    if (videoCaptureStatus() != VIDEO_CAPTURE_STATUS_RUNNING) {
        qWarning() << "stopVideoRecording: Camera not recording";
        return false;
    }

    m_videoRecordTimeUpdateTimer.stop();

    VideoManager::instance()->stopRecording();
    return true;
}

quint32 SimulatedCameraControl::recordTime()
{
    if (m_videoRecordTimeUpdateTimer.isActive()) {
        return m_videoRecordTimeElapsedTimer.elapsed();
    } else {
        return 0;
    }
}

bool SimulatedCameraControl::capturesVideo()
{
    return VideoManager::instance()->hasVideo();
}

bool SimulatedCameraControl::capturesPhotos()
{
    return SettingsManager::instance()->flyViewSettings()->showSimpleCameraControl()->rawValue().toBool();
}

bool SimulatedCameraControl::hasVideoStream()
{
    return VideoManager::instance()->hasVideo();
}

void SimulatedCameraControl::setPhotoCaptureMode(MavlinkCameraControl::PhotoCaptureMode photoCaptureMode)
{
    if (m_photoCaptureMode != photoCaptureMode) {
        m_photoCaptureMode = photoCaptureMode;
        emit photoCaptureModeChanged();
    }
}

bool SimulatedCameraControl::hasModes()
{
    if (capturesPhotos() && capturesVideo()) {
        return true;
    } else {
        return false;
    }
}


