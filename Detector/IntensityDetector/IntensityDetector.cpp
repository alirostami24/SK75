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
    m_detectorSearchRect = cv::Rect(0, 0, m_inputSize.width, m_inputSize.height);

    m_maxValidDistance = 0.05 * std::min(m_inputSize.width, m_inputSize.height);
    m_numberOfDetectedThreshold = 3;

    // Target position Estimation
    m_isTargetPositionEstimationEnabled = false;
    m_targetEstimationMemory.resize(3);
    m_targetEstimationMemoryIndex = 0;
    m_lastEstimatedTargetPosition = cv::Point(0, 0);
    m_distanceTolerance = 3;
}

IntensityDetector::~IntensityDetector()
{
    delete m_thDetector;
}

void IntensityDetector::initialize()
{
	m_isDetecorActivated = false;
	m_isDetectorInitialized = false;

	m_frame = cv::Mat(m_inputSize.height, m_inputSize.width, CV_8UC3);
	m_detectorSearchRect = cv::Rect(0, 0, m_inputSize.width, m_inputSize.height);

	m_maxValidDistance = 0.05 * std::min(m_inputSize.width, m_inputSize.height);
	m_numberOfDetectedThreshold = 3;

	// Target position Estimation
	m_isTargetPositionEstimationEnabled = false;
	m_targetEstimationMemory.resize(3);
	m_targetEstimationMemoryIndex = 0;
	m_lastEstimatedTargetPosition = cv::Point(0, 0);
	m_distanceTolerance = 3;
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
            m_thDetector->init(m_inputSize);
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
			isResutvalid = checkDetectValidity();
        }
		else
		{
			m_allDetectionValidityInfo.clear();
			m_validObjectRect = cv::Rect(-1, -1, -1, -1);
			isResutvalid = false;
		}

        
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

void IntensityDetector::setInputSize(const cv::Size &inputSize)
{
    m_inputSize = inputSize;
    m_thDetector->setInputSize(inputSize);
    m_thDetector->reset();
}

