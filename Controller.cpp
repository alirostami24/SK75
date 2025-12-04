#include "Controller.h"

Controller::
Controller() :
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

    m_mainWindow.show();

    sltRefreshSerialPortListRequested();

    const WId windowsID =
            m_mainWindow.getWindowID();

    m_videoCapture.setWindowID(windowsID);
    m_videoCapture.initialize();
    m_videoCapture.startCapture();

    m_isMCMD_Initialized = false;
    m_isMCMD_InProgress = false;
}

void Controller::
initializeConnections()
{
    connect(&m_futureWatcherMCMD,
            &QFutureWatcher<cv::Mat>::finished,
            this, &Controller::sltMCMD_Finished);

    connect(&m_mainWindow, &MainWindow::
            sigChangeSerialConnectionRequseted,
            this, &Controller::
            sltChangeSerialConnectionRequseted);

    connect(&m_mainWindow, &MainWindow::
            sigRefreshSerialPortListRequested,
            this, &Controller::
            sltRefreshSerialPortListRequested);

    connect(&m_serialController,
            &SerialController::sigNewDataReceived,
            this, &Controller::sltNewDataRecieved);

    connect(&m_mainWindow,
            &MainWindow::sigStartTrackRequested,
            this, &Controller::sltStartTrackRequested);

    connect(&m_mainWindow,
            &MainWindow::sigStopTrackRequested,
            this, &Controller::sltStopTrackRequested);

    connect(&m_mainWindow,
            &MainWindow::sigChangeOSD_Requested,
            this, &Controller::sltChangeOSD_Requested);

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

    m_mainWindow.setTrackState(trackingState);
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

    return toU8(sum & 0x00FF);
}

cv::Mat Controller::mcmd()
{
    const QByteArray frameBuffer =
            m_videoCapture.getFrameBuffer();

    const QSize frameSize =
            m_videoCapture.getFrameSize();

    cv::Mat frameMatBGR;

    if (m_isMCMD_Initialized == false)
    {
        m_isMCMD_Initialized = true;

        m_frameMat = cv::Mat(frameSize.height() * 1.5,
                             frameSize.width(),
                             CV_8UC1);

        const cv::Rect inputRect(
                    0, 0, frameSize.width(),
                    frameSize.height());

        const cv::Size inputSize(720, 576);

        m_mcmdController.init(inputRect, inputSize);
    }

    m_frameMat.data = (uchar*)frameBuffer.data();

    cv::cvtColor(m_frameMat, frameMatBGR, CV_YUV2BGR_I420);

    m_mcmdController.runMCMD(frameMatBGR);

    const cv::Rect targetBBox =
            m_mcmdController.getBoundingBox();

    cv::rectangle(frameMatBGR, targetBBox,
                  cv::Scalar(0, 0, 255), 2);

    cv::imshow("ProcessedFrame", frameMatBGR);

    return frameMatBGR;
}

void Controller::
sltMCMD_Finished()
{
    m_isMCMD_InProgress = false;

    const cv::Mat rgbMat =
            m_futureWatcherMCMD.result();

    if (rgbMat.empty() == false)
    {
        cv::imshow("HI", rgbMat);
    }
}

void Controller::
sltChangeSerialConnectionRequseted(
        const bool &shouldConnect)
{
    if (shouldConnect == true)
    {
        const QString portName =
                m_mainWindow.portName();

        int32_t errorCode;

        const bool isConnected =
                m_serialController.openConnection(
                    portName, 115200, errorCode);

        if (isConnected == false)
        {
            std::cerr << "Error Code: "
                      << errorCode << std::endl;
        }

        m_mainWindow.updateSerialState(isConnected);
    }
    else
    {
        m_serialController.closeConnection();

        m_mainWindow.updateSerialState(false);
    }
}

void Controller::
sltRefreshSerialPortListRequested()
{
    const QList<QSerialPortInfo> portList =
            QSerialPortInfo::availablePorts();

    QStringList portNameList;

    for (auto port : portList)
    {
        portNameList << port.portName();
    }

    m_mainWindow.updatePortList(portNameList);
}

void Controller::
sltNewDataRecieved(
        const QByteArray &packet)
{
    m_serialBuffer.push_back(packet);

    analyzePacket();
}

void Controller::
sltStartTrackRequested(
        const int16_t &xPos,
        const int16_t &yPos)
{
    QByteArray packet(16, char(0x00));
    packet[ 0] = m_commandHeader.at(0);
    packet[ 1] = m_commandHeader.at(1);
    packet[ 2] = 0x0D;
    packet[ 3] = toU8((xPos & 0x00FF));
    packet[ 4] = toU8((xPos & 0xFF00) >> 8);
    packet[ 5] = toU8((yPos & 0x00FF));
    packet[ 6] = toU8((yPos & 0xFF00) >> 8);
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
sltStopTrackRequested()
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
sltNewFrameReceived()
{
    return;

    if (m_isMCMD_InProgress == false)
    {
        m_isMCMD_InProgress = true;

        mcmd();

//        QFuture<cv::Mat> future =
//                QtConcurrent::run(this, &Controller::mcmd);

//        m_futureWatcherMCMD.setFuture(future);
    }
}

void Controller::
sltChangeOSD_Requested(
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

    qCritical() << packet.toHex(' ');

    m_serialController.
            writeData(packet);
}
