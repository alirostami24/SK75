#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QAbstractSocket>

#include "ControlPanelSDK_global.h"

#include <iostream>

class NetworkController : public QObject
{
    Q_OBJECT
public:
    explicit NetworkController(
            QObject *parent = nullptr);

    bool openConnection(
            const QString &hostAddress,
            const uint32_t &hostPortNumber,
            int32_t &errorCode);

    void closeConnection();

    bool writeData(const QByteArray &packet);

    bool isConnected() const;

private:
    QTcpSocket m_socket;

Q_SIGNALS:
    void sigNewDataReceived(
            const QByteArray &packet);

    void sigServerDisconnected();


private Q_SLOTS:
    void sltNewDataReceived();

};
