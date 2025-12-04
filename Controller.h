#pragma once

#include <iostream>

#include <QObject>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QSerialPortInfo>

#include "VideoCapture.h"
#include "SerialController.h"
#include "MCMD/MCMD_Controler.h"

#include "MainWindow.h"

class Controller : public QObject
{
    Q_OBJECT

public:
    Controller();

private:
    SerialController m_serialController;
    VideoCapture m_videoCapture;
    MCMD_Controler m_mcmdController;

    MainWindow m_mainWindow;

    cv::Mat m_frameMat;
    bool m_isMCMD_Initialized;
    bool m_isMCMD_InProgress;

    QByteArray m_serialBuffer;

    QByteArray m_commandHeader;
    QByteArray m_feedbackHeader;
    const uint8_t m_feedbackSize;

    //  *******************************
    //      Type Declaration
    //  *******************************
    template<typename T>
    inline uint8_t toU8(const T &input)
    {
        return static_cast<uint8_t>(input);
    }

    QFutureWatcher<cv::Mat> m_futureWatcherMCMD;

    void initialize();
    void initializeConnections();

    void analyzePacket();
    void interpretPacket(
            const QByteArray &packet);

    uint8_t calculateChecksum(
            const QByteArray &packet);

    cv::Mat mcmd();

private Q_SLOTS:
    void sltMCMD_Finished();

    void sltChangeSerialConnectionRequseted(
            const bool &shouldConnect);

    void sltRefreshSerialPortListRequested();

    void sltNewDataRecieved(
            const QByteArray &packet);

    void sltStartTrackRequested(
            const int16_t &xPos,
            const int16_t &yPos);

    void sltStopTrackRequested();

    void sltNewFrameReceived();

    void sltChangeOSD_Requested(
            const bool &state);
};
