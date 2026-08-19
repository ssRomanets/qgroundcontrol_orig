
#pragma once

#include "FactGroup.h"

class VehicleFactGroup: public FactGroup
{
    Q_OBJECT
    Q_PROPERTY(Fact* z       READ z       CONSTANT)
    Q_PROPERTY(Fact* vx      READ vx      CONSTANT)
    Q_PROPERTY(Fact* vy      READ vy      CONSTANT)
    Q_PROPERTY(Fact* vz      READ vz      CONSTANT)
    Q_PROPERTY(Fact* roll    READ roll    CONSTANT)
    Q_PROPERTY(Fact* pitch   READ pitch   CONSTANT)
    Q_PROPERTY(Fact* heading READ heading CONSTANT)

public:
    explicit VehicleFactGroup(QObject* parent = nullptr);

    Fact* z      () {return &m_zFact;}
    Fact* vx     () {return &m_vxFact;}
    Fact* vy     () {return &m_vyFact;}
    Fact* vz     () {return &m_vzFact;}
    Fact* roll   () {return &m_rollFact;}
    Fact* pitch  () {return &m_pitchFact;}
    Fact* heading() {return &m_headingFact;}

    void handleMessage(Vehicle* vehicle, const mavlink_message_t& message) override;

protected:
    void m_handleAttitude           (Vehicle* vehicle, const mavlink_message_t& message);
    void m_handleAttitudeQuaternion (Vehicle* vehicle, const mavlink_message_t& message);
    void m_handleLocalPositionNed   (Vehicle* vehicle, const mavlink_message_t& message);

    Fact m_zFact       = Fact(0, QStringLiteral("z"), FactMetaData::valueTypeDouble);
    Fact m_vyFact      = Fact(0, QStringLiteral("vx"), FactMetaData::valueTypeDouble);
    Fact m_vxFact      = Fact(0, QStringLiteral("vy"), FactMetaData::valueTypeDouble);
    Fact m_vzFact      = Fact(0, QStringLiteral("vz"), FactMetaData::valueTypeDouble);
    Fact m_rollFact    = Fact(0, QStringLiteral("roll"), FactMetaData::valueTypeDouble);
    Fact m_pitchFact   = Fact(0, QStringLiteral("pitch"), FactMetaData::valueTypeDouble);
    Fact m_headingFact = Fact(0, QStringLiteral("heading"), FactMetaData::valueTypeDouble);

private:
    void m_handleAttitudeWorker(double rollRadians, double pitchRadians, double yawRadians);
    bool m_receivingAttitudeQuaternion = false;
};


















