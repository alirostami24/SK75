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

#ifndef	_MCDWRAPPER_CPP_
#define	_MCDWRAPPER_CPP_

#include <ctime>
#include <cstring>
#include "MCDWrapper.hpp"
#include "params.hpp"

#if defined _WIN32 || defined _WIN64
int gettimeofday(struct timeval *tp, int *tz)
{
//	LARGE_INTEGER tickNow;
//	static LARGE_INTEGER tickFrequency;
//	static BOOL tickFrequencySet = FALSE;
//	if (tickFrequencySet == FALSE) {
//		QueryPerformanceFrequency(&tickFrequency);
//		tickFrequencySet = TRUE;
//	}
//	QueryPerformanceCounter(&tickNow);
//	tp->tv_sec = (long)(tickNow.QuadPart / tickFrequency.QuadPart);
//	tp->tv_usec = (long)(((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);

    return 0;
}
#else
#include <sys/time.h>
#endif

MCDWrapper::MCDWrapper()
{
    imgIplTemp = NULL;
    imgGray = NULL;
    detect_img = NULL;
    imgIpl = NULL;
}

MCDWrapper::~MCDWrapper()
{
    cvReleaseImage(&imgIplTemp);
    cvReleaseImage(&imgGray);
    cvReleaseImage(&detect_img);
    cvReleaseImage(&imgIpl);
}

void
 MCDWrapper::Init(IplImage * in_imgIpl)
{
    frm_cnt = 0;
    cvReleaseImage(&imgIplTemp);
    cvReleaseImage(&imgGray);
    cvReleaseImage(&detect_img);
    cvReleaseImage(&imgIpl);

	// Allocate
	imgIplTemp = cvCreateImage(cvSize(in_imgIpl->width, in_imgIpl->height), IPL_DEPTH_8U, 1);
    imgGray = cvCreateImage(cvSize(in_imgIpl->width, in_imgIpl->height), IPL_DEPTH_8U, 1);
	detect_img = cvCreateImage(cvSize(in_imgIpl->width, in_imgIpl->height), IPL_DEPTH_8U, 1);
    imgIpl = cvCreateImage(cvSize(in_imgIpl->width, in_imgIpl->height), in_imgIpl->depth, in_imgIpl->nChannels);

    cvCopy(in_imgIpl, imgIpl);

	// Smoothing using median filter
    //cvCvtColor(imgIpl, imgIplTemp, CV_RGB2GRAY);
	cvSmooth(imgIplTemp, imgGray, CV_MEDIAN, 5);

	m_LucasKanade.Init(imgGray);
	BGModel.init(imgGray);

//	cvCopy(imgGray, imgGrayPrev);
}

void MCDWrapper::Run(IplImage * in_imgIpl)
{
	//auto _start = std::chrono::system_clock::now();
    if (imgIpl == NULL) Init(in_imgIpl);

    cvCopy(in_imgIpl, imgIpl);
    frm_cnt++;

    cvCvtColor(imgIpl, imgIplTemp, CV_RGB2GRAY);

    cvSmooth(imgIplTemp, imgGray, CV_MEDIAN, 5);

	/*auto _end = std::chrono::system_clock::now();
	std::chrono::duration<double> differenceTime = _end - _start;
	double elapsedTime = differenceTime.count() * 1000;
	std::cerr << "MCDWrapper::Run part 1 time: " << elapsedTime << std::endl;*/
    // Get H
	//_start = std::chrono::system_clock::now();

    double h[9];
    m_LucasKanade.RunTrack(imgGray, 0);
	//m_LucasKanade.estimateAffine_CPU_ORB(imgGray, 0);

	/*_end = std::chrono::system_clock::now();
	differenceTime = _end - _start;
	elapsedTime = differenceTime.count() * 1000;
	std::cerr << "MCDWrapper::Run part 2 time: " << elapsedTime << std::endl;*/

	//_start = std::chrono::system_clock::now();

    m_LucasKanade.GetHomography(h);

	/*_end = std::chrono::system_clock::now();
	differenceTime = _end - _start;
	elapsedTime = differenceTime.count() * 1000;
	std::cerr << "MCDWrapper::Run part 3 time: " << elapsedTime << std::endl;*/

	//_start = std::chrono::system_clock::now();

    BGModel.motionCompensate(h);

	/*_end = std::chrono::system_clock::now();
	differenceTime = _end - _start;
	elapsedTime = differenceTime.count() * 1000;
	std::cerr << "MCDWrapper::Run part 4 time: " << elapsedTime << std::endl;*/
	
	//_start = std::chrono::system_clock::now();

    BGModel.update(detect_img);

	/*_end = std::chrono::system_clock::now();
	differenceTime = _end - _start;
	elapsedTime = differenceTime.count() * 1000;
	std::cerr << "MCDWrapper::Run part 5 time: " << elapsedTime << std::endl;*/
}

#endif				// _MCDWRAPPER_CPP_
