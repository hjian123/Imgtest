#include "iputool.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>


#define PAGE_ALIGN(x) (((x) + 4095) & ~4095)
#define MAX_PATH	128
#define deb_msg 1
#if deb_msg
#define deb_printf printf
#else
#define deb_printf
#endif
Iputool::Iputool(QObject *parent) : QObject(parent)
{
    ctrl_c_rev=0;
    options = "p:d:t:c:l:i:o:O:s:f:h";
    yuvSourceData=NULL;
    rgbData=NULL;
    p[0]="0";
    p[1]="-c";
    p[2]="1";
    p[3]="-i";
    p[4]="640,480,YUYV,0,0,0,0,0,0";
    p[5]="-O";
    p[6]="640,480,RGBP,0,0,0,0,0";
    p[7]="-s";
    p[8]="0";
    p[9]="-f";
    p[10]="cc.rgb";
    p[11]="wall-1024x768-565.yuv";
    prepare_ipu(12,p);
}

Iputool::~Iputool()
{

}

void Iputool::util_help()
{
    printf("options: \n\r");
    printf("p:d:t:c:l:i:o:O:s:f:h \r\n");
    printf("p: priority\r\n");
    printf("d: tak id\r\n");
    printf("t: timeout\r\n");
    printf("c: frame count\r\n");
    printf("l: loop count\r\n");
    printf("i: input width,height,format,crop pos.x, pos.y,w,h,deinterlace.enable,deinterlace.motion\r\n");
    printf("o: overlay enable,w,h,format,crop x,y,width,height,alpha mode,alpha value,colorkey enable, color key value\r\n");
    printf("O: output width,height,format,rotation, crop pos.x,pos.y,w,h\r\n");
    printf("s: output to fb enable\r\n");
    printf("f: output file name\r\n");
}

char *Iputool::skip_unwanted(char *ptr)
{
        int i = 0;
        static char buf[MAX_PATH];
        while (*ptr != '\0') {
                if (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') {
                        ptr++;
                        continue;
                }

                if (*ptr == '#')
                        break;

                buf[i++] = *ptr;
                ptr++;
        }

        buf[i] = 0;
        return (buf);
}

