#include "Controller.h"

const uint8_t SDK::gHeader1 = 0x54;
const uint8_t SDK::gHeader2 = 0x4F;
const uint8_t SDK::gFooter1 = 0x4E;
const uint8_t SDK::gFooter2 = 0x45;

Controller::
Controller() :
    m_videoCaptureDetection(0),
    m_videoCaptureRender(1),
    m_videoCaptureFeeder(2),
    m_panelSourceID_ByteIndex(2),
    m_panelDestinationID_ByteIndex(3),
    m_panelModuleID_ByteIndex(4),
    m_panelCommandID_ByteIndex(5),
    m_panelLengthByteIndex(6),
    m_panelMinimumLength(11),
    m_footerOffset(-1),
    m_feedbackSize(32)
{
    initialize();
    initializeConnections();
}

void Controller::
initialize()
{
    m_panelHeader.append(char(SDK::gHeader1));
    m_panelHeader.append(char(SDK::gHeader2));

    m_panelFooter.append(char(SDK::gFooter1));
    m_panelFooter.append(char(SDK::gFooter2));

    m_cameraCommandHeader.push_back(char(0xEB));
    m_cameraCommandHeader.push_back(char(0x90));

    m_cameraFeedbackHeader.push_back(char(0xEE));
    m_cameraFeedbackHeader.push_back(char(0x16));

    bool isConnected;
    int32_t errorCode;

    // ======================================
    //      Connect To Control Panel
    // ======================================
    isConnected = m_panelSerialController.
            openConnection(
                "ttyTHS1", 9600, errorCode);

    std::cerr << "===================================" << std::endl;
    std::cerr << "Panel Serial Port Name: " <<
                 m_panelSerialController.
                 serialPortName().toStdString()
              << std::endl;

    std::cerr << "Panel Serial Port Name: " <<
                 m_panelSerialController.
                 serialPortBaudrate() << std::endl;

    std::cerr << "Panel Serial State: " <<
                 (isConnected == true ?
                      "Connected" :
                      "Not Connected") << std::endl;

    if (isConnected == false)
    {
        const QString errorString =
                parseSerialErrorCode(errorCode);

        std::cerr << "Error String: "
                  << errorString.toStdString() << std::endl;
    }

    std::cerr << std::endl;

    // ======================================
    //      Connect To Camera
    // ======================================
    isConnected = m_cameraSerialController.
            openConnection(
                "ttyTHS2", 115200, errorCode);

    std::cerr << "===================================" << std::endl;
    std::cerr << "Camera Serial Port Name: " <<
                 m_cameraSerialController.
                 serialPortName().toStdString() << std::endl;

    std::cerr << "Camera Serial Port Name: " <<
                 m_cameraSerialController.
                 serialPortBaudrate() << std::endl;

    std::cerr << "Camera Serial State: " <<
                 (isConnected == true ?
                      "Connected" :
                      "Not Connected") << std::endl;

    if (isConnected == false)
    {
        const QString errorString =
                parseSerialErrorCode(errorCode);

        std::cerr << "Error String: "
                  << errorString.toStdString() << std::endl;
    }
    else
    {
        QTimer::singleShot(5000, this,
                           &Controller::
                           cameraAutoCorrection);
    }

    // ======================================
    //      Streaming & Detector
    // ======================================
    m_videoCaptureRender.initialize();
    m_videoCaptureRender.start();
    m_videoCaptureRender.enableDetecting(false);

    m_videoCaptureDetection.initialize();
    m_videoCaptureDetection.start();
    m_videoCaptureDetection.enableDetecting(true);

    m_videoCaptureFeeder.initialize();
    m_videoCaptureFeeder.start();
    m_videoCaptureFeeder.enableDetecting(false);

}

void Controller::
initializeConnections()
{
    connect(&m_panelSerialController,
            &SerialController::sigNewDataReceived,
            this, &Controller::sltPanelNewDataRecieved);

    connect(&m_cameraSerialController,
            &SerialController::sigNewDataReceived,
            this, &Controller::sltCameraNewDataRecieved);

    connect(&m_videoCaptureDetection, &VideoCapture::
            sigAutoLockDetected,this,
            &Controller::sltAutoLockDetected);
}

