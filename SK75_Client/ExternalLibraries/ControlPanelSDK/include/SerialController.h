#pragma once

#include <QObject>
#include <QSerialPort>

class SerialController : public QObject
{
    Q_OBJECT
public:
    explicit SerialController(
            QObject *parent = nullptr);

    bool openConnection(
            const QString &portName,
            const uint32_t &baudrate,
            int32_t &errorCode);

    void closeConnection();

    bool writeData(const QByteArray &packet);

    bool isConnected() const;

private:
    QSerialPort m_serialPort;

private Q_SLOTS:
    void sltNewDataReceived();

Q_SIGNALS:
    void sigNewDataReceived(
            const QByteArray &packet);
};
