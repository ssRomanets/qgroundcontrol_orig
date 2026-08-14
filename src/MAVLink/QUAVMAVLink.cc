#include "QUAVMAVLink.h"

#ifdef MAVLINK_EXTERNAL_RX_STATUS
    mavlink_status_t m_mavlink_status[MAVLINK_COMM_NUM_BUFFERS];
#endif

#ifdef MAVLINK_GET_CHANNEL_STATUS
mavlink_status_t* mavlink_get_channel_status(uint8_t channel)
{
#ifndef MAVLINK_EXTERNAL_RX_STATUS
    static QList<mavlink_status_t> m_mavlink_status(MAVLINK_COMM_NUM_BUFFERS);
#endif
    if (!QUAVMAVLink::isValidChannel(channel)) {
//        qWarning() << Q_FUNC_INFO << "Invalid Channel Number:" << channel;
        return nullptr;
    }

    return &m_mavlink_status[channel];
}
#endif

QUAVMAVLink::QUAVMAVLink(QObject *parent)
           : QObject(parent)
{
}

QUAVMAVLink::~QUAVMAVLink()
{
    // qDebug() << Q_FUNC_INFO << this;
}