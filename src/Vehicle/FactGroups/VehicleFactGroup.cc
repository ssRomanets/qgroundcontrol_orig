#include "VehicleFactGroup.h"
#include "Vehicle.h"
#include "QUAV.h"

#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>

VehicleFactGroup::VehicleFactGroup(QObject *parent)
    : FactGroup(100, QStringLiteral("/json/Vehicle/VehicleFact.json"), parent)
{
    m_addFact(&m_rollFact);
    m_addFact(&m_pitchFact);
    m_addFact(&m_headingFact);
}

void VehicleFactGroup::handleMessage(Vehicle* vehicle, const mavlink_message_t& message)
{
    switch (message.msgid) {
        case MAVLINK_MSG_ID_ATTITUDE:
        {
            m_handleAttitude(vehicle, message);
            break;
        }
        case MAVLINK_MSG_ID_ATTITUDE_QUATERNION:
        {
            m_handleAttitudeQuaternion(vehicle, message);
            break;
        }
        case MAVLINK_MSG_ID_LOCAL_POSITION_NED:
        {
            m_handleLocalPositionNed(vehicle, message);
            break;
        }
        default: break;
    }
}

void VehicleFactGroup::m_handleAttitudeWorker(double rollRadians, double pitchRadians, double yawRadians)
{
    double rollDegrees  = QUAV::limitAngleToPMPIf(rollRadians);
    double pitchDegrees = QUAV::limitAngleToPMPIf(pitchRadians);
    double yawDegrees   = QUAV::limitAngleToPMPIf(yawRadians);

    rollDegrees  = qRadiansToDegrees(rollDegrees);
    pitchDegrees = qRadiansToDegrees(pitchDegrees);
    yawDegrees   = qRadiansToDegrees(yawDegrees);

    if (yawDegrees < 0.0) { yawDegrees += 360.0;}

    // truncate to integer so widget never displays 360
    yawDegrees = trunc(yawDegrees);

    roll()->setRawValue(rollDegrees);
    pitch()->setRawValue(pitchDegrees);
    heading()->setRawValue(yawDegrees);
}

void VehicleFactGroup::m_handleAttitude(Vehicle* vehicle, const mavlink_message_t& message)
{
    if ((message.sysid != vehicle->id()) || (message.compid != vehicle->compId())) {
        return;
    }

    if (m_receivingAttitudeQuaternion) { return;}

    mavlink_attitude_t attitude{};
    mavlink_msg_attitude_decode(&message, &attitude);

    m_handleAttitudeWorker(attitude.roll, attitude.pitch, attitude.yaw);

    m_setTelemetryAvailable(true);
}

void VehicleFactGroup::m_handleAttitudeQuaternion(Vehicle* vehicle, const mavlink_message_t& message)
{
    // only accept the attitude message from the vehicle's flight controller
    if ((message.sysid != vehicle->id()) || (message.compid != vehicle->compId())) {return;}

    m_receivingAttitudeQuaternion = true;

    mavlink_attitude_quaternion_t attitudeQuaternion{};
    mavlink_msg_attitude_quaternion_decode(&message, &attitudeQuaternion);

    QQuaternion quat(attitudeQuaternion.q1, attitudeQuaternion.q2, attitudeQuaternion.q3, attitudeQuaternion.q4);

    QQuaternion repr_offset(
        attitudeQuaternion.repr_offset_q[0], attitudeQuaternion.repr_offset_q[1],
        attitudeQuaternion.repr_offset_q[2], attitudeQuaternion.repr_offset_q[3]
    );

    // if repr_offset is valid, rotate attitude and rates
    if (repr_offset.length() >= 0.5f) {
        quat *= repr_offset;
    }

    float attRoll, attPitch, attYaw;
    float q[] = {quat.scalar(), quat.x(), quat.y(), quat.z()};
    mavlink_quaternion_to_euler(q, &attRoll, &attPitch, &attYaw);

    m_handleAttitudeWorker(attRoll, attPitch, attYaw);

    m_setTelemetryAvailable(true);
}

void VehicleFactGroup::m_handleLocalPositionNed (Vehicle* vehicle, const mavlink_message_t& message)
{
    // only accept the attitude message from the vehicle's flight controller
    if ((message.sysid != vehicle->id()) || (message.compid != vehicle->compId())) {return;}

    m_zFact.setRawValue(mavlink_msg_local_position_ned_cov_get_az(&message));
    m_vxFact.setRawValue(mavlink_msg_local_position_ned_cov_get_vx(&message));
    m_vyFact.setRawValue(mavlink_msg_local_position_ned_cov_get_vy(&message));
    m_vzFact.setRawValue(mavlink_msg_local_position_ned_cov_get_vz(&message));
}




























