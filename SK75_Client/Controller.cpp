#include "Controller.h"

Controller::
Controller(QObject *parent) :
    QObject(parent),
    m_controlPanelSDK(
        CommunicationType_Serial)
{
    initialize();
    initializeConnections();
}

void Controller::
initialize()
{
    updateSerialPortName();

    m_mainWindow.show();
}

void Controller::
initializeConnections()
{
    connect(&m_mainWindow, &MainWindow::
            sigRefreshSerialPortListRequested,
            this, &Controller::
            sltRefreshSerialPortListRequested);

    connect(&m_mainWindow, &MainWindow::
            sigConnectToSerialPortRequested,
            this, &Controller::
            sltConnectToSerialPortRequested);

    connect(&m_mainWindow, &MainWindow::
            sigDisconnectFromSerialPortRequested,
            this, &Controller::
            sltDisconnectFromSerialPortRequested);

    connect(&m_mainWindow, &MainWindow::
            sigAutoLockRequested,
            this, &Controller::
            sltAutoLockRequested);

    connect(&m_mainWindow, &MainWindow::
            sigStopTrackRequested,
            this, &Controller::
            sltStopTrackRequested);
}

void Controller::
updateSerialPortName()
{
    const QList<QSerialPortInfo> infoList =
            QSerialPortInfo::availablePorts();

    QStringList portNameList;

    for (auto item : infoList)
    {
        portNameList << item.portName();
    }

    m_mainWindow.updateSerialPortsList(portNameList);
}

void Controller::
sltRefreshSerialPortListRequested()
{
    updateSerialPortName();
}

void Controller::
sltConnectToSerialPortRequested()
{
    const QString portName =
            m_mainWindow.serialPortName();

    const int32_t baudrate =
             m_mainWindow.serialPortBaudrate();

    SerialInfo info;
    info.setSerialPortName(portName);
    info.setBaudrate(baudrate);

    int32_t errorCode;

    const bool isConnected =
            m_controlPanelSDK.
            initialize(info, errorCode);

    qCritical() << "isConnected " << isConnected;

    m_mainWindow.updateSerialPortButton(isConnected);
}

void Controller::
sltDisconnectFromSerialPortRequested()
{
    m_controlPanelSDK.terminate();

    m_mainWindow.updateSerialPortButton(false);
}

void Controller::
sltAutoLockRequested()
{
    m_controlPanelSDK.processor.
            startAutoLock(0, 0, 1920, 1080);
}

void Controller::
sltStopTrackRequested()
{
    m_controlPanelSDK.processor.stopTrack();
}
