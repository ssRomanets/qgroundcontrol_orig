#include "MAVLinkMessageField.h"
#include "MAVLinkMessage.h"
#include "QUAVApplication.h"

#include <QtCharts/QLineSeries>
#include <QtCharts/QAbstractSeries>

QUAVMAVLinkMessageField::QUAVMAVLinkMessageField(const QString &name, const QString &type, QUAVMAVLinkMessage *parent)
    : QObject(parent)
    , m_type(type)
    , m_name(name)
    , m_msg(parent)
{
//    qDebug() << "Field:" << name << type;
}

QUAVMAVLinkMessageField::~QUAVMAVLinkMessageField()
{
    // qDebug() << Q_FUNC_INFO << this;
}

void QUAVMAVLinkMessageField::setSelectable(bool sel)
{
    if (m_selectable != sel) {
        m_selectable = sel;
        emit selectableChanged();
    }
}


void QUAVMAVLinkMessageField::updateValue(const QString &newValue, qreal v)
{
    if (m_value != newValue) {
        m_value = newValue;
        emit valueChanged();
    }
}