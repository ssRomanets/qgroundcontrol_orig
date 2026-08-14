#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QString>
#include <QtQmlIntegration/QtQmlIntegration>

class QUAVMAVLinkMessage;

class QUAVMAVLinkMessageField : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString                  name        READ name       CONSTANT)
    Q_PROPERTY(QString                  type        READ type       CONSTANT)
    Q_PROPERTY(QString                  value       READ value      NOTIFY valueChanged)
    Q_PROPERTY(bool                     selectable  READ selectable NOTIFY selectableChanged)

public:
    QUAVMAVLinkMessageField(const QString &name, const QString &type, QUAVMAVLinkMessage *parent = nullptr);
    ~QUAVMAVLinkMessageField();

    QString name() const    { return m_name;  }
    QString type() const    { return m_type;  }
    QString value() const   { return m_value; }
    bool selectable() const { return m_selectable; }

    void setSelectable(bool sel);
    void updateValue(const QString &newValue, qreal v);

signals:
    void selectableChanged();
    void valueChanged();

private:
    QString             m_type;
    QString             m_name;
    QString             m_value;
    bool                m_selectable = true;

    QUAVMAVLinkMessage* m_msg = nullptr;
};
