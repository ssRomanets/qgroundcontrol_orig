#pragma once

#include <QtCore/QString>

#include <QtSerialPort/QSerialPort>

#include "LinkConfiguration.h"
#include "LinkInterface.h"

#include <QThread>

class SerialConfiguration : public LinkConfiguration
{
    Q_OBJECT
    Q_PROPERTY(qint32                   baud            READ baud            WRITE setBaud        NOTIFY baudChanged)
    Q_PROPERTY(QSerialPort::DataBits    dataBits        READ dataBits        WRITE setDataBits    NOTIFY dataBitsChanged)
    Q_PROPERTY(QSerialPort::FlowControl flowControl     READ flowControl     WRITE setFlowControl NOTIFY flowControlChanged)
    Q_PROPERTY(QSerialPort::StopBits    stopBits        READ stopBits        WRITE setStopBits    NOTIFY stopBitsChanged)
    Q_PROPERTY(QSerialPort::Parity      parity          READ parity          WRITE setParity      NOTIFY parityChanged)
    Q_PROPERTY(QString                  portName        READ portName        WRITE setPortName    NOTIFY portNameChanged)
    Q_PROPERTY(QString                  portDisplayName READ portDisplayName                      NOTIFY portDisplayNameChanged)
    Q_PROPERTY(bool                     usbDirect       READ usbDirect       WRITE setUsbDirect   NOTIFY usbDirectChanged)

public:
    explicit SerialConfiguration(const QString& name, QObject *parent = nullptr);
    explicit SerialConfiguration(const SerialConfiguration *copy, QObject *parent = nullptr);
    virtual ~SerialConfiguration();

    LinkType type() const override { return LinkConfiguration::TypeSerial; }
    void copyFrom(const LinkConfiguration *source) override;

    qint32 baud() const { return m_baud; }
    void setBaud(qint32 baud) { if (baud != m_baud) { m_baud = baud; emit baudChanged(); } }


    QSerialPort::DataBits dataBits() const { return m_dataBits; }
    void setDataBits(QSerialPort::DataBits databits) {
        if (databits != m_dataBits) { m_dataBits = databits; emit dataBitsChanged(); }
    }

    QSerialPort::FlowControl flowControl() const { return m_flowControl; }
    void setFlowControl(QSerialPort::FlowControl flowControl) {
        if (flowControl != m_flowControl) { m_flowControl = flowControl; emit flowControlChanged(); }
    }

    QSerialPort::StopBits stopBits() const { return m_stopBits; }
    void setStopBits(QSerialPort::StopBits stopBits) {
        if (stopBits != m_stopBits) { m_stopBits = stopBits; emit stopBitsChanged(); }
    }

    QSerialPort::Parity parity() const { return m_parity; }
    void setParity(QSerialPort::Parity parity) {
        if (parity != m_parity) { m_parity = parity; emit parityChanged(); }
    }

    QString portName() const { return m_portName; }
    void setPortName(const QString &name);

    QString portDisplayName() const { return m_portDisplayName; }
    void setPortDisplayName(const QString &portDisplayName) {
        if (portDisplayName != m_portDisplayName) { m_portDisplayName = portDisplayName; emit portDisplayNameChanged(); }
    }

    bool usbDirect() const { return m_usbDirect; }
    void setUsbDirect(bool usbDirect) { if (usbDirect != m_usbDirect) { m_usbDirect = usbDirect; emit usbDirectChanged(); } }

    static QString cleanPortDisplayName(const QString &name);

signals:
    void baudChanged();
    void dataBitsChanged();
    void flowControlChanged();
    void stopBitsChanged();
    void parityChanged();
    void portNameChanged();
    void portDisplayNameChanged();
    void usbDirectChanged();

private:
    qint32                   m_baud        = QSerialPort::Baud57600;
    QSerialPort::DataBits    m_dataBits    = QSerialPort::Data8;
    QSerialPort::FlowControl m_flowControl = QSerialPort::NoFlowControl;
    QSerialPort::StopBits    m_stopBits    = QSerialPort::OneStop;
    QSerialPort::Parity      m_parity      = QSerialPort::NoParity;
    QString m_portName;
    QString m_portDisplayName;
    bool m_usbDirect = false;
};


class SerialWorker : public QObject
{
    Q_OBJECT

public:
    explicit SerialWorker(const SerialConfiguration* config, QObject* parent = nullptr);
    ~SerialWorker();

    bool isConnected() const;

signals:
    void disconnected();
    void dataReceived(const QByteArray& data);
    void errorOccurred(const QString& errorString);

public slots:
    void setupPort();
    bool connectToPort();
    void disconnectFromPort();
    void writeData(const QByteArray& data);

private slots:
    void m_onPortConnected();
    void m_onPortDisconnected();
    void m_onPortReadyRead();
    void m_onPortErrorOccured(QSerialPort::SerialPortError portError);
    void m_checkPortAvailability();

private:
    const SerialConfiguration* m_serialConfig = nullptr;
    QSerialPort*               m_port;
    QTimer*                    m_timer = nullptr;
    bool                       m_errorEmitted = false;
};

class SerialLink : public LinkInterface
{
    Q_OBJECT

public:
    explicit SerialLink(SharedLinkConfigurationPtr &config, QObject *parent = nullptr);
    virtual ~SerialLink();

    bool isConnected() const override;

public slots:
    void disconnect() override;

private slots:
    void m_onDisconnected();
    void m_onDataReceived(const QByteArray& data);
    void m_onErrorOccurred(const QString& errorString);

private:
    bool m_connect() override;
    void m_writeBytes(const QByteArray& data) override;

    const SerialConfiguration* m_serialConfig = nullptr;
    SerialWorker* m_worker                    = nullptr;
    QThread* m_workerThread                   = nullptr;
};