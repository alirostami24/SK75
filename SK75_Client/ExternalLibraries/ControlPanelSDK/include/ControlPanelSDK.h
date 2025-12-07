#pragma once

#include <QObject>

#include "ControlPanelSDK_global.h"

namespace SDK
{
class CONTROLPANELSDK_EXPORT ControlPanelSDK_Private;

///
/// \brief TV Camera module class
/// This class stores all methods and attributes
/// related to TV camera.
/// Users are able to modify TV camera
/// settings by setter methods of this clas.
/// Users also are able to fetch TV camera
/// latest settings by calling getter methods
/// of this class.
///
class CONTROLPANELSDK_EXPORT TV_Camera
{
public:
    ///
    /// \brief Default constructor of this class.
    /// \param parent
    /// Pointer to parent object
    ///
    TV_Camera(ControlPanelSDK_Private *parent);

    ///
    /// \brief Modifies TV camera power status
    /// \param state
    /// Object of \ref PowerStates enumerator
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool setPower(const PowerStates &state);

    ///
    /// \brief Get TV camera power status
    /// \return
    /// Object of \ref PowerStates enumerator
    ///
    PowerStates getPowerState() const;

    ///
    /// \brief Sends zoom stop command to TV camera
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool zoomStop();

    ///
    /// \brief Sends zoom out command to TV camera
    /// \param Specifies speed of zoom process
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool zoomOut(const uint8_t &speed);

    ///
    /// \brief Sends zoom in command to TV camera
    /// \param Specifies speed of zoom process
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool zoomIn(const uint8_t &speed);

    ///
    /// \brief Move camera lens to a specific position
    /// \param Specifies position of TV camera lens
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool directZoom(const uint8_t &position);

    uint16_t getZoomValue() const;

    float getFOV_Value() const;

    ///
    /// \brief Sends focus stop command to TV camera
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool focusStop();

    ///
    /// \brief Sends focus near command to TV camera
    /// \param Specifies speed of focus process
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool focusNear(const uint8_t &speed);

    ///
    /// \brief Sends focus far command to TV camera
    /// \param Specifies speed of focus process
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool focusFar(const uint8_t &speed);

    ///
    /// \brief Sends camera lens to a specific position
    /// \param Specifies destination position of lens
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool directFocus(const uint8_t &position);

    uint16_t getFocusValue() const;

    ///
    /// \brief Sends focus mode comamnd to TV camera
    /// \param Specifies focus mode of TV camera
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool setFocusMode(const FocusModes &mode);

    FocusModes getFocusMode() const;

    ///
    /// \brief Sends night mode command to TV camera
    /// \param Specifies night mode of TV camera
    /// \return
    /// Send command result status
    /// true for successfully written on communication link
    /// otherwise false will return
    ///
    bool setNightMode(const ActivationModes &mode);

private:
    ControlPanelSDK_Private *m_priv = nullptr;

    PowerStates m_powerState;

    FocusModes m_focusMode;

    float m_fov;

    uint16_t m_zoomValue;
    uint16_t m_focusValue;

    friend class ControlPanelSDK_Private;
};

///
/// \brief IR Camera module class
/// This class stores all methods and attributes
/// related to IR camera.
/// Users are able to modify IR camera
/// settings by setter methods of this clas.
/// Users also are able to fetch IR camera
/// latest settings by calling getter methods
/// of this class.
///
class CONTROLPANELSDK_EXPORT IR_Camera
{
public:
    IR_Camera(ControlPanelSDK_Private *priv);

    bool setPower(const bool &isOn);
    PowerStates getPowerState() const;

    bool zoomStop();
    bool zoomOut(const uint8_t &speed);
    bool zoomIn(const uint8_t &speed);
    bool directZoom(const uint8_t &position);
    uint16_t getZoomValue() const;

    bool digitalZoom1x();
    bool digitalZoom2x();
    bool digitalZoom4x();
    DigitalZooms getDigitalZoom() const;

    float getFOV_Value() const;

    bool focusStop();
    bool focusNear(const uint8_t &speed);
    bool focusFar(const uint8_t &speed);
    bool directFocus(const uint8_t &position);
    uint16_t getFocusValue() const;

    bool setFocusMode(const bool &isAuto);
    FocusModes getFocusMode() const;

    bool previousPolarity();
    bool nextPolarity();
    bool directPolarity(const Polarities &polarity);
    Polarities getPolarity() const;

    bool manualCalibration();
    bool autoCalibration();
    bool triggerCalibration();
    CalibrationModes getCalibrationMode() const;

    bool decreaseBrightness();
    bool increaseBrightness();
    bool directBrightness(const uint8_t &brightness);
    uint8_t getBrightnessValue() const;


    bool decreaseContrast();
    bool increaseContrast();
    bool directContrast(const uint8_t &contrast);
    uint8_t getContrastValue() const;

