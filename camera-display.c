#include <gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *convert, *sink;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    /* khởi tạo GStreamer */
    gst_init(&argc, &argv);

    /* Tạo các phần tử trong pipeline */
    source = gst_element_factory_make("v4l2src", "source");
    convert = gst_element_factory_make("videoconvert", "convert");
    sink = gst_element_factory_make("autovideosink", "sink");

    /* Tạo pipeline mới có tên là "camera-pipeline". */
    pipeline = gst_pipeline_new("camera-pipeline");

    /* Thêm các phần tử vào pipeline  */
    gst_bin_add_many(GST_BIN(pipeline), source, convert, sink, NULL);

    /* Liên kết các element với nhau để tạo pipeline. */
    if (!gst_element_link_many(source, convert, sink, NULL)) {
        g_printerr("Failed to link elements\n");
        return -1;
    }

    /* Thiết lập trạng thái của pipeline thành "playing" (đang chạy) */
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to set pipeline to playing state\n");
        return -1;
    }
    
    /* Chờ đến khi xảy ra lỗi hoặc kết thúc stream (EOS) */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                     GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Giải phóng tài nguyên và đưa pipeline về trạng thái "NULL". */
    if (msg != NULL) {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
