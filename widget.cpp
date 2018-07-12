#include "widget.h"
#include "ui_widget.h"
#include <QMenuBar>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->label_title->setText("数字车间智能监控管理平台");
    ui->label_gap->setText("                                                   ");
    ui->pushButtonmMenu->setText("Menu");
    int w = ui->label_LOGO->width() * 1.5;
    int h = ui->label_LOGO->height();
    QPixmap mp;
    mp.load("./LOGO.png");
    QPixmap fitpixmap = mp.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 饱满填充
    //ui->label_CameraA->setPixmap(fitpixmap);
    ui->label_LOGO->setPixmap(fitpixmap);

    connect(ui->pushButtonmMenu, &QPushButton::clicked, this, &Widget::frmMenu);
    for(int i = 0; i < 3; i++)
    {
        currentObjID[i] = 1;
    }

    /*for(int i = 0; i < 4; i++)
     {
         play_thd[i] = new PlayLocalM4();
         connect(play_thd[i], &PlayLocalM4::message, this, &Widget::recvImg);
         play_thd[i]->setRealPlay("132.120.136.54",8000,"admin","sipai_lab",1+i,false,0);  //CAMEREA ID 1234
         play_thd[i]->start();
     }
     human_thd = new CHuamDectThd(0);
    for(int i = 0; i < 3; i++)
    {
        //human_thd[i] = new CHuamDectThd(i);
        connect(play_thd[i], &PlayLocalM4::message,human_thd, &CHuamDectThd::recvImg);

    }
    connect(human_thd, &CHuamDectThd::message, this, &Widget::recvObjDect);
    human_thd->start();*/

}

Widget::~Widget()
{
    /*for(int i = 0; i < 3; i++)
    {
        human_thd[i]->StopRun();
    }*/
    human_thd->StopRun();
    for(int i = 0; i < 4; i++)
    {
        play_thd[i]->stopRealPlay();
    }
    delete ui;
}
void Widget::recvImg(QImage img, int idx)
{
    if(idx != 4)
    {
        mutex.lock();
        int count = objdects[idx - 1].count();
        QList<ObjdectRls> rls;
        for(int i = 0; i < count; i++)
        {
            rls.append(objdects[idx - 1].at(i));
        }
        mutex.unlock();
        if(count != 0)
        {
            //QVector<QRectF> rects;

            QPainter painter(&img);
            painter.setPen(QPen(Qt::blue, 4, Qt::DashLine));
            QFont font;
            font.setPointSize(35);
            painter.setFont(font);
            for(int i = 0; i < count; i++)
            {
                //rects.append(rls.at(i).rect);
                painter.drawRect(rls.at(i).rect);
                if(rls.at(i).withHat == 0)
                {
                    QString str = "NO HAT";
                    painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() - 25,str);
                }
                painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() + 25, QString::number(rls[i].ID, 10));
            }
            //painter.drawRects(rects);
        }
    }



    QLabel *cam[4] = {ui->label_CameraA, ui->label_CameraB, ui->label_CameraC, ui->label_CameraD};
    int w = ui->label_CameraA->width();
    int h = ui->label_CameraA->height();
    QPixmap mp = QPixmap::fromImage(img);
    QPixmap fitpixmap = mp.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 饱满填充
    //ui->label_CameraA->setPixmap(fitpixmap);
    cam[idx-1]->setPixmap(fitpixmap);
}
void Widget::recvObjDect(QList<ObjdectRls> list, int idx)
{
    if(idx < 0 || idx > 3)
        return;
    mutex.lock();
    int count = objdects[idx].count();
    if(count == 0)
    {
        for(int i = 0; i < list.count(); i++)
        {
            list[i].ObjID = currentObjID[idx];
            objdects[idx].append(list.at(i));
            currentObjID[idx]++;
        }
    }
    else
    {
        QMutableListIterator<ObjdectRls> it(objdects[idx]);
        QMutableListIterator<ObjdectRls> newit(list);
        while (it.hasNext())
        {
            ObjdectRls rls = it.next();
            bool isupdate = false;
            newit.toFront();
            while(newit.hasNext())
            {
                ObjdectRls newrls = newit.next();
                QRectF rect = rls.rect.intersected(newrls.rect);
                float rectsize = rect.width() * rect.height();
                float objsize = newrls.rect.width() * newrls.rect.height();
                if(rectsize / objsize > 0.6)
                {
                    rls.rect = newrls.rect;
                    rls.leaveframe = 0;
                    newit.remove();
                    isupdate = true;
                    break;
                }
            }
            if(isupdate == false)
            {
                rls.leaveframe++;
                if(rls.leaveframe > 2)
                {
                    it.remove();
                }
            }
        }
        newit.toFront();
        while(newit.hasNext())
        {
            it.insert(newit.next());
        }
    }

    mutex.unlock();
}
void Widget::recvReid(QList<ObjdectRls> list1, QList<ObjdectRls> list2)
{

}
void Widget::frmMenu()
{
    QMenu *menu;
    menu = new QMenu(this);
    menu->clear();
    menu->addSeparator();
    menu->addAction("setting", this, SLOT(sysSetup()));
    menu->addAction("start", this, SLOT(sysStart()));
    menu->exec(QCursor::pos());
    delete menu;
}
void Widget::sysSetup()
{
    int w = ui->label_CameraA->width();
    int h = ui->label_CameraA->height();
    QImage img;
    img.load("./test.jpg");
    QRectF r1 = QRectF(200,200,500,500);
    QRectF r2 = QRectF(300,300,500,500);
    QPainter painter(&img);
    painter.setPen(QPen(Qt::blue, 4, Qt::DashLine));
    painter.drawRect(r1);
    painter.drawRect(r2);
    QRectF rect = r1.intersected(r2);
    QFont font;
    font.setPointSize(35);
    painter.setFont(font);
    painter.drawText(225,225,"1");
    QPixmap mp = QPixmap::fromImage(img);
    QPixmap fitpixmap = mp.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 饱满填充
    ui->label_CameraA->setPixmap(fitpixmap);
    //
    return;
    showmsg msg;
    msg.camid="1";
    msg.status="背景颜色";
    msg.warning = true;
    ui->lAB->SetMsg(msg);
    /*reid_thd = new CPersonReIDThd();
    Mat img1 = imread("./1.jpg");
    Mat img2 = imread("./2.jpg");
    float score = reid_thd->GetSocre(img1,img2);
    qDebug() << score;*/
}
void Widget::sysStart()
{
    ui->pushButtonmMenu->setEnabled(false);
    ui->pushButtonmMenu->setVisible(false);
    for(int i = 0; i < 4; i++)
     {
         play_thd[i] = new PlayLocalM4();
         connect(play_thd[i], &PlayLocalM4::message, this, &Widget::recvImg);
         play_thd[i]->setRealPlay("132.120.136.54",8000,"admin","sipai_lab",1+i,false,0);  //CAMEREA ID 1234
         play_thd[i]->start();
     }
     human_thd = new CHuamDectThd(0);
    for(int i = 0; i < 3; i++)
    {
        //human_thd[i] = new CHuamDectThd(i);
        connect(play_thd[i], &PlayLocalM4::message,human_thd, &CHuamDectThd::recvImg);

    }
    connect(human_thd, &CHuamDectThd::message, this, &Widget::recvObjDect);
    human_thd->start();
    /*face_thd = new CFaceClsThread();
    connect(play_thd[1], &PlayLocalM4::message, face_thd, &CFaceClsThread::recvImg);
    face_thd->start();*/
}
