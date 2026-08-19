#include "SerialLink.h"
#include "QUAVSerialPortInfo.h"
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtCore/QTimer>

namespace {
    constexpr int CONNECT_TIMEOUT_MS    = 1000;
    constexpr int DISCONNECT_TIMEOUT_MS = 3000;
}

SerialConfiguration::SerialConfiguration(const QString &name, QObject *parent)
                   : LinkConfiguration(name, parent)
{
    qDebug() <<"SerialConfiguration::SerialConfiguration"<<" "<< this;
}


SerialConfiguration::SerialConfiguration(const SerialConfiguration *source, QObject *parent)
    : LinkConfiguration(source, parent)
{
    qDebug() <<"SerialConfiguration::SerialConfiguration (constructor copy)"<<" "<< this;

    SerialConfiguration::copyFrom(source);
}

SerialConfiguration::~SerialConfiguration()
{
    qDebug() <<"SerialConfiguration::~SerialConfiguration()"<<" "<< this;
}

void SerialConfiguration::setPortName(const QString &name)
{
    const QString portName = name.trimmed();
    if (portName.isEmpty()) {
        return;
    }

    if (portName != m_portName) {
        m_portName = portName;
        emit portNameChanged();
    }

    const QString portDisplayName = cleanPortDisplayName(portName);
    setPortDisplayName(portDisplayName);
}

void SerialConfiguration::copyFrom(const LinkConfiguration *source)
{
    Q_ASSERT(source);
    LinkConfiguration::copyFrom(source);

    const SerialConfiguration* const serialSource = qobject_cast<const SerialConfiguration*>(source);
    Q_ASSERT(serialSource);

    setBaud(serialSource->baud());
    setDataBits(serialSource->dataBits());
    setFlowControl(serialSource->flowControl());
    setStopBits(serialSource->stopBits());
    setParity(serialSource->parity());
    setPortName(serialSource->portName());
    setPortDisplayName(serialSource->portDisplayName());
    setUsbDirect(serialSource->usbDirect());
}

QString SerialConfiguration::cleanPortDisplayName(const QString &name)
{
    const QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : availablePorts) {
        if (portInfo.systemLocation() == name) {
            return portInfo.portName();
        }
    }
    return QString();
}

///////////////

SerialWorker::SerialWorker(const SerialConfiguration* config, QObject *parent)
            : QObject(parent)
            , m_serialConfig(config)
{
    qDebug() <<"SerialWorker::SerialWorker()"<<" "<< this;
    (void) qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");
}

SerialWorker::~SerialWorker()
{
    qDebug() <<"SerialWorker::~SerialWorker()"<<" "<< this;
}

bool SerialWorker::isConnected() const
{
    return (m_port && m_port->isOpen());
}

void SerialWorker::setupPort()
{
    Q_ASSERT(!m_port);
    m_port = new QSerialPort(this);

    Q_ASSERT(!m_timer);
    m_timer = new QTimer(this);

    (void) connect(m_port, &QSerialPort::aboutToClose,  this, &SerialWorker::m_onPortDisconnected);
    (void) connect(m_port, &QSerialPort::readyRead,     this, &SerialWorker::m_onPortReadyRead);
    (void) connect(m_port, &QSerialPort::errorOccurred, this, &SerialWorker::m_onPortErrorOccured);

    (void) connect(m_timer, &QTimer::timeout, this, &SerialWorker::m_checkPortAvailability);
    m_timer->start(CONNECT_TIMEOUT_MS);
}

bool SerialWorker::connectToPort()
{
    qWarning() << "bool SerialWorker::connectToPort()";

    if (isConnected()) {
        qWarning() << "Already connected to" << m_port->portName();
        return false;
    }

    m_port->setPortName(m_serialConfig->portName());

    const QUAVSerialPortInfo portInfo(*m_port);
    if (portInfo.isBootloader()) {
        qWarning() << "Not connecting to bootloader" << m_port->portName();
        emit errorOccurred(tr("Not connecting to a bootloader"));
        m_onPortDisconnected();
        return false;
    }

    qDebug() << "Attempting to open port" << m_port->portName();
    if (!m_port->open(QIODevice::ReadWrite)) {
        qWarning() << "Opening port" << m_port->portName() << "failed:" << m_port->errorString();

                // If auto-connect is enabled, we don't want to emit an error for PermissionError from devices already in use
        if (!m_errorEmitted && (!m_serialConfig->isAutoConnect() || m_port->error() != QSerialPort::PermissionError)) {
            emit errorOccurred(tr("Could not open port: %1").arg(m_port->errorString()));
            m_errorEmitted = true;
        }

        m_onPortDisconnected();

        return false;
    }

    m_onPortConnected();

    return true;
}

void SerialWorker::disconnectFromPort()
{
    qDebug() <<"void SerialWorker::disconnectFromPort()"<<" "<< this;

    if (!isConnected()) {
        qDebug() << "Already disconnected from port:" << m_port->portName();
        return;
    }

    qDebug() << "Attempting to close port:" << m_port->portName();
    m_port->close();
}

