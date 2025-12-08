#include "TH.hpp"

TH::TH()
{
    m_isTHActivated = false;
    m_isTHInitialized = false;
	m_minValidHeat = 50;
}

TH::~TH()
{
    m_maskTH.release();
}

void TH::enableTH(bool state)
{
    m_isTHActivated = state;
}

void TH::init(cv::Size inputSize)
{
    // Allocate
    m_maskTH.create(inputSize.height, inputSize.width, CV_8UC1);

    m_inputSize = inputSize;

    kernel_5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    kernel_3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    m_validMarginRatio = 0.5;

    m_tolerance = 0.8;


//	cvCopy(imgGray, imgGrayPrev);
    m_isTHInitialized = true;
}

void TH::run(cv::Mat input)
{
//    auto start = std::chrono::high_resolution_clock::now();

    m_inputFrame = input.clone();

    if (m_isTHInitialized == false)
    {
        init(cv::Size(input.cols, input.rows));
    }


    topHatCentroid(m_inputFrame);

//    double widthScale = static_cast<double>(m_inputRect.width) / static_cast<double>(m_maskTH.cols);
//    double heightScale = static_cast<double>(m_inputRect.height) / static_cast<double>(m_maskTH.rows);

//    m_targetCenteroid.x = widthScale * m_targetCenteroid.x + m_inputRect.x;
//    m_targetCenteroid.y = heightScale * m_targetCenteroid.y + m_inputRect.y;

	if ((m_targetCenteroid.x > 0) && (m_targetCenteroid.y > 0))
	{
		// Recentring and resizing method
		m_targetBBox = getObjectSizeMethod1(m_targetCenteroid, m_inputFrame.data);

		// Only resizing with OTSU
		//m_targetBBox = getObjectSizeMethod2(m_targetCenteroid, m_inputFrame.data);
	}
	else
	{
		m_targetBBox = cv::Rect(-1, -1, -1, -1);
	}
    

//    auto end = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double, std::milli> duration = end - start;
    //    std::cout << "TH motion detection execution time: " << duration.count() << " ms" << std::endl;
}

cv::Mat TH::getMask()
{
    return m_maskTH;
}

bool TH::isTHInitialized()
{
    return m_isTHInitialized;
}

bool TH::isTHActivated()
{
    return m_isTHActivated;
}

void TH::stopTH()
{
    m_isTHActivated = false;
}

void TH::clearMemory()
{
    m_targetBBox = cv::Rect();
}

void TH::reset()
{
    m_isTHInitialized = false;
    m_maskTH.release();
}


cv::Rect TH::getBoundingBox()
{
    return m_targetBBox;
}

void TH::setInputSize(const cv::Size &inputSize)
{
    m_inputSize = inputSize;
}

void TH::setTargetSize(const cv::Size2d &targetSize)
{
    m_tempTargetBBox.width = targetSize.width;
    m_tempTargetBBox.height = targetSize.height;
}

void TH::setTargetCenter(const cv::Point2d &targetCenter)
{
    m_tempTargetBBox.x = targetCenter.x - m_tempTargetBBox.width / 2;
    std::max(0, m_tempTargetBBox.x);
    if (m_tempTargetBBox.x + m_tempTargetBBox.width > m_inputSize.width)
    {
        m_tempTargetBBox.x = m_inputSize.width - m_tempTargetBBox.width;
    }

    m_tempTargetBBox.y = targetCenter.y - m_tempTargetBBox.height / 2;
    std::max(0, m_tempTargetBBox.y);
    if (m_tempTargetBBox.y + m_tempTargetBBox.height > m_inputSize.height)
    {
        m_tempTargetBBox.y = m_inputSize.height - m_tempTargetBBox.height;
    }
}

cv::Size2d TH::getTargetSize()
{
    return cv::Size2d(m_tempTargetBBox.width, m_tempTargetBBox.height);
}

cv::Point2d TH::getTargetCenter()
{
    cv::Point2d  center;
    center.x = m_tempTargetBBox.x + m_tempTargetBBox.width / 2;
    center.y = m_tempTargetBBox.y + m_tempTargetBBox.height / 2;
    return center;
}

