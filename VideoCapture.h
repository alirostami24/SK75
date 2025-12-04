#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <iostream>

#include <QSize>
#include <QDebug>
#include <QString>
#include <QWidget>

#include "gst/gst.h"
#include "gst/video/videooverlay.h"

class VideoCapture : public QObject
{
    Q_OBJECT

public:
    VideoCapture();

    void setWindowID(const guintptr &windowID);

    bool initialize();
    void startCapture();
    void stopCapture();

    void setForceAspectRatio(bool state);
    bool isInitialized() const;

    QByteArray getFrameBuffer() const;
    QSize getFrameSize() const;

private:
    struct GST_Data
    {
        GstElement *pipeline;
        GstElement *sink;
        GstElement *fakesink;
        GstElement *bin;
        GstElement *convert;

        GstPad *pad;

        GST_Data()
        {
            pipeline = nullptr;
            sink = nullptr;
            bin = nullptr;
            pad = nullptr;
        }
    };

    GST_Data m_gstData;

    guintptr m_windowID;

    QByteArray m_frameBuffer;
    QSize m_frameSize;

    static GstFlowReturn on_new_sample_from_sink(GstElement* sink, gpointer user_data);

Q_SIGNALS:
    void sigNewFrameReceived();
};

#endif // VIDEOCAPTURE_H
