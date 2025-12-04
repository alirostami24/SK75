#include <QApplication>
#include "opencv2/opencv.hpp"
#include <iostream>

#include "MCMD/MCDWrapper.hpp"

using namespace std;
using namespace cv;

std::vector<cv::Rect> extractBoundingBoxes(const cv::Mat& m_mask, const cv::Rect inputRect, const cv::Size inputSize);
cv::Size m_minValidObjectSize;
cv::Size m_maxValidObjectSize;
cv::Mat kernelOpening;
cv::Mat kernelClosing;

// Using time point and system_clock
std::chrono::time_point<std::chrono::system_clock> _start;
std::chrono::time_point<std::chrono::system_clock> _end;
double _maxElapsedTime = -1;
double _minElapsedTime = 1000;
void tic()
{
    _start = std::chrono::system_clock::now();
}

void toc()
{
    _end = std::chrono::system_clock::now();
    std::chrono::duration<double> differenceTime = _end - _start;
    double elapsedTime = differenceTime.count() * 1000;

    if (_maxElapsedTime < elapsedTime)
        _maxElapsedTime = elapsedTime;

    if (_minElapsedTime > elapsedTime)
        _minElapsedTime = elapsedTime;

    std::cerr << "elapsed time: " << elapsedTime << ", min: " << _minElapsedTime << ", max: " << _maxElapsedTime << "ms\n";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create a VideoCapture object and use camera to capture the video
    cv::VideoCapture cap("D:/T1Sanat/DataSet/Motion/18.mp4");

    // Check if camera opened successfully
        if (!cap.isOpened()) {
            cout << "Error opening video stream" << endl;
            return -1;
        }

        // Default resolutions of the frame are obtained.The default resolutions are system dependent.
        int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
        int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

        // Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file.
        //VideoWriter video("outcpp.avi", cv::VideoWriter::fourcc('M','J','P','G'), 10, Size(frame_width,frame_height));

        std::cout << "W: " << frame_width << ", " << frame_height << std::endl;

        cv::namedWindow("ProcessedFrame", cv::WINDOW_NORMAL);

        cv::Mat mask;

        Mat frame;
        cap >> frame;
        MCDWrapper *mcdwrapper = new MCDWrapper();
        int frameRate = cap.get(CAP_PROP_FPS);
        VideoWriter writeOutput("D:/T1Sanat/PT/MCMD_QT/MCMD_Result.mp4", CV_FOURCC('M', 'P', '4', 'V'), frameRate, Size(frame.cols, frame.rows));
        int nframe = 0;
        cv::Rect inputRect(480, 156, 960, 770);
        cv::Size inputSize(640, 480);
		cv::Mat inputFrame;
        while (1) {


            // Capture frame-by-frame
            cap >> frame;

            // If the frame is empty, break immediately
            if (frame.empty())
                break;

            nframe += 1;

			inputFrame = frame(inputRect);
			cv::resize(inputFrame, inputFrame, inputSize);
            IplImage m_inImage = cvIplImage(inputFrame);

            tic();
            if (nframe == 1)
            {
                mcdwrapper->Init(&m_inImage);
                m_minValidObjectSize = cv::Size(std::min(7.0, 0.1 * inputFrame.cols), std::min(7.0, 0.1 * inputFrame.rows));
                m_maxValidObjectSize = cv::Size(0.1 * inputFrame.cols, 0.1 * inputFrame.rows);
                kernelOpening = cv::getStructuringElement(cv::MORPH_RECT, Size(std::min(5.0, inputFrame.rows * 0.01), std::min(5.0, inputFrame.rows * 0.01)));
                kernelClosing = cv::getStructuringElement(cv::MORPH_RECT, Size(std::min(5.0, inputFrame.rows * 0.015), std::min(5.0, inputFrame.rows * 0.015)));
            }
            mcdwrapper->Run(&m_inImage);
            toc();

            //mask = cv::cvarrToMat(mcdwrapper->detect_img);

//			if (nframe >= 269)
//			{
//				std::cerr << "debugging" << std::endl;
//			}
            std::vector<cv::Rect> allTargetsBBox = extractBoundingBoxes(cv::cvarrToMat(mcdwrapper->detect_img), inputRect, inputSize);


            // Show bounding box results
            for (const auto &box : allTargetsBBox) {
                cv::rectangle(frame, box, cv::Scalar(0, 0, 255), 2);
                // Scalar(B,G,R), thickness=2
            }

            std::cout << "frame number: " << nframe << std::endl;
            cv::imshow("ProcessedFrame", frame);
			writeOutput.write(frame);

            // Press  ESC on keyboard to  exit
            char c = (char)waitKey(1);
            if (c == 27)
                break;
        }

        // When everything done, release the video capture and write object
        cap.release();
        writeOutput.release();

        std::cout << "Processing Finished!" << std::endl;

        // Closes all the frames
        cv::destroyAllWindows();


    return a.exec();
}

std::vector<cv::Rect> extractBoundingBoxes(const cv::Mat& m_mask, const cv::Rect inputRect, const cv::Size inputSize)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Rect> boundingBoxes;

    float removedAreaPercentage = 0.15;

    // Ensure mask is binary
    cv::Mat binaryMask;
    cv::threshold(m_mask, binaryMask, 0, 255, cv::THRESH_BINARY);

    cv::Mat opening, closing;
    cv::morphologyEx(binaryMask, opening, MORPH_OPEN, kernelOpening);
    cv::morphologyEx(opening, closing, MORPH_CLOSE, kernelClosing);

	double widthScale = static_cast<double>(inputRect.width) / static_cast<double>(m_mask.cols);
	double heightScale = static_cast<double>(inputRect.height) / static_cast<double>(m_mask.rows);

    // Find contours
    cv::findContours(closing, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::Mat drawing = cv::Mat::zeros(m_mask.size(), CV_8UC3);
    for (const auto& contour : contours) {
        cv::Rect rect = cv::boundingRect(contour);  // cv::Rect (int)
        std::vector<std::vector<cv::Point>> temp{ contour };
        cv::drawContours(drawing, temp, -1, cv::Scalar(0, 255, 0), 2);
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
