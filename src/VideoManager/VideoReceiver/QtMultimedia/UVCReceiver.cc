#include "UVCReceiver.h"
#include "QUAVApplication.h"
#include "SettingsManager.h"
#include "VideoSettings.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QPermissions>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QCameraDevice>
#include <QtMultimedia/QImageCapture>
#include <QtMultimedia/QMediaCaptureSession>
#include <QtMultimedia/QMediaDevices>

UVCReceiver::UVCReceiver(QObject *parent)
           : QtMultimediaReceiver(parent)
{
}

UVCReceiver::~UVCReceiver()
{

}

bool UVCReceiver::enabled()
{
    return !QMediaDevices::videoInputs().isEmpty();
}

QCameraDevice UVCReceiver::findCameraDevice(const QString& cameraId)
{
    const QList<QCameraDevice> videoInputs = QMediaDevices::videoInputs();
    for (const QCameraDevice& camera : videoInputs) {
        if (camera.description() == cameraId) {
            return camera;
        }
    }
    return QCameraDevice();
}

bool UVCReceiver::deviceExists(const QString& device)
{
    return !findCameraDevice(device).isNull();
}

void UVCReceiver::checkPermission()
{
    const QCameraPermission cameraPermission;
    if (qApp->checkPermission(cameraPermission) == Qt::PermissionStatus::Undetermined) {
        qApp->requestPermission(cameraPermission, quavApp(), [](const QPermission& permission) {
            if (permission.status() != Qt::PermissionStatus::Granted) {
                quavApp()->showAppMessage(QStringLiteral("Failed to get camera permission"));
            }
        });
    }
}

QString UVCReceiver::getSourceId()
{
    const QString videoSource = SettingsManager::instance()->videoSettings()->videoSource()->rawValue().toString();
    const QCameraDevice cameraDevice = findCameraDevice(videoSource);
    if (cameraDevice.isNull()) {
        return QString();
    }

    const QString videoSourceID = cameraDevice.description();
    qDebug() << "Found USB source:" << videoSourceID << "Name:" << videoSource;
    return videoSourceID;
}

QStringList UVCReceiver::getDeviceNameList()
{
    QStringList deviceNameList;
    const QList<QCameraDevice> videoInputs = QMediaDevices::videoInputs();
    for (const QCameraDevice& cameraDevice : videoInputs) { deviceNameList.append(cameraDevice.description());}
    return deviceNameList;
}













