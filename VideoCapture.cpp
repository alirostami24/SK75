#include "VideoCapture.h"

static VideoCapture* _myVideoCaptureInstance;
static Detector* _detector;

VideoCapture::
VideoCapture()
{
    QObject::connect(_detector, &Detector::sigAutoLockDetected,
            this, &VideoCapture::sigAutoLockDetected);

    QObject::connect(_detector, &Detector::sigDetectionDataUpdated,
            this, &VideoCapture::sltDetectionDataUpdated);

}

void VideoCapture::processNewFrame(guint8 *pData)
{
    cv::Mat frame = cv::Mat(_myVideoCaptureInstance->getFrameSize().height() * 1.5, _myVideoCaptureInstance->getFrameSize().width(),
                            CV_8UC3, (void*)(pData));

    if (_detector->isDetectorActivated())
    {
        _detector->detect(&frame);
    }
}

void VideoCapture::sltDetectionDataUpdated()
{
    cv::Rect objectRect = _detector->getDetectedBoundingBox();
    // drawing result
}


GstPadProbeReturn
VideoCapture::cb_have_data (GstPad          *pad,
                       GstPadProbeInfo *info,
                       gpointer         user_data)
{
    gint x, y;
    GstMapInfo map;
    // guint16 *ptr, t;
    guint16 *t;
    guint8 *ptr=nullptr;
    GstBuffer *buffer;

    // g_print("In %s %d \n", __func__,cntr_val++);

    buffer = GST_PAD_PROBE_INFO_BUFFER (info);

    buffer = gst_buffer_make_writable (buffer);

    /* Making a buffer writable can fail (for example if it
   * cannot be copied and is used more than once)
   */
    if (buffer == NULL)
        return GST_PAD_PROBE_OK;

    /* Mapping a buffer can fail (non-writable) */
    if (gst_buffer_map (buffer, &map, GST_MAP_WRITE)) {

        ptr = (guint8 *) map.data;
        processNewFrame((guint8 *)ptr);

        gst_buffer_unmap (buffer, &map);

    }

    GST_PAD_PROBE_INFO_DATA (info) = buffer;

    return GST_PAD_PROBE_OK;
}


bool VideoCapture::
initialize()
{
    QString pipestr = "rtspsrc location=rtsp://192.168.1.100/ch0/stream0 ! "
                      "application/x-rtp,media=video,clockrate=90000,"
                      "encoding-name=H264,payload=96 ! rtph264depay ! "
                      "h264parse ! queue ! avdec_h264 ! videoconvert name=mysource ! "
                      "xvimagesink sync=false name=mysink "
                      "force-aspect-ratio=false enable-navigation-events=false";

    GError *error = NULL;
    m_gstData.pipeline = gst_parse_launch(
                pipestr.toLatin1().data(), &error);

    if (error != NULL)
    {
        qCritical() << "Cannot initialize pipeline.\n"
                    << int(error->code) << error->message;

        return false;
    }

    m_gstData.conversion = gst_bin_get_by_name (GST_BIN(m_gstData.pipeline), "mysource");


    // This section is added to pull out the data buffer for pushing into image processing
    m_gstData.pad = gst_element_get_static_pad (m_gstData.conversion, "src");
    gst_pad_add_probe (m_gstData.pad, GST_PAD_PROBE_TYPE_BUFFER,
                       (GstPadProbeCallback) cb_have_data, NULL, NULL);
    gst_object_unref (m_gstData.pad);

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

QSize VideoCapture::
getFrameSize() const
{
    return m_frameSize;
}

void VideoCapture::setFrameSize(const QSize &frameSize)
{
    m_frameSize = frameSize;

    _myVideoCaptureInstance = this;

    _detector->setInputSize(cv::Size(frameSize.width(), frameSize.height()));
}
