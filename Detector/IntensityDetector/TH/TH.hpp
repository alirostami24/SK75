#ifndef	TH_H
#define	TH_H

/************************************************************************/
/* Basic Includes                                                       */
/************************************************************************/
#include	<iostream>
#include	<cstdlib>
#include	<cstring>
#include	<vector>
#include	<algorithm>
/************************************************************************/
/* Includes for the OpenCV                                              */
/************************************************************************/
#include "opencv2/opencv.hpp"

#include <chrono>

class TH {

/************************************************************************/
/*  Internal Variables					                                */
/************************************************************************/
 public:

    cv::Mat m_maskTH;


/************************************************************************/
/*  Methods								                                */
/************************************************************************/
 public:

	 TH();
	~TH();

    void enableTH(bool state);
    void init(cv::Size inputSize);
    void run(cv::Mat input);
    cv::Mat getMask();
    cv::Rect getBoundingBox();
    void setInputSize(const cv::Size& inputSize);
    bool isTHActivated();
    bool isTHInitialized();
    void stopTH();
    void clearMemory();
    void reset();



 private:
    void topHatCentroid(const cv::Mat& input);
    cv::Rect resizeObjectWithOTSUMethod1(const cv::Size2d& targetSize, const cv::Point2d& targetCenter, unsigned char *pData);
    cv::Rect resizeObjectWithOTSUMethod2(const cv::Size2d& targetSize, const cv::Point2d& targetCenter, unsigned char *pData);
    cv::Point2d calculateCentroidBB(cv::Mat &frame);
    void adjustBoundingBox(unsigned char *pData);
    void setTargetSize(const cv::Size2d& targetSize);
    void setTargetCenter(const cv::Point2d& targetCenter);
    cv::Size2d getTargetSize();
    cv::Point2d getTargetCenter();
    cv::Rect getObjectSizeMethod1(const cv::Point2d& targetCenter, unsigned char *pData);
    cv::Rect getObjectSizeMethod2(const cv::Point2d& targetCenter, unsigned char *pData);


    bool m_isTHActivated;
    bool m_isTHInitialized;

    cv::Size m_inputSize;
    cv::Mat m_inputFrame;
    cv::Mat m_inputCroped;
    cv::Mat m_inputResized;

    cv::Rect m_tempTargetBBox;
    cv::Rect m_targetBBox;

    cv::Mat kernel_5;
    cv::Mat kernel_3;

    cv::Point2d m_targetCenteroid;

    float m_validMarginRatio;
    float m_tolerance;
};

#endif				//TH_H
