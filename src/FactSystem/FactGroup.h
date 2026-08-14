#pragma once

#include <QObject>
#include <QtCore/QJsonArray>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

#include "Fact.h"
#include "MAVLinkLib.h"

class Vehicle;

/// Used to group Facts together into an object hierarachy.
class FactGroup: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool telemetryAvailable READ telemetryAvailable NOTIFY telemetryAvailableChanged)

public:
    explicit FactGroup(int updateRateMsecs, const QString& metaDataFile, QObject* parent = nullptr, bool ignoreCamelCase = false);
    explicit FactGroup(int updateRateMsecs,  QObject* parent = nullptr, bool ignoreCamelCase = false);
    virtual ~FactGroup();

    bool telemetryAvailable() const { return m_telemetryAvailable; }

    /// Allows a FactGroup to parse incoming messages and fill in values
    virtual void handleMessage(Vehicle* vehicle, const mavlink_message_t& message) {}

signals:
    void telemetryAvailableChanged(bool telemetryAvailable);

protected slots:
    virtual void m_updateAllValues();

protected:
    void m_addFact(Fact* fact, const QString& name);
    void m_addFact(Fact* fact) {m_addFact(fact, fact->name());}
    void m_setTelemetryAvailable(bool telemetryAvailable);

    const int m_updateRateMSecs = 0;

    QMap<QString, Fact*> m_nameToFactMap;
    QMap<QString, FactMetaData*> m_nameToFactMetaDataMap;

private:
    void m_setupTimer();

    QTimer m_updateTimer;
    bool   m_telemetryAvailable = false;
};



















