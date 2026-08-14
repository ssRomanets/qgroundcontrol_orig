#pragma once

#include <QtCore/QObject>

class StateMachine : public QObject
{
    Q_OBJECT

public:
    StateMachine(QObject* parent = nullptr);
    ~StateMachine();

    typedef void (*StateFn)(StateMachine* stateMachine);

    /// Start the state machine with the first step
    void start();

    /// Advance the state machine to the next state and call the state function
    virtual void advance();

    /// @return The number of states in the rgStates array
    virtual int stateCount() const = 0;

    /// @return Array of states to execute
    virtual const StateFn* rgStates() const = 0;

protected:
    bool m_active    = false;
    int m_stateIndex = -1;
};



