#include "QtMultimediaReceiver.h"

#include <QUrl>
#include <QCamera>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QMediaMetaData>
#include <QMediaRecorder>
#include <QImageCapture>

QtMultimediaReceiver::QtMultimediaReceiver(QObject* parent)
    : VideoReceiver(parent)
{
    qDebug() << Q_FUNC_INFO << this;

    m_mediaRecorder = new QMediaRecorder(this);
    m_imageCapture  = new QImageCapture(this);

    (void) connect(m_mediaRecorder, &QMediaRecorder::recorderStateChanged, this, [this](QMediaRecorder::RecorderState state) {
        emit recordingChanged(m_mediaRecorder->recorderState() == QMediaRecorder::RecorderState::RecordingState);
    });
}

QtMultimediaReceiver::~QtMultimediaReceiver()
{
    qDebug() << Q_FUNC_INFO << this;
}

VideoReceiver* QtMultimediaReceiver::createVideoReceiver(QObject* parent)
{
    Q_UNUSED(parent);
    return new QtMultimediaReceiver(nullptr);
}

void QtMultimediaReceiver::start()
{
    qDebug() << Q_FUNC_INFO;

    emit onStartComplete(STATUS_OK);
}

void QtMultimediaReceiver::capturePhoto(QMediaCaptureSession* captureSession, const QString& photoFile)
{
    if (captureSession->imageCapture() == nullptr) captureSession->setImageCapture(m_imageCapture);
    m_imageCapture->captureToFile(photoFile);
}

void QtMultimediaReceiver::startRecording(QMediaCaptureSession* captureSession, const QString& videoFile, FILE_FORMAT format)
{
    if (captureSession->recorder() != m_mediaRecorder) captureSession->setRecorder(m_mediaRecorder);

    qDebug() << Q_FUNC_INFO;

    if (!m_mediaRecorder->isAvailable()) {
        qWarning() << "Recording Unavailable";
        return;
    }

    switch (format) {
        case FILE_FORMAT_MKV:
        {
            m_mediaRecorder->setMediaFormat(QMediaFormat::FileFormat::Matroska);
            break;
        }
        case FILE_FORMAT_MOV:
        {
            m_mediaRecorder->setMediaFormat(QMediaFormat::FileFormat::QuickTime);
            break;
        }
        case FILE_FORMAT_MP4:
        {
            m_mediaRecorder->setMediaFormat(QMediaFormat::FileFormat::MPEG4);
            break;
        }
        default:
        {
            // QMediaFormat::AVI, WMV, Ogg, WebM
            m_mediaRecorder->setMediaFormat(QMediaFormat::FileFormat::UnspecifiedFormat);
            break;
        }
    }

    qDebug() << "videoFile"<<" "<<videoFile;

    m_mediaRecorder->setOutputLocation(QUrl::fromLocalFile(videoFile));
    m_mediaRecorder->record();

    qDebug() << "Recording";
}

void QtMultimediaReceiver::stopRecording()
{
    qDebug() << Q_FUNC_INFO;

    m_mediaRecorder->stop();

    qDebug() << "Stopped Recording";
}
















