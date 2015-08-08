#ifndef IPUTOOL_H
#define IPUTOOL_H

#include <QObject>

#include <stdint.h>
#include <linux/ipu.h>
#include <linux/mxcfb.h>
class Iputool : public QObject
{
    Q_OBJECT
public:
    explicit Iputool(QObject *parent = 0);
    ~Iputool();

     int start_ipu(void);
     unsigned char *yuvSourceData;
     unsigned char *rgbData;
     int prepare_ipu(int argc, char *argv[]);
     int ipuWidth;
     int ipuHeight;

signals:

public slots:

private:

     enum errType{
         Err1,Err2,Err3,Err4,Err5,Err6,Err7,Err8,Err9,Err10,Err11,Err12,Err13
     };
    errType ipuErr;
    typedef struct {
            struct ipu_task task;
            int fcount;
            int loop_cnt;
            int show_to_fb;
            char outfile[128];
    } ipu_test_handle_t;

     int ctrl_c_rev;
     char * options;
    ipu_test_handle_t test_handle;
    struct ipu_task *t;
    int ret;
    //struct sigaction act;
        //FILE * file_in = NULL;
        FILE * file_out;
    struct timeval begin, end;
    int sec, usec;
    int fd_ipu, fd_fb;
    int isize, ovsize;
    int alpsize, osize;
    void *inbuf, *vdibuf;
    void *ovbuf, *alpbuf;
   //     void *rgbData = NULL;
    dma_addr_t outpaddr;
    struct fb_var_screeninfo fb_var;
    struct fb_fix_screeninfo fb_fix;
    int blank;
    char *p[12];

    void util_help();
    char * skip_unwanted(char *ptr);
    int parse_options(char *buf, ipu_test_handle_t *test_handle);
    int parse_config_file(char *file_name, ipu_test_handle_t *test_handle);
   int parse_cmd_input(int argc, char ** argv, ipu_test_handle_t *test_handle);
  // void ctrl_c_handler(int signum, siginfo_t *info, void *myact);
   int process_cmdline(int argc, char **argv, ipu_test_handle_t *test_handle);
   static unsigned int fmt_to_bpp(unsigned int pixelformat);
   static void dump_ipu_task(struct ipu_task *t);




};

#endif // IPUTOOL_H
