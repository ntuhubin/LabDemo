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
#include "cpersonreidthd.h"
#include "cdbpro.h"

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
    void RecvReid(QList<ObjdectRls> list1, QList<ObjdectRls> list2);
    void recvreid(ObjdectRls rls1, ObjdectRls rls2);
public:
    CHuamDectThd(int idx);
    int DetectHuman(Mat img,  QImage qimg, cv::Rect rc);
    void StopRun();
    void CheckHat();
    int isCross(ObjdectRls rls1, ObjdectRls rls2);

    QList<ObjdectRls> maintainhuman[3];  //human in three camera,关联之后的检测结果
    int currentObjID[3];   //三个目标检测  当前可用的ID
    QList<ObjdectRls> facePerson;   //被抓拍到人脸的所有数据，(保留一小时?)
private:
    QImage curImg[3];  //
    QMutex mutex;
    int idd;
    volatile bool b_stop;
    QList<ObjdectRls> list[3];   //当前检测结果
    QList<ObjdectRls> hatlist[3];
    int dectindex;
    CFaceClsThread *face_thd;
    CPersonReIDThd *personReid;
    CDbPro *db;
private:
    void run();
    void MaintainObj();
    void MaintainObj_2();
    void PrepareReid(ObjdectRls Objrls);
    void MultiPrepareReid();

    bool isINOpArea(int x, int y);
    void ProcessOPArea();

    void AddEventDB(int camid, QString staffid, int eventid, QImage img);
};

#endif // CHUAMDECTTHD_H
