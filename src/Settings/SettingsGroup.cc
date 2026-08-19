
#include "SettingsGroup.h"
#include "QUAVCorePlugin.h"

#include <QtQml/QQmlEngine>

SettingsGroup::SettingsGroup(const QString& name, const QString& settingsGroup, QObject* parent)
    : QObject       (parent)
    , m_visible      (QUAVCorePlugin::instance()->overrideSettingsGroupVisibility(name))
    , m_name         (name)
    , m_settingsGroup(settingsGroup)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    m_nameToMetaDataMap = FactMetaData::createMapFromJsonFile(QString(kJsonFile).arg(name), this);
}

SettingsFact* SettingsGroup::m_createSettingsFact(const QString& factName)
{
    FactMetaData* m = m_nameToMetaDataMap[factName];
    if(!m) {
        qCritical() << "Fact name " << factName << "not found in" << QString(kJsonFile).arg(m_name);
        exit(-1);
    }
    return new SettingsFact(m_settingsGroup, m, this);
}

