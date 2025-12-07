#pragma once

#include <QString>
#include <QSerialPort>
#include <QAbstractSocket>
#include <QtCore/qglobal.h>

#include "TypeCastings_global.h"

#if defined(CONTROLPANELSDK_LIBRARY)
#  define CONTROLPANELSDK_EXPORT Q_DECL_EXPORT
#else
#  define CONTROLPANELSDK_EXPORT Q_DECL_IMPORT
#endif

namespace SDK
{
//  ----------------------------------
//      Global Variables
//  ----------------------------------
extern const uint8_t gHeader1;
extern const uint8_t gHeader2;
extern const uint8_t gFooter1;
extern const uint8_t gFooter2;

//  ----------------------------------
//      Global Enumerators
//  ----------------------------------
enum CommunicationTypes
{
    CommunicationType_Network,
    CommunicationType_Serial
};

enum Devices
{
    Device_GUI          = 0x70,
    Device_Processor    = 0x71,
    Device_MUX_Board    = 0x72,
};

enum Modules
{
    Module_TV_Camera    = 0x30,
    Module_IR_Camera    = 0x31,
    Module_Pedestal     = 0x32,
    Module_Processor    = 0x33,
    Module_LRF          = 0x34,
    Module_TMP_HMD      = 0x35,
};

enum TV_Commands
{
    TV_Command_Power                = 0x01,
    TV_Command_Zoom                 = 0x02,
    TV_Command_Focus                = 0x03,
    TV_Command_FocusMode            = 0x04,
    TV_Command_NightMode            = 0x05,

    TV_Command_PowerFeedback        = 0x70,
    TV_Command_ZoomValueFeedback    = 0x71,
    TV_Command_FocusValueFeedback   = 0x72,
    TV_Command_FocusModeFeedback    = 0x73,
    TV_Command_FOV_Feedback         = 0x74,
};

enum IR_Commands
{
    IR_Command_Power                    = 0x01,
    IR_Command_Zoom                     = 0x02,
    IR_Command_DigitalZoom              = 0x03,
    IR_Command_Focus                    = 0x04,
    IR_Command_FocusMode                = 0x05,
    IR_Command_Polarity                 = 0x06,
    IR_Command_CalibrationMode          = 0x07,
    IR_Command_CalibrationTrigger       = 0x08,
    IR_Command_Brightness               = 0x09,
    IR_Command_Contrast                 = 0x0A,
    IR_Command_MirrorImage              = 0x0B,

    IR_Command_PowerFeedback            = 0x70,
    IR_Command_ZoomValueFeedback        = 0x71,
    IR_Command_DigitalZoomFeedback      = 0x72,
    IR_Command_FocusValueFeedback       = 0x73,
    IR_Command_FocusModeFeedback        = 0x74,
    IR_Command_FOV_Feedback             = 0x75,
    IR_Command_PolarityFeedback         = 0x76,
    IR_Command_CalibrationModeFeedback  = 0x77,
    IR_Command_BrightnessFeedback       = 0x78,
    IR_Command_ContrastFeedback         = 0x79,
};

enum PedestalCommands
{
    PedestalCommand_Power                       = 0x01,
    PedestalCommand_GoToPosition                = 0x02,
    PedestalCommand_MovePedstal                 = 0x03,
    PedestalCommand_SetZero                     = 0x04,
    PedestalCommand_GoToZero                    = 0x05,
    PedestalCommand_AutoReference               = 0x06,
    PedestalCommand_SetGyro                     = 0x07,
    PedestalCommand_SetParkPosition             = 0x08,
    PedestalCommand_GoToParkPosition            = 0x09,

    PedestalCommand_PowerFeedback               = 0x70,
    PedestalCommand_AbsolutePositionFeedback    = 0x71,
    PedestalCommand_ZeroPositionFeedback        = 0x72,
    PedestalCommand_ParkPositionFeedback        = 0x73,
};

enum ProcessorCommands
{
    ProcessorCommand_SystemPower                    = 0x01,
    ProcessorCommand_SetActiveCamera                = 0x02,
    ProcessorCommand_SetPIP                         = 0x03,
    ProcessorCommand_EqualZoom                      = 0x04,
    ProcessorCommand_SetTrackAlgorithm              = 0x05,
    ProcessorCommand_SetTrackSize                   = 0x06,
    ProcessorCommand_SetTrackMode                   = 0x07,
    ProcessorCommand_StartPointTrack                = 0x08,
    ProcessorCommand_DrawTrackRectangle             = 0x09,
    ProcessorCommand_StartRectTrack                 = 0x0A,
    ProcessorCommand_StartTragetID_Track            = 0x0B,
    ProcessorCommand_StartAutoLock                  = 0x0C,
    ProcessorCommand_StopTrack                      = 0x0D,
    ProcessorCommand_RecenterTrack                  = 0x0E,
    ProcessorCommand_ResizeTrack                    = 0x0F,
    ProcessorCommand_SetLostObjectSearchByMD        = 0x10,
    ProcessorCommand_ForceMemory                    = 0x11,
    ProcessorCommand_SetSearchAlgorithm             = 0x12,
    ProcessorCommand_DrawSearchRectangle            = 0x13,
    ProcessorCommand_SetSearchActivation            = 0x14,
    ProcessorCommand_OSD                            = 0x15,
    ProcessorCommand_Reticle                        = 0x16,
    ProcessorCommand_MoveReticle                    = 0x17,
    ProcessorCommand_Enhancement                    = 0x18,
    ProcessorCommand_StreamingBitrate               = 0x19,
    ProcessorCommand_KeepAlive                      = 0x1A,
    ProcessorCommand_SetDateAndTime                 = 0x1B,
    ProcessorCommand_SetGPS_Position                = 0x1C,

