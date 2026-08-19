#pragma once

#include <QtMultimedia/QCameraDevice>

#include "QtMultimediaReceiver.h"

class UVCReceiver : public QtMultimediaReceiver
{
    Q_OBJECT

public:
    explicit UVCReceiver(QObject* parent = nullptr);
    ~UVCReceiver();

    static bool          enabled();
    static QCameraDevice findCameraDevice(const QString& cameraId);
    static void          checkPermission();
    static QString       getSourceId();
    static bool          deviceExists(const QString& device);
    static QStringList   getDeviceNameList();
};
