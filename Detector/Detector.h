#ifndef DETECTOR_H
#define DETECTOR_H

#include "DetectorInterface.h"
#include "IntensityDetector/IntensityDetector.h"

#include <opencv2/opencv.hpp>
#include <QPointF>
#include <QRect>
#include <QTimer>
#include <QElapsedTimer>
#include <QDataStream>
#include "globalCalculation.h"


class Detector : public QObject {
    Q_OBJECT
public:
    explicit Detector();
    ~Detector();

    struct AutoLockInfo {
        int numberOfDetected;
        cv::Rect bbox;
    };

    void enableDetecting(const bool& state);
    void enableAutoLock(const bool& state);
    std::string getName();
    void detect(cv::Mat *input);
    bool isDetectorActivated();
    void setInputSize(const cv::Size& inputSize);
    cv::Rect getDetectedBoundingBox(const QPointF &position = QPointF(0, 0));
    std::vector<IDetector::DetectionInfo> getAllDetectedObjects();
    void clearMemory();

private:

    enum SearchAlgorithms
    {
        SearchAlgorithm_Motion    = 0x00,
        SearchAlgorithm_Intensity     = 0x01,
        SearchAlgorithm_AI      = 0x02,
        SearchAlgorithm_ReferenceMatching      = 0x03
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

    cv::Size getTargetSize(const int &width, const int &height) const;
    IDetector *createDetectorFactory(SearchAlgorithms algorithm);

    // Detectores
    IntensityDetector* m_intensity;

    // Factory method element
    IDetector* m_detector;

    cv::Size m_inputSize;
    std::vector<IDetector::DetectionInfo> m_allObjectDetected;
    int m_scaleSize;
    cv::Rect m_detectorSearchRect;
    bool m_isDetectingActivated;
    cv::Rect m_targetBBox;

    std::vector<IDetector::DetectionInfo> m_vectorDetectionResult;
    int m_detectionCount;

    std::vector<AutoLockInfo> m_allAutoLockInfo;
    Calculator *m_calculator;

    bool m_isAutoLockActivated;
    double m_autoLockDistanceThreshold;
    int m_autoLockNumberOfDetectedThreshold;

    bool m_isActiveLostMovingObjectSearch;

    TrackSizes m_targetSize;



private Q_SLOTS:
    void sltAutoLockChecking();


Q_SIGNALS:
    void sigDetectionDataUpdated();
    void sigStopDetection();
    void sigAutoLockDetected(const QRectF &bbox);
};

#endif // DETECTOR_H
