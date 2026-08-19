#include "MAVLinkMessage.h"
#include "MAVLinkMessageField.h"
#include "QmlObjectListModel.h"

#include <QtCore/QTimeZone>

QUAVMAVLinkMessage::QUAVMAVLinkMessage(const mavlink_message_t &message, QObject *parent)
    : QObject(parent)
    , m_message(message)
    , m_fields(new QmlObjectListModel(this))

{
    const mavlink_message_info_t *const msgInfo = mavlink_get_message_info(&message);
    if (!msgInfo) {
    //    qWarning() << QStringLiteral("QGCMAVLinkMessage NULL msgInfo msgid(%1)").arg(message.msgid);
        return;
    }

    m_name = QString(msgInfo->name);
    //qDebug() << "New Message:" << m_name;

    for (unsigned int i = 0; i < msgInfo->num_fields; ++i) {
        QString type = QStringLiteral("?");
        switch (msgInfo->fields[i].type) {
            case MAVLINK_TYPE_CHAR:     type = QString("char");     break;
            case MAVLINK_TYPE_UINT8_T:  type = QString("uint8_t");  break;
            case MAVLINK_TYPE_INT8_T:   type = QString("int8_t");   break;
            case MAVLINK_TYPE_UINT16_T: type = QString("uint16_t"); break;
            case MAVLINK_TYPE_INT16_T:  type = QString("int16_t");  break;
            case MAVLINK_TYPE_UINT32_T: type = QString("uint32_t"); break;
            case MAVLINK_TYPE_INT32_T:  type = QString("int32_t");  break;
            case MAVLINK_TYPE_FLOAT:    type = QString("float");    break;
            case MAVLINK_TYPE_DOUBLE:   type = QString("double");   break;
            case MAVLINK_TYPE_UINT64_T: type = QString("uint64_t"); break;
            case MAVLINK_TYPE_INT64_T:  type = QString("int64_t");  break;
        }

        QUAVMAVLinkMessageField* const field = new QUAVMAVLinkMessageField(msgInfo->fields[i].name, type, this);
        m_fields->append(field);
    }
}

QUAVMAVLinkMessage::~QUAVMAVLinkMessage()
{
    m_fields->clearAndDeleteContents();
}

void QUAVMAVLinkMessage::updateFreq()
{
    const quint64 msgCount = m_count - m_lastCount;
    const qreal lastRateHz = m_actualRateHz;
    m_actualRateHz = (0.2 * m_actualRateHz) + (0.8 * msgCount);
    m_lastCount = m_count;
    if (m_actualRateHz != lastRateHz) {
        emit actualRateHzChanged();
    }
}

void QUAVMAVLinkMessage::setSelected(bool sel)
{
    if (sel != m_selected) {
        m_selected = sel;
        m_updateFields();
        emit selectedChanged();
    }
}

void QUAVMAVLinkMessage::setTargetRateHz(int32_t rate)
{
    if (rate != m_targetRateHz) {
        m_targetRateHz = rate;
        emit targetRateHzChanged();
    }
}

void QUAVMAVLinkMessage::update(const mavlink_message_t &message)
{
    m_count++;
    m_message = message;

    if (m_selected || m_fieldSelected) {
        // Don't update field info unless selected to reduce perf hit of message processing
        m_updateFields();
    }
    emit countChanged();
}