    bool setMirrorImage(
            const MirrorModes &mode);

private:
    ControlPanelSDK_Private *m_priv = nullptr;

    PowerStates m_powerState;

    DigitalZooms m_digitalZooms;
    FocusModes m_focusMode;
    Polarities m_polarity;
    CalibrationModes m_calibrationMode;

    float m_fov;

    uint16_t m_zoomValue;
    uint16_t m_focusValue;

    uint8_t m_brightness;
    uint8_t m_contrast;

    friend class ControlPanelSDK_Private;
};

class CONTROLPANELSDK_EXPORT Pedestal
{
public:
    Pedestal(ControlPanelSDK_Private *priv);

    bool setPower(const PowerStates &state);
    PowerStates getPowerState() const;

    bool goToPosition(const float &azimuth,
                      const float &elevation);

    bool movePedestal(const int8_t &xSpeed,
                      const int8_t &ySpeed);

    float getAzimuth() const;
    float getElevation() const;

    bool setAzimuthZeroPosition(
            const float &azimuth);

    bool setElevationZeroPosition(
            const float &elevation);

    bool setZeroPosition(const float &azimuth,
                         const float elevation);

    float getZeroPositionAzimuth() const;
    float getZeroPositionElevation() const;

    bool goToZeroPosition();

    bool autoReference();

    bool setGyro(const int8_t &xPosition,
                 const int8_t &yPosition);

    bool setParkPosition(const float &azimuth,
                         const float &elevation);

    bool goToParkPosition();

    float getParkPositionAzimuth() const;
    float getParkPositionElevation() const;

private:
    ControlPanelSDK_Private *m_priv = nullptr;

    PowerStates m_powerState;

    float m_azimuth;
    float m_elevation;

    float m_zeroAzimuth;
    float m_zeroElevation;

    float m_parkAzimuth;
    float m_parkElevation;

    friend class ControlPanelSDK_Private;
};

class CONTROLPANELSDK_EXPORT Processor
{
public:
    Processor(ControlPanelSDK_Private *priv);

    bool shutdownSystem();
    bool rebootSystem();

    bool setActiveCamera(
            const ActiveCameras &camera);

    bool setPIP(const PIP_Modes &mode);

    bool setEqualZoom(
            const PowerStates &state,
            const ActiveCameras &referenceCamera);

    ActiveCameras getActiveCamera() const;

    bool setTrackAlgorithm(
            const TrackAlgorithms &algorithm);

    TrackAlgorithms getTrackAlgorithm() const;

    bool setTrackSize(const TrackSizes &size);

    TrackSizes getTrackSize() const;

    bool setTrackMode(const TrackModes &mode);
    TrackModes getTrackMode() const;

    bool startPointTrack(const int16_t &xPosition,
                         const int16_t &yPosition);

    bool drawTrackRectangle(const int16_t &xPosition,
                            const int16_t &yPosition,
                            const uint16_t &width,
                            const uint16_t &height);

    bool startRectTrack(const int16_t &xPosition,
                        const int16_t &yPosition,
                        const uint16_t &width,
                        const uint16_t &height);

    bool startTargetID_Track(const uint8_t &targetID);

    bool startAutoLock(const int16_t &xPosition,
                       const int16_t &yPosition,
                       const uint16_t &width,
                       const uint16_t &height);

    bool stopTrack();

    bool recenterTrack();

    bool resizeTrack();

    bool setLostObjectMotionDetection(
            const uint8_t &active);

    uint8_t getLostObjectSearchByMD() const;

    ProcessorStatuses getProcessorStatus() const;

    bool setSearchAlgorithm(
            const SearchAlgorithms &algorithm);

    SearchAlgorithms getSearchAlgorithm() const;

    bool drawSearchRectangle(
            const int16_t &xPosition,
            const int16_t &yPosition);

    bool setSearchActivation(const ActivationModes &mode);

    bool setOSD(const ActiveCameras &camera,
                const OSD_States &state);

    bool setReticle(const ActiveCameras &camera,
                    const ReticleStates &state);

    bool moveReticle(const ActiveCameras &camera,
                     const MoveReticleModes &mode,
                     const int16_t &xPosition,
                     const int16_t &yPosition);

    bool setStreamBitrate(const uint16_t &bitrate);

    bool keepConnectionAlive();

    bool setDateTime(const uint32_t &date,
                     const uint32_t &time);

    bool setGPS_Position(const int32_t &latitude,
                         const int32_t &longitude,
                         const int32_t &altitude);

    QString getProcessorVersion() const;

    PowerStates getMUX_ConnectionStatus() const;

    GeoPos getGPS_Position() const;


private:
    ControlPanelSDK_Private *m_priv = nullptr;

