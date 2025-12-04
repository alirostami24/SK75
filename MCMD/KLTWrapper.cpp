// Copyright (c) 2016 Kwang Moo Yi.
// All rights reserved.

// This  software  is  strictly   for  non-commercial  use  only.  For
// commercial       use,       please        contact       me       at
// kwang.m<dot>yi<AT>gmail<dot>com.   Also,  when  used  for  academic
// purposes, please cite  the paper "Detection of  Moving Objects with
// Non-stationary Cameras in 5.8ms:  Bringing Motion Detection to Your
// Mobile Device,"  Yi et  al, CVPRW 2013  Redistribution and  use for
// non-commercial purposes  in source  and binary forms  are permitted
// provided that  the above  copyright notice  and this  paragraph are
// duplicated  in   all  such   forms  and  that   any  documentation,
// advertising  materials,   and  other  materials  related   to  such
// distribution and use acknowledge that the software was developed by
// the  Perception and  Intelligence Lab,  Seoul National  University.
// The name of the Perception  and Intelligence Lab and Seoul National
// University may not  be used to endorse or  promote products derived
// from this software without specific prior written permission.  THIS
// SOFTWARE IS PROVIDED ``AS IS''  AND WITHOUT ANY WARRANTIES.  USE AT
// YOUR OWN RISK!

#include "KLTWrapper.hpp"

#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "iostream"
//#include <QtGlobal>


KLTWrapper::KLTWrapper(void)
{
	// For LK funciton in opencv
	win_size = 10;
	points[0] = points[1] = 0;
	status = 0;
	count = 0;
	flags = 0;

	eig = NULL;
	temp = NULL;
	maskimg = NULL;
}

KLTWrapper::~KLTWrapper(void)
{
	cvReleaseImage(&eig);
	cvReleaseImage(&temp);
	cvReleaseImage(&maskimg);
}

void KLTWrapper::Init(IplImage * imgGray)
{
	int ni = imgGray->width;
	int nj = imgGray->height;

	// Allocate Maximum possible + some more for safety
	MAX_COUNT = (float (ni) / float (GRID_SIZE_W) + 1.0)*(float (nj) / float (GRID_SIZE_H) + 1.0);

	// Pre-allocate
    //image = cvCreateImage(cvGetSize(imgGray), 8, 3);
	imgPrevGray = cvCreateImage(cvGetSize(imgGray), 8, 1);
	pyramid = cvCreateImage(cvGetSize(imgGray), 8, 1);
	prev_pyramid = cvCreateImage(cvGetSize(imgGray), 8, 1);
	points[0] = (CvPoint2D32f *) cvAlloc(MAX_COUNT * sizeof(points[0][0]));
	points[1] = (CvPoint2D32f *) cvAlloc(MAX_COUNT * sizeof(points[0][0]));
	status = (char *)cvAlloc(MAX_COUNT);
	flags = 0;

	if (eig != NULL) {
		cvReleaseImage(&eig);
		cvReleaseImage(&temp);
		cvReleaseImage(&maskimg);
	}

	eig = cvCreateImage(cvGetSize(imgGray), 32, 1);
	temp = cvCreateImage(cvGetSize(imgGray), 32, 1);
	maskimg = cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_8U, 1);

	// Gen mask
	BYTE *pMask = (BYTE *) maskimg->imageData;
	int widthStep = maskimg->widthStep;
	for (int j = 0; j < nj; ++j) {
		for (int i = 0; i < ni; ++i) {
			pMask[i + j * widthStep] = (i >= ni / 5) && (i <= ni * 4 / 5) && (j >= nj / 5) && (j <= nj * 4 / 5) ? (BYTE) 255 : (BYTE) 255;
		}
	}

	// Init homography
	for (int i = 0; i < 9; i++)
		matH[i] = i / 3 == i % 3 ? 1 : 0;

    count = 0;

	orb = cv::ORB::create(
		MAX_COUNT,       // nFeatures
		1.2f,       // scaleFactor
		8,          // nLevels
		31,         // edgeThreshold
		0,          // firstLevel
		2,          // WTA_K
		cv::ORB::HARRIS_SCORE,
		31,         // patchSize
		20          // fastThreshold
	);

	matcher = cv::BFMatcher(cv::NORM_HAMMING, /*crossCheck=*/false);

}

