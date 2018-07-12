#ifndef PLAYLOCALM4_H
#define PLAYLOCALM4_H
#include <QThread>
//#include <QtGui>
//#include <QPainter>
#include <QImage>
#include <QDebug>
#include"ccameracontrol.h"

#include <opencv2/opencv.hpp>


using namespace cv;
class PlayLocalM4:public QThread
{
    Q_OBJECT
signals:
    void message(QImage img, int idx);
public:
    PlayLocalM4();
    ~PlayLocalM4();
    void setPlayClient(char *path, int idx);
    void setRealPlay(char *ip, int port, char *name, char *pwd, int idx, bool save, int stream);
    void stopRealPlay();
    void stopLocalplay();

private:
    unsigned int picsize;
    volatile bool stopflag;
    CCameraControl* cam;
    int idd;
    //CFaceClassification *face;

private:
    void run();
};

#endif // PLAYLOCALM4_H