int Iputool::parse_options(char *buf, Iputool::ipu_test_handle_t *test_handle)
{
        struct ipu_task *t = &test_handle->task;
        char *str;

        /* general */
        str = strstr(buf, "priority");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->priority = strtol(str, NULL, 10);
                                printf("priority\t= %d\n", t->priority);
                        }
                        return 0;
                }
        }
        str = strstr(buf, "task_id");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->task_id = strtol(str, NULL, 10);
                                printf("task_id\t\t= %d\n", t->task_id);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "timeout");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->timeout = strtol(str, NULL, 10);
                                printf("timeout\t\t= %d\n", t->timeout);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "fcount");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                test_handle->fcount = strtol(str, NULL, 10);
                                printf("fcount\t\t= %d\n", test_handle->fcount);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "loop_cnt");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                test_handle->loop_cnt = strtol(str, NULL, 10);
                                printf("loop_cnt\t= %d\n", test_handle->loop_cnt);
                        }
                        return 0;
                }
        }

        /* input */
        str = strstr(buf, "in_width");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->input.width = strtol(str, NULL, 10);
                                printf("in_width\t= %d\n", t->input.width);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "in_height");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->input.height = strtol(str, NULL, 10);
                                printf("in_height\t= %d\n", t->input.height);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "in_fmt");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->input.format =
                                        v4l2_fourcc(str[0], str[1], str[2], str[3]);
                                printf("in_fmt\t\t= %s\n", str);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "in_posx");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->input.crop.pos.x =
                                        strtol(str, NULL, 10);
                                printf("in_posx\t\t= %d\n",
                                        t->input.crop.pos.x);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "in_posy");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->input.crop.pos.y =
                                        strtol(str, NULL, 10);
                                printf("in_posy\t\t= %d\n",
                                        t->input.crop.pos.y);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "in_crop_w");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->input.crop.w =
                                        strtol(str, NULL, 10);
                                printf("in_crop_w\t= %d\n",
                                        t->input.crop.w);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "in_crop_h");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->input.crop.h =
                                        strtol(str, NULL, 10);
                                printf("in_crop_h\t= %d\n",
                                        t->input.crop.h);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "deinterlace_en");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->input.deinterlace.enable =
                                        strtol(str, NULL, 10);
                                printf("deinterlace_en\t= %d\n",
                                        t->input.deinterlace.enable);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "motion_sel");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->input.deinterlace.motion =
                                        strtol(str, NULL, 10);
                                printf("motion_sel\t= %d\n",
                                        t->input.deinterlace.motion);
                        }
                        return 0;
                }
        }

        /* overlay */
        str = strstr(buf, "overlay_en");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay_en = strtol(str, NULL, 10);
                                printf("overlay_en\t= %d\n", t->overlay_en);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "ov_width");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.width = strtol(str, NULL, 10);
                                printf("ov_width\t= %d\n", t->overlay.width);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "ov_height");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.height = strtol(str, NULL, 10);
                                printf("ov_height\t= %d\n", t->overlay.height);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "ov_fmt");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.format =
                                        v4l2_fourcc(str[0], str[1], str[2], str[3]);
                                printf("ov_fmt\t\t= %s\n", str);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "ov_posx");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.crop.pos.x =
                                        strtol(str, NULL, 10);
                                printf("ov_posx\t\t= %d\n",
                                        t->overlay.crop.pos.x);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "ov_posy");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.crop.pos.y =
                                        strtol(str, NULL, 10);
                                printf("ov_posy\t\t= %d\n",
                                        t->overlay.crop.pos.y);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "ov_crop_w");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.crop.w =
                                        strtol(str, NULL, 10);
                                printf("ov_crop_w\t= %d\n",
                                        t->overlay.crop.w);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "ov_crop_h");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.crop.h =
                                        strtol(str, NULL, 10);
                                printf("ov_crop_h\t= %d\n",
                                        t->overlay.crop.h);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "alpha_mode");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.alpha.mode =
                                        strtol(str, NULL, 10);
                                printf("alpha_mode\t= %d\n",
                                        t->overlay.alpha.mode);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "alpha_value");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.alpha.gvalue =
                                        strtol(str, NULL, 10);
                                printf("alpha_value\t= %d\n",
                                        t->overlay.alpha.gvalue);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "colorkey_en");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.colorkey.enable =
                                        strtol(str, NULL, 10);
                                printf("colorkey_en\t= %d\n",
                                        t->overlay.colorkey.enable);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "colorkey_value");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->overlay.colorkey.value =
                                        strtol(str, NULL, 16);
                                printf("colorkey_value\t= 0x%x\n",
                                        t->overlay.colorkey.value);
                        }
                        return 0;
                }
        }

        /* output */
        str = strstr(buf, "out_width");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->output.width = strtol(str, NULL, 10);
                                printf("out_width\t= %d\n", t->output.width);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "out_height");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->output.height = strtol(str, NULL, 10);
                                printf("out_height\t= %d\n", t->output.height);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "out_fmt");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->output.format =
                                        v4l2_fourcc(str[0], str[1], str[2], str[3]);
                                printf("out_fmt\t\t= %s\n", str);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "out_rot");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->output.rotate = strtol(str, NULL, 10);
                                printf("out_rot\t\t= %d\n", t->output.rotate);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "out_posx");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->output.crop.pos.x = strtol(str, NULL, 10);
                                printf("out_posx\t= %d\n", t->output.crop.pos.x);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "out_posy");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->output.crop.pos.y = strtol(str, NULL, 10);
                                printf("out_posy\t= %d\n", t->output.crop.pos.y);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "out_crop_w");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->output.crop.w =
                                        strtol(str, NULL, 10);
                                printf("out_crop_w\t= %d\n",
                                        t->output.crop.w);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "out_crop_h");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                t->output.crop.h =
                                        strtol(str, NULL, 10);
                                printf("out_crop_h\t= %d\n",
                                        t->output.crop.h);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "out_to_fb");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                test_handle->show_to_fb = strtol(str, NULL, 10);
                                printf("out_to_fb\t= %d\n", test_handle->show_to_fb);
                        }
                        return 0;
                }
        }

        str = strstr(buf, "out_filename");
        if (str != NULL) {
                str = index(buf, '=');
                if (str != NULL) {
                        str++;
                        if (*str != '\0') {
                                memcpy(test_handle->outfile, str, strlen(str));
                                printf("out_filename\t= %s\n", test_handle->outfile);
                        }
                        return 0;
                }
        }

        return 0;
}

int Iputool::parse_config_file(char *file_name, Iputool::ipu_test_handle_t *test_handle)
{
        FILE *fp;
        char line[128];
        char *ptr;

        fp = fopen(file_name, "r");
        if (fp == NULL) {
                printf("Failed to open config file\n");
                return -1;
        }

        printf("\nGet config from config file %s:\n\n", file_name);

        while (fgets(line, MAX_PATH, fp) != NULL) {
                ptr = skip_unwanted(line);
                parse_options(ptr, test_handle);
        }

        printf("\n");

        fclose(fp);
        return 0;
}


