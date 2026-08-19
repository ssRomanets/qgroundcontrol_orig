#pragma once

#include "StateMachine.h"
#include "MAVLinkLib.h"
#include "Vehicle.h"

class InitialConnectStateMachine : public StateMachine
{
    Q_OBJECT

public:
    InitialConnectStateMachine(Vehicle* vehicle, QObject* parent = nullptr);
    ~InitialConnectStateMachine();

    // Overrides from StateMachine
    int            stateCount (void) const final;
    const StateFn* rgStates   (void) const final;

    void advance() override;

private slots:
    void statndardModesRequestCompleted();

private:
    static void m_stateRequestStandardModes (StateMachine* stateMachine);
    static void m_stateRequestParameters (StateMachine* stateMachine);

    Vehicle* m_vehicle;

    static constexpr const StateMachine::StateFn m_rgStates[] = {
        m_stateRequestStandardModes,
        m_stateRequestParameters
    };

    static constexpr int m_cStates = sizeof(m_rgStates)/sizeof(m_rgStates[0]);
};