cv::Rect TH::getObjectSizeMethod1(const cv::Point2d &targetCenter, unsigned char *pData)
{
    cv::Rect targetBBox10 = resizeObjectWithOTSUMethod1(cv::Size2d(10, 10), targetCenter, pData);

    if ((targetBBox10.width >= 10 * m_tolerance) || (targetBBox10.height >= 10 * m_tolerance))
    {
        cv::Rect targetBBox30 = resizeObjectWithOTSUMethod1(cv::Size2d(30, 30), targetCenter, pData);

            if ((targetBBox30.width >= 30 * m_tolerance) || (targetBBox30.height >= 30 * m_tolerance))
            {
                cv::Rect targetBBox60 = resizeObjectWithOTSUMethod1(cv::Size2d(60, 60), targetCenter, pData);
                if ((targetBBox60.width >= 60 * m_tolerance) || (targetBBox60.height >= 60 * m_tolerance))
                {
                    cv::Rect targetBBox90 = resizeObjectWithOTSUMethod1(cv::Size2d(90, 90), targetCenter, pData);
                    if ((targetBBox90.width >= 90 * m_tolerance) || (targetBBox90.height >= 90 * m_tolerance))
                    {
                        cv::Rect targetBBox128 = resizeObjectWithOTSUMethod1(cv::Size2d(128, 128), targetCenter, pData);
                        return targetBBox128;
                    }
                    else
                    {
                        return targetBBox90;
                    }
                }
                else
                {
                    return targetBBox60;
                }
            }
            else
            {
                return targetBBox30;
            }
    }
    else
    {
        return targetBBox10;
    }
}

cv::Rect TH::getObjectSizeMethod2(const cv::Point2d &targetCenter, unsigned char *pData)
{
    cv::Rect targetBBox10 = resizeObjectWithOTSUMethod2(cv::Size2d(10, 10), targetCenter, pData);

    if ((targetBBox10.width >= 10 * m_tolerance) || (targetBBox10.height >= 10 * m_tolerance))
    {
        cv::Rect targetBBox30 = resizeObjectWithOTSUMethod2(cv::Size2d(30, 30), targetCenter, pData);

            if ((targetBBox30.width >= 30 * m_tolerance) || (targetBBox30.height >= 30 * m_tolerance))
            {
                cv::Rect targetBBox60 = resizeObjectWithOTSUMethod2(cv::Size2d(60, 60), targetCenter, pData);
                if ((targetBBox60.width >= 60 * m_tolerance) || (targetBBox60.height >= 60 * m_tolerance))
                {
                    cv::Rect targetBBox90 = resizeObjectWithOTSUMethod2(cv::Size2d(90, 90), targetCenter, pData);
                    if ((targetBBox90.width >= 90 * m_tolerance) || (targetBBox90.height >= 90 * m_tolerance))
                    {
                        cv::Rect targetBBox128 = resizeObjectWithOTSUMethod2(cv::Size2d(128, 128), targetCenter, pData);
                        return targetBBox128;
                    }
                    else
                    {
                        return targetBBox90;
                    }
                }
                else
                {
                    return targetBBox60;
                }
            }
            else
            {
                return targetBBox30;
            }
    }
    else
    {
        return targetBBox10;
    }
}

