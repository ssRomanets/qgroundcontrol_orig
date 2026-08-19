
#include "EsriMapProvider.h"
#include "SettingsManager.h"
#include "AppSettings.h"

QByteArray EsriMapProvider::getToken() const
{
    return SettingsManager::instance()->appSettings()->esriToken()->rawValue().toString().toUtf8();
}

QString EsriMapProvider::m_getURL(int x, int y, int zoom) const
{
    return m_mapUrl.arg(m_mapTypeId).arg(zoom).arg(y).arg(x);
}
