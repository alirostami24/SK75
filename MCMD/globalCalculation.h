#ifndef CALCULATION_H
#define CALCULATION_H

#include <opencv2/opencv.hpp>

class Calculator
{
public:
    Calculator();

    ///
    /// \brief centerDistance
    /// Return distance between the centers of two ROIs.
    /// \param r1 and r2
    /// ROIs.
    /// \return
    /// Calculating the distance between the centers of two ROIs.
    double centerDistance(const cv::Rect& r1, const cv::Rect& r2);

    ///
    /// \brief centerDistance
    /// Return distance between the centers of two ROIs.
    /// \param rect
    /// ROIs.
    /// \param point
    /// Clicked position.
    /// \return
    /// Calculating the distance between the centers of two ROIs.
    double centerDistance(const cv::Rect& rect, const cv::Point2d& point);
};

#endif // CALCULATION_H