void TH::topHatCentroid(const cv::Mat &input)
{
    cv::Mat gray;
    if (input.channels() == 3) {
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = input;  // If already grayscale, use as is
    }
	cv::Mat blurMat;
    cv::medianBlur(gray, blurMat, 5);

	cv::Mat topHatMat;
    cv::morphologyEx(blurMat, topHatMat, cv::MORPH_TOPHAT, kernel_3);

    cv::Mat normMat;
    cv::normalize(topHatMat, normMat, 0, 255, cv::NORM_MINMAX);


    //double minVal, maxVal;
    //cv::Point minLoc, maxLoc;
    //cv::minMaxLoc(normMat, &minVal, &maxVal, &minLoc, &maxLoc);

    cv::Mat thresh_binary;
    cv::threshold(normMat, thresh_binary, 200, 255, cv::THRESH_BINARY);

    cv::Mat dilate_result;
    cv::dilate(thresh_binary, dilate_result, kernel_5, cv::Point(-1, -1), 1);

	cv::Mat opening_result;
    cv::morphologyEx(dilate_result, opening_result, cv::MORPH_OPEN, kernel_5);

	cv::Mat labels, stats, centroids;
	int numComponents = cv::connectedComponentsWithStats(opening_result, labels, stats, centroids);

	int imageArea = opening_result.rows * opening_result.cols;
	int wValidMargin = std::max(10, static_cast<int>(opening_result.cols * 0.01));
	int hValidMargin = std::max(10, static_cast<int>(opening_result.rows * 0.01));
	
	//cv::Mat mask = cv::Mat::zeros(opening_result.size(), opening_result.type());
	m_candidatesInfo.clear();
	CandidateInfo candidateInfo;
	cv::Mat grayObject;
	double meanObjectHeat;
	for (int i = 1; i < numComponents; ++i) {
		if (stats.at<int>(i, 4) < imageArea * 0.5)
		{
			cv::Rect bbox(stats.at<int>(i, 0), stats.at<int>(i, 1), stats.at<int>(i, 2), stats.at<int>(i, 3));
			if ((bbox.x > wValidMargin) &&
				(bbox.y > hValidMargin) &&
				((opening_result.cols - (bbox.x + bbox.width)) > wValidMargin) &&
				((opening_result.rows - (bbox.y + bbox.height)) > hValidMargin)
				)
			{
				grayObject = gray(bbox).clone();
				meanObjectHeat = cv::mean(grayObject)[0];
				if (meanObjectHeat > m_minValidHeat)
				{
					candidateInfo.bbox = bbox;
					candidateInfo.mediumHeat = meanObjectHeat;
					m_candidatesInfo.push_back(candidateInfo);
				}
			}

		}

	}

	
	if (m_candidatesInfo.size() > 0)
	{
		double maxHeat = 0;
		cv::Rect bbox;
		for (int i = 0; i < m_candidatesInfo.size(); i++)
		{
			if (m_candidatesInfo[i].mediumHeat > maxHeat)
			{
				bbox = m_candidatesInfo[i].bbox;
				maxHeat = m_candidatesInfo[i].mediumHeat;
			}
		}

		m_targetCenteroid.x = bbox.x + (bbox.width / 2);
		m_targetCenteroid.y = bbox.y + (bbox.height / 2);
	}
	else
	{
		m_targetCenteroid.x = -1;
		m_targetCenteroid.y = -1;
	}
    /*cv::Moments moments = cv::moments(m_maskTH);

    m_targetCenteroid.x = moments.m10 / moments.m00;
    m_targetCenteroid.y = moments.m01 / moments.m00;*/

}

cv::Rect TH::resizeObjectWithOTSUMethod1(const cv::Size2d &targetSize, const cv::Point2d &targetCenter, unsigned char *pData)
{
    setTargetSize(targetSize);
    setTargetCenter(targetCenter);
    adjustBoundingBox(pData);
    return m_tempTargetBBox;
}

