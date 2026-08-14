#pragma once

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QTimer>
#include <QMediaCaptureSession>

class VideoReceiver : public QObject
{
    Q_OBJECT

public:
    explicit VideoReceiver(QObject* parent = nullptr) : QObject(parent) {}

    bool isThermal() const { return (m_name == QStringLiteral("thermalVideo"));}

    QString name() const { return m_name;}
    void setName(const QString& name) { if (name != m_name) {m_name = name; emit nameChanged(m_name);}}

    QString uri()  const { return m_uri;}
    void setUri(const QString& uri) {if (uri != m_uri) { m_uri = uri; emit uriChanged(m_uri);}}

    bool started() const { return m_started;}
    void setStarted(bool started) {if (started != m_started) {m_started = started; emit startedChanged(m_started);}}

    enum FILE_FORMAT {
        FILE_FORMAT_MIN = 0,
        FILE_FORMAT_MKV  = FILE_FORMAT_MIN,
        FILE_FORMAT_MOV,
        FILE_FORMAT_MP4,
        FILE_FORMAT_MAX = FILE_FORMAT_MP4
    };
    Q_ENUM(FILE_FORMAT)
    static bool isValidFileFormat(FILE_FORMAT format) { return ((format >= FILE_FORMAT_MIN) && (format <= FILE_FORMAT_MAX));}

    enum STATUS {
        STATUS_MIN = 0,
        STATUS_OK  = STATUS_MIN,
        STATUS_FAIL,
        STATUS_INVALID_STATE,
        STATUS_INVALID_URL,
        STATUS_NOT_IMPLEMENTED,
        STATUS_MAX = STATUS_NOT_IMPLEMENTED
    };
    Q_ENUM(STATUS)

signals:
    void recordingChanged       (bool active);
    void nameChanged            (const QString& name);
    void uriChanged             (const QString& uri);
    void startedChanged         (bool started);

    void onStartComplete(STATUS status);

public slots:
    virtual void start() = 0;

    virtual void capturePhoto   (QMediaCaptureSession* captureSession, const QString& photoFile) = 0;
    virtual void startRecording (QMediaCaptureSession* captureSession, const QString& videoFile, FILE_FORMAT format) = 0;
    virtual void stopRecording  () = 0;

protected:
    QString m_name;
    QString m_uri;
    bool    m_started = false;
};












