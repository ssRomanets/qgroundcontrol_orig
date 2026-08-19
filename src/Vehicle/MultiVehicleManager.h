#pragma once

#include <QtCore/QObject>

class LinkInterface;
class Vehicle;
class QmlObjectListModel;

class MultiVehicleManager : public QObject
{
    Q_OBJECT
    Q_MOC_INCLUDE("Vehicle.h")
    Q_PROPERTY(Vehicle* activeVehicle               READ activeVehicle WRITE setActiveVehicle NOTIFY activeVehicleChanged )
    Q_PROPERTY(bool parameterReadyVehicleAvailable  READ m_getParameterReadyVehicleAvailable  NOTIFY parameterReadyVehicleAvailableChanged)
    Q_PROPERTY(QmlObjectListModel* vehicles         READ vehicles                           CONSTANT)

public:
    explicit MultiVehicleManager(QObject *parent = nullptr);
    ~MultiVehicleManager();

    static MultiVehicleManager* instance();
    static void registerQmlTypes();

    void init();
    Q_INVOKABLE Vehicle* getVehicleById(int vehicleId) const;
    QmlObjectListModel*  vehicles()      const { return m_vehicles;}
    Vehicle*             activeVehicle() const { return m_activeVehicle;}
    void                 setActiveVehicle(Vehicle* vehicle);

signals:
    void parameterReadyVehicleAvailableChanged(bool parameterReadyVehicleAvailable);
    void activeVehicleChanged(Vehicle* activeVehicle);

private slots:
    void m_deleteVehiclePhase1(Vehicle* vehicle);
    void m_deleteVehiclePhase2(Vehicle* vehicle);
    void m_setActiveVehiclePhase2(Vehicle* vehicle);
    void m_vehicleHeartbeatInfo(LinkInterface* link, int vehicleId, int componentId, int vehicleFirmwareType, int vehicleType);

private:
    void m_setActiveVehicle(Vehicle* vehicle);
    bool m_getParameterReadyVehicleAvailable() const { return m_parameterReadyVehicleAvailable; }

    QmlObjectListModel* m_vehicles                       = nullptr;
    bool                m_parameterReadyVehicleAvailable = false;
    Vehicle*            m_activeVehicle                  = nullptr;
};











