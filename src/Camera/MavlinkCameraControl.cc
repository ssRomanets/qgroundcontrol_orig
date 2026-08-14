#include "MavlinkCameraControl.h"
#include "QmlObjectListModel.h"

MavlinkCameraControl::MavlinkCameraControl(QObject *parent)
    : FactGroup(0,parent,true)
{
    qDebug() << Q_FUNC_INFO << this;
}

MavlinkCameraControl::~MavlinkCameraControl()
{
    qDebug() << Q_FUNC_INFO << this;
}

QString MavlinkCameraControl::cameraModeToStr(CameraMode mode)
{
    switch (mode) {
        case CAM_MODE_UNDEFINED: return QStringLiteral("CAM_MODE_UNDEFINED");
        case CAM_MODE_PHOTO:     return QStringLiteral("CAM_MODE_PHOTO");
        case CAM_MODE_VIDEO:     return QStringLiteral("CAM_MODE_VIDEO");
        case CAM_MODE_SURVEY:    return QStringLiteral("CAM_MODE_SURVEY");
        default:                 return QStringLiteral("Unknown");
    }
}



