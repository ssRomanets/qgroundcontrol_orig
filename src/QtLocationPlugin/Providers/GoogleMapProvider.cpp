
#include "GoogleMapProvider.h"

void GoogleMapProvider::m_getSecGoogleWords(int x, int y, QString& sec1, QString& sec2) const
{
    sec1 = QStringLiteral(""); // after &x=...
    sec2 = QStringLiteral(""); // after &zoom=...
    const int seclen = ((x * 3) + y) % 8;
    sec2 = m_secGoogleWord.left(seclen);
    if ((y >= 10000) && (y < 100000)) {
        sec1 = QStringLiteral("&s=");
    }
}

QString GoogleMapProvider::m_getURL(int x, int y, int zoom) const
{
    QString sec1;
    QString sec2;
    m_getSecGoogleWords(x, y, sec1, sec2);
    return m_mapUrl
        .arg(m_getServerNum(x, y, 4))
        .arg(m_versionRequest, m_version, m_language)
        .arg(x)
        .arg(sec1)
        .arg(y)
        .arg(zoom)
        .arg(sec2, m_scale);
}
