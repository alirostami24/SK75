#include "Detector.h"

Detector::Detector()
{
    m_inputSize.width = 720;
    m_inputSize.height = 576;

    m_scaleSize = 3;
    m_isDetectingActivated = false;

    // algorithms settings
    m_intensity = new IntensityDetector();

    m_detector = createDetectorFactory(SearchAlgorithms::SearchAlgorithm_Intensity);

    m_isAutoLockActivated = false;
    m_autoLockDistanceThreshold = 0.05 * std::min(m_inputSize.width, m_inputSize.height);
    m_autoLockNumberOfDetectedThreshold = 3;

    m_detectorSearchRect = cv::Rect(0, 0, m_inputSize.width, m_inputSize.height);

    //// Intensity Detector Connection
    QObject::connect(m_intensity, &IntensityDetector::sigDetectionDataUpdated,
                     this, &Detector::sigDetectionDataUpdated);

    QObject::connect(m_intensity, &IntensityDetector::sigAutoLockingIsPossible,
                     this, &Detector::sltAutoLockChecking);

}

Detector::~Detector()
{
    delete m_intensity;
}

bool Detector::isDetectorActivated()
{
    return m_isDetectingActivated;
}

bool Detector::isAutoLockActivated()
{
    return m_isAutoLockActivated;
}

void Detector::setInputSize(const cv::Size &inputSize)
{
    std::cerr << "set input size: w: " << inputSize.width << " h: " << inputSize.height << std::endl;
    m_inputSize = inputSize;
    m_detector->setInputSize(inputSize);
    m_detector->initialize();
}

void Detector::enableDetecting(const bool &state)
{
    m_isDetectingActivated = state;
    m_detector->enableDetection(state);

    if (state == false)
    {
        m_allAutoLockInfo.clear();
        Q_EMIT sigStopDetection();
    }

}

void Detector::enableAutoLock(const bool &state)
{
    m_isAutoLockActivated = state;
    m_allAutoLockInfo.clear();
}

std::string Detector::getName()
{
    return m_detector->getName();
}

void Detector::detect(cv::Mat *input)
{
    cv::cvtColor(*input, m_frame, cv::COLOR_BGRA2BGR);
    m_detector->detect(m_frame.data);
}

cv::Size Detector::getTargetSize(const int &width, const int &height) const
{
    cv::Size targetSize;
    if ((char)m_targetSize == TrackSize_ExtraSmall) { // 0: means tiny-sized target
        targetSize.width = static_cast<int>(width * 0.025);
        targetSize.height = static_cast<int>(height * 0.025);
    } else if ((char)m_targetSize == TrackSize_Small) { // 1: means small-sized target
        targetSize.width = static_cast<int>(width * 0.04);
        targetSize.height = static_cast<int>(height * 0.04);
    } else if ((char)m_targetSize == TrackSize_Medium) { // 2: means medium-sized target
        targetSize.width = static_cast<int>(width * 0.06);
        targetSize.height = static_cast<int>(height * 0.06);
    } else if ((char)m_targetSize == TrackSize_Large) { // 3: means large-sized target
        targetSize.width = static_cast<int>(width * 0.08);
        targetSize.height = static_cast<int>(height * 0.08);
    } else if ((char)m_targetSize == TrackSize_ExtraLarge) { // 4: means extra large-sized target
        targetSize.width = static_cast<int>(width * 0.1);
        targetSize.height = static_cast<int>(height * 0.1);
    } else if ((char)m_targetSize == TrackSize_FullSize) { // 5: like medium-sized target
        targetSize.width = static_cast<int>(width * 0.06);
        targetSize.height = static_cast<int>(height * 0.06);
    }

    return targetSize;
}
cv::Rect Detector::getDetectedBoundingBox(const QPointF &position)
{
    /*cv::Rect targetBBox;
    cv::Size halfImageSize;
    halfImageSize.width = m_inputSize.width / 2;
    halfImageSize.height = m_inputSize.height / 2;

    cv::Point targetCenter_;
    targetCenter_.x = halfImageSize.width + (halfImageSize.width * position.x());
    targetCenter_.y = halfImageSize.height - (halfImageSize.height * position.y());*/

    cv::Rect targetBBox = m_detector->getDetectedBoundingBox();
    /* if ((targetBBox.width <= 0) && (targetBBox.height <= 0))
    {
        cv::Size targetSize = getTargetSize(m_inputSize.width, m_inputSize.height);
        targetBBox.x = std::max(0, targetCenter_.x - (targetSize.width / 2));
        targetBBox.y = std::max(0, targetCenter_.y - (targetSize.height / 2));
        targetBBox.width = std::min(m_inputSize.width - targetBBox.x, targetSize.width);
        targetBBox.height = std::min(m_inputSize.height - targetBBox.y, targetSize.height);
    }*/

    return  targetBBox;

}

