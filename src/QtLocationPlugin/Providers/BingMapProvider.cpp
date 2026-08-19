
#include "BingMapProvider.h"

QString BingMapProvider::m_getURL(int x, int y, int zoom) const
{
    const QString key = m_tileXYToQuadKey(x, y, zoom);
    return m_mapUrl.arg(m_getServerNum(x, y, 4)).arg(m_mapTypeId, key, m_imageFormat, m_versionBingMaps, m_language);
}