void IntensityDetector::enableDetection(bool state)
{
    m_isDetecorActivated = state;
    if (state == false)
    {
        clearMemory();
        m_thDetector->enableTH(false);
        m_thDetector->clearMemory();
    }
    else
    {
        m_thDetector->enableTH(true);
        m_thDetector->clearMemory();
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
    m_thDetector->clearMemory();
    m_thDetector->stopTH();
}

void IntensityDetector::clearMemory()
{
    m_vectorDetectionResult.clear();
    //m_detectorSearchRect = cv::Rect();
    m_thDetector->clearMemory();
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
                cv::Point2d posEstimation;
                posEstimation.x = std::numeric_limits<double>::quiet_NaN();
                posEstimation.y = std::numeric_limits<double>::quiet_NaN();

                if (m_allDetectionValidityInfo.size() == 1)
                {
                    if ((m_isTargetPositionEstimationEnabled) && (m_targetEstimationMemoryIndex >= m_numberOfDetectedThreshold))
                    {
                            // Update the estimation for next frame
                            Translation meanTranslation;
                            meanTranslation = {0, 0};
                            for (size_t i = 0; i < m_targetEstimationMemory.size(); ++i) {
                                meanTranslation.horizontal += m_targetEstimationMemory.at(i).horizontal;
                                meanTranslation.vertical   += m_targetEstimationMemory.at(i).vertical;
                            }

                            posEstimation.x = std::abs(meanTranslation.horizontal / m_targetEstimationMemory.size());
                            posEstimation.y = std::abs(meanTranslation.vertical / m_targetEstimationMemory.size());
                    }
                }
                for (int i = 0; i < m_allDetectionValidityInfo.size(); ++i) {

                    if (checkedDetections.at<uchar>(i, 0) == 1) {
                        continue;
                    }

                    DetectionValidityInfo& info = m_allDetectionValidityInfo[i];

                    double distance = m_calculator->centerDistance(bbox, info.bbox);

                    if ((m_isTargetPositionEstimationEnabled) && (isValidPoint(posEstimation)))
                    {
                        double maxValidDistance = std::max(posEstimation.x, posEstimation.y);
                        if (distance < (maxValidDistance * m_distanceTolerance))
                        {
                            detectionValidityInfo.bbox = bbox;
                            detectionValidityInfo.numberOfDetected = info.numberOfDetected + 1;
                            newDetectionValidityInfo.push_back(detectionValidityInfo);
                            checkedDetections.at<uchar>(i, 0) = 1;

                            if (detectionValidityInfo.numberOfDetected >= m_numberOfDetectedThreshold)
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
                    else if (distance < m_maxValidDistance)
                    {
                        detectionValidityInfo.bbox = bbox;
                        detectionValidityInfo.numberOfDetected = info.numberOfDetected + 1;
                        newDetectionValidityInfo.push_back(detectionValidityInfo);
                        checkedDetections.at<uchar>(i, 0) = 1;

                        if (detectionValidityInfo.numberOfDetected >= m_numberOfDetectedThreshold)
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
               detectionValidityInfo.numberOfDetected = m_numberOfDetectedThreshold + 1;
               m_allDetectionValidityInfo.push_back(detectionValidityInfo);

               m_validObjectRect.x = satisfiedAutoLockBBox.x;
               m_validObjectRect.y = satisfiedAutoLockBBox.y;
               m_validObjectRect.width = satisfiedAutoLockBBox.width;
               m_validObjectRect.height = satisfiedAutoLockBBox.height;

               if (m_isTargetPositionEstimationEnabled) {

                   m_targetEstimationMemory.at(m_targetEstimationMemoryIndex % m_targetEstimationMemory.size()).horizontal
                           = satisfiedAutoLockBBox.x - m_lastEstimatedTargetPosition.x;
                   m_targetEstimationMemory.at(m_targetEstimationMemoryIndex % m_targetEstimationMemory.size()).vertical
                           = satisfiedAutoLockBBox.y - m_lastEstimatedTargetPosition.y;

                   m_lastEstimatedTargetPosition = cv::Point(satisfiedAutoLockBBox.x, satisfiedAutoLockBBox.y);

                   m_targetEstimationMemoryIndex++;
               }

               return true;
        }
        else if (!newDetectionValidityInfo.empty())
        {
            m_allDetectionValidityInfo.clear();
            m_allDetectionValidityInfo.assign(newDetectionValidityInfo.begin(), newDetectionValidityInfo.end());
            m_validObjectRect = cv::Rect(-1, -1, -1, -1);
            if (m_isTargetPositionEstimationEnabled) {

                if (newDetectionValidityInfo.size() == 1)
                {
                    if ((m_lastEstimatedTargetPosition.x > 0) && (m_lastEstimatedTargetPosition.y > 0))
                    {
                        m_targetEstimationMemory.at(m_targetEstimationMemoryIndex % m_targetEstimationMemory.size()).horizontal
                            = newDetectionValidityInfo[0].bbox.x - m_lastEstimatedTargetPosition.x;
                        m_targetEstimationMemory.at(m_targetEstimationMemoryIndex % m_targetEstimationMemory.size()).vertical
                            = newDetectionValidityInfo[0].bbox.y - m_lastEstimatedTargetPosition.y;

                        m_targetEstimationMemoryIndex++;
                    }

                    m_lastEstimatedTargetPosition = cv::Point(newDetectionValidityInfo[0].bbox.x, newDetectionValidityInfo[0].bbox.y);

                }
                else
                {
                    m_targetEstimationMemory.clear();
                    m_targetEstimationMemoryIndex = 0;
                    m_lastEstimatedTargetPosition = cv::Point(0, 0);
                }
            }
            return false;
        }
		else
		{
			m_allDetectionValidityInfo.clear();
			m_validObjectRect = cv::Rect(-1, -1, -1, -1);
			return false;
		}
    }
}

bool IntensityDetector::isValidPoint(const cv::Point2d &point)
{
      return !std::isnan(point.x) && !std::isnan(point.y) && std::isfinite(point.x) && std::isfinite(point.y);
}
