#pragma once

#include <QScopedPointer>

#include <QtCore/QLoggingCategory>
#include <QtCore/QMetaObject>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtMultimedia/QMediaCaptureSession>

#include "VideoReceiver.h"

class QCamera;
class QMediaRecorder;
class QImageCapture;

class QtMultimediaReceiver : public VideoReceiver
{
    Q_OBJECT

public:
    explicit QtMultimediaReceiver(QObject* parent = nullptr);
    virtual ~QtMultimediaReceiver();

    static VideoReceiver* createVideoReceiver(QObject* parent);

public slots:
    void start() override;

    void capturePhoto   (QMediaCaptureSession* captureSession, const QString& photoFile)                                    override;
    void startRecording (QMediaCaptureSession* captureSession, const QString &videoFile, VideoReceiver::FILE_FORMAT format) override;
    void stopRecording();

protected:
    QMediaRecorder*      m_mediaRecorder = nullptr;
    QImageCapture*       m_imageCapture  = nullptr;
};














