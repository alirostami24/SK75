#pragma once

#include <iostream>

#include <QObject>
#include <QSerialPortInfo>

#include "ControlPanelSDK.h"
#include "ControlPanelSDK_global.h"

#include "MainWindow.h"

using namespace SDK;

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(QObject *parent = nullptr);

private:
    ControlPanelSDK m_controlPanelSDK;

    MainWindow m_mainWindow;

    void initialize();
    void initializeConnections();

    void updateSerialPortName();

private Q_SLOTS:
    void sltRefreshSerialPortListRequested();

    void sltConnectToSerialPortRequested();
    void sltDisconnectFromSerialPortRequested();

    void sltAutoLockRequested();
    void sltStopTrackRequested();
};
