#pragma once

#include <QtCore/QList>
#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtQmlIntegration/QtQmlIntegration>

#include "MAVLinkLib.h"

// TODO: Q_NAMESPACE
class QUAVMAVLink : public QObject
{
    Q_OBJECT

public:
    // Creating an instance of QGCMAVLink is only meant to be used for the Qml Singleton
    QUAVMAVLink(QObject *parent = nullptr);
    ~QUAVMAVLink();

    static bool isValidChannel(uint8_t channel) { return (channel < MAVLINK_COMM_NUM_BUFFERS); }
    static bool isValidChannel(mavlink_channel_t channel) { return isValidChannel(static_cast<uint8_t>(channel)); }
};