cv::Rect TH::resizeObjectWithOTSUMethod2(const cv::Size2d &targetSize, const cv::Point2d &targetCenter, unsigned char *pData)
{
    m_inputFrame.data = pData;
    setTargetSize(targetSize);
    setTargetCenter(targetCenter);
    cv::Mat output_image = m_inputFrame(m_tempTargetBBox).clone();
    cv::Mat gray_image;
    cv::cvtColor(output_image, gray_image, cv::COLOR_BGR2GRAY);
    cv::Mat binaryImage;
    cv::threshold(gray_image, binaryImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::Rect rectangle = cv::boundingRect(binaryImage);
    rectangle.x += m_tempTargetBBox.x;
    rectangle.y += m_tempTargetBBox.y;

    setTargetSize(cv::Size2d(rectangle.width, rectangle.height));
    setTargetCenter(cv::Point2d(rectangle.x + rectangle.width / 2, rectangle.y + rectangle.height / 2));

    return m_tempTargetBBox;
}

cv::Point2d TH::calculateCentroidBB(cv::Mat &frame)
{
    cv::Point2d center = getTargetCenter();
        cv::Size2d boundingBoxSize = getTargetSize();

        if (center.x <= 0 && center.y <= 0)
            return cv::Point(-1, -1);

        //cv::Rect rect;
        /*cv::Size2i sizeBB = targetSize();

        rect.width = sizeBB.width;
        rect.height = sizeBB.height;*/
        //rect = getPredictedBoundingBox();
        //center = cv::Point2f(rect.x + 0.5*rect.width, rect.y + 0.5*rect.height);

        int initialNumberOfRows = static_cast<int>(frame.rows * 0.1);
        int initialNumberOfColumns = static_cast<int>(frame.cols * 0.1);

        std::vector<std::vector<int>> baseInput;
        std::vector<std::vector<int>>  meanArray;
        std::vector<std::vector<int>>  secondMeanArray;
        std::vector<std::vector<int>>  varianceArray;

        baseInput.resize(initialNumberOfRows);
        meanArray.resize(initialNumberOfRows);
        secondMeanArray.resize(initialNumberOfRows);
        varianceArray.resize(initialNumberOfRows);

        for (int i = 0; i < initialNumberOfRows; i++)
        {
            baseInput[i].resize(initialNumberOfColumns);
            meanArray[i].resize(initialNumberOfColumns);
            secondMeanArray[i].resize(initialNumberOfColumns);
            varianceArray[i].resize(initialNumberOfColumns);
        }

        int columnIndex = 5;
        int rowIndex = 5;
        double factor = 2.0 / 3.0;
        int numberOfColumns = static_cast<int>(boundingBoxSize.width * 2);
        int numberOfRows = static_cast<int>(boundingBoxSize.height * 2);

        if (numberOfRows > (initialNumberOfRows - 5))
            numberOfRows = initialNumberOfRows - 5;
        if (numberOfColumns > (initialNumberOfColumns - 5))
            numberOfColumns = initialNumberOfColumns - 5;

        cv::Point boundingBoxTopLeftPoint;
        cv::Point boundingBoxBottomRightPoint;
        boundingBoxTopLeftPoint.x = static_cast<int>(center.x - numberOfColumns / 2);
        boundingBoxTopLeftPoint.y = static_cast<int>(center.y - numberOfRows / 2);
        boundingBoxBottomRightPoint.x = static_cast<int>(center.x + numberOfColumns / 2);
        boundingBoxBottomRightPoint.y = static_cast<int>(center.y + numberOfRows / 2);

        if (boundingBoxTopLeftPoint.x < 10)
            boundingBoxTopLeftPoint.x = 10;
        else if (boundingBoxTopLeftPoint.x > (frame.cols - 10))
            boundingBoxTopLeftPoint.x = (frame.rows - 10);

        if (boundingBoxBottomRightPoint.x < 20)
            boundingBoxBottomRightPoint.x = 20;
        else if (boundingBoxBottomRightPoint.x > (frame.cols - 5))
            boundingBoxBottomRightPoint.x = (frame.rows - 5);

        if (boundingBoxTopLeftPoint.x >= boundingBoxBottomRightPoint.x)
            boundingBoxTopLeftPoint.x = boundingBoxBottomRightPoint.x - 3;

        if (boundingBoxTopLeftPoint.y < 20)
            boundingBoxTopLeftPoint.y = 20;
        else if (boundingBoxTopLeftPoint.y > (frame.rows - 25))
            boundingBoxTopLeftPoint.y = (frame.rows - 25);

        if (boundingBoxBottomRightPoint.y < 30)
            boundingBoxBottomRightPoint.y = 30;
        else if (boundingBoxBottomRightPoint.y > (frame.rows - 15))
            boundingBoxBottomRightPoint.y = (frame.rows - 15);

        if (boundingBoxTopLeftPoint.y >= boundingBoxBottomRightPoint.y)
            boundingBoxTopLeftPoint.y = boundingBoxBottomRightPoint.y - 3;

        for (int i = boundingBoxTopLeftPoint.y + 1; i < boundingBoxBottomRightPoint.y - 1; i++)
            for (int j = boundingBoxTopLeftPoint.x + 1; j < boundingBoxBottomRightPoint.x - 1; j++) {
                baseInput[i - boundingBoxTopLeftPoint.y][j - boundingBoxTopLeftPoint.x] = static_cast<int>(static_cast<unsigned char*>(frame.data)[(j + i * frame.cols) * 3 + 1]);
            }

        for (int i = 1; i < numberOfRows; i++) {
            for (int j = 1; j < numberOfColumns; j++) {
                meanArray[i][j] = meanArray[i - 1][j] + meanArray[i][j - 1] - meanArray[i - 1][j - 1] + baseInput[i][j];
                secondMeanArray[i][j] = secondMeanArray[i - 1][j] + secondMeanArray[i][j - 1] - secondMeanArray[i - 1][j - 1] + (baseInput[i][j] * baseInput[i][j]);
            }
        }
        int mean;
        int secondMean;
        int maxVariance = 0;
        for (int i = rowIndex + 1; i < numberOfRows - rowIndex - 1; i++)
            for (int j = columnIndex + 1; j < numberOfColumns - columnIndex - 1; j++)
            {
                mean = meanArray[i + rowIndex][j + columnIndex]
                    - (meanArray[i - rowIndex - 1][j + columnIndex]
                        + meanArray[i + rowIndex][j - columnIndex - 1]
                        - meanArray[i - rowIndex - 1][j - columnIndex - 1]);

                secondMean = secondMeanArray[i + rowIndex][j + columnIndex]
                    - (secondMeanArray[i - rowIndex - 1][j + columnIndex]
                        + secondMeanArray[i + rowIndex][j - columnIndex - 1]
                        - secondMeanArray[i - rowIndex - 1][j - columnIndex - 1]);

                varianceArray[i][j] = (2 * rowIndex + 1) * (2 * columnIndex + 1) * secondMean - mean * mean;
                varianceArray[i][j] = static_cast<int>(sqrt(static_cast<double>(varianceArray[i][j]))
                    / static_cast<double>((2 * rowIndex + 1)*(2 * columnIndex + 1)));

                if (varianceArray[i][j] > maxVariance)
                    maxVariance = varianceArray[i][j];
            }

        double sumBW = 1.0;
        double sumxBW = 0.0;
        double sumyBW = 0.0;

        for (int i = rowIndex + 1; i < numberOfRows - rowIndex - 1; i++) {
            for (int j = columnIndex + 1; j < numberOfColumns - columnIndex - 1; j++) {
                if (varianceArray[i][j] > (factor*maxVariance)) {
                    sumBW++;
                    sumxBW += (j - static_cast<double>(numberOfColumns) / 2.0);
                    sumyBW += (i - static_cast<double>(numberOfRows) / 2.0);
                }
            }
        }

        double tempXCenter;
        double tempYCenter;
        double xCenter;
        double yCenter;
        xCenter = (static_cast<double>(sumxBW) / static_cast<double>(sumBW));
        yCenter = (static_cast<double>(sumyBW) / static_cast<double>(sumBW));

        tempXCenter = xCenter;
        tempYCenter = yCenter;
        center.x += tempXCenter;
        center.y += tempYCenter;

        if (center.x > (frame.cols - 10))
            center.x = (frame.cols - 10);
        else if (center.x < 10)
            center.x = 10;

        if (center.y > (frame.rows - 25))
            center.y = (frame.rows - 25);
        else if (center.y < 10)
            center.y = 10;

        double a = 0.0;
        double b = 0.0;
        double c = 0.0;
        double S = 1.0;
        double B;

        for (int i = 0; i < numberOfRows; i++) {
            for (int j = 0; j < numberOfColumns; j++)
            {
                if (varianceArray[i][j] > ((factor * maxVariance)))
                    B = 1.0;
                else
                    B = 0.0;

                S += B;
                a += ((static_cast<double>(j) - xCenter) * (static_cast<double>(j) - xCenter) * B);
                b += (2.0 * (static_cast<double>(j) - xCenter) * (static_cast<double>(i) - yCenter) * B);
                c += ((static_cast<double>(i) - yCenter) * (static_cast<double>(i) - yCenter) * B);
            }
        }

        if (b != 0.0 && std::abs(a - c) < 0.000001)
        {
            double errorRelativeLength = 1.0;
            double sin2t;
            double cos2t;
            double squareValue;
            double X[4] = { 1.0, 1.0, 1.0, 1.0 };
            squareValue = sqrt(b * b + (a - c) * (a - c));

            sin2t = b / squareValue;
            cos2t = (a - c) / squareValue;
            X[0] = (a + c) + (a - c) * cos2t + b * sin2t;
            X[1] = (a + c) - (a - c) * cos2t + b * sin2t;
            X[2] = (a + c) + (a - c) * cos2t - b * sin2t;
            X[3] = (a + c) - (a - c) * cos2t - b * sin2t;

            double maxX = X[0];
            double minX = X[0];

            for (int i = 0; i < 4; i++)
            {
                if (X[i] > maxX)
                    maxX = X[i];
                if (X[i] < minX)
                    minX = X[i];
            }

            if (minX < 1.0)
                minX = 1.0;

            errorRelativeLength = sqrt(maxX / minX);

            if (errorRelativeLength < 1.0)
                errorRelativeLength = 1.0;
            else if (errorRelativeLength > 4.0)
                errorRelativeLength = 4.0;

            if (boundingBoxSize.height > 70.0)
                boundingBoxSize.height = 70.0;
            else if (boundingBoxSize.height < 10.0)
                boundingBoxSize.height = 10.0;

            if (boundingBoxSize.width > 90.0)
                boundingBoxSize.width = 90.0;
            else if (boundingBoxSize.width < 15.0)
                boundingBoxSize.width = 15.0;

            numberOfRows = static_cast<int>(boundingBoxSize.height) * 2;
            numberOfColumns = static_cast<int>(boundingBoxSize.width) * 2;

            if (numberOfRows > (-5))
                numberOfRows = -5;
            if (numberOfColumns > (initialNumberOfColumns - 5))
                numberOfColumns = initialNumberOfColumns - 5;

            boundingBoxTopLeftPoint.x = static_cast<int>(center.x - numberOfColumns / 2);
            boundingBoxTopLeftPoint.y = static_cast<int>(center.y - numberOfRows / 2);
            boundingBoxBottomRightPoint.x = static_cast<int>(center.x + numberOfColumns / 2);
            boundingBoxBottomRightPoint.y = static_cast<int>(center.y + numberOfRows / 2);

            if (boundingBoxTopLeftPoint.x < 10)
                boundingBoxTopLeftPoint.x = 10;
            else if (boundingBoxTopLeftPoint.x > (frame.cols - 10))
                boundingBoxTopLeftPoint.x = (frame.cols - 10);

            if (boundingBoxBottomRightPoint.x < 20)
                boundingBoxBottomRightPoint.x = 20;
            else if (boundingBoxBottomRightPoint.x > (frame.cols - 5))
                boundingBoxBottomRightPoint.x = (frame.cols - 5);

            if (boundingBoxTopLeftPoint.x >= boundingBoxBottomRightPoint.x)
                boundingBoxTopLeftPoint.x = boundingBoxBottomRightPoint.x - 3;

            if (boundingBoxTopLeftPoint.y < 20)
                boundingBoxTopLeftPoint.y = 20;
            else if (boundingBoxTopLeftPoint.y > (frame.rows - 25))
                boundingBoxTopLeftPoint.y = (frame.rows - 25);
            if (boundingBoxBottomRightPoint.y < 30)
                boundingBoxBottomRightPoint.y = 30;
            else if (boundingBoxBottomRightPoint.y > (frame.rows - 15))
                boundingBoxBottomRightPoint.y = (frame.rows - 15);
            if (boundingBoxTopLeftPoint.y >= boundingBoxBottomRightPoint.y)
                boundingBoxTopLeftPoint.y = boundingBoxBottomRightPoint.y - 3;

            for (int i = boundingBoxTopLeftPoint.y + 1; i < boundingBoxBottomRightPoint.y - 1; i++) {
                for (int j = boundingBoxTopLeftPoint.x + 1; j < boundingBoxBottomRightPoint.x - 1; j++) {
                    baseInput[i - boundingBoxTopLeftPoint.y][j - boundingBoxTopLeftPoint.x] = static_cast<int>(static_cast<unsigned char*>(frame.data)[(j + i * frame.cols) * 3 + 1]);
                }
            }

            for (int i = 1; i < numberOfRows; i++) {
                for (int j = 1; j < numberOfColumns; j++) {
                    meanArray[i][j] = meanArray[i - 1][j] + meanArray[i][j - 1] - meanArray[i - 1][j - 1] + baseInput[i][j];
                    secondMeanArray[i][j] = secondMeanArray[i - 1][j]
                        + secondMeanArray[i][j - 1]
                        - secondMeanArray[i - 1][j - 1]
                        + (baseInput[i][j] * baseInput[i][j]);
                }
            }

            maxVariance = 0;
            for (int i = rowIndex + 1; i < numberOfRows - rowIndex - 1; i++) {
                for (int j = columnIndex + 1; j < numberOfColumns - columnIndex - 1; j++) {

                    mean = meanArray[i + rowIndex][j + columnIndex] - (meanArray[i - rowIndex - 1][j + columnIndex]
                        + meanArray[i + rowIndex][j - columnIndex - 1]
                        - meanArray[i - rowIndex - 1][j - columnIndex - 1]);

                    secondMean = secondMeanArray[i + rowIndex][j + columnIndex]
                        - (secondMeanArray[i - rowIndex - 1][j + columnIndex]
                            + secondMeanArray[i + rowIndex][j - columnIndex - 1]
                            - secondMeanArray[i - rowIndex - 1][j - columnIndex - 1]);

                    varianceArray[i][j] = (2 * rowIndex + 1) * (2 * columnIndex + 1) * secondMean - mean * mean;
                    varianceArray[i][j] = static_cast<int>(sqrt(static_cast<double>(varianceArray[i][j]))
                        / static_cast<double>((2 * rowIndex + 1) * (2 * columnIndex + 1)));
                    if (varianceArray[i][j] > maxVariance)
                        maxVariance = varianceArray[i][j];
                }
            }

            sumBW = 1.0;
            sumxBW = 0;
            sumyBW = 0;

            for (int i = rowIndex + 1; i < numberOfRows - rowIndex - 1; i++) {
                for (int j = columnIndex + 1; j < numberOfColumns - columnIndex - 1; j++) {
                    if (varianceArray[i][j] > (factor * maxVariance))
                    {
                        sumBW++;
                        sumxBW += (j - static_cast<double>(numberOfColumns) / 2.0);
                        sumyBW += (i - static_cast<double>(numberOfRows) / 2.0);
                    }
                }
            }

            xCenter = (static_cast<double>(sumxBW) / static_cast<double>(sumBW));
            yCenter = (static_cast<double>(sumyBW) / static_cast<double>(sumBW));

            tempXCenter = xCenter;
            tempYCenter = yCenter;

            center.x += tempXCenter;
            center.y += tempYCenter;

            if (center.x > (frame.cols - 10))
                center.x = (frame.cols - 10);
            else if (center.x < 10)
                center.x = 10;

            if (center.y > (frame.rows - 25))
                center.y = (frame.rows - 25);
            else if (center.y < 10)
                center.y = 10;

        }

        return center;
}

void TH::adjustBoundingBox(unsigned char *pData)
{
    m_inputFrame.data = pData;
        cv::Point2f point2f = calculateCentroidBB(m_inputFrame);
        cv::Size2d boundingBoxSize = getTargetSize();
        if (point2f.x <= 0 && point2f.y <= 0)
            return;

        ////cv::Point2f
        //point2f = simpleCalculateCentroidBB(m_inputFrame);


        if (point2f.x > (m_inputFrame.cols - boundingBoxSize.width / 2 - 3))
            point2f.x = static_cast<float>(m_inputFrame.cols - boundingBoxSize.width / 2 - 3);
        else if (point2f.x < (boundingBoxSize.width / 2 + 3))
            point2f.x = static_cast<float>(boundingBoxSize.width / 2 + 3);

        if (point2f.y > (m_inputFrame.rows - boundingBoxSize.height / 2 - 3))
            point2f.y = static_cast<float>(m_inputFrame.rows - boundingBoxSize.height / 2 - 3);
        else if (point2f.y < (boundingBoxSize.height / 2 + 3))
            point2f.y = static_cast<float>(boundingBoxSize.height / 2 + 3);

        //setTargetCenter(point2f);


        //// adjust bounding box
        cv::Rect rect;
        cv::Size2d sizeBB = getTargetSize();

        rect.x = std::max(static_cast<int>(point2f.x - (0.5* sizeBB.width)), 0);
        rect.y = std::max(static_cast<int>(point2f.y - (0.5* sizeBB.height)), 0);
        rect.width = std::min(static_cast<int>(sizeBB.width), m_inputFrame.cols - rect.x);
        rect.height = std::min(static_cast<int>(sizeBB.height), m_inputFrame.rows - rect.y);

        cv::Mat object = m_inputFrame(rect);

        if (object.channels() > 1)
        {
            cv::cvtColor(object, object, CV_BGR2GRAY);
        }

        cv::Mat temp;
        // down-scale and upscale the image to filter out the noise
        pyrDown(object, temp, cv::Size(object.cols / 2, object.rows / 2));
        pyrUp(temp, object, object.size());

        // binarization
        // method 1
        // creat kernel structure
        //int kernelSize = std::min(rect.width, rect.height) * 0.2;
        //cv::Rect thresholdRect;
        //thresholdRect.x = std::max(static_cast<int>((object.cols * 0.5) - kernelSize), 0);
        //thresholdRect.y = std::max(static_cast<int>((object.rows * 0.5) - kernelSize), 0);
        //thresholdRect.width = std::min(2 * kernelSize, object.cols - thresholdRect.x);
        //thresholdRect.height = std::min(2 * kernelSize, object.rows - thresholdRect.y);

        //float meanBrightness = cv::mean(object(thresholdRect))[0];

        //// apply threshold
        //cv::Mat binaryImage = object >= meanBrightness;

        // method 1
        cv::Mat binaryImage;
        cv::threshold(object, binaryImage, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

        // denoising
        //// get opening morphology for denoising
        //cv::Mat morphologyImage;

        //
        //cv::Mat openingeKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));

        //cv::morphologyEx(binaryImage, morphologyImage, cv::MORPH_OPEN, openingeKernel);

        //// get closing morphology for denoising
        //// creat kernel structure
        //int closingKernelSize = static_cast<double>(kernelSize * 0.5) + 1;
        //cv::Mat closingKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(closingKernelSize, closingKernelSize));
        //cv::morphologyEx(morphologyImage, morphologyImage, cv::MORPH_CLOSE, closingKernel);

        // Get image components
        cv::Mat labels;
        cv::Mat stats;
        cv::Mat centroids;
        cv::connectedComponentsWithStats(binaryImage, labels, stats, centroids, 4);

        // descending sorting by area
        cv::Mat areaStats;
        cv::sortIdx(stats.col(4), areaStats, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);

        int indexArea = areaStats.at<int>(1, 0);

        // for debugging
        //cv::Mat validMat = (labels == indexArea);

        double targetWidth = stats.at<int>(indexArea, 2);
        double targetHeight = stats.at<int>(indexArea, 3);

        point2f.x = std::max(rect.x + stats.at<int>(indexArea, 0) + static_cast<double>(targetWidth * 0.5), 0.0);
        point2f.y = std::max(rect.y + stats.at<int>(indexArea, 1) + static_cast<double>(targetHeight * 0.5), 0.0);

        targetWidth = std::min(targetWidth + (targetWidth * m_validMarginRatio), static_cast<double>(m_inputFrame.cols - point2f.x));
        targetHeight = std::min(targetHeight + (targetHeight * m_validMarginRatio), static_cast<double>(m_inputFrame.rows - point2f.y));

        m_tempTargetBBox.width = targetWidth;
        m_tempTargetBBox.height = targetHeight;

        m_tempTargetBBox.x = point2f.x - m_tempTargetBBox.width / 2;
        m_tempTargetBBox.y = point2f.y - m_tempTargetBBox.height / 2;
}
