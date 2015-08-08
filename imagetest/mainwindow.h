#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
//#include "iputool.h"
#include "v4l2cap.h"
extern "C"
{
    #include "mxc_ipudev_test.h"
}
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
  //  QImage *frame;
    void yuyv_2_rgb888(void);
    QByteArray frame_buffer;
    QByteArray  yuv_buf;
    uchar * data;
    QTimer *timer;
  //  Iputool * dataConverter;
    V4l2cap * v4l2Cap;
    int rs;
    unsigned int len;

 //   bool firstImage;
    int photoWidth;
    int photoHeight;

private slots:
    void paintEvent(QPaintEvent *);
    int timerUpdate(void);


//    QByteArray  *pointer=NULL;
};

#endif // MAINWINDOW_H