int Iputool::parse_cmd_input(int argc, char **argv, Iputool::ipu_test_handle_t *test_handle){
        char opt;
        char fourcc[5];
        struct ipu_task *t = &test_handle->task;

        printf("pass cmdline %d, %s\n", argc, argv[0]);

        /*default settings*/
        t->priority = 0;
        t->task_id = 0;
        t->timeout = 1000;
        test_handle->fcount = 50;
        test_handle->loop_cnt = 1;
        t->input.width = 320;
        t->input.height = 240;
        t->input.format =  v4l2_fourcc('I', '4','2', '0');
        t->input.crop.pos.x = 0;
        t->input.crop.pos.y = 0;
        t->input.crop.w = 0;
        t->input.crop.h = 0;
        t->input.deinterlace.enable = 0;
        t->input.deinterlace.motion = 0;

        t->overlay_en = 0;
        t->overlay.width = 320;
        t->overlay.height = 240;
        t->overlay.format = v4l2_fourcc('I', '4','2', '0');
        t->overlay.crop.pos.x = 0;
        t->overlay.crop.pos.y = 0;
        t->overlay.crop.w = 0;
        t->overlay.crop.h = 0;
        t->overlay.alpha.mode = 0;
        t->overlay.alpha.gvalue = 0;
        t->overlay.colorkey.enable = 0;
        t->overlay.colorkey.value = 0x555555;

        t->output.width = 1024;
        t->output.height = 768;
        t->output.format = v4l2_fourcc('U', 'Y','V', 'Y');
        t->output.rotate = 180;
        t->output.crop.pos.x = 0;
        t->output.crop.pos.y = 0;
        t->output.crop.w = 0;
        t->output.crop.h = 0;

        test_handle->show_to_fb = 1;
        memcpy(test_handle->outfile,"ipu0-1st-ovfb",13);

        while((opt = getopt(argc, argv, options)) > 0)    //char * options = "p:d:t:c:l:i:o:O:s:f:h";
        {
                deb_printf("\nnew option : %c \n", opt);
                switch(opt)
                {
                        case 'p':/*priority*/
                                if(NULL == optarg)
                                        break;
                                t->priority = strtol(optarg, NULL, 10);
                                deb_printf("priority set %d \n",t->priority);
                                break;
                        case 'd': /*task id*/
                                if(NULL == optarg)
                                        break;
                                t->task_id = strtol(optarg, NULL, 10);
                                deb_printf("task id set %d \n",t->task_id);
                                break;
                        case 't':
                                if(NULL == optarg)
                                        break;
                                t->timeout = strtol(optarg, NULL, 10);
                        case 'c':
                                if(NULL == optarg)
                                        break;
                                test_handle->fcount = strtol(optarg, NULL, 10);
                                deb_printf("frame count set %d \n",test_handle->fcount);
                                break;
                        case 'l':
                                if(NULL == optarg)
                                        break;
                                test_handle->loop_cnt = strtol(optarg, NULL, 10);
                                deb_printf("loop count set %d \n",test_handle->loop_cnt);
                                break;
                        case 'i': /*input param*/
                                if(NULL == optarg)
                                        break;
                                memset(fourcc,0,sizeof(fourcc));
                                sscanf(optarg,"%d,%d,%c%c%c%c,%d,%d,%d,%d,%d,%d",
                                                &(t->input.width),&(t->input.height),
                                                &(fourcc[0]),&(fourcc[1]), &(fourcc[2]), &(fourcc[3]),
                                                &(t->input.crop.pos.x),&(t->input.crop.pos.y),
                                                &(t->input.crop.w), &(t->input.crop.h),
                                                (int *)&(t->input.deinterlace.enable), (int *)&(t->input.deinterlace.motion));
                                t->input.format = v4l2_fourcc(fourcc[0], fourcc[1],
                                                fourcc[2], fourcc[3]);
                                deb_printf("input w=%d,h=%d,fucc=%s,cpx=%d,cpy=%d,cpw=%d,cph=%d,de=%d,dm=%d\n",
                                                t->input.width,t->input.height,
                                                fourcc,t->input.crop.pos.x,t->input.crop.pos.y,
                                                t->input.crop.w, t->input.crop.h,
                                                t->input.deinterlace.enable, t->input.deinterlace.motion);
                                break;
                        case 'o':/*overlay setting*/
                                if(NULL == optarg)
                                        break;
                                memset(fourcc,0,sizeof(fourcc));
                                sscanf(optarg,"%d,%d,%d,%c%c%c%c,%d,%d,%d,%d,%d,%d,%d,%x",
                                                (int *)&(t->overlay_en),&(t->overlay.width),&(t->overlay.height),
                                                &(fourcc[0]),&(fourcc[1]), &(fourcc[2]), &(fourcc[3]),
                                                &(t->overlay.crop.pos.x),&(t->overlay.crop.pos.y),
                                                &(t->overlay.crop.w), &(t->overlay.crop.h),
                                                (int *)&(t->overlay.alpha.mode), (int *)&(t->overlay.alpha.gvalue),
                                                (int *)&(t->overlay.colorkey.enable),&(t->overlay.colorkey.value));
                                t->overlay.format = v4l2_fourcc(fourcc[0], fourcc[1],
                                                fourcc[2], fourcc[3]);
                                deb_printf("overlay en=%d,w=%d,h=%d,fourcc=%c%c%c%c,cpx=%d,\
                                                cpy=%d,cw=%d,ch=%d,am=%c,ag=%d,ce=%d,cv=%x\n",
                                                t->overlay_en, t->overlay.width,t->overlay.height,
                                                fourcc[0],fourcc[1], fourcc[2], fourcc[3],
                                                t->overlay.crop.pos.x,t->overlay.crop.pos.y,
                                                t->overlay.crop.w, t->overlay.crop.h,
                                                t->overlay.alpha.mode, t->overlay.alpha.gvalue,
                                                t->overlay.colorkey.enable,t->overlay.colorkey.value);
                                break;
                        case 'O':/*output setting*/
                                memset(fourcc,0,sizeof(fourcc));
                                if(NULL == optarg)
                                        break;
                                sscanf(optarg,"%d,%d,%c%c%c%c,%d,%d,%d,%d,%d",
                                                &(t->output.width),&(t->output.height),
                                                &(fourcc[0]),&(fourcc[1]), &(fourcc[2]), &(fourcc[3]),
                                                (int *)&(t->output.rotate),&(t->output.crop.pos.x),
                                                &(t->output.crop.pos.y),&(t->output.crop.w),
                                                &(t->output.crop.h));
                                t->output.format = v4l2_fourcc(fourcc[0],
                                                fourcc[1],fourcc[2], fourcc[3]);
                                deb_printf(optarg,"%d,%d,%s,%d,%d,%d,%d,%d\n",
                                                t->output.width,t->output.height,
                                                fourcc,t->output.rotate,t->output.crop.pos.x,
                                                t->output.crop.pos.y,t->output.crop.w,
                                                t->output.crop.h);
                                break;
                        case 's':/*fb setting*/
                                if(NULL == optarg)
                                        break;
                                test_handle->show_to_fb = strtol(optarg, NULL, 10);
                                deb_printf("show to fb %d\n", test_handle->show_to_fb);
                                break;
                        case 'f':/*output0 file name*/
                                if(NULL == optarg)
                                        break;
                                memset(test_handle->outfile,0,sizeof(test_handle->outfile));
                                sscanf(optarg,"%s",test_handle->outfile);
                                deb_printf("output file name %s \n",test_handle->outfile);
                                break;
                        case 'h':
                                util_help();
                                break;
                        default:
                                return 0;
                }
        }
        return 0;
}

