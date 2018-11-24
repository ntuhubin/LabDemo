#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QList>
#include <QPainter>
#include <QMutex>
#include <QDebug>
#include <QMetaType>
#include "playlocalm4.h"
#include "chuamdectthd.h"
#include "g.h"
#include "cfaceclsthread.h"
#include "cpersonreidthd.h"
#include "lab.h"
#include "registerdlg.h"
#include "querydlg.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void recvImg(QImage img, int idx);
    void recvObjDect(QList<ObjdectRls> list, int idx);
    void recvReid(QList<ObjdectRls> list1, QList<ObjdectRls> list2);
private:
    Ui::Widget *ui;
    PlayLocalM4 *play_thd[4];
    CHuamDectThd *human_thd;
    CPersonReIDThd *reid_thd;
    QList<ObjdectRls> objdects[3];
    QList<ObjdectRls> showobjs;   //右侧栏显示, max 5
    QList<ObjdectRls> showlst[3];
    int currentObjID[3];   //三个目标检测  当前可用的ID
    QMutex mutex;
    CFaceClsThread *face_thd;
    QPolygon m1pts;
    QPolygon m2pts;
    QList<QString> oplist;
    int showindex;
    int lastshowcount;
    QString hatColor[11];
private:
    void DealShowObjs(ObjdectRls rls);
    int IsInList(QString name);
    void DealShowobjs(QList<ObjdectRls> list, int index);
    bool IsOperator(QString name);
    int D2H(int d);
private slots:
    void frmMenu();
    void sysSetup();
    void sysStart();
    void sysRegister();
    void sysQuery();
    void upShow();
    void downShow();
private:
    QList<TraceInfo> tracelst;
    int curtraceid;
    int currentframes;
    void DealTraceLst(ObjdectRls rls);
    int GetCurrentTraceObj(QList<ObjdectRls> list);
    void GotoMachine();
    bool Machine_Violation;
};

#endif // WIDGET_H
