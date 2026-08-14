#pragma once

#include <QtCore/QObject>
#include <QtCore/QRunnable>
#include <QtCore/QSize>
#include <QDateTime>

#include <QMediaCaptureSession>

class Vehicle;
class VideoReceiver;
class VideoSettings;

class VideoManager : public QObject
{
    Q_OBJECT
    Q_MOC_INCLUDE("Vehicle.h")

    Q_PROPERTY(bool     uvcEnabled           READ uvcEnabled    CONSTANT)
    Q_PROPERTY(bool     fullScreen           READ fullScreen    WRITE setfullScreen  NOTIFY fullScreenChanged)
    Q_PROPERTY(bool     hasVideo             READ hasVideo                           NOTIFY hasVideoChanged)
    Q_PROPERTY(bool     isUvc                READ isUvc                              NOTIFY isUvcChanged)
    Q_PROPERTY(bool     recording            READ recording                          NOTIFY recordingChanged)
    Q_PROPERTY(QString  uvcVideoSourceID     READ uvcVideoSourceID                   NOTIFY uvcVideoSourceIDChanged)

public:
    explicit VideoManager(QObject *parent = nullptr);
    ~VideoManager();

    static VideoManager* instance();

    void capturePhoto   (QMediaCaptureSession* captureSession);

    Q_INVOKABLE void startRecording (QMediaCaptureSession* captureSession);
    Q_INVOKABLE void stopRecording();

    void        init();
    bool        fullScreen()       const { return m_fullScreen; }
    bool        hasVideo()         const;
    bool        isUvc()            const;
    bool        recording()        const { return m_recording;}
    QString     uvcVideoSourceID() const { return m_uvcVideoSourceID;}
    void        setfullScreen(bool on);
    static bool uvcEnabled();

signals:
    void fullScreenChanged();
    void hasVideoChanged();
    void isUvcChanged();
    void recordingChanged();
    void uvcVideoSourceIDChanged();

private slots:
    void m_setActiveVehicle(Vehicle* vehicle);
    void m_videoSourceChanged();

private:
    void m_initVideoReceiver(VideoReceiver* receiver);
    bool m_updateUVC        (VideoReceiver* receiver);
    bool m_updateSettings   (VideoReceiver* receiver);
    bool m_updateVideoUri   (VideoReceiver* receiver, const QString& uri);
    void m_startReceiver    (VideoReceiver* receiver);
    static void m_cleanupOldVideos();

    QList<VideoReceiver*> m_videoReceivers;

    VideoSettings* m_videoSettings = nullptr;

    bool m_initialized = false;
    bool m_fullScreen = false;
    QAtomicInteger<bool> m_recording = false;
    QString m_uvcVideoSourceID;
    Vehicle* m_activeVehicle = nullptr;
};