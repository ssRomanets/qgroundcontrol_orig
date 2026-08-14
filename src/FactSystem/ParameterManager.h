#pragma once

#include <QtCore/QObject>

#include "Fact.h"
#include "FactMetaData.h"
#include "MAVLinkLib.h"

class Vehicle;

class ParameterManager : public QObject
{
    Q_OBJECT

public:
    ParameterManager(Vehicle* vehicle);
    ~ParameterManager();

    static void registerQmlTypes();

    void mavlinkMessageReceived(const mavlink_message_t& message);

    /// Re-request the full set of parameters from the autopilot
    void refreshAllParameters(uint8_t componentId = MAV_COMP_ID_ALL);

private:
    /// Called whenever a parameter is updated or first seen.
    void m_handleParamValue(
        int componentId, const QString& parameterName, int parameterCount,
        int parameterIndex, MAV_PARAM_TYPE mavParamType, const QVariant& parameterValue
    );

    void m_checkInitialLoadComplete();

    Vehicle* m_vehicle = nullptr;

    bool m_initialLoadComplete = false;
    bool m_tryftp = false;
};


















