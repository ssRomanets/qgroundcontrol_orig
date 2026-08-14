#include "LinkConfiguration.h"
#include "SerialLink.h"

LinkConfiguration::LinkConfiguration(const QString& name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    qDebug() << Q_FUNC_INFO << this;
}

LinkConfiguration::LinkConfiguration(const LinkConfiguration *copy, QObject *parent)
    : QObject(parent)
      , m_link(copy->m_link)
      , m_name(copy->name())
      , m_dynamic(copy->isDynamic())
      , m_autoConnect(copy->isAutoConnect())
      , m_highLatency(copy->isHighLatency())
{
    qDebug() << Q_FUNC_INFO << this;

    Q_ASSERT(!m_name.isEmpty());
}

LinkConfiguration::~LinkConfiguration()
{
     qDebug() << Q_FUNC_INFO << this;
}

void LinkConfiguration::copyFrom(const LinkConfiguration *source)
{
    Q_ASSERT(source);

    setLink(source->m_link.lock());
    setName(source->name());
    setDynamic(source->isDynamic());
    setAutoConnect(source->isAutoConnect());
    setHighLatency(source->isHighLatency());
}

void LinkConfiguration::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        emit nameChanged(name);
    }
}

void LinkConfiguration::setLink(const SharedLinkInterfacePtr link)
{
    if (link.get() != this->link()) {
        m_link = link;
        emit linkChanged();

    }
}

void LinkConfiguration::setDynamic(bool dynamic)
{
    if (dynamic != m_dynamic) {
        m_dynamic = dynamic;
        emit dynamicChanged();
    }
}

void LinkConfiguration::setAutoConnect(bool autoc)
{
    if (autoc != m_autoConnect) {
        m_autoConnect = autoc;
        emit autoConnectChanged();
    }
}

void LinkConfiguration::setHighLatency(bool hl)
{
    if (hl != m_highLatency) {
        m_highLatency = hl;
        emit highLatencyChanged();
    }
}