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
    int currentObjID[3];   //三个目标检测  当前可用的ID
    QMutex mutex;
    CFaceClsThread *face_thd;
private slots:
    void frmMenu();
    void sysSetup();
    void sysStart();
    void sysRegister();
};

#endif // WIDGET_H
