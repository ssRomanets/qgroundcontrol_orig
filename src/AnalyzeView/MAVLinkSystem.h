#pragma once

#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtQmlIntegration/QtQmlIntegration>

#include "QmlObjectListModel.h"

class QUAVMAVLinkMessage;

class QUAVMAVLinkSystem : public QObject
{
    Q_OBJECT
    // QML_ELEMENT
    Q_PROPERTY(quint8               id          READ id                             CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  messages    READ messages                       CONSTANT)
    Q_PROPERTY(QList<int>           compIDs     READ compIDs                        NOTIFY compIDsChanged)
    Q_PROPERTY(QStringList          compIDsStr  READ compIDsStr                     NOTIFY compIDsChanged)
    Q_PROPERTY(int                  selected    READ selected   WRITE setSelected   NOTIFY selectedChanged)

public:
    QUAVMAVLinkSystem(quint8 id, QObject *parent = nullptr);
    ~QUAVMAVLinkSystem();

    quint8 id()                    const { return m_id; }
    QmlObjectListModel* messages() const { return m_messages; }
    QList<int> compIDs()           const { return m_compIDs; }
    QStringList compIDsStr()       const { return m_compIDsStr; }
    int selected()                 const { return m_selected; }

    void setSelected(int sel);
    QUAVMAVLinkMessage* findMessage(uint32_t id, uint8_t compId);
    int findMessage(const QUAVMAVLinkMessage* message);
    void append(QUAVMAVLinkMessage* message);
    QUAVMAVLinkMessage* selectedMsg();

signals:
    void compIDsChanged();
    void selectedChanged();

private:
    void m_checkCompID(const QUAVMAVLinkMessage* message);
    void m_resetSelection();

private:
    quint8              m_id = 0;
    QmlObjectListModel* m_messages = nullptr; ///< List of QGCMAVLinkMessage
    QList<int>          m_compIDs;
    QStringList         m_compIDsStr;
    int                 m_selected = 0;
};
