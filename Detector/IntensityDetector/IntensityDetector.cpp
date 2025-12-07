#include "IntensityDetector.h"

IntensityDetector::IntensityDetector() :
    m_minimumLength(8)
{
    m_inputSize.width = 640;
    m_inputSize.height = 512;

    m_thDetector = new TH();

    m_isDetecorActivated = false;
    m_isDetectorInitialized = false;

    m_frame = cv::Mat(m_inputSize.height, m_inputSize.width, CV_8UC3);
    m_inputRect = cv::Rect(0, 0, m_inputSize.width, m_inputSize.height);
    m_detectorSearchRect = cv::Rect(0, 0, m_inputSize.width, m_inputSize.height);

    m_autoLockDistanceThreshold = 0.05 * std::min(m_inputSize.width, m_inputSize.height);
    m_autoLockNumberOfDetectedThreshold = 3;
}

IntensityDetector::~IntensityDetector()
{
    delete m_thDetector;
}

std::string IntensityDetector::getName() const
{
    return "IntensityDetector";
}

std::string IntensityDetector::getVersion() const
{
    return "v1.0.0";
}

bool IntensityDetector::detect(unsigned char *pData)
{
    bool isResutvalid = false;
    if(m_thDetector->isTHActivated() == true)
    {
        m_frame.data = pData;

//        auto start = std::chrono::high_resolution_clock::now();

//            // Process frame here
        if (m_thDetector->isTHInitialized() == false)
        {
            m_thDetector->init(m_inputRect, m_inputSize);
        }
         m_thDetector->run(m_frame);
//        auto end = std::chrono::high_resolution_clock::now();

//         // Duration in milliseconds
//         std::chrono::duration<double, std::milli> duration = end - start;
//         std::cout << "TH detection execution time: " << duration.count() << " ms" << std::endl;

        cv::Rect targetBBox = m_thDetector->getBoundingBox();

        if ((targetBBox.width > 0) && (targetBBox.height > 0))
        {
            IDetector::DetectionInfo result;
            result.ID = 1;
            result.bbox = targetBBox;

            m_vectorDetectionResult.clear();
            m_vectorDetectionResult.push_back(result);
        }

        isResutvalid = checkDetectValidity();
        if (isResutvalid)
        {
            Q_EMIT sigDetectionDataUpdated();
            Q_EMIT sigAutoLockingIsPossible();
        }
        return isResutvalid;
    }

    return isResutvalid;
}

cv::Rect2f IntensityDetector::getDetectedBoundingBox(const cv::Point2d &targetCenter) const
{
//    double minDistance = std::numeric_limits<double>::max();
//    cv::Rect2f closest;
//    for (const auto& detection : m_vectorDetectionResult)
//    {
//        // Compute the distance to targetCenter
//        cv::Rect bbox = detection.bbox;
//        double distance = m_calculator->centerDistance(bbox, targetCenter);
//        // Update the closest ROI if a closer one is found
//        if (distance < minDistance) {
//            minDistance = distance;
//            closest = bbox;
//        }
//    }
////    std::cerr << "closest: x: " << closest.x << " y: " << closest.y <<
////                 " w: " << closest.width << " h: " << closest.height << std::endl;
//    return closest;

    return m_validObjectRect;
}

std::vector<IDetector::DetectionInfo> IntensityDetector::getAllDetectedObjects() const
{
    return m_vectorDetectionResult;
}

void IntensityDetector::enableDetection(bool state)
{
    m_isDetecorActivated = state;
    if (state == false)
    {
        clearMemory();
        m_thDetector->enableTH(false);
        m_thDetector->reset();
    }
    else
    {
        m_thDetector->enableTH(true);
        m_thDetector->reset();
    }
}

bool IntensityDetector::isDetectionActivated()
{
    return m_isDetecorActivated;
}

void IntensityDetector::stopDetection()
{
    m_isDetecorActivated = false;
    m_vectorDetectionResult.clear();
    //m_detectorSearchRect = cv::Rect();
    m_thDetector->reset();
    m_thDetector->stopTH();
}

void IntensityDetector::clearMemory()
{
    m_vectorDetectionResult.clear();
    //m_detectorSearchRect = cv::Rect();
    m_thDetector->reset();
}

bool IntensityDetector::checkDetectValidity()
{
    if (m_thDetector->isTHActivated() == true)
    {
        std::vector<IDetector::DetectionInfo> detectionsInfo = getAllDetectedObjects();
        DetectionValidityInfo detectionValidityInfo;
        std::vector<DetectionValidityInfo> newDetectionValidityInfo;
        cv::Rect bbox;
        cv::Mat checkedDetections = cv::Mat::zeros(m_allDetectionValidityInfo.size(), 1, CV_8UC1);
        double minDistanceOfCenter = 10000;
        cv::Rect satisfiedAutoLockBBox = cv::Rect(0,0,0,0);

        for (auto it = detectionsInfo.begin(); it != detectionsInfo.end(); ++it) {
            bbox = it->bbox;

            if (m_allDetectionValidityInfo.size() > 0)
            {
                for (int i = 0; i < m_allDetectionValidityInfo.size(); ++i) {

                    if (checkedDetections.at<uchar>(i, 0) == 1) {
                        continue;
                    }

                    DetectionValidityInfo& info = m_allDetectionValidityInfo[i];

                    double distance = m_calculator->centerDistance(bbox, info.bbox);

                    if (distance < m_autoLockDistanceThreshold)
                    {
                        detectionValidityInfo.bbox = bbox;
                        detectionValidityInfo.numberOfDetected = info.numberOfDetected + 1;
                        newDetectionValidityInfo.push_back(detectionValidityInfo);
                        checkedDetections.at<uchar>(i, 0) = 1;

                        if (detectionValidityInfo.numberOfDetected >= m_autoLockNumberOfDetectedThreshold)
                        {
                            double distanceOfCenter = m_calculator->centerDistance(m_detectorSearchRect, bbox);
                            if (distanceOfCenter < minDistanceOfCenter)
                            {
                                minDistanceOfCenter = distanceOfCenter;
                                satisfiedAutoLockBBox = bbox;
                            }
                        }
                    }
                }
            }
            else
            {
                detectionValidityInfo.bbox = bbox;
                detectionValidityInfo.numberOfDetected = 1;
                newDetectionValidityInfo.push_back(detectionValidityInfo);
            }
        }

        if (satisfiedAutoLockBBox.width > 0 && satisfiedAutoLockBBox.height > 0)
        {
               m_allDetectionValidityInfo.clear();

               detectionValidityInfo.bbox = satisfiedAutoLockBBox;
               detectionValidityInfo.numberOfDetected = m_autoLockNumberOfDetectedThreshold + 1;
               m_allDetectionValidityInfo.push_back(detectionValidityInfo);

               m_validObjectRect.x = satisfiedAutoLockBBox.x;
               m_validObjectRect.y = satisfiedAutoLockBBox.y;
               m_validObjectRect.width = satisfiedAutoLockBBox.width;
               m_validObjectRect.height = satisfiedAutoLockBBox.height;

               return true;
        }
        else if (!newDetectionValidityInfo.empty())
        {
            m_allDetectionValidityInfo.clear();
            m_allDetectionValidityInfo.assign(newDetectionValidityInfo.begin(), newDetectionValidityInfo.end());
            m_validObjectRect = cv::Rect();
            return false;
        }
    }
}
