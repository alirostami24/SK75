#include "VideoCapture.h"

static VideoCapture* _myVideoCaptureInstance;
static Detector* _detector;

VideoCapture::
VideoCapture()
{
    //    QObject::connect(_detector, &Detector::sigAutoLockDetected,
    //                     this, &VideoCapture::sigAutoLockDetected);

    //    QObject::connect(_detector, &Detector::sigDetectionDataUpdated,
    //                     this, &VideoCapture::sltDetectionDataUpdated);
}

void VideoCapture::processNewFrame(guint8 *pData)
{
    cv::Mat frame = cv::Mat(_myVideoCaptureInstance->getFrameSize().height(),
                            _myVideoCaptureInstance->getFrameSize().width(),
                            CV_8UC3, (void*)(pData));

//    cv::Mat *bgrFrame = &_myVideoCaptureInstance->m_bgrFrame;

    if (_detector->isDetectorActivated())
    {
        _detector->detect(&frame);

        const auto objects = _detector->getAllDetectedObjects();

        for (auto item : objects)
        {
            cv::rectangle(frame,item.bbox,
                          cv::Scalar(0, 0, 255), 2);
        }

//        cv::Rect rect(40, 40, 50, 50);

//        cv::rectangle(frame, rect,
//                      cv::Scalar(0, 255, 255));
    }
}

void VideoCapture::sltDetectionDataUpdated()
{
    cv::Rect objectRect = _detector->getDetectedBoundingBox();
    // drawing result
}

GstFlowReturn
VideoCapture::on_new_sample_from_sink(GstElement* sink, gpointer user_data)
{
    VideoCapture* videoCapture =
            static_cast<VideoCapture*>(user_data);

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


        auto startTime = std::chrono::system_clock::now();

        processNewFrame(_data);

        auto stopTime = std::chrono::system_clock::now();

        videoCapture->m_frameBuffer =
                QByteArray((char*)_data, _size);

        std::cerr << "process time duration: " <<
                     std::chrono::duration_cast<std::chrono::milliseconds>(
                         stopTime - startTime).count() << std::endl;

        gst_buffer_unmap (buffer, &info);

        Q_EMIT videoCapture->sigNewFrameReceived();

        return GST_FLOW_OK;
    }

    return GST_FLOW_ERROR;
}


//GstPadProbeReturn
//VideoCapture::cb_have_data (GstPad          *pad,
//                            GstPadProbeInfo *info,
//                            gpointer         user_data)
//{
//    VideoCapture *videoCapture =
//            static_cast<VideoCapture*>(user_data);

//    gint x, y;
//    GstMapInfo map;
//    // guint16 *ptr, t;
//    guint16 *t;
//    guint8 *ptr=nullptr;
//    GstBuffer *buffer;

//    // g_print("In %s %d \n", __func__,cntr_val++);

//    buffer = GST_PAD_PROBE_INFO_BUFFER (info);

//    buffer = gst_buffer_make_writable (buffer);

//    /* Making a buffer writable can fail (for example if it
//   * cannot be copied and is used more than once)
//   */
//    if (buffer == NULL)
//    {
//        return GST_PAD_PROBE_OK;
//    }

//    /* Mapping a buffer can fail (non-writable) */
//    if (gst_buffer_map (buffer, &map, GST_MAP_WRITE))
//    {
//        ptr = (guint8 *) map.data;
//        processNewFrame((guint8 *)ptr);

//        gst_buffer_unmap (buffer, &map);
//    }

//    //    GST_PAD_PROBE_INFO_DATA (info) = buffer;

//    return GST_PAD_PROBE_OK;
//}


bool VideoCapture::
initialize()
{
    //    if (m_windowID == guintptr())
    //    {
    //        return false;
    //    }

    //    QString pipestr = "rtspsrc location=rtsp://192.168.1.100/ch0/stream0 ! "
    //                      "application/x-rtp,media=video,clockrate=90000,"
    //                      "encoding-name=H264,payload=96 ! rtph264depay ! "
    //                      "h264parse ! avdec_h264 ! videoconvert ! tee name=t ! "
    //                      "queue ! d3dvideosink sync=false name=mysink "
    //                      "force-aspect-ratio=false enable-navigation-events=false t. ! "
    //                      "queue ! appsink name=myfakesink sync=false";

    QString pipestr = "rtspsrc location=rtsp://192.168.1.100/ch0/stream0 latency=100 protocols=udp ! "
                      "rtpjitterbuffer latency=100 ! rtph264depay ! h264parse ! "
                      "nvv4l2decoder enable-max-performance=true ! nvvidconv ! "
                      "videoconvert name=mysource ! video/x-raw,format=BGR ! "
                      "appsink name=mysink sync=false";

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

    g_object_set(G_OBJECT(m_gstData.sink), "emit-signals", TRUE, "sync", FALSE, NULL);
    g_signal_connect(m_gstData.sink, "new-sample", G_CALLBACK(on_new_sample_from_sink), this);

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

void VideoCapture::enableDetecting(const bool &state)
{
    _detector->enableDetecting(state);
}

void VideoCapture::enableAutoLock(const bool &state)
{
    _detector->enableAutoLock(state);
}

QByteArray VideoCapture::
getFrameBuffer()
{
    return m_frameBuffer;
}

QSize VideoCapture::
getFrameSize() const
{
    return m_frameSize;
}

void VideoCapture::setFrameSize(const QSize &frameSize)
{
    m_frameSize = frameSize;

    m_bgrFrame = cv::Mat(frameSize.height(),
                         frameSize.width(),
                         CV_8UC3);

//    m_I420_Frame = cv::Mat(frameSize.height(),
//                           frameSize.width(),
//                           CV_8UC1);

    _myVideoCaptureInstance = this;

    _detector = new Detector();

    _detector->setInputSize(cv::Size(frameSize.width(), frameSize.height()));
}