void QUAVMAVLinkMessage::m_updateFields()
{
    const mavlink_message_info_t* msgInfo = mavlink_get_message_info(&m_message);
    if (!msgInfo) {
    //    qWarning() << "QUAVMAVLinkMessage::update NULL msgInfo msgid" << m_message.msgid;
        return;
    }

    if (m_fields->count() != static_cast<int>(msgInfo->num_fields)) {
    //    qWarning() << "QGCMAVLinkMessage::update msgInfo field count mismatch msgid" << m_message.msgid;
        return;
    }

    uint8_t *const msg = reinterpret_cast<uint8_t*>(&m_message.payload64[0]);
    for (unsigned int i = 0; i < msgInfo->num_fields; ++i) {
        QUAVMAVLinkMessageField* const field = qobject_cast<QUAVMAVLinkMessageField*>(m_fields->get(static_cast<int>(i)));
        if (!field) {
            continue;
        }

        const unsigned int        offset              = msgInfo->fields[i].wire_offset;
        const unsigned int        array_length        = msgInfo->fields[i].array_length;
        static const unsigned int array_buffer_length = (MAVLINK_MAX_PAYLOAD_LEN + MAVLINK_NUM_CHECKSUM_BYTES + 7);

        switch (msgInfo->fields[i].type) {
        case MAVLINK_TYPE_CHAR:
            field->setSelectable(false);
            if (array_length > 0) {
                char *const str = reinterpret_cast<char*>(msg + offset);
                str[array_length - 1] = '\0';
                const QString v(str);
                field->updateValue(v, 0);
            } else {
                char b = *(reinterpret_cast<char*>(msg + offset));
                const QString v(b);
                field->updateValue(v, 0);
            }
            break;
        case MAVLINK_TYPE_UINT8_T:
            if (array_length > 0) {
                const uint8_t *const nums = msg + offset;
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                    string += tmp.arg(nums[j]);
                }
                string += QString::number(nums[array_length - 1]);
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                const uint8_t u = *(msg + offset);
                field->updateValue(QString::number(u), static_cast<qreal>(u));
            }
            break;
        case MAVLINK_TYPE_INT8_T:
            if (array_length > 0) {
                const int8_t *const nums = reinterpret_cast<int8_t*>(msg + offset);
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                    string += tmp.arg(nums[j]);
                }
                string += QString::number(nums[array_length - 1]);
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                const int8_t n = *(reinterpret_cast<int8_t*>(msg + offset));
                field->updateValue(QString::number(n), static_cast<qreal>(n));
            }
            break;
        case MAVLINK_TYPE_UINT16_T:
            if (array_length > 0) {
                uint16_t nums[array_buffer_length / sizeof(uint16_t)]{};
                (void) memcpy(nums, msg + offset,  sizeof(uint16_t) * array_length);
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                    string += tmp.arg(nums[j]);
                }
                string += QString::number(nums[array_length - 1]);
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                uint16_t n = 0;
                (void) memcpy(&n, msg + offset, sizeof(uint16_t));
                field->updateValue(QString::number(n), static_cast<qreal>(n));
            }
            break;
        case MAVLINK_TYPE_INT16_T:
            if (array_length > 0) {
                int16_t nums[array_buffer_length / sizeof(int16_t)]{};
                (void) memcpy(nums, msg + offset, sizeof(int16_t) * array_length);
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                    string += tmp.arg(nums[j]);
                }
                string += QString::number(nums[array_length - 1]);
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                int16_t n;
                memcpy(&n, msg + offset, sizeof(int16_t));
                field->updateValue(QString::number(n), static_cast<qreal>(n));
            }
            break;
        case MAVLINK_TYPE_UINT32_T:
            if (array_length > 0) {
                uint32_t nums[array_buffer_length / sizeof(uint32_t)]{};
                (void) memcpy(nums, msg + offset, sizeof(uint32_t) * array_length);
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                    string += tmp.arg(nums[j]);
                }
                string += QString::number(nums[array_length - 1]);
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                uint32_t n;
                (void) memcpy(&n, msg + offset, sizeof(uint32_t));
                if (m_message.msgid == MAVLINK_MSG_ID_SYSTEM_TIME) {
                    const QDateTime d = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(n), QTimeZone::utc());
                    field->updateValue(d.toString("HH:mm:ss"), static_cast<qreal>(n));
                } else {
                    field->updateValue(QString::number(n), static_cast<qreal>(n));
                }
            }
            break;
        case MAVLINK_TYPE_INT32_T:
            if (array_length > 0) {
                int32_t nums[array_buffer_length / sizeof(int32_t)]{};
                (void) memcpy(nums, msg + offset,  sizeof(int32_t) * array_length);
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                    string += tmp.arg(nums[j]);
                }
                string += QString::number(nums[array_length - 1]);
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                int32_t n;
                (void) memcpy(&n, msg + offset, sizeof(int32_t));
                field->updateValue(QString::number(n), static_cast<qreal>(n));
            }
            break;
        case MAVLINK_TYPE_FLOAT:
            if (array_length > 0) {
                float nums[array_buffer_length / sizeof(float)]{};
                (void) memcpy(nums, msg + offset,  sizeof(float) * array_length);
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                   string += tmp.arg(static_cast<double>(nums[j]));
                }
                string += QString::number(static_cast<double>(nums[array_length - 1]));
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                float fv;
                (void) memcpy(&fv, msg + offset, sizeof(float));
                field->updateValue(QString::number(static_cast<double>(fv)), static_cast<qreal>(fv));
            }
            break;
        case MAVLINK_TYPE_DOUBLE:
            if (array_length > 0) {
                double nums[array_buffer_length / sizeof(double)]{};
                (void) memcpy(nums, msg + offset, sizeof(double) * array_length);
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                    string += tmp.arg(nums[j]);
                }
                string += QString::number(static_cast<double>(nums[array_length - 1]));
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                double d;
                (void) memcpy(&d, msg + offset, sizeof(double));
                field->updateValue(QString::number(d), static_cast<qreal>(d));
            }
            break;
        case MAVLINK_TYPE_UINT64_T:
            if (array_length > 0) {
                uint64_t nums[array_buffer_length / sizeof(uint64_t)]{};
                (void) memcpy(nums, msg + offset, sizeof(uint64_t) * array_length);
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                    string += tmp.arg(nums[j]);
                }
                string += QString::number(nums[array_length - 1]);
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                uint64_t n;
                (void) memcpy(&n, msg + offset, sizeof(uint64_t));
                if(m_message.msgid == MAVLINK_MSG_ID_SYSTEM_TIME) {
                    const QDateTime d = QDateTime::fromMSecsSinceEpoch(n / 1000, QTimeZone::utc());
                    field->updateValue(d.toString("yyyy MM dd HH:mm:ss"), static_cast<qreal>(n));
                } else {
                    field->updateValue(QString::number(n), static_cast<qreal>(n));
                }
            }
            break;
        case MAVLINK_TYPE_INT64_T:
            if (array_length > 0) {
                int64_t nums[array_buffer_length / sizeof(int64_t)]{};
                (void) memcpy(nums, msg + offset,  sizeof(int64_t) * array_length);
                const QString tmp("%1, ");
                QString string;
                for (unsigned int j = 0; j < array_length - 1; ++j) {
                    string += tmp.arg(nums[j]);
                }
                string += QString::number(nums[array_length - 1]);
                field->updateValue(string, static_cast<qreal>(nums[0]));
            } else {
                int64_t n;
                (void) memcpy(&n, msg + offset, sizeof(int64_t));
                field->updateValue(QString::number(n), static_cast<qreal>(n));
            }
            break;
        default:
            break;
        }
    }
}
