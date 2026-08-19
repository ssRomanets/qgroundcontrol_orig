
#include "GenericMapProvider.h"
#include "SettingsManager.h"
#include "AppSettings.h"

QString CustomURLMapProvider::m_getURL(int x, int y, int zoom) const
{
    QString url = SettingsManager::instance()->appSettings()->customURL()->rawValue().toString();
    (void) url.replace("{x}", QString::number(x));
    (void) url.replace("{y}", QString::number(y));
    static const QRegularExpression zoomRegExp("\\{(z|zoom)\\}");
    (void) url.replace(zoomRegExp, QString::number(zoom));
    return url;
}

QString CyberJapanMapProvider::m_getURL(int x, int y, int zoom) const
{
    return m_mapUrl.arg(m_mapName).arg(zoom).arg(x).arg(y).arg(m_imageFormat);
}

QString LINZBasemapMapProvider::m_getURL(int x, int y, int zoom) const
{
    return _mapUrl.arg(zoom).arg(x).arg(y).arg(m_imageFormat);
}

QString OpenStreetMapProvider::m_getURL(int x, int y, int zoom) const
{
    return m_mapUrl.arg(zoom).arg(x).arg(y);
}

QString StatkartMapProvider::m_getURL(int x, int y, int zoom) const
{
    return m_mapUrl.arg(zoom).arg(y).arg(x);
}

QString EniroMapProvider::m_getURL(int x, int y, int zoom) const
{
    return m_mapUrl.arg(zoom).arg(x).arg((1 << zoom) - 1 - y).arg(m_imageFormat);
}

QString SvalbardMapProvider::m_getURL(int x, int y, int zoom) const
{
    return m_mapUrl.arg(zoom).arg(y).arg(x);
}

QString MapQuestMapProvider::m_getURL(int x, int y, int zoom) const
{
    return m_mapUrl.arg(m_getServerNum(x, y, 4)).arg(m_mapName).arg(zoom).arg(x).arg(y).arg(m_imageFormat);
}

QString VWorldMapProvider::m_getURL(int x, int y, int zoom) const
{
    if ((zoom < 5) || (zoom > 19)) {
        return QString();
    }

    const int gap = zoom - 6;

    const int x_min = 53 * pow(2, gap);
    const int x_max = (55 * pow(2, gap)) + (2 * gap - 1);
    if ((x < x_min) || (x > x_max)) {
        return QString();
    }

    const int y_min = 22 * pow(2, gap);
    const int y_max = (26 * pow(2, gap)) + (2 * gap - 1);
    if ((y < y_min) || (y > y_max)) {
        return QString();
    }

    const QString VWorldMapToken = SettingsManager::instance()->appSettings()->vworldToken()->rawValue().toString();
    return m_mapUrl.arg(VWorldMapToken, m_mapName).arg(zoom).arg(y).arg(x).arg(m_imageFormat);
}
