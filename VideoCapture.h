#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <iostream>

#include <QRect>
#include <QSize>
#include <QDebug>
#include <QObject>
#include <QCalendar>
#include <QDateTime>

#include "gst/gst.h"
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "ControlPanelSDK.h"
#include "TypeCastings_global.h"

#include "Detector/Detector.h"

class VideoCapture : public QObject
{
    Q_OBJECT

    struct Data
    {
        GstElement* pipeline = Q_NULLPTR;
        GstElement* sink = Q_NULLPTR;
        GstElement* source = Q_NULLPTR;
        GstElement* window = Q_NULLPTR;
        GstElement* valve = Q_NULLPTR;
        GstBus* bus = Q_NULLPTR;
        GstPad *pad = Q_NULLPTR;
    };

private:
    QSize m_resolution;
    Data m_data;
    bool m_isDetectorInitialized;

private:
    static GstFlowReturn on_new_sample_from_sink(
            GstElement* sink,
            gpointer user_data);

    static gboolean bus_message(
            GstBus* bus, GstMessage* msg,
            gpointer user_data);

    static GstPadProbeReturn modifyBuffer(
            GstPad *pad,
            GstPadProbeInfo *info,
            gpointer user_data);

public:
    explicit VideoCapture(
            const uint8_t &mode);

    ~VideoCapture();

    void initialize();
    void finalize();
    void start();
    void stop();

    void setResolution(
            const QSize& resolution);

    void enableDetecting(const bool &state);
    void enableAutoLock(const bool &state);

    cv::Size frameSize() const;

private:
    enum Types
    {
        Type_Detection,
        Type_Render,
        Type_Feeder,
    };

    Detector m_detector;

    cv::Size m_frameSize;
    int m_frameCounter;

    Types m_pipeType;

    static void processNewFrame(cv::Mat &frame,
            VideoCapture *videoCapture);

    int maxDuration = std::numeric_limits<int>::min();

    std::chrono::system_clock::time_point startTime;

Q_SIGNALS:
    void sigNewFrameReceived();
    void sigAutoLockDetected(const QRectF &bbox);
};

#endif // VIDEOCAPTURE_H
