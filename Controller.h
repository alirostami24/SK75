#pragma once

#include <iostream>

#include <QObject>
#include <QSerialPortInfo>

#include "ControlPanelSDK.h"
#include "ControlPanelSDK_global.h"

#include "VideoCapture.h"
#include "SerialController.h"

using namespace SDK;

class Controller : public QObject
{
    Q_OBJECT

public:
    Controller();

private:
    ControlPanelSDK m_controlPanelSDK;

    SerialController m_serialController;
    VideoCapture m_videoCapture;

    QByteArray m_serialBuffer;

    QByteArray m_commandHeader;
    QByteArray m_feedbackHeader;
    const uint8_t m_feedbackSize;

    void initialize();
    void initializeConnections();

    void analyzePacket();
    void interpretPacket(
            const QByteArray &packet);

    uint8_t calculateChecksum(
            const QByteArray &packet);

    void startCameraTrack(const int16_t &xPos,
                          const int16_t &yPos);

    void stopCameraTrack();

    void changeCameraOSD_Visibility(
            const bool &state);

private Q_SLOTS:
    void sltProcessorDataChanged(
            const ProcessorCommands &command);

    void sltNewDataRecieved(
            const QByteArray &packet);

    void sltNewFrameReceived();

};
