#include "QmlComponentInfo.h"

QmlComponentInfo::QmlComponentInfo(const QString &title, QUrl url, QUrl icon, QObject *parent)
    : QObject(parent)
    , m_title(title)
    , m_url(url)
    , m_icon(icon)
{
}

QmlComponentInfo::~QmlComponentInfo()
{
}