    ProcessorCommand_ActiveCameraFeedback           = 0x70,
    ProcessorCommand_TrackAlgorithmFeedback         = 0x71,
    ProcessorCommand_TrackSizeFeedback              = 0x72,
    ProcessorCommand_TrackModeFeedback              = 0x73,
    ProcessorCommand_ProcessorStatusFeedback        = 0x74,
    ProcessorCommand_LostObjectSearchByMDFeedback   = 0x75,
    ProcessorCommand_SearchAlgorithmFeedback        = 0x76,
    ProcessorCommand_TrackErrorFeedback             = 0x77,
    ProcessorCommand_TargetGeoPositionFeedback      = 0x78,
    ProcessorCommand_EnhancementFeedback            = 0x79,
    ProcessorCommand_KeepAliveFeedback              = 0x7A,
    ProcessorCommand_MUX_ConnectionStatusFeedback   = 0x7B,
    ProcessorCommand_GPS_PositionFeedback           = 0x7C,
};

enum LRF_Commands
{
    LRF_Command_Power               = 0x01,
    LRF_Command_ContinuesShot       = 0x02,
    LRF_Command_Measure             = 0x03,
    LRF_Command_StopMeasure         = 0x04,

    LRF_Command_PowerFeedback       = 0x70,
    LRF_Command_DistanceFeedback    = 0x71,
};

enum TMP_HMD_Commands
{
    TMP_HMD_HeaterParameter             = 0x01,