std::vector<IDetector::DetectionInfo> Detector::getAllDetectedObjects()
{
    m_allObjectDetected = m_detector->getAllDetectedObjects();
    return m_allObjectDetected;
}

void Detector::clearMemory()
{
    m_detector->clearMemory();
    m_allObjectDetected.clear();
}

IDetector *Detector::createDetectorFactory(SearchAlgorithms algorithm)
{
    IDetector* detector = nullptr;
    switch (algorithm)
    {
    //        case SearchAlgorithm_Motion:
    //        {
    //            detector = m_motionDetector;
    //            break;
    //        }
    case SearchAlgorithm_Intensity:
    {
        detector = m_intensity;
        break;
    }
        //        case SearchAlgorithm_AI:
        //        {
        ////            detector = m_AI_Detector;    ###
        //            break;
        //        }
        //        case SearchAlgorithm_ReferenceMatching:
        //        {
        ////            detector = m_ReferenceMatching;    ###
        //            setSearchRectangle(QPointF(0,0));
        //            break;
        //        }
    default:
    {
        detector = m_intensity;
        break;
    }
    }

    return detector;
}

void Detector::sltAutoLockChecking()
{
    //    if (m_isAutoLockActivated)
    //    {
    //        std::vector<IDetector::DetectionInfo> detectionsInfo = m_detector->getAllDetectedObjects();
    //        AutoLockInfo autoLockInfo;
    //        std::vector<AutoLockInfo> newAutoLockInfo;
    //        cv::Rect bbox;
    //        cv::Mat checkedDetections = cv::Mat::zeros(m_allAutoLockInfo.size(), 1, CV_8UC1);
    //        double minDistanceOfCenter = 10000;
    //        cv::Rect satisfiedAutoLockBBox = cv::Rect(0,0,0,0);

    //        for (auto it = detectionsInfo.begin(); it != detectionsInfo.end(); ++it) {
    //            bbox = it->bbox;

    //            if (m_allAutoLockInfo.size() > 0)
    //            {

    //                for (int i = 0; i < m_allAutoLockInfo.size(); ++i) {

    //                    if (checkedDetections.at<uchar>(i, 0) == 1) {
    //                        continue;
    //                    }

    //                    AutoLockInfo& info = m_allAutoLockInfo[i];

    //                    double distance = m_calculator->centerDistance(bbox, info.bbox);

    //                    if (distance < m_autoLockDistanceThreshold)
    //                    {
    //                        autoLockInfo.bbox = bbox;
    //                        autoLockInfo.numberOfDetected = info.numberOfDetected + 1;
    //                        newAutoLockInfo.push_back(autoLockInfo);
    //                        checkedDetections.at<uchar>(i, 0) = 1;

    //                        if (autoLockInfo.numberOfDetected >= m_autoLockNumberOfDetectedThreshold)
    //                        {
    //                            double distanceOfCenter = m_calculator->centerDistance(m_detectorSearchRect, bbox);
    //                            if (distanceOfCenter < minDistanceOfCenter)
    //                            {
    //                                minDistanceOfCenter = distanceOfCenter;
    //                                satisfiedAutoLockBBox = bbox;
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //            else
    //            {
    //                autoLockInfo.bbox = bbox;
    //                autoLockInfo.numberOfDetected = 1;
    //                newAutoLockInfo.push_back(autoLockInfo);
    //            }
    //        }

    //        if (satisfiedAutoLockBBox.width > 0 && satisfiedAutoLockBBox.height > 0)
    //        {
    //            if (isDetectorActivated())
    //            {
    //                m_allAutoLockInfo.clear();
    //                Q_EMIT sigAutoLockDetected(QRectF(satisfiedAutoLockBBox.x,
    //                                                  satisfiedAutoLockBBox.y,
    //                                                  satisfiedAutoLockBBox.width,
    //                                                  satisfiedAutoLockBBox.height));
    //            }
    //        }
    //        else if (!newAutoLockInfo.empty())
    //        {
    //            m_allAutoLockInfo.clear();
    //            m_allAutoLockInfo.assign(newAutoLockInfo.begin(), newAutoLockInfo.end());
    //        }
    //    }

    cv::Rect satisfiedAutoLockBBox = m_intensity->getDetectedBoundingBox();

    if ((!satisfiedAutoLockBBox.empty()) && (satisfiedAutoLockBBox.width > 0) && (satisfiedAutoLockBBox.height > 0))
    {
        if (m_isAutoLockActivated == true)
        {
            Q_EMIT sigAutoLockDetected(QRectF(satisfiedAutoLockBBox.x,
                                              satisfiedAutoLockBBox.y,
                                              satisfiedAutoLockBBox.width,
                                              satisfiedAutoLockBBox.height));
        }
    }
}
