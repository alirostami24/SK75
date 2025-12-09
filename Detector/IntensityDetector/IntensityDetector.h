#ifndef INTENSITYDETECTOR_H
#define INTENSITYDETECTOR_H

#include "Detector/DetectorInterface.h"
#include "Detector/globalCalculation.h"
#include <QObject>
#include "TH/TH.hpp"

class IntensityDetector : public QObject, public IDetector
{
    Q_OBJECT

public:
    // Constructor
    explicit IntensityDetector();
    virtual ~IntensityDetector();

	void initialize() override;

    ///
    /// \brief getName
    /// returns the name of detector.
    std::string getName() const override;

    ///
    /// \brief getVersion
    /// returns the version of detector.
    std::string getVersion() const override;

    ///
    /// \brief
    /// The main function of the detection procedure.
    /// \param pData
    /// A buffer by which the data of the input frame is passed to the detector.
    bool detect(unsigned char* pData) override;

    ///
    /// \brief getDetectedBoundingBox
    /// Returns the bounding box area of ​​the closest detected object to the user's click location.
    /// \param targetCenter
    /// The approximate center of the object clicked by the user.
    /// \return
    /// A bounding box of ​​the closest detected object.
    cv::Rect2f getDetectedBoundingBox(const cv::Point2d& targetCenter = cv::Point2d(0, 0)) const override;

    ///
    /// \brief getAllDetectedObjects
    /// Returns the bounding boxes and id of all detected objects.
    /// \return
    /// Returns a vector containing the bounding boxes and id of all detected objects.
    std::vector<IDetector::DetectionInfo> getAllDetectedObjects() const override;

    ///
    /// \brief setInputSize
    /// Set input size.
    ///
    void setInputSize(const cv::Size& inputSize) override;


    /// \brief enableDetection
    /// Activate and deactive detector
    /// \param state
    ///
    void enableDetection(bool state) override;

    /// \brief isDetectionActivated
    /// Detection of detector activation
    /// \return
    ///
    bool isDetectionActivated() override;

    ///
    /// \brief
    /// This function stops the detector.
    ///
    void stopDetection() override;

    ///
    /// \brief
    /// This function clears the detector memory.
    ///
    void clearMemory() override;




private:
    struct DetectionValidityInfo {
        int numberOfDetected;
        cv::Rect bbox;
    };

    bool checkDetectValidity();
    bool isValidPoint(const cv::Point2d& point);

    TH *m_thDetector;
    Calculator *m_calculator;

    cv::Rect m_detectorSearchRect;
    std::vector<IDetector::DetectionInfo> m_vectorDetectionResult;
    bool m_isDetecorActivated;

    cv::Mat m_frame;

    cv::Size m_inputSize;

    bool m_isDetectorInitialized;

    std::vector<DetectionValidityInfo> m_allDetectionValidityInfo;
    double m_maxValidDistance;
    int m_numberOfDetectedThreshold;
    cv::Rect2f m_validObjectRect;

    // Target position Estimation
    struct Translation {
        double horizontal;
        double vertical;
    };

    bool m_isTargetPositionEstimationEnabled;
    std::vector<Translation> m_targetEstimationMemory;
    int m_targetEstimationMemoryIndex;
    cv::Point m_lastEstimatedTargetPosition;
    double m_distanceTolerance;

Q_SIGNALS:
    void sigAutoLockingIsPossible();
    void sigDetectionDataUpdated();

};

#endif // INTENSITYDETECTOR_H