    TMP_HMD_HeaterParameter_Feedback    = 0x70,
    TMP_HMD_Command_Feedback            = 0x71,
};

enum PowerStates
{
    PowerState_Off  = 0x00,
    PowerState_On   = 0x01,
};

enum ZoomCommands
{
    ZoomCommand_Stop    = 0x00,
    ZoomCommand_ZoomOut = 0x01,
    ZoomCommand_ZoomIn  = 0x02,
    ZoomCommand_Direct  = 0x03,
};

enum FocusCommands
{
    FocusCommand_Stop   = 0x00,
    FocusCommand_Near   = 0x01,
    FocusCommand_Far    = 0x02,
    FocusCommand_Direct = 0x03,
};

enum FocusModes
{
    FocusMode_Manual    = 0x00,
    FocusMode_Automatic = 0x01,
};

enum ActivationModes
{
    Deactivate  = 0x00,
    Activate    = 0x01,
};

enum DigitalZooms
{
    DigitalZoom_1x  = 0x00,
    DigitalZoom_2x  = 0x01,
    DigitalZoom_4x  = 0x02,
};

enum PolarityCommands
{
    PolarityCommand_Decrease    = 0x00,
    PolarityCommand_Increase    = 0x01,
    PolarityCommand_Direct      = 0x02,
};

enum Polarities
{
    Polarity_WhiteHot           = 0x00,
    Polarity_BlackHot           = 0x01,
};

enum CalibrationModes
{
    CalibrationMode_Manual      = 0x00,
    CalibrationMode_Automatic   = 0x01,
};

enum BrightnessModes
{
    BrightnessMode_Decrease     = 0x00,
    BrightnessMode_Increase     = 0x01,
    BrightnessMode_Direct       = 0x02,
};

enum ContrastModes
{
    ContrastMode_Decrease   = 0x00,
    ContrastMode_Increase   = 0x01,
    ContrastMode_Direct     = 0x02,
};

enum MirrorModes
{
    MirrorMode_Off          = 0x00,
    MirrorMode_Horizontal   = 0x01,
    MirrorMode_Vertical     = 0x02,
    MirrorMode_Diagnal      = 0x03,
};

enum ZeroModes
{
    ZeroMode_AzimuthOnly            = 0x00,
    ZeroMode_ElevationOnly          = 0x01,
    ZeroMode_AzimuthAndElevation    = 0x02,
};

enum SystemPowerStates
{
    SystemPowerState_Shutdown   = 0x00,
    SystemPowerState_Restart    = 0x01,
};

enum ActiveCameras
{
    ActiveCamera_TV = 0x00,
    ActiveCamera_IR = 0x01,
};

enum PIP_Modes
{
    PIP_Mode_Disable        = 0x00,
    PIP_Mode_TopLeft        = 0x01,
    PIP_Mode_TopRight       = 0x02,
    PIP_Mode_BottomRight    = 0x03,
    PIP_Mode_BottomLeft     = 0x04,
};

enum TrackAlgorithms
{
    TrackAlgorithm_Centriod     = 0x00,
    TrackAlgorithm_TempMatching = 0x01,
    TrackAlgorithm_AI           = 0x02,
};

enum TrackSizes
{
    TrackSize_ExtraSmall    = 0x00,
    TrackSize_Small         = 0x01,
    TrackSize_Medium        = 0x02,
    TrackSize_Large         = 0x03,
    TrackSize_ExtraLarge    = 0x04,
    TrackSize_FullSize      = 0x05
};

enum TrackModes
{
    TrackMode_SelectByClick             = 0x00,
    TrackMode_SelectBySearchAndClick    = 0x01,
    TrackMode_SelectByRectangle         = 0x02,
    TrackMode_SelectByTargetID          = 0x03,
    TrackMode_SelectByAutoLock          = 0x04,
};

//enum TrackModes
//{
//    TrackMode_SelectByClick             = 0x00,
//    TrackMode_SelectBySearchAndClick    = 0x01,
//    TrackMode_SelectByRectangle         = 0x02,
//    TrackMode_SelectByAutoLock          = 0x03,
//};

enum ProcessorStatuses
{
    ProcessorStatus_NoProcess   = 0x00,
    ProcessorStatus_Track       = 0x01,
    ProcessorStatus_TrackMemory = 0x02,
    ProcessorStatus_Search      = 0x03,
    ProcessorStatus_Enhance     = 0x04,
    ProcessorStatus_Stabilize   = 0x05,
};

enum SearchAlgorithms
{
    SearchAlgorithm_Motion    = 0x00,
    SearchAlgorithm_Intensity     = 0x01,
    SearchAlgorithm_AI      = 0x02,
    SearchAlgorithm_ReferenceMatching      = 0x03
};

enum OSD_States
{
    OSD_State_Hide  = 0x00,
    OSD_State_Show  = 0x01,
};

enum ReticleStates
{
    ReticleState_Hide                   = 0x00,
    ReticleState_Cross                  = 0x01,
    ReticleState_RectangleCornerOnly    = 0x02,
};

enum MoveReticleModes
{
    MoveReticleMode_Reset       = 0x00,
    MoveReticleMode_Direct      = 0x01,
    MoveReticleMode_MoveUp      = 0x02,
    MoveReticleMode_MoveDown    = 0x03,
    MoveReticleMode_MoveLeft    = 0x04,
    MoveReticleMode_MoveRight   = 0x05,
};

enum TrackStates
{
    TrackState_StopTrack    = 0x00,
    TrackState_StartTrack   = 0x01,
};

enum TrackerEnhancements
{
    TrackerEnhancement_CLAHE    = 0x00,
    TrackerEnhancement_HE       = 0x01,
};

enum LRF_States
{
    LRF_State_SingleShot    = 0x00,
    LRF_State_Continuous    = 0x01,
};

//  ----------------------------------
//      Global Structures
//  ----------------------------------
struct GeoPos
{
    GeoPos(const double &lat = 0.0,
           const double &lon = 0.0,
           const int16_t &alt = 0)
    {
        latitude = lat;
        longitude = lon;
        altitude = alt;
    }

    double latitude;
    double longitude;
    int16_t altitude;
};

//  ----------------------------------
//      Global Classes
//  ----------------------------------
class CommunicationInfo
{
private:
    CommunicationTypes m_type;

    QString m_serialPortName;
    int32_t m_baudrate;

    QString m_destinationIP_Address;
    uint16_t m_destinationPortNumber;

    friend class SerialInfo;
    friend class NetworkInfo;
};

class SerialInfo : public CommunicationInfo
{
public:
    SerialInfo(const QString &portName = "",
               const int32_t &baudrate = 0)
    {
        m_type = CommunicationType_Serial;

        setSerialPortName(portName);
        setBaudrate(baudrate);
    }

    void setSerialPortName(
            const QString &portName)
    {
        m_serialPortName = portName;
    }

    QString serialPortName() const
    {
        return m_serialPortName;
    }

    void setBaudrate(
            const int32_t &baudrate)
    {
        m_baudrate = baudrate;
    }

    int32_t baudrate() const
    {
        return m_baudrate;
    }

