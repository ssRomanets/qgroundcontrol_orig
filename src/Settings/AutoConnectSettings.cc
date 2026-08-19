
#include "AutoConnectSettings.h"

DECLARE_SETTINGGROUP(AutoConnect, "AutoConnect")
{
}

DECLARE_SETTINGSFACT_NO_FUNC(AutoConnectSettings, autoConnectPixhawk)
{
    if (!m_autoConnectPixhawkFact) { m_autoConnectPixhawkFact = m_createSettingsFact(autoConnectPixhawkName); }
    return m_autoConnectPixhawkFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(AutoConnectSettings, autoConnectNmeaPort)
{
    if (!m_autoConnectNmeaPortFact) { m_autoConnectNmeaPortFact = m_createSettingsFact(autoConnectNmeaPortName); }
    return m_autoConnectNmeaPortFact;
}