#include "MAVLinkSystem.h"
#include "MAVLinkMessage.h"

QUAVMAVLinkSystem::QUAVMAVLinkSystem(quint8 id, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_messages(new QmlObjectListModel(this))
{
//    qDebug() << "New Vehicle:" << id;
}

QUAVMAVLinkSystem::~QUAVMAVLinkSystem()
{
    m_messages->clearAndDeleteContents();
}

QUAVMAVLinkMessage* QUAVMAVLinkSystem::findMessage(uint32_t id, uint8_t compId)
{
    for (int i = 0; i < m_messages->count(); i++) {
        QUAVMAVLinkMessage* const msg = qobject_cast<QUAVMAVLinkMessage*>(m_messages->get(i));
        if(msg) {
            if((msg->id() == id) && (msg->compId() == compId)) {
                return msg;
            }
        }
    }
    return nullptr;
}

int QUAVMAVLinkSystem::findMessage(const QUAVMAVLinkMessage* message)
{
    for (int i = 0; i < m_messages->count(); i++) {
        const QUAVMAVLinkMessage* const msg = qobject_cast<const QUAVMAVLinkMessage*>(m_messages->get(i));
        if (msg && (msg == message)) {
            return i;
        }
    }
    return -1;
}

void QUAVMAVLinkSystem::m_resetSelection()
{
    for (int i = 0; i < m_messages->count(); i++) {
        QUAVMAVLinkMessage* const msg = qobject_cast<QUAVMAVLinkMessage*>(m_messages->get(i));
        if (msg && msg->selected()) {
            msg->setSelected(false);
            emit msg->selectedChanged();
        }
    }
}

void QUAVMAVLinkSystem::setSelected(int sel)
{
    if (sel >= m_messages->count()) {  return; }

    m_selected = sel;
    emit selectedChanged();
    m_resetSelection();
    QUAVMAVLinkMessage* const msg = qobject_cast<QUAVMAVLinkMessage*>(m_messages->get(sel));
    if(msg && !msg->selected()) {
        msg->setSelected(true);
        emit msg->selectedChanged();
    }
}

QUAVMAVLinkMessage* QUAVMAVLinkSystem::selectedMsg()
{
    QUAVMAVLinkMessage* selectedMsg = nullptr;
    if (m_messages->count()) {
        selectedMsg = qobject_cast<QUAVMAVLinkMessage*>(m_messages->get(m_selected));
    }
    return selectedMsg;
}

static bool messages_sort(const QObject* a, const QObject* b)
{
    const QUAVMAVLinkMessage* const aa = qobject_cast<const QUAVMAVLinkMessage*>(a);
    const QUAVMAVLinkMessage* const bb = qobject_cast<const QUAVMAVLinkMessage*>(b);
    if (!aa || !bb) { return false; }

    if (aa->name() == bb->name()) {  return (aa->name() < bb->name()); }

    return (aa->name() < bb->name());
}

void QUAVMAVLinkSystem::append(QUAVMAVLinkMessage* message)
{
    QUAVMAVLinkMessage* selectedMsg = nullptr;
    if (m_messages->count()) {
        selectedMsg = qobject_cast<QUAVMAVLinkMessage*>(m_messages->get(m_selected));
    } else {
        message->setSelected(true);
    }
    m_messages->append(message);

    if (m_messages->count() > 0) {
        m_messages->beginResetModel();
        std::sort(m_messages->objectList()->begin(), m_messages->objectList()->end(), messages_sort);
        m_messages->endResetModel();
        m_checkCompID(message);
    }

    if (selectedMsg) {
        const int idx = findMessage(selectedMsg);
        if (idx >= 0) {
            m_selected = idx;
            emit selectedChanged();
        }
    }
}

void QUAVMAVLinkSystem::m_checkCompID(const QUAVMAVLinkMessage* message)
{
    if (m_compIDsStr.isEmpty()) { m_compIDsStr << tr("Comp All"); }

    if (!m_compIDs.contains(static_cast<int>(message->compId()))) {
        const int compId = static_cast<int>(message->compId());
        m_compIDs.append(compId);
        m_compIDsStr << tr("Comp %1").arg(compId);
        emit compIDsChanged();
    }
}