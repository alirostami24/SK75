#include "Controller.h"

Controller::
Controller() :
    m_controlPanelSDK(
        CommunicationType_Serial),
    m_feedbackSize(32)
{
    initialize();
    initializeConnections();
}

void Controller::
initialize()
{
    m_commandHeader.push_back(char(0xEB));
    m_commandHeader.push_back(char(0x90));

    m_feedbackHeader.push_back(char(0xEE));
    m_feedbackHeader.push_back(char(0x16));

    bool isConnected;
    int32_t errorCode;

    // ======================================
    //  Connect To Control Panel
    // ======================================
    SerialInfo serialInfo;
    serialInfo.setSerialPortName("ttyTHS1");
    serialInfo.setBaudrate(9600);

    isConnected = m_controlPanelSDK.
            initialize(serialInfo, errorCode);

    qCritical() << "===================================" << endl;
    qCritical() << "Control Panel Serial State:" <<
                   (isConnected == true ? "Connected" : "Not Connected");

    if (isConnected == false)
    {
        const QString errorString =
                parseSerialErrorCode(errorCode);

        qCritical() << "Error String:" << errorString;
    }

    qCritical() << endl;

    // Connect To Camera
    isConnected = m_serialController.
            openConnection(
                "ttyTHS0", 115200, errorCode);

    qCritical() << "===================================" << endl;
    qCritical() << "Camera Serial State:" <<
                   (isConnected == true ? "Connected" : "Not Connected");

    if (isConnected == false)
    {
        const QString errorString =
                parseSerialErrorCode(errorCode);

        qCritical() << "Error String:" << errorString;
    }

    QObject::connect(&m_videoCapture, &VideoCapture::sigAutoLockDetected,
            this, &Controller::sltAutoLockDetected);

    m_videoCapture.initialize();
    m_videoCapture.setFrameSize(QSize(640, 512));

    m_videoCapture.enableDetecting(true);


}

void Controller::
initializeConnections()
{
    connect(&m_controlPanelSDK, &ControlPanelSDK::
            sigProcessorDataChanged,
            this, &Controller::
            sltProcessorDataChanged);

    connect(&m_serialController,
            &SerialController::sigNewDataReceived,
            this, &Controller::sltNewDataRecieved);

    // connect(&m_mainWindow,
    //         &MainWindow::sigStartTrackRequested,
    //         this, &Controller::sltStartTrackRequested);

    // connect(&m_mainWindow,
    //         &MainWindow::sigStopTrackRequested,
    //         this, &Controller::sltStopTrackRequested);

    // connect(&m_mainWindow,
    //         &MainWindow::sigChangeOSD_Requested,
    //         this, &Controller::sltChangeOSD_Requested);

    connect(&m_videoCapture, &VideoCapture::
            sigNewFrameReceived, this,
            &Controller::sltNewFrameReceived);
}

void Controller::
analyzePacket()
{
    while (m_serialBuffer.length() >=
           m_feedbackSize)
    {
        const int16_t headerIndex =
                m_serialBuffer.indexOf(
                    m_feedbackHeader);

        if (headerIndex < 0)
        {
            m_serialBuffer.clear();

            break;
        }

        const int16_t endIndex = headerIndex +
                m_feedbackSize - 1;

        if (endIndex >= m_serialBuffer.length())
        {
            break;
        }

        const QByteArray packet =
                m_serialBuffer.mid(
                    headerIndex, m_feedbackSize);

        const uint8_t calculatedChecksum =
                calculateChecksum(packet);

        const uint8_t feedbackChecksum =
                packet.at(m_feedbackSize - 1);

        m_serialBuffer.remove(0, endIndex + 1);

        if (calculatedChecksum == feedbackChecksum)
        {
            interpretPacket(packet);
        }
    }
}

void Controller::
interpretPacket(
        const QByteArray &packet)
{
    const uint8_t byte2 = packet.at(2);

    const bool trackingState =
            (byte2 & 0x02) >> 1;

    //    m_mainWindow.setTrackState(trackingState);
}

uint8_t Controller::
calculateChecksum(
        const QByteArray &packet)
{
    uint16_t sum = 0x00;

    for (int i = 0; i < packet.length() - 1; i++)
    {
        sum += packet.at(i);
    }

    return toUInt8(sum & 0x00FF);
}

