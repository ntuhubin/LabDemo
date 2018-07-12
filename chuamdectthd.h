#ifndef CHUAMDECTTHD_H
#define CHUAMDECTTHD_H

#include <QThread>
#include <QMutex>
#include <QImage>
#include <QList>
#include <QMetaType>
#include <QRectF>
#include "objectdect/object_detection_wrapper.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "publicfun.h"
#include "g.h"

using namespace cv;
using namespace std;

class CHuamDectThd:public QThread
{
    Q_OBJECT
signals:
    void message(QList<ObjdectRls> list, int idx);
public slots:
    void recvImg(QImage img, int idx);   //recieve from camera
public:
    CHuamDectThd(int idx);
    int DetectHuman(Mat img, int *rect, QImage qimg);
    void StopRun();
    void CheckHat();
    int isCross(ObjdectRls rls1, ObjdectRls rls2);
private:
    QImage curImg[3];  //
    QMutex mutex;
    int idd;
    volatile bool b_stop;
    QList<ObjdectRls> list[3];
    QList<ObjdectRls> hatlist[3];
    int dectindex;
private:
    void run();
};

#endif // CHUAMDECTTHD_H
