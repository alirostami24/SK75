#pragma once

#include "opencv2/opencv.hpp"


#include "MCMD/MCDWrapper.hpp"
#include "globalCalculation.h"


class MCMD_Controler
{
public:
    MCMD_Controler();
    void init(cv::Rect inputRect, cv::Size inputSize);
    void runMCMD(const cv::Mat& input);
    std::vector<cv::Rect> getBoundingBoxes();
    cv::Rect getBoundingBox();

private:
    std::vector<cv::Rect> extractBoundingBoxes(const cv::Mat& m_mask, const cv::Rect inputRect, const cv::Size inputSize);
    cv::Rect extractNearestBoundingBox();

    Calculator *m_calculator;
    MCDWrapper *m_mcdwrapper;
    cv::Size m_minValidObjectSize;
    cv::Size m_maxValidObjectSize;
    cv::Mat kernelOpening;
    cv::Mat kernelClosing;

    cv::Rect m_inputRect;
    cv::Size m_inputSize;
    cv::Mat m_inputFrame;
    cv::Mat m_inputCroped;
    cv::Mat m_inputResized;
    bool m_isInitialized;

    cv::Rect m_targetBBox;
    std::vector<cv::Rect> m_allTargetsBBox;


};
