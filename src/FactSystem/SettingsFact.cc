#include "SettingsFact.h"
#include "QUAVApplication.h"
#include "QUAVCorePlugin.h"

#include <QtCore/QSettings>

SettingsFact::SettingsFact(QObject *parent)
    : Fact(parent)
{
    // qDebug() << Q_FUNC_INFO << this;
}

SettingsFact::SettingsFact(const QString &settingsGroup, FactMetaData *metaData, QObject *parent)
    : Fact(0, metaData->name(), metaData->type(), parent)
    , m_settingsGroup(settingsGroup)
{
    // qDebug() << Q_FUNC_INFO << this;
    QSettings settings;

    if (!m_settingsGroup.isEmpty()) {
        settings.beginGroup(m_settingsGroup);
    }

    // Allow core plugin a chance to override the default value
    m_visible = QUAVCorePlugin::instance()->adjustSettingMetaData(settingsGroup, *metaData);
    setMetaData(metaData);

    if (metaData->defaultValueAvailable()) {
        const QVariant rawDefaultValue = metaData->rawDefaultValue();
        if (quavApp()->runningUnitTests()) {
            // Don't use saved settings
            m_rawValue = rawDefaultValue;
        } else if (m_visible) {
            QVariant typedValue;
            QString errorString;
            (void) metaData->convertAndValidateRaw(settings.value(m_name, rawDefaultValue), true /* conertOnly */, typedValue, errorString);
            m_rawValue = typedValue;
        } else {
            // Setting is not visible, force to default value always
            settings.setValue(m_name, rawDefaultValue);
            m_rawValue = rawDefaultValue;
        }
    }

    (void) connect(this, &Fact::rawValueChanged, this, &SettingsFact::m_rawValueChanged);
}

SettingsFact::SettingsFact(const SettingsFact &other, QObject *parent)
    : Fact(other, parent)
{
    // qDebug() << Q_FUNC_INFO << this;
    *this = other;
}

SettingsFact::~SettingsFact()
{
    // qDebug() << Q_FUNC_INFO << this;
}

const SettingsFact &SettingsFact::operator=(const SettingsFact &other)
{
    Fact::operator=(other);

    m_settingsGroup = other.m_settingsGroup;

    return *this;
}

void SettingsFact::m_rawValueChanged(const QVariant &value)
{
    QSettings settings;

    if (!m_settingsGroup.isEmpty()) {
        settings.beginGroup(m_settingsGroup);
    }

    settings.setValue(m_name, value);
}
