#include "VideoCapture.h"

VideoCapture::
VideoCapture()
{

}

GstFlowReturn
VideoCapture::on_new_sample_from_sink(GstElement* sink, gpointer user_data)
{
    VideoCapture* videoCapture = static_cast<VideoCapture*>(user_data);
    GstSample* sample;
    GstCaps *caps;
    GstStructure *structure;
    g_signal_emit_by_name(sink, "pull-sample", &sample);

    if (sample)
    {
        caps = gst_sample_get_caps(sample);

        if (caps)
        {
            structure = gst_caps_get_structure(caps, 0);

            if (structure)
            {
                int width = 0;
                int height = 0;

                gst_structure_get_int(structure, "width", &width);
                gst_structure_get_int(structure, "height", &height);

                // Format is I420

                videoCapture->m_frameSize.setWidth(width);
                videoCapture->m_frameSize.setHeight(height);
            }
        }

        GstMapInfo info;
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        gst_buffer_map(buffer, &info, (GstMapFlags)(GST_MAP_READ));

        auto _data = info.data;
        auto _size = info.size;

        QByteArray ba((char*)_data, _size);

        videoCapture->m_frameBuffer = ba;

        gst_buffer_unmap(buffer, &info);
        gst_sample_unref(sample);

        Q_EMIT videoCapture->sigNewFrameReceived();

        return GST_FLOW_OK;
    }

    return GST_FLOW_ERROR;
}

void VideoCapture::
setWindowID(const guintptr &windowID)
{
    m_windowID = windowID;
}

bool VideoCapture::
initialize()
{
    if (m_windowID == guintptr())
    {
        return false;
    }

    QString pipestr = "rtspsrc location=rtsp://192.168.1.100/ch0/stream0 ! "
                      "application/x-rtp,media=video,clockrate=90000,"
                      "encoding-name=H264,payload=96 ! rtph264depay ! "
                      "h264parse ! decodebin ! videoconvert ! tee name=t ! "
                      "queue ! d3dvideosink sync=false name=mysink "
                      "force-aspect-ratio=false enable-navigation-events=false t. ! "
                      "queue ! appsink name=myfakesink sync=false";

    GError *error = NULL;
    m_gstData.pipeline = gst_parse_launch(
                pipestr.toLatin1().data(), &error);

    if (error != NULL)
    {
        qCritical() << "Cannot initialize pipeline.\n"
                    << int(error->code) << error->message;

        return false;
    }

    m_gstData.sink = gst_bin_get_by_name(
                GST_BIN(m_gstData.pipeline),
                "mysink");

    gst_video_overlay_set_window_handle(
                GST_VIDEO_OVERLAY(m_gstData.sink),
                m_windowID);

    m_gstData.fakesink = gst_bin_get_by_name(
                GST_BIN(m_gstData.pipeline),
                "myfakesink");

    g_object_set(G_OBJECT(m_gstData.fakesink), "emit-signals", TRUE, "sync", FALSE, NULL);
    g_signal_connect(m_gstData.fakesink, "new-sample", G_CALLBACK(on_new_sample_from_sink), this);

    return true;
}

void VideoCapture::startCapture()
{
    if (m_gstData.pipeline != nullptr)
    {
        gst_element_set_state(
                    GST_ELEMENT(m_gstData.pipeline),
                    GST_STATE_PLAYING);
    }
}

void VideoCapture::stopCapture()
{
    if (m_gstData.pipeline != nullptr)
    {
        gst_element_set_state(
                    GST_ELEMENT(m_gstData.pipeline),
                    GST_STATE_NULL);

        gst_object_unref(m_gstData.pipeline);

        m_gstData.pipeline = nullptr;
    }
}

void VideoCapture::
setForceAspectRatio(bool state)
{
    if (m_gstData.sink)
    {
        g_object_set(G_OBJECT(m_gstData.sink), "force-aspect-ratio", state, NULL);
    }
}

bool VideoCapture::
isInitialized() const
{
    return m_gstData.sink != nullptr;
}

QByteArray VideoCapture::
getFrameBuffer() const
{
    return m_frameBuffer;
}

QSize VideoCapture::
getFrameSize() const
{
    return m_frameSize;
}