void SerialWorker::writeData(const QByteArray& data)
{
    if (data.isEmpty()) {
        emit errorOccurred(tr("Data to Send is Empty"));
        return;
    }

    if (!isConnected()) {
        emit errorOccurred(tr("Port is not Connected"));
        return;
    }

    if (!m_port->isWritable()) {
        emit errorOccurred(tr("Port is not Writable"));
        return;
    }

    qint64 totalBytesWritten = 0;
    while (totalBytesWritten < data.size()) {
        const qint64 bytesWritten = m_port->write(data.constData() + totalBytesWritten, data.size() - totalBytesWritten);
        if (bytesWritten == -1) {
            emit errorOccurred(tr("Could Not Send Data - Write Failed: %1").arg(m_port->errorString()));
            return;
        } else if (bytesWritten == 0) {
            emit errorOccurred(tr("Could Not Send Data - Write Returned 0 Bytes"));
            return;
        }
        totalBytesWritten += bytesWritten;
    }
}

void SerialWorker::m_onPortConnected()
{
    qDebug() << "Port connected:" << m_port->portName();

    m_port->setDataTerminalReady(true);
    m_port->setBaudRate(m_serialConfig->baud());
    m_port->setDataBits(static_cast<QSerialPort::DataBits>(m_serialConfig->dataBits()));
    m_port->setFlowControl(static_cast<QSerialPort::FlowControl>(m_serialConfig->flowControl()));
    m_port->setStopBits(static_cast<QSerialPort::StopBits>(m_serialConfig->stopBits()));
    m_port->setParity(static_cast<QSerialPort::Parity>(m_serialConfig->parity()));

    m_errorEmitted = false;
}

void SerialWorker::m_onPortDisconnected()
{
    qDebug() << "Port disconnected:" << m_port->portName();
    m_errorEmitted = false;
    emit disconnected();
}

void SerialWorker::m_onPortReadyRead()
{
    const QByteArray data = m_port->readAll();
    if (!data.isEmpty()) {
//        qDebug() <<"void SerialWorker::m_onPortReadyRead() data.size()"<<" "<< data.size();
        emit dataReceived(data);
    }
}

void SerialWorker::m_onPortErrorOccured(QSerialPort::SerialPortError portError)
{
    const QString errorString = m_port->errorString();
    qWarning() << "Port error:" << portError << errorString;

    switch (portError) {
        case QSerialPort::NoError:
        {
            qDebug() << "About to open port" << m_port->portName();
            return;
        }
        // We get this when a usb cable is unplugged Fallthrough
        case QSerialPort::ResourceError:
        case QSerialPort::PermissionError:
        {
            if (m_serialConfig->isAutoConnect()) {
                return;
            }
            break;
        }
        default:   break;
    }

    if (!m_errorEmitted) {
        emit errorOccurred(errorString);
        m_errorEmitted = true;
    }
}

void SerialWorker::m_checkPortAvailability()
{
    if (!isConnected()) { return; }

    bool portExists = false;
    const auto availablePorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& info : availablePorts) {
        if (info.portName() == m_serialConfig->portDisplayName()) {
            portExists = true;
            break;
        }
    }

    if (!portExists) { m_port->close();}
}

///////////////

SerialLink::SerialLink(SharedLinkConfigurationPtr &config, QObject *parent)
    : LinkInterface(config, parent)
    , m_serialConfig(qobject_cast<const SerialConfiguration*>(config.get()))
    , m_worker(new SerialWorker(m_serialConfig))
    , m_workerThread(new QThread(this))
{
    qDebug() <<"SerialLink::SerialLink()"<<" "<< this;

    m_workerThread->setObjectName(QStringLiteral("Serial_%1").arg(m_serialConfig->name()));

    (void) m_worker->moveToThread(m_workerThread);

    (void) connect(m_workerThread, &QThread::started, m_worker, &SerialWorker::setupPort);
    (void) connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    (void) connect(m_worker, &SerialWorker::disconnected,  this, &SerialLink::m_onDisconnected,  Qt::QueuedConnection);
    (void) connect(m_worker, &SerialWorker::dataReceived,  this, &SerialLink::m_onDataReceived,  Qt::QueuedConnection);
    (void) connect(m_worker, &SerialWorker::errorOccurred, this, &SerialLink::m_onErrorOccurred, Qt::QueuedConnection);

    m_workerThread->start();
}

SerialLink::~SerialLink()
{
    qDebug() <<"SerialLink::~SerialLink()"<<" "<< this;

    (void) QMetaObject::invokeMethod(m_worker, "disconnectFromPort", Qt::BlockingQueuedConnection);

    m_workerThread->quit();
    if (!m_workerThread->wait(DISCONNECT_TIMEOUT_MS)) {
        qWarning() << "Failed to wait for Serial Thread to close";
    }
}

bool SerialLink::isConnected() const
{
    return m_worker->isConnected();
}

void SerialLink::m_onDisconnected()
{
    emit disconnected();
}

void SerialLink::m_onDataReceived(const QByteArray& data)
{
    emit bytesReceived(this, data);
}

void SerialLink::m_onErrorOccurred(const QString& errorString)
{
    qWarning() << "Communication error:" << errorString;
    emit communicationError(tr("Serial Link Error"), tr("Link %1: (Port: %2) %3").arg(m_serialConfig->name(), m_serialConfig->portName(), errorString));
}

bool SerialLink::m_connect()
{
    return QMetaObject::invokeMethod(m_worker, "connectToPort", Qt::QueuedConnection);
}

void SerialLink::disconnect() {
    (void) QMetaObject::invokeMethod(m_worker, "disconnectFromPort", Qt::QueuedConnection);
}

void SerialLink::m_writeBytes(const QByteArray& data)
{
    (void) QMetaObject::invokeMethod(m_worker, "writeData", Qt::QueuedConnection, Q_ARG(QByteArray, data));
}