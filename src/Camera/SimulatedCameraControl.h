#pragma once

#include "MavlinkCameraControl.h"

#include <QtCore/QTimer>
#include <QtCore/QElapsedTimer>

class Vehicle;

/// Creates a simulated Camera Control which supports:
///     Video record if a manual stream is available
///     Photo capture using DO_DIGICAM_CONTROL if the setting is enabled
///     It does not support time lapse capture

class SimulatedCameraControl : public MavlinkCameraControl
{
    Q_OBJECT

public:
    SimulatedCameraControl(Vehicle* vehicle, QObject* parent = nullptr);
    virtual ~SimulatedCameraControl();

    void setCameraModeVideo () override;
    void setCameraModePhoto () override;
    bool takePhoto          (QMediaCaptureSession* captureSession)  override;

    bool startVideoRecording(QMediaCaptureSession* captureSession)  override;
    bool stopVideoRecording()                                       override;
    bool toggleVideoRecording(QMediaCaptureSession* captureSession) override;

    bool                capturesVideo()                override;
    bool                capturesPhotos()               override;
    bool                hasVideoStream()               override;

    bool                photosInVideoMode() override { return true;}
    bool                videoInPhotoMode () override { return false;}
    int                 photoCount       () override { return m_photoCount;}

    void                setCameraMode(CameraMode mode) override;
    void                setPhotoCaptureMode(PhotoCaptureMode mode) override;

    bool                hasModes                   ()  override;
    quint32             recordTime                 ()  override;
    QString             recordTimeStr              ()  override;

    VideoCaptureStatus  videoCaptureStatus()           override;
    PhotoCaptureStatus  photoCaptureStatus()           override {return m_photoCaptureStatus;}

    PhotoCaptureMode    photoCaptureMode()             override {return m_photoCaptureMode;}
    CameraMode          cameraMode ()                  override { return m_cameraMode;}

    // The following overrides are defaulted/unsupported
    void                resetSettings()     override {}
    QStringList         activeSettings()    override {return QStringList();}

private:
    void m_setCameraMode(CameraMode mode);

    Vehicle*            m_vehicle                = nullptr;
    CameraMode          m_cameraMode             = CAM_MODE_UNDEFINED;
    VideoCaptureStatus  m_videoCaptureStatus     = VIDEO_CAPTURE_STATUS_STOPPED;
    PhotoCaptureStatus  m_photoCaptureStatus     = PHOTO_CAPTURE_IDLE;

    PhotoCaptureMode    m_photoCaptureMode       = PHOTO_CAPTURE_SINGLE;
    QTimer              m_videoRecordTimeUpdateTimer;
    QElapsedTimer       m_videoRecordTimeElapsedTimer;

    int                 m_photoCount {0};
};