void KLTWrapper::InitFeatures(IplImage * imgGray)
{
	/* automatic initialization */
	double quality = 0.01;
	double min_distance = 10;

	int ni = imgGray->width;
	int nj = imgGray->height;

	count = ni / GRID_SIZE_W * nj / GRID_SIZE_H;

	int cnt = 0;
	for (int i = 0; i < ni / GRID_SIZE_W - 1; ++i) {
		for (int j = 0; j < nj / GRID_SIZE_H - 1; ++j) {
			points[1][cnt].x = i * GRID_SIZE_W + GRID_SIZE_W / 2;
			points[1][cnt++].y = j * GRID_SIZE_H + GRID_SIZE_H / 2;
		}
	}

	SwapData(imgGray);
}
///
/// \brief KLTWrapper::RunTrack
///
/// \param imgGray
///
/// \param prevGray
///
///
void KLTWrapper::RunTrack(IplImage * imgGray, IplImage * prevGray)
{

	int i, k;
    int *nMatch = new int[MAX_COUNT];

	if (prevGray == 0) {
		prevGray = imgPrevGray;
	} else {
		flags = 0;
	}
	//auto _start = std::chrono::system_clock::now();
    //memset(image->imageData, 0, image->imageSize);
	if (count > 0) {
        cvCalcOpticalFlowPyrLK(prevGray, imgGray, prev_pyramid, pyramid,
                       points[0], points[1], count, cvSize(win_size, win_size), 3, status, 0, cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03), flags);
		flags |= CV_LKFLOW_PYR_A_READY;
        for (i = k = 0; i < std::min(count, MAX_COUNT); i++) {
			if (!status[i]) {
				continue;
			}

			nMatch[k++] = i;
		}
		count = k;
	}

	/*auto _end = std::chrono::system_clock::now();
	std::chrono::duration<double> differenceTime = _end - _start;
	double elapsedTime = differenceTime.count() * 1000;
	std::cerr << "Part 1 RunTrack time: " << elapsedTime << " ms" << std::endl;*/
	//auto _start = std::chrono::system_clock::now();

	MakeAffineTransform();
	
 //   if (count >= 10) {
	//	// Make homography matrix with correspondences
	//	//std::cerr << "nMatch size: " << sizeof(nMatch) << " and count: " << count << std::endl;
	//	MakeHomoGraphy(nMatch, count);
	//} else {
	//	for (int ii = 0; ii < 9; ++ii) {
	//		matH[ii] = ii % 3 == ii / 3 ? 1.0f : 0.0f;
	//	}
	//}
	//auto _end = std::chrono::system_clock::now();
	//std::chrono::duration<double> differenceTime = _end - _start;
	//double elapsedTime = differenceTime.count() * 1000;
	//std::cerr << "Part 2 RunTrack time: " << elapsedTime << " ms" << std::endl;

	//_start = std::chrono::system_clock::now();
	InitFeatures(imgGray);
    delete [] nMatch;

	/*_end = std::chrono::system_clock::now();
	differenceTime = _end - _start;
	elapsedTime = differenceTime.count() * 1000;
	std::cerr << "Part 3 RunTrack time: " << elapsedTime << " ms" << std::endl;*/
}

bool KLTWrapper::estimateAffine_CPU_ORB(IplImage * imgGray, IplImage * prevGray)
{
	if (prevGray == 0) {
		prevGray = imgPrevGray;
	}

	if (!imgGray || !prevGray)
	{ 
		setIdentity3x3();
		cv::Mat matPrev = cv::cvarrToMat(prevGray);
		initORBFeatures(&matPrev);
		cvCopy(imgGray, imgPrevGray);
		return false; 
	}

	// Convert to cv::Mat
    cv::Mat matCurr = cv::cvarrToMat(imgGray);
	orb->detectAndCompute(matCurr, cv::noArray(), kptsCurr, descCurr);

	if (kptsPrev.size() < 10 || kptsCurr.size() < 10 ||
		descPrev.empty() || descCurr.empty()) {
		setIdentity3x3();
		return false;
	}

	// 2) Match with BFMatcher (Hamming), KNN + Lowe ratio
	matcher.knnMatch(descPrev, descCurr, knn, 2);

	std::vector<cv::Point2f> ptsPrev, ptsCurr;
	ptsPrev.reserve(knn.size());
	ptsCurr.reserve(knn.size());

	const float ratio = 0.75f;
	for (const auto& v : knn) {
		if (v.size() < 2) continue;
		const cv::DMatch& m1 = v[0];
		const cv::DMatch& m2 = v[1];
		if (m1.distance < ratio * m2.distance) {
			ptsPrev.push_back(kptsPrev[m1.queryIdx].pt);
			ptsCurr.push_back(kptsCurr[m1.trainIdx].pt);
		}
	}

	// Copy keypoints (deep copy of vector)
	kptsPrev = kptsCurr;

	// Copy descriptors (deep copy of matrix)
	descPrev = descCurr.clone();

	if (ptsPrev.size() < 3 || ptsCurr.size() < 3) { // affine needs ≥3
		setIdentity3x3();
		return false;
	}

	// Estimate affine transform (2x3)
	cv::Mat affine = estimateAffine2D(ptsCurr, ptsPrev, cv::noArray(), cv::RANSAC);

	if (!affine.empty()) {
		// Convert affine 2x3 to 3x3 homography-like matrix
		cv::Mat H = cv::Mat::eye(3, 3, CV_64F);
		affine.copyTo(H(cv::Rect(0, 0, 3, 2)));

		// Copy to output array
		for (int i = 0; i < 9; i++) {
			matH[i] = H.at<double>(i / 3, i % 3);
		}
	}
	else {
		setIdentity3x3();
		return false;
	}
	return true;
}