void Controller::
analyzePanelPacket()
{
    while (m_panelSerialBuffer.length() >=
           m_panelMinimumLength)
    {
        const int16_t startIndex =
                m_panelSerialBuffer.
                indexOf(m_panelHeader);

        if (startIndex == -1)
        {
            m_panelSerialBuffer.clear();

            break;
        }

        int16_t offset = startIndex;

        if (m_footerOffset != -1)
        {
            offset = m_footerOffset;
        }

        const int16_t endIndex =
                m_panelSerialBuffer.indexOf(
                    m_panelFooter, offset);

        if (endIndex == -1)
        {
            break;
        }

        const uint8_t packetLength =
                endIndex - startIndex + 2;

        if (packetLength < m_panelMinimumLength)
        {
            m_footerOffset = endIndex + 1;

            break;
        }
        else
        {
            m_footerOffset = -1;
        }

        const QByteArray packet =
                m_panelSerialBuffer.mid(
                    startIndex, packetLength);

        // qCritical() << " App -> Examine Packet "
        //             << packet.toHex(' ');

        const uint8_t sourceID =
                packet.at(m_panelSourceID_ByteIndex);

        if (sourceID != Device_GUI &&
                sourceID != Device_MUX_Board)
        {
            m_panelSerialBuffer.remove(
                        0, startIndex + 1);
            break;
        }

        const uint8_t destinationID =
                packet.at(m_panelDestinationID_ByteIndex);

        if (destinationID != Device_Processor)
        {
            m_panelSerialBuffer.remove(
                        0, startIndex + 1);
            break;
        }

        const uint8_t length =
                packet.at(m_panelLengthByteIndex);

        if (packetLength != (length + 11))
        {
            m_panelSerialBuffer.remove(
                        0, startIndex + 1);
            break;
        }

        const uint8_t checksum1_Index = length + 7;

        const uint8_t checksum2_Index =
                (checksum1_Index + 1);

        const uint8_t checksum1 =
                packet.at(checksum1_Index);

        const uint8_t checksum2 =
                packet.at(checksum2_Index);

        uint8_t calculatedChecksum1 = 0x00;
        uint8_t calculatedChecksum2 = 0x00;

        CommandCreator::calculateChecksum(
                    packet, calculatedChecksum1,
                    calculatedChecksum2);

        if ((checksum1 != calculatedChecksum1) ||
                (checksum2 != calculatedChecksum2))
        {
            m_panelSerialBuffer.remove(
                        0, startIndex + 1);
            break;
        }

        // qCritical() << " App -> Packet is Valid. ";

        m_panelSerialBuffer.remove(
                    startIndex, packetLength);

        interpretPanelPacket(packet);
    }
}

void Controller::
interpretPanelPacket(
        const QByteArray &packet)
{
    const uint8_t moduleID_Byte = packet.at(
                m_panelModuleID_ByteIndex);

    switch (moduleID_Byte)
    {
    case Module_Processor:
    {
        interpretProcessorPacket(packet);

        break;
    }
    default:
    {
        break;
    }
    }
}

