#pragma once

#include <iostream>

#include <QByteArray>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "ControlPanelSDK_global.h"

namespace SDK
{
class CONTROLPANELSDK_EXPORT CommandCreator
{
public:
    // =============================
    //      TV Camera
    // =============================
    QByteArray tvCameraPower(
            const PowerStates &power);

    QByteArray tvCameraZoom(
            const ZoomCommands &command,
            const uint8_t &value);

    QByteArray tvCameraFocus(
            const FocusCommands &command,
            const uint8_t &value);

    QByteArray tvCameraFocusMode(
            const FocusModes &mode);

    QByteArray tvCameraNightMode(
            const ActivationModes &mode);

    // =============================
    //      TV Camera Feedbacks
    // =============================
    QByteArray tvCameraPowerFeedback(
            const PowerStates &powerState);

    QByteArray tvCameraZoomValueFeedback(
            const uint16_t &zoomValue);

    QByteArray tvCameraFocusValueFeedback(
            const uint16_t &focusValue);

    QByteArray tvCameraFocusModeFeedback(
            const FocusModes &focusMode);

    QByteArray tvCameraFOV_Feedback(
            const float &fov);

    // =============================
    //      IR Camera
    // =============================
    QByteArray irCameraPower(
            const PowerStates &power);

    QByteArray irCameraZoom(
            const ZoomCommands &command,
            const uint8_t &value);

    QByteArray irCameraDigitalZoom(
            const DigitalZooms &digitalZoom);

    QByteArray irCameraFocus(
            const FocusCommands &command,
            const uint8_t &value);

    QByteArray irCameraFocusMode(
            const FocusModes &mode);

    QByteArray irCameraPolarity(
            const PolarityCommands &mode,
            const uint8_t &value);

    QByteArray irCameraCalibrationMode(
            const CalibrationModes &mode);

    QByteArray irCameraCalibrationTrigger();

    QByteArray irCameraBrightness(
            const BrightnessModes &mode,
            const uint8_t &value);

    QByteArray irCameraContrast(
            const ContrastModes &mode,
            const uint8_t &value);

    QByteArray irCameraMirrorImage(
            const MirrorModes &mode);

    // =============================
    //      IR Camera Feedbacks
    // =============================
    QByteArray irCameraPowerFeedback(
            const PowerStates &powerState);

    QByteArray irCameraZoomValueFeedback(
            const uint16_t &zoomValue);

    QByteArray irCameraDigitalZoomFeedback(
            const DigitalZooms &digitalZoom);

    QByteArray irCameraFocusValueFeedback(
            const uint16_t &focusValue);

    QByteArray irCameraFocusModeFeedback(
            const FocusModes &focusMode);

    QByteArray irCameraFOV_Feedback(
            const float &fov);

    QByteArray irCameraPolarityFeedback(
            const Polarities &polarity);

    QByteArray irCameraCalibrationModeFeedback(
            const CalibrationModes &mode);

    QByteArray irCameraBrightnessFeedback(
            const uint8_t &brightness);

    QByteArray irCameraContrastFeedback(
            const uint8_t &contrast);

    // =============================
    //      Pedestal
    // =============================
    QByteArray pedestalPower(
            const PowerStates &power);

    QByteArray pedestalGoToPosition(
            const float &azimuth,
            const float &elevation);

    QByteArray pedestalMove(
            const int8_t &xSpeed,
            const int8_t &ySpeed);

    QByteArray pedestalSetZero(
            const ZeroModes &mode,
            const float &azimuth,
            const float &elevation);

     QByteArray pedestalGoToZero();

     QByteArray pedestalAutoReference();

     QByteArray pedestalSetGyro(
             const float &xPosition,
             const float &yPosition);

     QByteArray pedestalSetParkPosition(
             const float &azimuth,
             const float &elevation);

     QByteArray pedestalGoToParkPosition();

     // =============================
     //      Pedestal Feedbacks
     // =============================
     QByteArray pedestalPowerFeedback(
             const PowerStates &powerState);

     QByteArray pedestalAbsolutePositionFeedback(
             const double &azimuth,
             const double &elevation,
             const uint8_t &hour,
             const uint8_t &minute,
             const uint8_t &second,
             const uint32_t &microSecond);

     QByteArray pedestalZeroPositionFeedback(
             const float &azimuth,
             const float &elevation);

     QByteArray pedestalParkPositionFeedback(
             const float &azimuth,
             const float &elevation);

     // =============================
     //      Processor
     // =============================
     QByteArray processorSystemPower(
             const SystemPowerStates &state);

     QByteArray processorActiveCamera(
             const ActiveCameras &camera);

     QByteArray processorSetPIP(
             const PIP_Modes &mode);

     QByteArray processorEqualZoom(
             const PowerStates &state,
             const ActiveCameras &reference);

     QByteArray processorTrackAlgorithm(
             const TrackAlgorithms &algorithm);

