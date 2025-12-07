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

    QSize getFrameSize() const;
    void setFrameSize(const QSize &frameSize);

private:
    struct GST_Data
    {
        GstElement *pipeline;
        GstElement *conversion;

        GstPad *pad;

        GST_Data()
        {
            pipeline = nullptr;
            sink = nullptr;
            bin = nullptr;
            pad = nullptr;
        }
    };

    Detector m_detector;
    GST_Data m_gstData;

    QByteArray m_frameBuffer;
    QSize m_frameSize;

    static GstPadProbeReturn cb_have_data(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
    static void processNewFrame(guint8 *pData);


Q_SIGNALS:
    void sigNewFrameReceived();
};

#endif // VIDEOCAPTURE_H
