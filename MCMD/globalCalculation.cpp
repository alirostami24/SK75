#include "globalCalculation.h"

Calculator::Calculator()
{

}

double Calculator::centerDistance(const cv::Rect &r1, const cv::Rect &r2)
{
    cv::Point2d center1(r1.x + r1.width / 2.0, r1.y + r1.height / 2.0);
    cv::Point2d center2(r2.x + r2.width / 2.0, r2.y + r2.height / 2.0);

    double dx = center2.x - center1.x;
    double dy = center2.y - center1.y;

    return std::sqrt(dx * dx + dy * dy);
}

double Calculator::centerDistance(const cv::Rect &rect, const cv::Point2d &point)
{
    cv::Point2d center1(rect.x + rect.width / 2.0, rect.y + rect.height / 2.0);

    double dx = point.x - center1.x;
    double dy = point.y - center1.y;

    return std::sqrt(dx * dx + dy * dy);
}
