#ifndef V4L2CAP_H
#define V4L2CAP_H

#include "iputool.h"
#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <sys/time.h>
#include <linux/mxc_v4l2.h>


//摄像头支持的分辨率为640X480,320X240,720X480,720X576,1280X720
//1920X1080 2592X1944 176X144 1024X768
#define IMGWIDTH 640
#define IMGHEIGHT 480
class V4l2cap : public QObject
{
    Q_OBJECT
public:
    explicit V4l2cap(QObject *parent = 0);
    ~V4l2cap();
    int getYuv(void);
    int stopCapturing(void);
    Iputool * dataConverter;
//    unsigned char * yuv_buffer_pointer;
    int imgWidth;
    int imgHeight;

signals:

private:
//    char v4l_capture_dev[100];
//    char v4l_output_dev[100];
    int fd_capture_v4l;
    int fd_output_v4l;
    int fd_fb;
    int g_cap_mode;
    int g_input;
    int g_fmt;
    int g_rotate;
    int g_vflip;
    int g_hflip;
    int g_vdi_enable;
    int g_vdi_motion;
    int g_tb;
    int g_output;
    int g_output_num_buffers;
    int g_capture_num_buffers;
    int g_in_width;
    int g_in_height;
    int g_display_width;
    int g_display_height;
    int g_display_top;
    int g_display_left;
    int g_frame_size;
    int g_frame_period;
    v4l2_std_id g_current_std;
    FILE * file_out;
 //   struct v4l2_queryctrl queryctrl;
 //   struct v4l2_querymenu querymenu;


    struct testbuffer
    {
        unsigned char *start;
        size_t offset;
        unsigned int length;
    };

    struct testbuffer output_buffers[4];
    struct testbuffer capture_buffers[3];

   int startCapturing(void);
   int prepareOutput(void);
   int v4lCaptureSetup(void);
   int v4lOutputSetup(void);
 //  int mxc_v4l_tvin_test(void);
   int defaultSet(void);
 //  void enumerate_menu (int fd_v4l);
  // void EnumerAllControls(int fd_v4l);


public slots:
};

#endif // V4L2CAP_H