//void Iputool::ctrl_c_handler(int signum, siginfo_t *info, void *myact)
//{
//    ctrl_c_rev = 1;
//}
int Iputool::process_cmdline(int argc, char **argv, Iputool::ipu_test_handle_t *test_handle){
    int i;
    int pre_set = 0;
    struct ipu_task *t = &test_handle->task;

    if (argc == 1)
        return -1;

    for (i = 1; i < argc; i++)
        if (strcmp(argv[i], "-C") == 0)
        {
            pre_set = 1;
            parse_config_file(argv[++i], test_handle);
    }

     if(pre_set == 0)
         parse_cmd_input(argc,argv,test_handle);

    if ((t->input.width == 0) || (t->input.height == 0) ||
            (t->output.width == 0) ||
            (t->output.height == 0)
            || (test_handle->fcount < 1))
        return -1;

    return 0;
}

unsigned int Iputool::fmt_to_bpp(unsigned int pixelformat){
        unsigned int bpp;

        switch (pixelformat)
        {
                case IPU_PIX_FMT_RGB565:
                /*interleaved 422*/
                case IPU_PIX_FMT_YUYV:
                case IPU_PIX_FMT_UYVY:
                /*non-interleaved 422*/
                case IPU_PIX_FMT_YUV422P:
                case IPU_PIX_FMT_YVU422P:
                        bpp = 16;
                        break;
                case IPU_PIX_FMT_BGR24:
                case IPU_PIX_FMT_RGB24:
                case IPU_PIX_FMT_YUV444:
                case IPU_PIX_FMT_YUV444P:
                        bpp = 24;
                        break;
                case IPU_PIX_FMT_BGR32:
                case IPU_PIX_FMT_BGRA32:
                case IPU_PIX_FMT_RGB32:
                case IPU_PIX_FMT_RGBA32:
                case IPU_PIX_FMT_ABGR32:
                        bpp = 32;
                        break;
                /*non-interleaved 420*/
                case IPU_PIX_FMT_YUV420P:
                case IPU_PIX_FMT_YVU420P:
                case IPU_PIX_FMT_YUV420P2:
                case IPU_PIX_FMT_NV12:
        case IPU_PIX_FMT_TILED_NV12:
                        bpp = 12;
                        break;
                default:
                        bpp = 8;
                        break;
        }
        return bpp;
}

