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


#ifdef DEBUG_PROCESS_TIME
    const auto stopTime = std::chrono::system_clock::now();

    const int duration = std::chrono::duration_cast<
            std::chrono::milliseconds>(stopTime - videoCapture->startTime).count();

    videoCapture->startTime = stopTime;

    videoCapture->maxDuration = std::max(videoCapture->maxDuration, duration);

    std::cerr << "====================== \n";
    std::cerr << "X Duration: " << duration << " " << videoCapture->maxDuration << std::endl;
#endif

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

#ifdef DEBUG_PROCESS_TIME
        const auto sTime = std::chrono::system_clock::now();
#endif

        bgraMat.setTo(0);

        cv::circle(bgraMat, cv::Point(200, 180), 5, cv::Scalar(255, 255, 255), -1);

        processNewFrame(bgraMat, videoCapture);

        const std::string path = QApplication::applicationDirPath().toStdString() + "/bgraMat.bmp";

        cv::imwrite(path, bgraMat);


#ifdef DEBUG_PROCESS_TIME
        const auto stTime = std::chrono::system_clock::now();

        const int dur = std::chrono::duration_cast<
                std::chrono::milliseconds>(stTime - sTime).count();

        std::cerr << "Duration: " << dur << std::endl;

        std::cerr << std::endl;
#endif

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

bool VideoCapture::isDetecting()
{
    return m_detector.isDetectorActivated();
}

void VideoCapture::
enableAutoLock(const bool &state)
{
    m_detector.enableAutoLock(state);
}

bool VideoCapture::isAutoLock()
{
    return m_detector.isAutoLockActivated();
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

        const auto object = detector->
                getDetectedBoundingBox();

        //        const cv::Size halfSize(frame.cols / 2,
        //                                frame.rows / 2);

        //        const cv::Rect refinedRect(
        //                    halfSize.width + object.x,
        //                    halfSize.height + object.y,
        //                    object.width,
        //                    object.height);



        if ((object.width > 0) &&
                (object.height > 0))
        {
            cv::rectangle(frame, object,
                          cv::Scalar(0, 0, 255), 2);
        }
    }
}

VideoCapture::
VideoCapture(const uint8_t &mode) :
    m_isDetectorInitialized(false),
    m_frameSize(720, 576)
{
    m_frameCounter = 0;

    startTime = std::chrono::system_clock::now();

    uint8_t pipeMode = mode;
    pipeMode = std::min(uint8_t(2), pipeMode);

    m_pipeType = static_cast<Types>(pipeMode);

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

    QString sink = "xvimagesink";

#if defined(_WIN64) || defined(_WIN32)
    sink = "autovideosink";
#endif

#if defined(DUMMY_VIDEO)

    pipeStr = "videotestsrc pattern=18 ! "
              "video/x-raw,width=720,height=576,framerate=30/1,format=BGRA ! "
              "videocrop top=0 bottom=0 left=0 right=0 name=yuy2Source ! videoconvert ! " + sink;;

#else
    switch (m_pipeType)
    {
    case Type_Detection:
    {
        //        pipeStr = "rtspsrc location=rtsp://192.168.1.100/ch0/stream0 latency=100 protocols=udp ! "
        //                  "rtpjitterbuffer latency=100 ! rtph264depay ! h264parse ! "
        //                  "nvv4l2decoder enable-max-performance=true ! nvvidconv ! "
        //                  "video/x-raw, format=BGRx ! videoconvert  name=mysource ! fakesink";


#ifdef _WIN32
        pipeStr = "udpsrc port=20000 caps=\"application/x-rtp,media=(string)video,"
                  "clock-rate=(int)90000,encoding-name=(string)H264, payload=(int)96\" ! "
                  "rtph264depay ! decodebin ! videoconvert name=mysource ! "
                  "video/x-raw,format=BGRA ! videoconvert ! fakesink";
#elif defined(__linux)
        pipeStr = "udpsrc port=20000 ! application/x-rtp,media=video,encoding-name=H264,payload=96 ! "
                  "rtph264depay ! h264parse ! decodebin ! nvvidconv ! video/x-raw, fformat=RGBA ! "
                  "videoconvert name=mysource ! video/x-raw,format=BGRA ! fakesink";
#endif
        break;
    }
    case Type_Render:
    {
        // pipeStr = "gst-launch-1.0 rtspsrc location=rtsp://192.168.1.100/ch0/stream0 ! "
        //           "decodebin name=mysource ! autovideosink sync=false";

        pipeStr = "udpsrc port=20001 caps=\"application/x-rtp,media=(string)video,"
                  "clock-rate=(int)90000,encoding-name=(string)H264, payload=(int)96\" ! "
                  "rtph264depay ! decodebin ! videoconvert ! autovideosink sync=false";

        break;
    }
    case Type_Feeder:
    {

#ifdef _WIN32
        pipeStr = "videotestsrc pattern=black is-live=1 ! "
                  "video/x-raw,format=YUY2,width=720,height=576,framerate=30/1 ! "
                  "videoconvert ! x264enc ! h264parse ! rtph264pay ! tee name=t ! "
                  "queue ! udpsink host=127.0.0.1 port=20000 t. ! queue ! "
                  "udpsink host=127.0.0.1 port=20001";
#elif defined(__linux)
        pipeStr = "rtspsrc location=rtsp://192.168.1.100/ch0/stream0 "
                  "latency=100 protocols=udp ! tee name=t ! "
                  "queue ! udpsink host=127.0.0.1 port=20000 t. ! "
                  "queue ! udpsink host=127.0.0.1 port=20001";
#endif

        break;
    }
    }
#endif

    std::cerr << "\n pipeStr: "
              << pipeStr.toStdString()
              << std::endl;

    m_data.pipeline = gst_parse_launch(
                pipeStr.toLatin1().data(), NULL);

    // To get YUY2 buffer
    if (m_pipeType == Type_Detection)
    {
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
}

void VideoCapture::start()
{
    if (m_data.pipeline)
    {
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


