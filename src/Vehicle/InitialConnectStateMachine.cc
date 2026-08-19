#include "InitialConnectStateMachine.h"
#include "Vehicle.h"
#include "StandardModes.h"
#include "ParameterManager.h"

InitialConnectStateMachine::InitialConnectStateMachine(Vehicle *vehicle, QObject *parent)
                          : StateMachine(parent)
                          , m_vehicle(vehicle)
{

}

InitialConnectStateMachine::~InitialConnectStateMachine()
{

}

int InitialConnectStateMachine::stateCount(void) const
{
    return m_cStates;
}

const InitialConnectStateMachine::StateFn* InitialConnectStateMachine::rgStates(void) const
{
    return &m_rgStates[0];
}

void InitialConnectStateMachine::advance()
{
    StateMachine::advance();
}

void InitialConnectStateMachine::m_stateRequestStandardModes(StateMachine* stateMachine)
{
    InitialConnectStateMachine* connectMachine = static_cast<InitialConnectStateMachine*>(stateMachine);
    Vehicle* vehicle            = connectMachine->m_vehicle;

    qDebug() << "m_stateRequestStandardModes";
    connect(
        vehicle->m_standardModes, &StandardModes::requestCompleted,
        connectMachine, &InitialConnectStateMachine::statndardModesRequestCompleted
    );
    vehicle->m_standardModes->request();
}

void InitialConnectStateMachine::statndardModesRequestCompleted()
{
    disconnect(
        m_vehicle->m_standardModes, &StandardModes::requestCompleted,
        this, &InitialConnectStateMachine::statndardModesRequestCompleted
    );
    advance();
}

void InitialConnectStateMachine::m_stateRequestParameters(StateMachine* stateMachine)
{
    InitialConnectStateMachine* connectMachine = static_cast<InitialConnectStateMachine*>(stateMachine);
    Vehicle*                    vehicle        = connectMachine->m_vehicle;

    vehicle->m_parameterManager->refreshAllParameters();
}




















