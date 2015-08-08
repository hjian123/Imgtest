#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCursor(Qt::BlankCursor);
 //  dataConverter = new Iputool;
   v4l2Cap = new V4l2cap;
   photoHeight=v4l2Cap->imgHeight;
   photoWidth=v4l2Cap->imgWidth;
  // firstImage=true;
//   timer =new QTimer(this);
//   connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
//   timer->start(15);
   //timer->setSingleShot(true);
   v4l2Cap->getYuv();
//   dataConverter->yuvSourceData=v4l2Cap->yuv_buffer_pointer;
//   dataConverter->start_ipu(12,p);
   QImage *image= new QImage((uchar *)v4l2Cap->dataConverter->rgbData,photoWidth,photoHeight,QImage::Format_RGB16);
   ui->label->setPixmap(QPixmap::fromImage(*image,Qt::AutoColor));
 //  delete frame;
/*
    QFile *file=new QFile("testz.yuv");
  //  QFile *filesave=new QFile("kk.txt");
  //   qDebug()<< "creat Qfile";
     if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << "Open file failed.";
     }
        yuv_buf=file->readAll();
//    //    yuyv_2_rgb888();

        dataConverter->yuvSourceData=(unsigned char *)yuv_buf.data();
        p[0]="0";
        p[1]="-c";
        p[2]="1";
        p[3]="-i";
        p[4]="1024,600,YUYV,0,0,0,0,0,0";
        p[5]="-O";
        p[6]="1024,600,RGBP,0,0,0,0,0";
        p[7]="-s";
        p[8]="0";
        p[9]="-f";
        p[10]="cc.rgb";
        p[11]="wall-1024x768-565.yuv";
        dataConverter->start_ipu(12,p);
        frame= new QImage((uchar *)dataConverter->rgbData,IMGWIDTH,480,QImage::Format_RGB16);
        ui->label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));
*/
}

MainWindow::~MainWindow(){
    delete ui;
    v4l2Cap->stopCapturing();
}

void MainWindow::yuyv_2_rgb888(){
         uint   i=0,j=0;
         uchar y1,y2,u,v;
         int r,g,b;
         unsigned long yuv_size = 1024*600*2;
       //  printf("start change to RGB  \n");

         for(i=0;i<yuv_size;i+=4)
         {
                 y1=yuv_buf[i];
                 u= yuv_buf[i+1];
                 y2= yuv_buf[i+2];
                 v = yuv_buf[i+3];

                 r = y1 + 1.042*(v-128);
                 g = y1 - 0.34414*(u-128) - 0.71414*(v-128);
                 b = y1 + 1.772*(u-128);

                 if(r>255) r = 255;
                 else if(r<0) r = 0;
                 if(g>255) g= 255;
                 else if(g<0) g = 0;
                 if(b>255) b= 255;
                 else if(b<0) b = 0;
//                 *(data++)=(uchar)r;
//                 *(data++)=(uchar)g;
//                 *(data++)=(uchar)b;
                 frame_buffer[j++] = (char)r;
                 frame_buffer[j++] = (char)g;
                 frame_buffer[j++] = (char)b;
                 r = y2 + 1.042*(v-128);
                 g = y2 - 0.34414*(u-128) - 0.71414*(v-128);
                 b = y2 + 1.772*(u-128);

                 if(r>255) r = 255;
                 else if(r<0) r = 0;
                 if(g>255) g= 255;
                 else if(g<0) g = 0;
                 if(b>255) b= 255;
                 else if(b<0) b = 0;
                 frame_buffer[j++] = (char)r;
                 frame_buffer[j++] = (char)g;
                 frame_buffer[j++] = (char)b;
            }
         //  printf("change to RGB OK \n");
}

void MainWindow::paintEvent(QPaintEvent *)
{
  //  struct timeval start, end;
  //  float timeuse;
  //  gettimeofday(&start,NULL);

    QDateTime time = QDateTime::currentDateTime();
    QString  str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    ui->lineEdit->setText(str);
    v4l2Cap->getYuv();
 //   qDebug("rgbData %d\n",*(v4l2Cap->dataConverter->rgbData+100));
    QImage *frame= new QImage((uchar *)v4l2Cap->dataConverter->rgbData,photoWidth,photoHeight,QImage::Format_RGB16);
    ui->label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));
//    gettimeofday(&end,NULL);
//    timeuse=1000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000;
//    qDebug("timeuse %f\n",timeuse);
}
int MainWindow::timerUpdate()
{
  //  struct timeval start, end;
  //  float timeuse;
  //  gettimeofday(&start,NULL);
    QDateTime time = QDateTime::currentDateTime();
    QString  str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    ui->lineEdit->setText(str);
    v4l2Cap->getYuv();
 //   qDebug("rgbData %d\n",*(v4l2Cap->dataConverter->rgbData+100));
    QImage *frame= new QImage((uchar *)v4l2Cap->dataConverter->rgbData,photoWidth,photoHeight,QImage::Format_RGB16);
    ui->label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));
//    gettimeofday(&end,NULL);
//    timeuse=1000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000;
//    qDebug("timeuse %f\n",timeuse);
   return 0;
}