void Iputool::dump_ipu_task(ipu_task *t){
    printf("====== ipu task ======\n");
    printf("input:\n");
    printf("\tforamt: 0x%x\n", t->input.format);
    printf("\twidth: %d\n", t->input.width);
    printf("\theight: %d\n", t->input.height);
    printf("\tcrop.w = %d\n", t->input.crop.w);
    printf("\tcrop.h = %d\n", t->input.crop.h);
    printf("\tcrop.pos.x = %d\n", t->input.crop.pos.x);
    printf("\tcrop.pos.y = %d\n", t->input.crop.pos.y);
    if (t->input.deinterlace.enable) {
        printf("deinterlace enabled with:\n");
        if (t->input.deinterlace.motion != HIGH_MOTION)
            printf("\tlow/medium motion\n");
        else
            printf("\thigh motion\n");
    }
    printf("output:\n");
    printf("\tforamt: 0x%x\n", t->output.format);
    printf("\twidth: %d\n", t->output.width);
    printf("\theight: %d\n", t->output.height);
    printf("\troate: %d\n", t->output.rotate);
    printf("\tcrop.w = %d\n", t->output.crop.w);
    printf("\tcrop.h = %d\n", t->output.crop.h);
    printf("\tcrop.pos.x = %d\n", t->output.crop.pos.x);
    printf("\tcrop.pos.y = %d\n", t->output.crop.pos.y);
    if (t->overlay_en) {
        printf("overlay:\n");
        printf("\tforamt: 0x%x\n", t->overlay.format);
        printf("\twidth: %d\n", t->overlay.width);
        printf("\theight: %d\n", t->overlay.height);
        printf("\tcrop.w = %d\n", t->overlay.crop.w);
        printf("\tcrop.h = %d\n", t->overlay.crop.h);
        printf("\tcrop.pos.x = %d\n", t->overlay.crop.pos.x);
        printf("\tcrop.pos.y = %d\n", t->overlay.crop.pos.y);
        if (t->overlay.alpha.mode == IPU_ALPHA_MODE_LOCAL)
            printf("combine with local alpha\n");
        else
            printf("combine with global alpha %d\n", t->overlay.alpha.gvalue);
        if (t->overlay.colorkey.enable)
            printf("colorkey enabled with 0x%x\n", t->overlay.colorkey.value);
    }
}

