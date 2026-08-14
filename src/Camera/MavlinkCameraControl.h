#pragma once

#include "FactGroup.h"

#include <QtCore/QObject>
#include <QMediaCaptureSession>

/// Abstract base class for all camera controls: real and simulated
class MavlinkCameraControl : public FactGroup
{
    Q_OBJECT

public:
    explicit MavlinkCameraControl(QObject* parent = nullptr);
    virtual ~MavlinkCameraControl();

    enum CameraMode {
        CAM_MODE_UNDEFINED = -1,
        CAM_MODE_PHOTO     = 0,
        CAM_MODE_VIDEO     = 1,
        CAM_MODE_SURVEY    = 2
    };

    enum VideoCaptureStatus {
        VIDEO_CAPTURE_STATUS_STOPPED = 0,
        VIDEO_CAPTURE_STATUS_RUNNING,
        VIDEO_CAPTURE_STATUS_LAST,
        VIDEO_CAPTURE_STATUS_UNDEFINED = 255
    };

    enum PhotoCaptureStatus {
        PHOTO_CAPTURE_IDLE = 0,
        PHOTO_CAPTURE_IN_PROGRESS,
        PHOTO_CAPTURE_INTERVAL_IDLE,
        PHOTO_CAPTURE_INTERVAL_IN_PROGRESS,
        PHOTO_CAPTURE_LAST,
        PHOTO_CAPTURE_STATUS_UNDEFINED = 255
    };

    enum PhotoCaptureMode {
        PHOTO_CAPTURE_SINGLE = 0,
        PHOTO_CAPTURE_TIMELAPSE
    };

    Q_ENUM(CameraMode)
    Q_ENUM(VideoCaptureStatus)
    Q_ENUM(PhotoCaptureStatus)
    Q_ENUM(PhotoCaptureMode)

    Q_PROPERTY(bool capturesVideo                    READ capturesVideo                                 NOTIFY infoChanged )
    Q_PROPERTY(bool capturesPhotos                   READ capturesPhotos                                NOTIFY infoChanged )
    Q_PROPERTY(bool hasModes                         READ hasModes                                      NOTIFY infoChanged )
    Q_PROPERTY(bool hasVideoStream                   READ hasVideoStream                                NOTIFY infoChanged )
    Q_PROPERTY(bool photosInVideoMode                READ photosInVideoMode                             NOTIFY infoChanged )
    Q_PROPERTY(bool videoInPhotoMode                 READ videoInPhotoMode                              NOTIFY infoChanged )
    Q_PROPERTY(int  photoCount                       READ photoCount                                    NOTIFY photoCountChanged )

    Q_PROPERTY(QStringList        activeSettings     READ activeSettings                                NOTIFY activeSettingsChanged )
    Q_PROPERTY(VideoCaptureStatus videoCaptureStatus READ videoCaptureStatus                            NOTIFY videoCaptureStatusChanged)
    Q_PROPERTY(PhotoCaptureStatus photoCaptureStatus READ photoCaptureStatus                            NOTIFY photoCaptureStatusChanged)
    Q_PROPERTY(CameraMode         cameraMode         READ cameraMode          WRITE setCameraMode       NOTIFY cameraModeChanged)
    Q_PROPERTY(PhotoCaptureMode   photoCaptureMode   READ photoCaptureMode    WRITE setPhotoCaptureMode NOTIFY photoCaptureModeChanged)
    Q_PROPERTY(quint32            recordTime         READ recordTime                                    NOTIFY recordTimeChanged)
    Q_PROPERTY(QString            recordTimeStr      READ recordTimeStr                                 NOTIFY recordTimeChanged)

    Q_INVOKABLE virtual void setCameraModeVideo () = 0;
    Q_INVOKABLE virtual void setCameraModePhoto () = 0;
    Q_INVOKABLE virtual bool takePhoto           (QMediaCaptureSession* captureSession = nullptr) = 0;

    Q_INVOKABLE virtual bool startVideoRecording(QMediaCaptureSession* captureSession = nullptr)  = 0;
    Q_INVOKABLE virtual bool stopVideoRecording()   = 0;
    Q_INVOKABLE virtual bool toggleVideoRecording(QMediaCaptureSession* captureSession = nullptr) = 0;

    Q_INVOKABLE virtual void resetSettings () = 0;

    virtual bool capturesVideo     () = 0;
    virtual bool capturesPhotos    () = 0;
    virtual bool hasModes          () = 0;
    virtual bool hasVideoStream    () = 0;

    virtual bool photosInVideoMode () = 0;
    virtual bool videoInPhotoMode  () = 0;
    virtual int  photoCount        () = 0;

    virtual VideoCaptureStatus videoCaptureStatus () = 0;
    virtual PhotoCaptureStatus photoCaptureStatus () = 0;
    virtual PhotoCaptureMode   photoCaptureMode   () = 0;
    virtual CameraMode         cameraMode         () = 0;
    virtual QStringList        activeSettings     () = 0;

    virtual quint32 recordTime    () = 0;
    virtual QString recordTimeStr () = 0;

    virtual void setCameraMode      (CameraMode mode) = 0;
    virtual void setPhotoCaptureMode (PhotoCaptureMode mode) = 0;

    QString cameraModeToStr (CameraMode mode);

signals:
    void infoChanged                ();
    void photoCountChanged          ();
    void videoCaptureStatusChanged  ();
    void photoCaptureStatusChanged  ();
    void photoCaptureModeChanged    ();
    void cameraModeChanged          ();
    void activeSettingsChanged      ();
    void recordTimeChanged          ();
};