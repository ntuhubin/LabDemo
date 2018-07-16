#ifndef CFACECLSTHREAD_H
#define CFACECLSTHREAD_H

#include <QThread>
#include <QImage>
#include <QMetaType>
#include <QMutex>
#include <QDebug>
#include "cfaceclassification.h"
#include "g.h"

class CFaceClsThread:public QThread
{
    Q_OBJECT
signals:
    void sendcls(QList<ClsResult> dectresult);
public slots:
    void recvImg(QImage img, int idx);   //recieve from camera 1
public:
    CFaceClsThread();
    CFaceClassification *face;
    void SetParam(QImage img, int devID);
    ~CFaceClsThread();
    void setStart();
    void setStop();
    bool isGet;
    QImage RecvImg;
    int devid;   //对应行人检测中的行人ID
    volatile bool stopflag;
    QMutex mutex;
    QList<SaveRegPerson> capList;

    //int UpdateCapList(ClsInfo clsinfo,  QImage dimg);
private:
    void run();
};

#endif // CFACECLSTHREAD_H
