#pragma once

#include "MAVLinkLib.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>

class Vehicle;

class StandardModes : public QObject
{
    Q_OBJECT

public:
    StandardModes(QObject* parent, Vehicle* vehicle);

    void request();

    void gotMessage(MAV_RESULT result, const mavlink_message_t& message);

signals:
    void requestCompleted();

private:
    void requestMode(int modeIndex);

    Vehicle* const m_vehicle;

    bool m_requestActive {false};
    bool m_wantReset     {false};
};
















