#include "StateMachine.h"

StateMachine::StateMachine(QObject *parent)
    : QObject(parent)
{

}

StateMachine::~StateMachine()
{

}

void StateMachine::start()
{
    m_active = true;
    advance();
}

void StateMachine::advance()
{
    if (!m_active) {
        return;
    }

    m_stateIndex++;
    if (m_stateIndex < stateCount()) {
        (*rgStates()[m_stateIndex])(this);
    } else {
        m_active = false;
    }
}



