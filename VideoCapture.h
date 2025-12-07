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
            pad = nullptr;
        }
    };

    GST_Data m_gstData;

    Detector m_detector;

    QByteArray m_frameBuffer;
    QSize m_frameSize;

    static GstPadProbeReturn cb_have_data(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
    static void processNewFrame(guint8 *pData);

private Q_SLOTS:
    void sltDetectionDataUpdated();

Q_SIGNALS:
    void sigNewFrameReceived();
    void sigAutoLockDetected(const QRectF &bbox);
};

#endif // VIDEOCAPTURE_H