    ActiveCameras m_activeCamera;
    TrackAlgorithms m_trackAlgorithm;
    TrackSizes m_trackSize;
    TrackModes m_trackMode;
    ProcessorStatuses m_processorStatus;
    uint8_t m_lostObjectIndex;
    SearchAlgorithms m_searchAlgorithm;
    GeoPos m_targetGeoPosition;
    QString m_processorVersion;
    PowerStates m_muxConnectionStatus;
    GeoPos m_gpsPosition;

    friend class ControlPanelSDK_Private;
};

class CONTROLPANELSDK_EXPORT LRF
{
public:
    LRF(ControlPanelSDK_Private *priv);

    bool setPower(const PowerStates &state);
    PowerStates getPowerState() const;

    bool setContinuesShot(const LRF_States &mode, const uint16_t &interval);
    LRF_States getLRF_State() const;

    float getDistance() const;

    bool measure();
    bool stopMeasure();

private:
    ControlPanelSDK_Private *m_priv = nullptr;

    PowerStates m_powerState;
    LRF_States m_lrfState;

    float m_distance;

    friend class ControlPanelSDK_Private;
};

class CONTROLPANELSDK_EXPORT TempAndHumidity
{
public:
    TempAndHumidity(ControlPanelSDK_Private *priv);

    bool setHeaterParameter(const uint8_t &frequency,
                            const uint8_t &dutyCycle);

    uint8_t getHeaterFrequency() const;
    uint8_t getHeaterDutyCycle() const;

    uint8_t getSensorID() const;
    uint8_t getSensorsCount() const;

    float getTemperature() const;
    float getRelativeHumidity() const;

private:
    ControlPanelSDK_Private *m_priv = nullptr;

    uint8_t m_sensorID;
    uint8_t m_sensorsCount;

    uint8_t m_heaterFrequency;
    uint8_t m_heaterDutyCycle;

    float m_temperature;
    float m_relativeHumidity;

    friend class ControlPanelSDK_Private;
};

///
/// \brief Main class in ControlPanelSDK library
/// This class is backbone of ControlPanelSDK library.
/// Developers can interact with this library through
/// this class's methods and attributes.
/// First step of using this library, is to choose
/// desire communication type, this library suports
/// two different types communications:
/// 1) TCP Socket connection
/// 2) Serial connection
///
/// After creating this class object with desire
/// communication type, \ref initialize method must call
/// by right configuration
///
///
class CONTROLPANELSDK_EXPORT ControlPanelSDK : public QObject
{
    Q_OBJECT
private:
    ControlPanelSDK_Private *m_priv = nullptr;

public:
    TV_Camera tvCamera;
    IR_Camera irCamera;
    Pedestal pedestal;
    Processor processor;
    LRF lrf;
    TempAndHumidity tempAndHum;

    ///
    /// \brief Default constructor of ControlPanelSDK class
    /// \param type
    /// This parameter, specifies communication type
    ///
    ControlPanelSDK(const CommunicationTypes &type);

    ///
    /// \brief Default Deconstructor of ControlPanelSDK class
    ///
    ~ControlPanelSDK();

    // ===============================
    //  General Public Methods
    // ===============================

    ///
    /// \brief Initialize serial communication configuration
    /// \param serialInfo
    /// Object to \ref SerialInfo structure
    /// this object used to store serial
    /// communication configuration
    /// \param errorCode
    /// Object of \ref QSerialPort::SerialPortError
    ///
    /// \return
    /// start connection result
    /// if connection established successfully true will returns
    /// otherwise false will return
    ///
    bool initialize(const SerialInfo &serialInfo,
                    int32_t &errorCode);

    ///
    /// \brief Initialize TCP socket communication configuration
    /// \param serialInfo
    /// Object of \ref NetworkInfo structure
    /// this object used to store serial
    /// communication configuration
    /// \param errorCode
    /// Object of \ref QAbstractSocket::SocketError enumerator
    ///
    /// \return
    /// start connection result
    /// if connection established successfully true will returns
    /// otherwise false will return
    ///
    bool initialize(const NetworkInfo &serialInfo,
                    int32_t &errorCode);

    ///
    /// \brief terminate TCP socket or serial connection
    /// This method will stop any running connection and
    /// wipe all data related to them.
    ///
    void terminate();

    ///
    /// \brief Returns communication status
    ///
    /// \return
    /// Object of PowerStates enumerator
    ///
    PowerStates getConnectionState();

Q_SIGNALS:
    void sigTV_CameraDataChanged(
            const TV_Commands &command);

    void sigIR_CameraDataChanged(
            const IR_Commands &command);

    void sigPedestalDataChanged(
            const PedestalCommands &command);

    void sigProcessorDataChanged(
            const ProcessorCommands &command);

    void sigLRF_DataChanged(
            const LRF_Commands &command);

    void sigTempAndHumDataChanged();

    void sigNetworkServerDisconnected();

    void sigConnectionStatusChanged();

    void sigpacketSent(const QByteArray &packet);
};
}