void Controller::
interpretProcessorPacket(
        const QByteArray &packet)
{
    const uint8_t commandByte =
            packet.at(m_panelCommandID_ByteIndex);

    const ProcessorCommands command =
            static_cast<ProcessorCommands>(
                commandByte);

    const uint8_t packetSize = packet.length();

    switch (command)
    {
    case ProcessorCommand_StartAutoLock:
    {
//        if (packetSize != m_panelMinimumLength + 8)
//        {
//            break;
//        }

        int16_t xPosRatio = 0x0000;
        xPosRatio |= toInt16(packet.at(7)) & 0x00FF;
        xPosRatio |= (toInt16(packet.at(8)) & 0x00FF) << 8;

        const bool isX_PosValid =
                xPosRatio >= -1000 &&
                xPosRatio <= 1000;

//        if (isX_PosValid == false)
//        {
//            break;
//        }

        int16_t yPosRatio = 0x0000;
        yPosRatio |= toInt16(packet.at(9)) & 0x00FF;
        yPosRatio |= (toInt16(packet.at(10)) & 0x00FF) << 8;

        const bool isY_PosValid =
                yPosRatio >= -1000 &&
                yPosRatio <= 1000;

//        if (isY_PosValid == false)
//        {
//            break;
//        }

        const QPointF posRatio(
                    xPosRatio / 1000.0,
                    yPosRatio / 1000.0);

        uint16_t width = 0x0000;
        width |= toUInt16(packet.at(11)) & 0x00FF;
        width |= (toUInt16(packet.at(12)) & 0x00FF) << 8;

//        if (width > 1000)
//        {
//            break;
//        }

        uint16_t height = 0x0000;
        height |= toUInt16(packet.at(13)) & 0x00FF;
        height |= (toUInt16(packet.at(14)) & 0x00FF) << 8;

//        if (height > 1000)
//        {
//            break;
//        }

        const QRectF rectangle(xPosRatio / 1000.0,
                               yPosRatio / 1000.0,
                               width / 1000.0,
                               height / 1000.0);

        processStartAutoTrack(rectangle);

        break;
    }
    case ProcessorCommand_StopTrack:
    {
        if (packetSize != m_panelMinimumLength)
        {
            break;
        }

        processStopTrack();

        break;
    }
    case ProcessorCommand_SetDateAndTime:
    {
        uint32_t decimalDate = 0x00000000;
        decimalDate |= (toUInt32(packet.at( 7)) & 0x000000FF) << 0;
        decimalDate |= (toUInt32(packet.at( 8)) & 0x000000FF) << 8;
        decimalDate |= (toUInt32(packet.at( 9)) & 0x000000FF) << 16;
        decimalDate |= (toUInt32(packet.at(10)) & 0x000000FF) << 24;

        uint32_t decimalTime = 0x00000000;
        decimalTime |= (toUInt32(packet.at(11)) & 0x000000FF) << 0;
        decimalTime |= (toUInt32(packet.at(12)) & 0x000000FF) << 8;
        decimalTime |= (toUInt32(packet.at(13)) & 0x000000FF) << 16;
        decimalTime |= (toUInt32(packet.at(14)) & 0x000000FF) << 24;

        const QString dateString =
                QString::number(decimalDate);

        const QString timeString =
                QString::number(decimalTime);

        const QDate date = QDate::fromString(
                    dateString, "yyyyMMdd");

        const QTime time = QTime::fromString(
                    timeString, "hhmmss");

        QString RTC_Time;
        RTC_Time += date.toString("yyyy-MM-dd");
        RTC_Time += " ";
        RTC_Time += time.toString("hh:mm:ss");

        const QString program = QString("%1/setDateTime.sh")
                .arg(QCoreApplication::applicationDirPath());

        QStringList args;
        args << RTC_Time;

        QProcess process;
        process.start(program, args,
                      QIODevice::ReadWrite);

        const bool isFinished =
                process.waitForFinished(2000);

        std::cerr << "********************************"
                     "********************************\n";

        std::cerr << "  Change system date time to "
                  << RTC_Time.toStdString() << ": ";

        if (isFinished == true)
        {
            std::cerr << "exitcode: "
                      << process.exitCode()
                      << std::endl;
        }
        else
        {
            std::cerr << "Timeout\n";
        }

        process.close();

        std::cerr << "********************************"
                     "********************************\n";
    }
    default:
    {
        return;
    }
    }
}

void Controller::
analyzeCameraPacket()
{
    while (m_cameraSerialBuffer.length() >=
           m_feedbackSize)
    {
        const int16_t headerIndex =
                m_cameraSerialBuffer.indexOf(
                    m_cameraFeedbackHeader);

        if (headerIndex < 0)
        {
            m_cameraSerialBuffer.clear();

            break;
        }

        const int16_t endIndex = headerIndex +
                m_feedbackSize - 1;

        if (endIndex >= m_cameraSerialBuffer.length())
        {
            break;
        }

        const QByteArray packet =
                m_cameraSerialBuffer.mid(
                    headerIndex, m_feedbackSize);

        const uint8_t calculatedChecksum =
                calculateChecksum(packet);

        const uint8_t feedbackChecksum =
                packet.at(m_feedbackSize - 1);

        m_cameraSerialBuffer.remove(0, endIndex + 1);

        if (calculatedChecksum == feedbackChecksum)
        {
            interpretCameraPacket(packet);
        }
    }
}

