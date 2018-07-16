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
#include "cfaceclsthread.h"

using namespace cv;
using namespace std;

class CHuamDectThd:public QThread
{
    Q_OBJECT
signals:
    void message(QList<ObjdectRls> list, int idx);
public slots:
    void recvImg(QImage img, int idx);   //recieve from camera
    void recvFace(QList<ClsResult> dectresult);
public:
    CHuamDectThd(int idx);
    int DetectHuman(Mat img, int *rect, QImage qimg);
    void StopRun();
    void CheckHat();
    int isCross(ObjdectRls rls1, ObjdectRls rls2);

    QList<ObjdectRls> maintainhuman[3];  //human in three camera,关联之后的检测结果
    int currentObjID[3];   //三个目标检测  当前可用的ID
private:
    QImage curImg[3];  //
    QMutex mutex;
    int idd;
    volatile bool b_stop;
    QList<ObjdectRls> list[3];   //当前检测结果
    QList<ObjdectRls> hatlist[3];
    int dectindex;
    CFaceClsThread *face_thd;
private:
    void run();
    void MaintainObj();
};

#endif // CHUAMDECTTHD_H