     QByteArray processorTrackSize(
             const TrackSizes &size);

     QByteArray processorTrackMode(
             const TrackModes &mode);

     QByteArray processorStartPointTrack(
             const int16_t &xPosition,
             const int16_t &yPosition);

     QByteArray processorDrawTrackRectangle(
             const int16_t &xPosition,
             const int16_t &yPosition,
             const uint16_t &width,
             const uint16_t &height);

     QByteArray processorStartRectTrack(
             const int16_t &xPosition,
             const int16_t &yPosition,
             const uint16_t &width,
             const uint16_t &height);

     QByteArray processorStartTargetID_Track(
             const uint8_t &targetID);

     QByteArray processorStartAutoLock(
             const int16_t &xPosition,
             const int16_t &yPosition,
             const uint16_t &width,
             const uint16_t &height);

     QByteArray processorStopTrack();

     QByteArray processorRecenterTrack();

     QByteArray processorResizeTrack();

     QByteArray processorSetLostObject(
             const uint8_t &index);

     QByteArray processorForceMemory(
             const ActivationModes &mode);

     QByteArray processorSearchAlgorithm(
             const SearchAlgorithms &algorithm);

     QByteArray processorDrawSearchRectangle(
             const int16_t &xPosition,
             const int16_t &yPosition);

     QByteArray processorSearchActivation(
             const ActivationModes &mode);

     QByteArray processorOSD(
             const ActiveCameras &camera,
             const OSD_States &state);

     QByteArray processorReticle(
             const ActiveCameras &camera,
             const ReticleStates &state);

     QByteArray processorMoveReticle(
             const ActiveCameras &camera,
             const MoveReticleModes &mode,
             const int16_t &xPosition,
             const int16_t &yPosition);

     QByteArray processorEnhancement(
             const ActivationModes &mode,
             const TrackerEnhancements &algorithm);

     QByteArray processorStreamBitrate(
             const uint16_t &bitrate);

     QByteArray processorKeepConnectionAlive();

     QByteArray processorSetDateTime(
             const uint32_t &date,
             const uint32_t &time);

     QByteArray processorSetGPS_Position(
             const int32_t &latitude,
             const int32_t &longitude,
             const int16_t &altitude);

     // =============================
     //      Processor Feedbacks
     // =============================
     QByteArray processorActiveCameraFeedback(
             const ActiveCameras &activeCamera);

     QByteArray processorTrackAlgorithmFeedback(
             const TrackAlgorithms &algorithm);

     QByteArray processorTrackSizeFeedback(
             const TrackSizes &size);

     QByteArray processorTrackModeFeedback(
             const TrackModes &mode);

     QByteArray processorLostObjectSearchByMDFeedback(
             const uint8_t &index);

     QByteArray processorStatusFeedback(
             const ProcessorStatuses &status);

     QByteArray processorSearchAlgorithmFeedback(
             const SearchAlgorithms &algorithm);

     QByteArray processorTrackerErrorFeedback(
             const int16_t &xError,
             const int16_t &yError);

     QByteArray processorTargetGeoPositionFeedback(
             const int32_t &latitude,
             const int32_t &longitude,
             const int16_t altitude);

     QByteArray processorEnhancementFeedback(
             const ActivationModes &mode,
             const TrackerEnhancements &algorithm);

     QByteArray processorKeepConnectionAliveFeedback(
             const QString &version);

     QByteArray processorMUX_ConnectionStatusFeedback(
             const PowerStates &status);

     QByteArray processorGPS_PositionFeedback(
             const int32_t &latitude,
             const int32_t &longitude,
             const int16_t &altitude,
             const uint8_t &numberOfSatellites);

     // =============================
     //      LRF
     // =============================
     QByteArray lrfPower(const PowerStates &power);

     QByteArray lrfContinuesShot(
             const LRF_States &mode,
             const uint16_t &interval);

     QByteArray lrfMeasure();
     QByteArray lrfStopMeasure();

     // =============================
     //      LRF Feedbacks
     // =============================
     QByteArray lrfPowerFeedback(
             const PowerStates &powerState);

     QByteArray lrfDistanceFeedback(
             const LRF_States &state,
             const float &distance);

     // =============================
     //      Temp & Hum
     // =============================
     QByteArray tempHumSetHeaterParameter(
             const uint8_t &frequency,
             const uint8_t &dutyCycle);

     // =============================
     //      General Methods
     // =============================
    static void calculateChecksum(
            const QByteArray &packet,
            uint8_t &checksum1,
            uint8_t &checksum2);

private:
    union Int32_ToArray
    {
        uint8_t array[4];
        uint32_t uInt;
        int32_t sInt;
    };

    union Int16_ToArray
    {
        uint8_t array[2];
        uint16_t uInt;
        int16_t sInt;
    };

    QByteArray createCommand(
            const Devices &source,
            const Devices &destination,
            const Modules &moduleID,
            const uint8_t &command,
            const QByteArray &payload);

};
}
