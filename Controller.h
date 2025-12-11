#pragma once

#include <iostream>

#include <QObject>
#include <QProcess>
#include <QDateTime>
#include <QApplication>
#include <QSerialPortInfo>

#include "CommandCreator.h"
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
    SerialController m_panelSerialController;

    SerialController m_cameraSerialController;

    VideoCapture m_videoCaptureDetection;
    VideoCapture m_videoCaptureRender;
    VideoCapture m_videoCaptureFeeder;

    QByteArray m_panelSerialBuffer;
    QByteArray m_cameraSerialBuffer;

    const uint8_t m_panelSourceID_ByteIndex;
    const uint8_t m_panelDestinationID_ByteIndex;
    const uint8_t m_panelModuleID_ByteIndex;
    const uint8_t m_panelCommandID_ByteIndex;
    const uint8_t m_panelLengthByteIndex;

    const uint8_t m_panelMinimumLength;

    int16_t m_footerOffset;

    QByteArray m_panelHeader;
    QByteArray m_panelFooter;

    QByteArray m_cameraCommandHeader;
    QByteArray m_cameraFeedbackHeader;
    const uint8_t m_feedbackSize;

    void initialize();
    void initializeConnections();

    void analyzePanelPacket();
    void interpretPanelPacket(
            const QByteArray &packet);

    void interpretProcessorPacket(
            const QByteArray &packet);

    void analyzeCameraPacket();
    void interpretCameraPacket(
            const QByteArray &packet);

    uint8_t calculateChecksum(
            const QByteArray &packet);

    void processStartAutoTrack(
            const QRectF &rect);

    void processStopTrack();

    void startCameraTrack(const int16_t &xPos,
                          const int16_t &yPos);

    void stopCameraTrack();

    void changeCameraOSD_Visibility(
            const bool &state);

private Q_SLOTS:
    void sltPanelNewDataRecieved(
            const QByteArray &packet);

    void sltCameraNewDataRecieved(
            const QByteArray &packet);

    void sltAutoLockDetected(const QRectF &bbox);

};