    bool operator==(const SerialInfo &info)
    {
        if (info.serialPortName() ==
                serialPortName() &&
                info.baudrate() == baudrate())
        {
            return true;
        }

        return false;
    }

    bool operator!=(const SerialInfo &info)
    {
        if (info.serialPortName() !=
                serialPortName() ||
                info.baudrate() != baudrate())
        {
            return true;
        }

        return false;
    }
};

class NetworkInfo : public CommunicationInfo
{
public:
    NetworkInfo(const QString &ipAddress = "",
                const uint16_t &portNumber = 0)
    {
        m_type = CommunicationType_Network;

        setDestinationIP_Address(ipAddress);
        setDestinationPortNumber(portNumber);
    }

    void setDestinationIP_Address(
            const QString &address)
    {
        m_destinationIP_Address = address;
    }

    QString destinationIP_Address() const
    {
        return m_destinationIP_Address;
    }

    void setDestinationPortNumber(
            const uint16_t &portNumber)
    {
        m_destinationPortNumber = portNumber;
    }

    uint16_t destinationPortNumber() const
    {
        return m_destinationPortNumber;
    }

    bool operator==(const NetworkInfo &info)
    {
        if (info.destinationIP_Address() ==
                destinationIP_Address() &&
                info.destinationPortNumber() ==
                destinationPortNumber())
        {
            return true;
        }

        return false;
    }

    bool operator!=(const NetworkInfo &info)
    {
        if (info.destinationIP_Address() !=
                destinationIP_Address() ||
                info.destinationPortNumber() !=
                destinationPortNumber())
        {
            return true;
        }

        return false;
    }
};

//  ----------------------------------
//      Global Methods
//  ----------------------------------
inline QString parseSerialErrorCode(
        const int32_t &errorCode)
{
    switch (errorCode) {
    case QSerialPort::NoError:
    {
        return "No error occurred.";
    }
    case QSerialPort::DeviceNotFoundError:
    {
        return "Device not found.";
    }
    case QSerialPort::PermissionError:
    {
        return "Permission error.";
    }
    case QSerialPort::OpenError:
    {
        return "Failed to open serial port.";
    }
    case QSerialPort::WriteError:
    {
        return "Write error occurred.";
    }
    case QSerialPort::ReadError:
    {
        return "Read error occurred.";
    }
    default:
    {
        return "Unknown error.";
    }
    }
}

inline QString parseNetworkErrorCode(
        const int32_t &errorCode)
{
    switch (errorCode)
    {
    case -1:
    {
        return "No error occurred.";
    }
    case QAbstractSocket::ConnectionRefusedError:
    {
        return "Connection Refused Error.";
    }
    case QAbstractSocket::RemoteHostClosedError:
    {
        return "Remote Host Closed Error.";
    }
    case QAbstractSocket::HostNotFoundError:
    {
        return "Host Not Found Error.";
    }
    case QAbstractSocket::SocketAccessError:
    {
        return "Socket Access Error.";
    }
    case QAbstractSocket::SocketResourceError:
    {
        return "Socket Resource Error.";
    }
    case QAbstractSocket::SocketTimeoutError:
    {
        return "Socket Timeout Error.";
    }
    case QAbstractSocket::DatagramTooLargeError:
    {
        return "Datagram Too Large Error.";
    }
    case QAbstractSocket::NetworkError:
    {
        return "Network Error.";
    }
    case QAbstractSocket::AddressInUseError:
    {
        return "Address In Use Error.";
    }
    case QAbstractSocket::SocketAddressNotAvailableError:
    {
        return "Socket Address Not Available Error.";
    }
    case QAbstractSocket::UnsupportedSocketOperationError:
    {
        return "Unsupported Socket Operation Error.";
    }
    case QAbstractSocket::ProxyAuthenticationRequiredError:
    {
        return "Proxy Authentication Required Error.";
    }
    case QAbstractSocket::SslHandshakeFailedError:
    {
        return "SSL Handshake Failed Error.";
    }
    case QAbstractSocket::UnfinishedSocketOperationError:
    {
        return "Unfinished Socket Operation Error.";
    }
    case QAbstractSocket::ProxyConnectionRefusedError:
    {
        return "Proxy Connection Refused Error.";
    }
    case QAbstractSocket::ProxyConnectionClosedError:
    {
        return "Proxy Connection Closed Error.";
    }
    case QAbstractSocket::ProxyConnectionTimeoutError:
    {
        return "Proxy Connection Timeout Error.";
    }
    case QAbstractSocket::ProxyNotFoundError:
    {
        return "Proxy Not Found Error.";
    }
    case QAbstractSocket::ProxyProtocolError:
    {
        return "Proxy Protocol Error.";
    }
    case QAbstractSocket::OperationError:
    {
        return "Operation Error.";
    }
    default:
    {
        return "Unknown Socket Error.";
    }
    }
}
}
