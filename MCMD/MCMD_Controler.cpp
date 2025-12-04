#include "MCMD_Controler.h"

MCMD_Controler::MCMD_Controler()
{
    m_mcdwrapper = new MCDWrapper();
    m_isInitialized = false;
}

void MCMD_Controler::init(cv::Rect inputRect, cv::Size inputSize)
{
    m_inputRect = inputRect;
    m_inputSize = inputSize;

    m_minValidObjectSize = cv::Size(std::min(7.0, 0.1 * inputSize.width), std::min(7.0, 0.1 * inputSize.height));
    //m_maxValidObjectSize = cv::Size(0.1 * inputSize.width, 0.1 * inputSize.height);
    m_maxValidObjectSize = cv::Size(std::min(150.0, 0.7 * inputSize.width), std::min(150.0, 0.7 * inputSize.height));
    kernelOpening = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(std::min(5.0, inputSize.height * 0.01), std::min(5.0, inputSize.height * 0.01)));
    kernelClosing = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(std::min(5.0, inputSize.height * 0.015), std::min(5.0, inputSize.height * 0.015)));
    m_isInitialized = true;
}

void MCMD_Controler::runMCMD(const cv::Mat& input)
{
    m_inputFrame = input.clone();
    m_inputCroped = m_inputFrame(m_inputRect);
    cv::resize(m_inputCroped, m_inputResized, m_inputSize);
    IplImage m_inImage = cvIplImage(m_inputResized);

    if (m_isInitialized == false)
    {
        init(cv::Rect(0, 0, input.cols, input.rows), cv::Size(input.cols, input.rows));
    }

    m_mcdwrapper->Run(&m_inImage);

    //mask = cv::cvarrToMat(mcdwrapper->detect_img); // debugging

    //// Returns all bounding boxes found.
    m_allTargetsBBox = extractBoundingBoxes(cv::cvarrToMat(m_mcdwrapper->detect_img), m_inputRect, m_inputSize);

    //// Returns only closest to the center bounding box found.
    m_targetBBox = extractNearestBoundingBox();
}

std::vector<cv::Rect> MCMD_Controler::getBoundingBoxes()
{
    return m_allTargetsBBox;
}

cv::Rect MCMD_Controler::getBoundingBox()
{
    return m_targetBBox;
}

std::vector<cv::Rect> MCMD_Controler::extractBoundingBoxes(const cv::Mat &m_mask, const cv::Rect inputRect, const cv::Size inputSize)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Rect> boundingBoxes;

    float removedAreaPercentage = 0.15;

    // Ensure mask is binary
    cv::Mat binaryMask;
    cv::threshold(m_mask, binaryMask, 0, 255, cv::THRESH_BINARY);

    cv::Mat opening, closing;
    cv::morphologyEx(binaryMask, opening, cv::MORPH_OPEN, kernelOpening);
    cv::morphologyEx(opening, closing, cv::MORPH_CLOSE, kernelClosing);

    double widthScale = static_cast<double>(inputRect.width) / static_cast<double>(m_mask.cols);
    double heightScale = static_cast<double>(inputRect.height) / static_cast<double>(m_mask.rows);

    // Find contours
    cv::findContours(closing, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    //cv::Mat drawing = cv::Mat::zeros(m_mask.size(), CV_8UC3);
    for (const auto& contour : contours) {
        cv::Rect rect = cv::boundingRect(contour);  // cv::Rect (int)
        std::vector<std::vector<cv::Point>> temp{ contour };
        //cv::drawContours(drawing, temp, -1, cv::Scalar(0, 255, 0), 2);
        if ((rect.width >= m_minValidObjectSize.width) &&
            (rect.height >= m_minValidObjectSize.height) &&
            (rect.width <= m_maxValidObjectSize.width) &&
            (rect.height <= m_maxValidObjectSize.height))
        {
            if ((rect.x >= removedAreaPercentage * inputSize.width) &&
                (rect.y >= removedAreaPercentage * inputSize.height) &&
                (rect.x + rect.width <= (1.0 - removedAreaPercentage) * inputSize.width) &&
                (rect.y + rect.height <= (1.0 - removedAreaPercentage) * inputSize.height))
            {
                rect.x = widthScale * rect.x + inputRect.x;
                rect.y = heightScale * rect.y + inputRect.y;
                rect.width *= widthScale;
                rect.height *= heightScale;
                boundingBoxes.push_back(rect);
            }

        }
    }

    return boundingBoxes;
}

cv::Rect MCMD_Controler::extractNearestBoundingBox()
{
    cv::Rect boundingBox;
    cv::Point inputCenter(m_inputFrame.cols / 2, m_inputFrame.rows / 2);
    double minDistance = std::numeric_limits<double>::max();
    for (size_t i = 0; i < m_allTargetsBBox.size(); ++i)
    {
        // Access each rectangle
        cv::Rect& rect = m_allTargetsBBox[i];
        double distance = m_calculator->centerDistance(rect, inputCenter);
        if (distance < minDistance) {
                minDistance = distance;
                boundingBox = rect;
        }
    }

    return boundingBox;
}