void Controller::
interpretCameraPacket(
        const QByteArray &packet)
{
    const uint8_t byte2 = packet.at(2);

    const bool trackingState =
            (byte2 & 0x02) >> 1;

    const bool isDetecting =
            m_videoCaptureDetection.
            isDetecting();

    const bool isAutoLock =
            m_videoCaptureDetection.
            isAutoLock();

    if (trackingState == false &&
            isDetecting == false &&
            isAutoLock == true)
    {
        m_videoCaptureDetection.enableDetecting(true);
        m_videoCaptureDetection.enableAutoLock(false);
    }
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
processStartAutoTrack(
        const QRectF &rect)
{
    qCritical() << "process Start Auto Track " << rect;

    Q_UNUSED(rect);

    m_videoCaptureDetection.
            enableAutoLock(true);
}

void Controller::
processStopTrack()
{
    qCritical() << "process Stop Track ";

    m_videoCaptureDetection.
            enableDetecting(true);

    m_videoCaptureDetection.
            enableAutoLock(false);

    stopCameraTrack();

    QTimer::singleShot(1000, this,
                       &Controller::
                       cameraGotoCenter);
}

void Controller::
cameraGotoCenter()
{
    QByteArray packet(16, char(0x00));
    packet[ 0] = m_cameraCommandHeader.at(0);
    packet[ 1] = m_cameraCommandHeader.at(1);
    packet[ 2] = 0x2b;
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

    m_cameraSerialController.
            writeData(packet);
}

void Controller::
startCameraTrack(const int16_t &xPos,
                 const int16_t &yPos)
{
    QByteArray packet(16, char(0x00));
    packet[ 0] = m_cameraCommandHeader.at(0);
    packet[ 1] = m_cameraCommandHeader.at(1);
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

    m_cameraSerialController.
            writeData(packet);
}

void Controller::
stopCameraTrack()
{
    QByteArray packet(16, char(0x00));
    packet[ 0] = m_cameraCommandHeader.at(0);
    packet[ 1] = m_cameraCommandHeader.at(1);
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

    m_cameraSerialController.
            writeData(packet);
}

void Controller::
cameraAutoCorrection()
{
    QByteArray packet(16, char(0x00));

    packet[ 0] = m_cameraCommandHeader.at(0);
    packet[ 1] = m_cameraCommandHeader.at(1);
    packet[ 2] = 0x70;
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

    std::cerr << "cameraAutoCorrection"
              << packet.toHex(' ').toStdString()
              << std::endl;

    m_cameraSerialController.
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
        packet[ 0] = m_cameraCommandHeader.at(0);
        packet[ 1] = m_cameraCommandHeader.at(1);
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
        packet[ 0] = m_cameraCommandHeader.at(0);
        packet[ 1] = m_cameraCommandHeader.at(1);
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

    m_cameraSerialController.
            writeData(packet);
}

void Controller::
sltPanelNewDataRecieved(
        const QByteArray &packet)
{
    m_panelSerialBuffer.
            push_back(packet);

    analyzePanelPacket();
}

void Controller::
sltCameraNewDataRecieved(
        const QByteArray &packet)
{
    m_cameraSerialBuffer.
            push_back(packet);

    analyzeCameraPacket();
}

void Controller::
sltAutoLockDetected(const QRectF &bbox)
{
    m_videoCaptureDetection.enableDetecting(false);

    const cv::Size frameSize =
            m_videoCaptureDetection.frameSize();

    const QPointF centerPos = bbox.center();

    const QPointF realValue(
                centerPos.x() -
                (frameSize.width / 2),
                (frameSize.height / 2) -
                centerPos.y());

    qCritical() << "startTrack" << realValue;

    startCameraTrack(realValue.x(),
                     realValue.y());
}
