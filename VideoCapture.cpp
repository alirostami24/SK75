#include "VideoCapture.h"

/* called when the appsink notifies us that there is a new buffer ready for
 * processing */
GstFlowReturn
VideoCapture::on_new_sample_from_sink(GstElement* sink, gpointer user_data)
{
    VideoCapture* videoCapture = static_cast<VideoCapture*>(user_data);
    GstSample* sample;
    g_signal_emit_by_name(sink, "pull-sample", &sample);
    if (sample) {
        GstMapInfo info;
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        gst_buffer_map(buffer, &info, (GstMapFlags)(GST_MAP_READ));

        auto _data = info.data;
        auto _size = info.size;

        //qCritical() << "xxxxxxx: on_new_sample_from_sink  BGR " << _size;

        QByteArray ba((char*)_data, _size);
        gst_buffer_unmap(buffer, &info);
        gst_sample_unref(sample);

        //Q_EMIT videoCapture->sigFrameReady(ba);
        return GST_FLOW_OK;
    }

    return GST_FLOW_ERROR;
}

gboolean VideoCapture::bus_message(GstBus* bus, GstMessage* msg, gpointer user_data)
{

    VideoCapture* videoCapture = static_cast<VideoCapture*>(user_data);

    switch (GST_MESSAGE_TYPE(msg)) {

    case GST_MESSAGE_EOS:

        break;

    case GST_MESSAGE_ERROR: {
        gchar* debug;
        GError* error;

        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);
        g_printerr("Error: %s\n", error->message);
        g_error_free(error);
        break;
    }
    }

    return TRUE;
}

GstPadProbeReturn VideoCapture::
modifyBuffer(GstPad *pad,
             GstPadProbeInfo *info,
             gpointer user_data)
{
    VideoCapture *videoCapture =
            static_cast<VideoCapture*>(user_data);

    GstBuffer *buffer = GST_PAD_PROBE_INFO_BUFFER(info);
    buffer = gst_buffer_make_writable(buffer);

    if (buffer == nullptr)
    {
        return GST_PAD_PROBE_OK;
    }

    GstCaps *caps = gst_pad_get_current_caps(pad);

    if (caps == nullptr)
    {
        return GST_PAD_PROBE_OK;
    }

    GstStructure *structure =
            gst_caps_get_structure(caps, 0);

    if (structure == nullptr)
    {
        return GST_PAD_PROBE_OK;
    }

    cv::Size *size = &videoCapture->m_frameSize;

    gst_structure_get_int(
                structure, "width",
                &size->width);

    gst_structure_get_int(
                structure, "height",
                &size->height);

    std::string format =
            gst_structure_get_string(
                structure, "format");


    if (format != "BGRA")
    {
        return GST_PAD_PROBE_OK;
    }

    bool *isInitialized =
            &videoCapture->
            m_isDetectorInitialized;

    if (*isInitialized == false)
    {
        videoCapture->m_detector.
                setInputSize(*size);

        *isInitialized = true;
    }

    GstMapInfo map;

    if (gst_buffer_map(buffer, &map, GST_MAP_WRITE))
    {
        cv::Mat bgraMat(size->height, size->width,
                        CV_8UC4, (void *)map.data);

        processNewFrame(bgraMat, videoCapture);

        gst_buffer_unmap(buffer, &map);
    }

    return GST_PAD_PROBE_OK;
}

void VideoCapture::
setResolution(const QSize &resolution)
{
    m_resolution = resolution;
}

void VideoCapture::
enableDetecting(const bool &state)
{
    m_detector.enableDetecting(state);
}

void VideoCapture::
enableAutoLock(const bool &state)
{
    m_detector.enableAutoLock(state);
}

cv::Size VideoCapture::
frameSize() const
{
    return m_frameSize;
}

void VideoCapture::
processNewFrame(cv::Mat &frame,
                VideoCapture *videoCapture)
{
    Detector *detector = &videoCapture->m_detector;

    if (detector->isDetectorActivated())
    {
        detector->detect(&frame);

//        const auto objects =
//                detector->getAllDetectedObjects();

//        for (auto item : objects)
//        {
//            cv::rectangle(frame,item.bbox,
//                          cv::Scalar(0, 0, 255), 2);
//        }

        std::cerr << "ddddd ------- \n";

        const auto objects =
                detector->getDetectedBoundingBox();

        if ((objects.width > 0) && (objects.height > 0))
        {
            cv::rectangle(frame,objects,
                          cv::Scalar(0, 0, 255), 2);
        }
    }
}

VideoCapture::
VideoCapture() :
    m_isDetectorInitialized(false)
{
    m_frameCounter = 0;

    connect(&m_detector, &Detector::sigAutoLockDetected,
            this, &VideoCapture::sigAutoLockDetected);
}

VideoCapture::
~VideoCapture()
{

}

void VideoCapture::initialize()
{
    QString pipeStr;

#if defined(DUMMY_VIDEO)
    QString sink = "xvimagesink";

#if defined(_WIN64) || defined(_WIN32)
    sink = "autovideosink";
#endif

    pipeStr = "videotestsrc pattern=18 ! "
              "video/x-raw,width=720,height=576,framerate=30/1,format=BGRA ! "
              "videocrop top=0 bottom=0 left=0 right=0 name=yuy2Source ! videoconvert ! " + sink;;

#else
    pipeStr= "rtspsrc location=rtsp://192.168.1.100/ch0/stream0 latency=100 protocols=udp ! "
             "rtpjitterbuffer latency=100 ! rtph264depay ! h264parse ! "
             "nvv4l2decoder enable-max-performance=true ! nvvidconv ! video/x-raw, format=BGRx ! "
             "videoconvert  name=mysource ! video/x-raw,format=BGRA ! "
             "videoconvert ! "
             "xvimagesink name=mysink sync=false";
#endif

    std::cerr << "\n pipeStr: "
              << pipeStr.toStdString()
              << std::endl;

    m_data.pipeline = gst_parse_launch(
                pipeStr.toLatin1().data(), NULL);

    // To get YUY2 buffer
    m_data.source = gst_bin_get_by_name(
                GST_BIN(m_data.pipeline), "mysource");

    m_data.pad = gst_element_get_static_pad(
                m_data.source, "src");

    gst_pad_add_probe(m_data.pad, GST_PAD_PROBE_TYPE_BUFFER,
                      modifyBuffer,
                      reinterpret_cast<gpointer>(this), NULL);

    gst_object_unref(m_data.pad);

    m_data.bus = gst_pipeline_get_bus(GST_PIPELINE(m_data.pipeline));

    gst_bus_add_watch(m_data.bus, (GstBusFunc)bus_message, this);

    gst_object_unref(m_data.bus);
}

void VideoCapture::start()
{
    if (m_data.pipeline) {
        gst_element_set_state(GST_ELEMENT(m_data.pipeline), GST_STATE_PLAYING);
    }
}

void VideoCapture::stop()
{
    if (m_data.pipeline) {
        gst_element_set_state(m_data.pipeline, GST_STATE_NULL);
        gst_object_unref(m_data.pipeline);
        m_data.pipeline = Q_NULLPTR;
    }
}


