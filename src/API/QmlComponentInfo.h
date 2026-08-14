#pragma once

#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>
#include <QtCore/QUrl>

/// Represents a Qml component which can be loaded from a resource.
class QmlComponentInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString  title   READ title  CONSTANT)
    Q_PROPERTY(QUrl     url     READ url    CONSTANT)
    Q_PROPERTY(QUrl     icon    READ icon   CONSTANT)

public:
    QmlComponentInfo(const QString &title, QUrl url, QUrl icon = QUrl(), QObject *parent = nullptr);
    ~QmlComponentInfo();

    const QString& title() const { return m_title; }
    QUrl url()             const { return m_url; }
    QUrl icon()            const { return m_icon; }

protected:
    const QString m_title;
    const QUrl    m_url;
    const QUrl    m_icon;
};











