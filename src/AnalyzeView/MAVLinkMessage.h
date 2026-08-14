
#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtQmlIntegration/QtQmlIntegration>

#include "MAVLinkLib.h"

class QmlObjectListModel;

class QUAVMAVLinkMessage : public QObject
{
    Q_OBJECT
    Q_MOC_INCLUDE("QmlObjectListModel.h")

    Q_PROPERTY(quint32              id              READ id             CONSTANT)
    Q_PROPERTY(quint32              sysId           READ sysId          CONSTANT)
    Q_PROPERTY(quint32              compId          READ compId         CONSTANT)
    Q_PROPERTY(QString              name            READ name           CONSTANT)
    Q_PROPERTY(qreal                actualRateHz    READ actualRateHz   NOTIFY actualRateHzChanged)
    Q_PROPERTY(int32_t              targetRateHz    READ targetRateHz   NOTIFY targetRateHzChanged)
    Q_PROPERTY(quint64              count           READ count          NOTIFY countChanged)
    Q_PROPERTY(QmlObjectListModel*  fields          READ fields         CONSTANT)
    Q_PROPERTY(bool                 fieldSelected   READ fieldSelected  NOTIFY fieldSelectedChanged)
    Q_PROPERTY(bool                 selected        READ selected       NOTIFY selectedChanged)

public:
    explicit QUAVMAVLinkMessage(const mavlink_message_t &message, QObject *parent = nullptr);
    ~QUAVMAVLinkMessage();

    quint32 id() const                   { return m_message.msgid;  }
    quint8 sysId() const                 { return m_message.sysid; }
    quint8 compId() const                { return m_message.compid; }
    QString name() const                 { return m_name;  }
    qreal actualRateHz() const           { return m_actualRateHz; }
    int32_t targetRateHz() const         { return m_targetRateHz; }
    quint64 count() const                { return m_count; }
    quint64 lastCount() const            { return m_lastCount; }
    QmlObjectListModel *fields() const   { return m_fields; }
    bool fieldSelected() const           { return m_fieldSelected; }
    bool selected() const                { return m_selected; }

    void update(const mavlink_message_t& message);
    void updateFreq();
    void setSelected(bool sel);
    void setTargetRateHz(int32_t rate);

signals:
    void countChanged();
    void actualRateHzChanged();
    void targetRateHzChanged();
    void fieldSelectedChanged();
    void selectedChanged();

private:
    void m_updateFields();

    mavlink_message_t   m_message{};
    QmlObjectListModel* m_fields = nullptr;
    QString             m_name;
    qreal               m_actualRateHz = 0.0;
    int32_t             m_targetRateHz = 0;
    uint64_t            m_count = 1;
    uint64_t            m_lastCount = 0;
    bool                m_fieldSelected = false;
    bool                m_selected = false;
};
