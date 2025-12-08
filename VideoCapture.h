#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <iostream>

#include <QSize>
#include <QDebug>
#include <QString>
#include <QWidget>

#include "gst/gst.h"
#include "gst/video/videooverlay.h"
#include "Detector/Detector.h"

class VideoCapture : public QObject
{
    Q_OBJECT

public:
    VideoCapture();

    bool initialize();
    void startCapture();
    void stopCapture();

    void enableDetecting(const bool& state);
    void enableAutoLock(const bool& state);

    QByteArray getFrameBuffer();
    QSize getFrameSize() const;
    void setFrameSize(const QSize &frameSize);

private:
    struct GST_Data
    {
        GstElement *pipeline;
        GstElement *sink;

        GstPad *pad;

        GST_Data()
        {
            pipeline = nullptr;
            pad = nullptr;
        }
    };

    cv::Mat m_bgrFrame;
    cv::Mat m_I420_Frame;

    GST_Data m_gstData;

    Detector m_detector;

    QByteArray m_frameBuffer;
    QSize m_frameSize;

    static GstFlowReturn on_new_sample_from_sink(GstElement* sink, gpointer user_data);

    static void processNewFrame(guint8 *pData);

private Q_SLOTS:
    void sltDetectionDataUpdated();

Q_SIGNALS:
    void sigNewFrameReceived();
    void sigAutoLockDetected(const QRectF &bbox);
};

#endif // VIDEOCAPTURE_H
