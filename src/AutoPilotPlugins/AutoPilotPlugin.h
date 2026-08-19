#pragma once

#include <QtCore/QVariantList>
#include <QtCore/QObject>
#include <QtCore/QString>

class Vehicle;

/// The AutoPilotPlugin class is an abstract base class which represent the methods and objects
/// which are specific to a certain AutoPilot. This is the only place where AutoPilot specific
/// code should reside in QGroundControl. The remainder of the QGroundControl source is
/// generic to a common mavlink implementation.

class AutoPilotPlugin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool setupComplete READ setupComplete NOTIFY setupCompleteChanged)

public:
    explicit AutoPilotPlugin(Vehicle* vehicle, QObject* parent = nullptr);
    virtual ~AutoPilotPlugin();

    /// Called when parameters are ready for the first time. Note that parameters may still be missing.
    /// Overrides must call base class.
    virtual void parametersReadyPreChecks();

    bool setupComplete() const { return m_setupComplete;}

signals:
    void setupCompleteChanged();

protected:
    Vehicle* m_vehicle = nullptr;
    bool m_setupComplete = false;
};