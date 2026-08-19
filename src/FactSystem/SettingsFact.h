
#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

#include "Fact.h"

/// A SettingsFact is Fact which holds a QSettings value.
class SettingsFact : public Fact
{
    Q_OBJECT
    Q_PROPERTY(bool visible MEMBER m_visible CONSTANT)

public:
    explicit SettingsFact(QObject *parent = nullptr);
    explicit SettingsFact(const QString &settingsGroup, FactMetaData *metaData, QObject *parent = nullptr);
    explicit SettingsFact(const SettingsFact &other, QObject *parent = nullptr);
    ~SettingsFact();

    const SettingsFact &operator=(const SettingsFact &other);

    // Must be called before any references to fact
    void setVisible(bool visible) { m_visible = visible; }

private slots:
    void m_rawValueChanged(const QVariant &value);

private:
    QString m_settingsGroup;
    bool m_visible = true;
};
