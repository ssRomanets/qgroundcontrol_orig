#include "FactGroup.h"

FactGroup::FactGroup(int updateRateMsecs, const QString& metaDataFile, QObject* parent, bool ignoreCamelCase)
    : QObject(parent)
    , m_updateRateMSecs(updateRateMsecs)
{
    m_setupTimer();
    m_nameToFactMetaDataMap = FactMetaData::createMapFromJsonFile(metaDataFile, this);
}

FactGroup::FactGroup(int updateRateMsecs,  QObject* parent, bool ignoreCamelCase)
         : QObject(parent)
         , m_updateRateMSecs(updateRateMsecs)
{
    m_setupTimer();
}

FactGroup::~FactGroup()
{
}

void FactGroup::m_setupTimer()
{
    if (m_updateRateMSecs > 0) {
        (void)connect(&m_updateTimer, &QTimer::timeout, this, &FactGroup::m_updateAllValues);
        m_updateTimer.setSingleShot(false);
        m_updateTimer.setInterval(m_updateRateMSecs);
        m_updateTimer.start();
    }
}

void FactGroup::m_addFact(Fact* fact, const QString& name)
{
    if (m_nameToFactMap.contains(name)) {
        qWarning() << "Duplicate Fact" << name;
        return;
    }

    fact->setSendValueChangedSignals(m_updateRateMSecs == 0);
    if (m_nameToFactMetaDataMap.contains(name)) {
        fact->setMetaData(m_nameToFactMetaDataMap[name], true);
    }
    m_nameToFactMap[name] = fact;
}

void FactGroup::m_updateAllValues()
{
    for (Fact* fact: m_nameToFactMap) {
        fact->sendDeferredValueChangedSignal();
    }
}

void FactGroup::m_setTelemetryAvailable(bool telemetryAvailable)
{
    if (telemetryAvailable != m_telemetryAvailable) {
        m_telemetryAvailable = telemetryAvailable;
        emit telemetryAvailableChanged(m_telemetryAvailable);
    }
}



















































