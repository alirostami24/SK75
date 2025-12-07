#ifndef DETECTORINTERFACE_H
#define DETECTORINTERFACE_H


#include <opencv2/core.hpp>
#include <QRect>

///
/// \brief The ITracker class:
/// The interface for the tracking classes. In this class all main functions and properties are listed.
class IDetector {
public:
    virtual ~IDetector() = default;

    struct DetectionInfo {
        int ID;
        cv::Rect bbox;
    };

    enum DetectorResultState {
            DetectorResultState_Valid = 0x01,
            DetectorResultState_NothingDetected = 0x02,
            DetectorResultState_Initialize = 0x03,
            DetectorResultState_Reconnected = 0x04
        };

    struct TCPDetectionInfo {
            quint8 confidence;
            QRect bbox;
        };

    ///
    /// \brief initialize
    /// Initializes the detector.
    virtual void initialize() {};

    ///
    /// \brief getName
    /// returns the name of detector.
    virtual std::string getName() const {return "None";};

    ///
    /// \brief getVersion
    /// returns the version of detector.
    virtual std::string getVersion() const {return "None";};

    ///
    /// \brief
    /// The main function of the detection procedure.
    /// \param pData
    /// A buffer by which the data of the input frame is passed to the detector.
    virtual bool detect(unsigned char* pData) {return false;};

    ///
    /// \brief getDetectedBoundingBox
    /// Returns the bounding box area of ​​the closest detected object to the user's click location.
    /// \param targetCenter
    /// The approximate center of the object clicked by the user.
    /// \return
    /// A bounding box of ​​the closest detected object.
    virtual cv::Rect2f getDetectedBoundingBox(const cv::Point2d& targetCenter) const {return cv::Rect2f(-1,-1,0,0);};

    ///
    /// \brief getAllDetectedObjects
    /// Returns the bounding boxes and id of all detected objects.
    /// \return
    /// Returns a vector containing the bounding boxes and id of all detected objects.
    virtual std::vector<IDetector::DetectionInfo> getAllDetectedObjects() const { return std::vector<IDetector::DetectionInfo>();};

    /// \brief enableDetection
    /// Activate and deactive detector
    /// \param state
    ///
    virtual void enableDetection(bool state) {};

    /// \brief isDetectionActivated
    /// Detection of detector activation
    /// \return
    ///
    virtual bool isDetectionActivated() {return false;};

    ///
    /// \brief
    /// This function stops the detector.
    ///
    virtual void stopDetection() {};

    ///
    /// \brief
    /// This function clears the detector memory.
    ///
    virtual void clearMemory() {};

};

#endif // DETECTORINTERFACE_H