int Iputool::start_ipu(void)
{
    int done_cnt = 0;
    int done_loop = 0;
    int total_cnt = 0;
    int run_time = 0;
task_begin:
    if (t->input.deinterlace.enable &&
                (t->input.deinterlace.motion != HIGH_MOTION)){
               printf("Memcpy from yuvSourceData to vdibuf\n");
               memcpy(vdibuf, yuvSourceData, isize);
               printf("Memcpy from yuvSourceData to vdibuf finished\n");
               ret = 0;
        }
//		if (fread(vdibuf, 1, isize, file_in) < isize) {
//			ret = 0;
//			printf("Can not read enough data from input file\n");
//			goto err13;
//		}


    while((done_cnt < test_handle.fcount) && (ctrl_c_rev == 0)) {
       // gettimeofday(&begin, NULL);
//         printf("get yuvSourceData!!!!!\n");
        if (t->input.deinterlace.enable &&
            (t->input.deinterlace.motion != HIGH_MOTION)) {

                        memcpy(inbuf, vdibuf, isize);
                        printf("Memcpy from yuvSourceData to vdibuf\n");
                        memcpy(vdibuf, yuvSourceData, isize);
                        printf("Memcpy from yuvSourceData to vdibuf finished\n");
        } else{
                memcpy(inbuf, yuvSourceData, isize);
        }
//		if (ret < isize) {
//			ret = 0;
//			printf("Can not read enough data from input file\n");
//			break;
//		}

        if (test_handle.show_to_fb)
            t->output.paddr = outpaddr;

        ret = ioctl(fd_ipu, IPU_QUEUE_TASK, t);
        if (ret < 0) {
            printf("ioct IPU_QUEUE_TASK fail\n");
            break;
        }

        if (test_handle.show_to_fb) {
            ret = ioctl(fd_fb, FBIOPAN_DISPLAY, &fb_var);
            if (ret < 0) {
                printf("fb ioct FBIOPAN_DISPLAY fail\n");
                break;
            }
        } else {
              //         printf("Copy rgbData to rgbData  osize%d\n",osize);

                 //      rgbData=(unsigned char *)rgbData;
          //            printf("rgbData addr:%p,value:%d\n",rgbData,*(rgbData+100));
             //          ret = fwrite(rgbData, 1, osize, file_out);
                       // memcpy(rgbData,rgbData,osize);
            //           printf("Copy finished\n");
//			if (ret < osize) {
//				ret = -1;
//				printf("Can not write enough data into output file\n");
//				break;
//			}
        }
        done_cnt++;
        total_cnt++;

  //      gettimeofday(&end, NULL);
  //      sec = end.tv_sec - begin.tv_sec;
  //      usec = end.tv_usec - begin.tv_usec;

        if (usec < 0) {
            sec--;
            usec = usec + 1000000;
        }
        run_time += (sec * 1000000) + usec;
    }

    if (ret >= 0) {
        done_loop++;
        if ((done_loop < test_handle.loop_cnt) && (ctrl_c_rev == 0)) {
            done_cnt = 0;
                //	fseek(file_in, 0L, SEEK_SET);
            goto task_begin;
        }
    }

//    printf("total frame count %d avg frame time %d us, fps %f\n",
//            total_cnt, run_time/total_cnt, total_cnt/(run_time/1000000.0));
err0:
  //  printf("no err occer!!!");
    return ret;
}