void KLTWrapper::initORBFeatures(cv::Mat *imgGray)
{
	orb->detectAndCompute(*imgGray, cv::noArray(), kptsPrev, descPrev);
}

void KLTWrapper::SwapData(IplImage * imgGray)
{
    cvCopy(imgGray, imgPrevGray);
	CV_SWAP(prev_pyramid, pyramid, swap_temp);
	CV_SWAP(points[0], points[1], swap_points);
}

void KLTWrapper::GetHomography(double *pmatH)
{
	memcpy(pmatH, matH, sizeof(matH));
}

void KLTWrapper::MakeHomoGraphy(int *pnMatch, int nCnt)
{

//#if defined (Q_OS_LINUX) || defined (Q_OS_WIN32)
	double h[9];
	CvMat _h = cvMat(3, 3, CV_64F, h);
	std::vector < CvPoint2D32f > pt1, pt2;
	CvMat _pt1, _pt2;
	int i;

	pt1.resize(nCnt);
	pt2.resize(nCnt);
	for (i = 0; i < nCnt; i++) {
		//REVERSE HOMOGRAPHY
		pt1[i] = points[1][pnMatch[i]];
		pt2[i] = points[0][pnMatch[i]];
	}

	_pt1 = cvMat(1, nCnt, CV_32FC2, &pt1[0]);
	_pt2 = cvMat(1, nCnt, CV_32FC2, &pt2[0]);
    if (!cvFindHomography(&_pt1, &_pt2, &_h, CV_RANSAC, 1))
//      if(!cvFindHomography( &_pt1, &_pt2, &_h, CV_LMEDS, 1))
	{
		return;
	}

	for (i = 0; i < 9; i++) {
		matH[i] = h[i];
    }
//#endif
}

void KLTWrapper::MakeAffineTransform()
{
	//    // Convert to cv::Mat
//    cv::Mat matPrev = cv::cvarrToMat(prevGray);
//    cv::Mat matCurr = cv::cvarrToMat(imgGray);

	if (count < 3) {
		for (int i = 0; i < 9; i++)
			matH[i] = (i % 3 == i / 3) ? 1.0 : 0.0;
		return;
	}

	// Convert to vector<Point2f>
	std::vector<cv::Point2f> prevPts(count), nextPts(count);
	for (int i = 0; i < count; i++) {
		prevPts[i] = cv::Point2f(points[0][i].x, points[0][i].y);
		nextPts[i] = cv::Point2f(points[1][i].x, points[1][i].y);
	}

	// Estimate affine transform (2x3)
	cv::Mat affine = estimateAffine2D(nextPts, prevPts, cv::noArray(), cv::RANSAC);

	if (!affine.empty()) {
		// Convert affine 2x3 to 3x3 homography-like matrix
		cv::Mat H = cv::Mat::eye(3, 3, CV_64F);
		affine.copyTo(H(cv::Rect(0, 0, 3, 2)));

		// Copy to output array
		for (int i = 0; i < 9; i++) {
			matH[i] = H.at<double>(i / 3, i % 3);
		}
	}
	else {
		// Fallback to identity if estimation failed
		for (int i = 0; i < 9; i++)
			matH[i] = (i % 3 == i / 3) ? 1.0 : 0.0;
	}
}

void KLTWrapper::setIdentity3x3()
{
	for (int i = 0; i < 9; ++i) matH[i] = (i % 3 == i / 3) ? 1.0 : 0.0;
}
