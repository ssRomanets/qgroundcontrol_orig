#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

class QmlObjectListModel;

class HealthAndArmingCheckReport : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(bool supported READ supported NOTIFY updated)
    Q_PROPERTY(bool canArm    READ canArm    NOTIFY updated)

    HealthAndArmingCheckReport(QObject* parent = nullptr);
    virtual ~HealthAndArmingCheckReport();

    bool supported() const {return m_supported;}
    bool canArm()    const {return m_canArm;}

signals:
    void updated();

private:
    bool m_supported{false};
    bool m_canArm{false};

};