void Controller::
startCameraTrack(const int16_t &xPos,
        const int16_t &yPos)
{
    QByteArray packet(16, char(0x00));
    packet[ 0] = m_commandHeader.at(0);
    packet[ 1] = m_commandHeader.at(1);
    packet[ 2] = 0x0D;
    packet[ 3] = toUInt8((xPos & 0x00FF));
    packet[ 4] = toUInt8((xPos & 0xFF00) >> 8);
    packet[ 5] = toUInt8((yPos & 0x00FF));
    packet[ 6] = toUInt8((yPos & 0xFF00) >> 8);
    packet[ 7] = 0x00;
    packet[ 8] = 0x00;
    packet[ 9] = 0x00;
    packet[10] = 0x00;
    packet[11] = 0x00;
    packet[12] = 0x00;
    packet[13] = 0x00;
    packet[14] = 0x00;
    packet[15] = calculateChecksum(packet);

    m_serialController.
            writeData(packet);
}

void Controller::
stopCameraTrack()
{
    QByteArray packet(16, char(0x00));
    packet[ 0] = m_commandHeader.at(0);
    packet[ 1] = m_commandHeader.at(1);
    packet[ 2] = 0x0E;
    packet[ 3] = 0x00;
    packet[ 4] = 0x00;
    packet[ 5] = 0x00;
    packet[ 6] = 0x00;
    packet[ 7] = 0x00;
    packet[ 8] = 0x00;
    packet[ 9] = 0x00;
    packet[10] = 0x00;
    packet[11] = 0x00;
    packet[12] = 0x00;
    packet[13] = 0x00;
    packet[14] = 0x00;
    packet[15] = calculateChecksum(packet);

    m_serialController.
            writeData(packet);
}

void Controller::
changeCameraOSD_Visibility(
        const bool &state)
{
    QByteArray packet(16, char(0x00));

    // ===========================
    //      enable OSD
    // ===========================
    if (state == true)
    {
        packet[ 0] = m_commandHeader.at(0);
        packet[ 1] = m_commandHeader.at(1);
        packet[ 2] = 0x40;
        packet[ 3] = 0x00;
        packet[ 4] = 0x00;
        packet[ 5] = 0x00;
        packet[ 6] = 0x00;
        packet[ 7] = 0x00;
        packet[ 8] = 0x00;
        packet[ 9] = 0x00;
        packet[10] = 0x00;
        packet[11] = 0x00;
        packet[12] = 0x00;
        packet[13] = 0x00;
        packet[14] = 0x00;
        packet[15] = calculateChecksum(packet);
    }
    else
    {
        packet[ 0] = m_commandHeader.at(0);
        packet[ 1] = m_commandHeader.at(1);
        packet[ 2] = 0x41;
        packet[ 3] = 0x00;
        packet[ 4] = 0x00;
        packet[ 5] = 0x00;
        packet[ 6] = 0x00;
        packet[ 7] = 0x00;
        packet[ 8] = 0x00;
        packet[ 9] = 0x00;
        packet[10] = 0x00;
        packet[11] = 0x00;
        packet[12] = 0x00;
        packet[13] = 0x00;
        packet[14] = 0x00;
        packet[15] = calculateChecksum(packet);
    }

    m_serialController.
            writeData(packet);
}

void Controller::
sltProcessorDataChanged(
        const ProcessorCommands &command)
{
    switch (command)
    {
    case ProcessorCommand_StartAutoLock:
    {
        m_videoCapture.enableAutoLock(true);
        break;
    }
    case ProcessorCommand_StopTrack:
    {
        m_videoCapture.enableDetecting(true);
        m_videoCapture.enableAutoLock(false);
        // stop auto lock
        break;
    }
    }
}

void Controller::
sltNewDataRecieved(
        const QByteArray &packet)
{
    m_serialBuffer.push_back(packet);

    analyzePacket();
}

void Controller::
sltNewFrameReceived()
{

}

void Controller::sltAutoLockDetected(const QRectF &bbox)
{
    m_videoCapture.enableDetecting(false);
    /// start tracker
}

