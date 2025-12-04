#include "SerialController.h"

SerialController::
SerialController(QObject *parent) :
    QObject(parent)
{
    connect(&m_serialPort, &QSerialPort::readyRead,
            this, &SerialController::sltNewDataReceived);
}

bool SerialController::
openConnection(const QString &portName,
               const uint32_t &baudrate,
               int32_t &errorCode)
{
    m_serialPort.setPortName(portName);
    m_serialPort.setBaudRate(baudrate);

    const bool state = m_serialPort.
            open(QIODevice::ReadWrite);

    errorCode = m_serialPort.error();
    return state;
}

void SerialController::
closeConnection()
{
    m_serialPort.close();
}

bool SerialController::
writeData(const QByteArray &packet)
{
    const qint64 writtenBytes =
            m_serialPort.write(packet);

    if (writtenBytes > 0)
    {
        return true;
    }

    return false;
}

bool SerialController::isConnected() const
{
    return m_serialPort.isOpen();
}

void SerialController::
sltNewDataReceived()
{
    QByteArray packet =
            m_serialPort.readAll();

    Q_EMIT sigNewDataReceived(packet);
}

QString SerialController::
serialPortName() const
{
    return m_serialPort.portName();
}

int32_t SerialController::
serialPortBaudrate() const
{
    return m_serialPort.baudRate();
}