int Iputool::prepare_ipu(int argc, char *argv[])
{
    t = &test_handle.task;
    ret = 0;  
    file_out = NULL;
    fd_ipu = 0, fd_fb = 0;
    isize = 0, ovsize = 0;
    alpsize = 0, osize = 0;
    inbuf = NULL, vdibuf = NULL;
    ovbuf = NULL, alpbuf = NULL;


    memset(&test_handle, 0, sizeof(ipu_test_handle_t));

    if (process_cmdline(argc, argv, &test_handle) < 0) {
        printf("\nMXC IPU device Test\n\n" \
            "Usage: %s -C <config file> <input raw file>\n",
            argv[0]);
        return -1;
    }

        //file_in = fopen(argv[argc-1], "rb");
//	if (file_in == NULL){
//		printf("there is no such file for reading %s\n", argv[argc-1]);
//		ret = -1;
//		goto err0;
//	}

    fd_ipu = open("/dev/mxc_ipu", O_RDWR, 0);
    if (fd_ipu < 0) {
        printf("open ipu dev fail\n");
        ret = -1;
        ipuErr=Err1;
        goto err1;
    }

    if (IPU_PIX_FMT_TILED_NV12F == t->input.format) {
        isize = PAGE_ALIGN(t->input.width * t->input.height/2) +
            PAGE_ALIGN(t->input.width * t->input.height/4);
        isize = t->input.paddr = isize * 2;
    } else
        isize = t->input.paddr =
            t->input.width * t->input.height
            * fmt_to_bpp(t->input.format)/8;
    ret = ioctl(fd_ipu, IPU_ALLOC, &t->input.paddr);
    if (ret < 0) {
        printf("ioctl IPU_ALLOC fail\n");
        ipuErr=Err2;
        goto err2;
    }
   // printf("mmap isize %d:\n",isize);
    inbuf = mmap(0, isize, PROT_READ | PROT_WRITE,
        MAP_SHARED, fd_ipu, t->input.paddr);
    if (!inbuf) {
        printf("mmap fail\n");
        ret = -1;
        ipuErr=Err3;
        goto err3;
    }

    if (t->input.deinterlace.enable &&
        (t->input.deinterlace.motion != HIGH_MOTION)) {
        t->input.paddr_n = isize;
        ret = ioctl(fd_ipu, IPU_ALLOC, &t->input.paddr_n);
        if (ret < 0) {
            printf("ioctl IPU_ALLOC fail\n");
            ipuErr=Err4;
            goto err4;
        }
        vdibuf = mmap(0, isize, PROT_READ | PROT_WRITE,
                MAP_SHARED, fd_ipu, t->input.paddr_n);
        if (!vdibuf) {
            printf("mmap fail\n");
            ret = -1;
            ipuErr=Err5;
            goto err5;
        }
    }

    if (t->overlay_en) {
        ovsize = t->overlay.paddr =
            t->overlay.width * t->overlay.height
            * fmt_to_bpp(t->overlay.format)/8;
        ret = ioctl(fd_ipu, IPU_ALLOC, &t->overlay.paddr);
        if (ret < 0) {
            printf("ioctl IPU_ALLOC fail\n");
            ipuErr=Err6;
            goto err6;
        }
        ovbuf = mmap(0, ovsize, PROT_READ | PROT_WRITE,
                MAP_SHARED, fd_ipu, t->overlay.paddr);
        if (!ovbuf) {
            printf("mmap fail\n");
            ret = -1;
            ipuErr=Err7;
            goto err7;
        }

        /*fill overlay buffer with dedicated data*/
        memset(ovbuf, 0x00, ovsize/4);
        memset(ovbuf+ovsize/4, 0x55, ovsize/4);
        memset(ovbuf+ovsize/2, 0xaa, ovsize/4);
        memset(ovbuf+ovsize*3/4, 0xff, ovsize/4);

        if (t->overlay.alpha.mode == IPU_ALPHA_MODE_LOCAL) {
            alpsize = t->overlay.alpha.loc_alp_paddr =
                t->overlay.width * t->overlay.height;
            ret = ioctl(fd_ipu, IPU_ALLOC, &t->overlay.alpha.loc_alp_paddr);
            if (ret < 0) {
                printf("ioctl IPU_ALLOC fail\n");
                ipuErr=Err8;
                goto err8;
            }
            alpbuf = mmap(0, alpsize, PROT_READ | PROT_WRITE,
                    MAP_SHARED, fd_ipu, t->overlay.alpha.loc_alp_paddr);
            if (!alpbuf) {
                printf("mmap fail\n");
                ret = -1;
                ipuErr=Err9;
                goto err9;
            }

            /*fill loc alpha buffer with dedicated data*/
            memset(alpbuf, 0x00, alpsize/4);
            memset(alpbuf+alpsize/4, 0x55, alpsize/4);
            memset(alpbuf+alpsize/2, 0xaa, alpsize/4);
            memset(alpbuf+alpsize*3/4, 0xff, alpsize/4);
        }
    }

    if (test_handle.show_to_fb) {
        int found = 0, i;
        char fb_dev[] = "/dev/fb0";
        char fb_name[16];

        if (!strcmp(test_handle.outfile, "ipu0-1st-ovfb"))
            memcpy(fb_name, "DISP3 FG", 9);
        if (!strcmp(test_handle.outfile, "ipu0-2nd-fb"))
            memcpy(fb_name, "DISP3 BG - DI1", 15);
        if (!strcmp(test_handle.outfile, "ipu1-1st-ovfb"))
            memcpy(fb_name, "DISP4 FG", 9);
        if (!strcmp(test_handle.outfile, "ipu1-2nd-fb"))
            memcpy(fb_name, "DISP4 BG - DI1", 15);

        for (i=0; i<5; i++) {
            fb_dev[7] = '0';
            fb_dev[7] += i;
            fd_fb = open(fb_dev, O_RDWR, 0);
            if (fd_fb > 0) {
                ioctl(fd_fb, FBIOGET_FSCREENINFO, &fb_fix);
                if (!strcmp(fb_fix.id, fb_name)) {
                    printf("found fb dev %s\n", fb_dev);
                    found = 1;
                    break;
                } else
                    close(fd_fb);
            }
        }

        if (!found) {
            printf("can not find fb dev %s\n", fb_name);
            ret = -1;
            ipuErr=Err10;
            goto err10;
        }

        ioctl(fd_fb, FBIOGET_VSCREENINFO, &fb_var);
        fb_var.xres = t->output.width;
        fb_var.xres_virtual = fb_var.xres;
        fb_var.yres = t->output.height;
        fb_var.yres_virtual = fb_var.yres;
        fb_var.activate |= FB_ACTIVATE_FORCE;
        fb_var.vmode |= FB_VMODE_YWRAP;
        fb_var.nonstd = t->output.format;
        fb_var.bits_per_pixel = fmt_to_bpp(t->output.format);

        ret = ioctl(fd_fb, FBIOPUT_VSCREENINFO, &fb_var);
        if (ret < 0) {
            printf("fb ioctl FBIOPUT_VSCREENINFO fail\n");
            ipuErr=Err11;
            goto err11;
        }
        ioctl(fd_fb, FBIOGET_VSCREENINFO, &fb_var);
        ioctl(fd_fb, FBIOGET_FSCREENINFO, &fb_fix);

        outpaddr = fb_fix.smem_start;
        blank = FB_BLANK_UNBLANK;
        ioctl(fd_fb, FBIOBLANK, blank);
    } else {
        osize = t->output.paddr =
            t->output.width * t->output.height
            * fmt_to_bpp(t->output.format)/8;
        ret = ioctl(fd_ipu, IPU_ALLOC, &t->output.paddr);
        if (ret < 0) {
            printf("ioctl IPU_ALLOC fail\n");
            ipuErr=Err10;
            goto err10;
        }
                rgbData = (unsigned char *)mmap(0, osize, PROT_READ | PROT_WRITE,
                MAP_SHARED, fd_ipu, t->output.paddr);
                if (!rgbData) {
            printf("mmap fail\n");
            ret = -1;
            ipuErr=Err11;
            goto err11;
        }

                file_out = fopen(test_handle.outfile, "wb");
                if (file_out == NULL) {
                        printf("can not open output file %s\n", test_handle.outfile);
                        ret = -1;
                        ipuErr=Err12;
                        goto err12;
                }
    }

again:
    ret = ioctl(fd_ipu, IPU_CHECK_TASK, t);
    if (ret != IPU_CHECK_OK) {
        if (ret > IPU_CHECK_ERR_MIN) {
            if (ret == IPU_CHECK_ERR_SPLIT_INPUTW_OVER) {
                t->input.crop.w -= 8;
                goto again;
            }
            if (ret == IPU_CHECK_ERR_SPLIT_INPUTH_OVER) {
                t->input.crop.h -= 8;
                goto again;
            }
            if (ret == IPU_CHECK_ERR_SPLIT_OUTPUTW_OVER) {
                t->output.crop.w -= 8;
                goto again;
            }
            if (ret == IPU_CHECK_ERR_SPLIT_OUTPUTH_OVER) {
                t->output.crop.h -= 8;
                goto again;
            }
            ret = 0;
            printf("ipu task check fail\n");
            ipuErr=Err13;
            goto err13;
        }
    }
    dump_ipu_task(t);

err13:
    if(ipuErr==Err13){
        printf("err13 !!!!\n");
        if (fd_fb) {
            blank = FB_BLANK_POWERDOWN;
            ioctl(fd_fb, FBIOBLANK, blank);
        }
    }
//	if (file_out)
//		fclose(file_out);
err12:
    if(ipuErr==Err12){
        printf("err12 !!!!\n");
        if (rgbData)
                munmap(rgbData, osize);
    }
err11:
    if(ipuErr==Err11){
        printf("err11 !!!!\n");
        if (fd_fb)
        close(fd_fb);
         if (t->output.paddr)
        ioctl(fd_ipu, IPU_FREE, &t->output.paddr);
    }
err10:
    if(ipuErr==Err10){
        printf("err10 !!!!\n");
        if (alpbuf)
            munmap(alpbuf, alpsize);
    }
err9:
    if(ipuErr==Err9){
        printf("err9 !!!!\n");
         if (t->overlay.alpha.loc_alp_paddr)
            ioctl(fd_ipu, IPU_FREE, &t->overlay.alpha.loc_alp_paddr);
    }
err8:
    if(ipuErr==Err8){
        printf("err8 !!!!\n");
        if (ovbuf)
            munmap(ovbuf, ovsize);
    }
err7:
    if(ipuErr==Err7){
         printf("err7 !!!!\n");
         if (t->overlay.paddr)
               ioctl(fd_ipu, IPU_FREE, &t->overlay.paddr);
    }
err6:
    if(ipuErr==Err6){
        printf("err6 !!!!\n");
        if (vdibuf)
            munmap(vdibuf, isize);
    }
err5:
    if(ipuErr==Err5){
        printf("err5 !!!!\n");
         if (t->input.paddr_n)
             ioctl(fd_ipu, IPU_FREE, &t->input.paddr_n);
    }
err4:
    if(ipuErr==Err4){
        printf("err4 !!!!\n");
        if (inbuf)
            munmap(inbuf, isize);
    }
err3:
    if(ipuErr==Err3){
        printf("err3 !!!!\n");
        if (t->input.paddr)
            ioctl(fd_ipu, IPU_FREE, &t->input.paddr);
    }
err2:
    if(ipuErr==Err2){
        printf("err2 !!!!\n");
        if (fd_ipu)
            close(fd_ipu);
    }
err1:
    if(ipuErr==Err1){
//	if (file_in)
//		fclose(file_in);
    }
    return 0;
}

