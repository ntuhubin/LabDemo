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
    oplist.append("4386");

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
    m1pts.setPoints(4, 1374,427, 1550,471,1329,630,1157,555);
    m2pts.setPoints(4, 1139,628, 1273,690,1000,950,760,880);
    //m1pts.setPoints(4, 1140,410, 1500,410,1500,650,1140,650);
    //m2pts.setPoints(4, 850,600, 1300,600,1300,1000,850,1000);
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
        //play_thd[i]->stopRealPlay();
        play_thd[i]->stopLocalplay();
    }
    delete ui;
}
bool Widget::IsOperator(QString name)
{
    if(name == "")
        return false;
    return oplist.contains(name);
}
void Widget::recvImg(QImage img, int idx)  //摄像头返回图像，编号从1开始
{
    if(idx != 4)
    {
        mutex.lock();
        int count = objdects[idx - 1].count();
        QList<ObjdectRls> rls = objdects[idx - 1];
        /*for(int i = 0; i < count; i++)
        {
            rls.append(objdects[idx - 1].at(i));
        }*/

        if(count != 0 || idx == 1)
        {
            QPainter painter(&img);
            painter.setPen(QPen(Qt::blue, 4, Qt::DashLine));
            QFont font;
            font.setPointSize(35);
            painter.setFont(font);
            for(int i = 0; i < count; i++)
            {             
                if(rls.at(i).withHat == 0)
                {
                    QString str = "NO HAT";
                    painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() - 25,str);
                }
                painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() + 25, QString::number(rls[i].ID, 10));
                painter.drawText(rls.at(i).rect.x() + 105,rls.at(i).rect.y() + 25, rls[i].name);
                if(idx == 1)
                {
                    if(rls.at(i).OPFrame >= 50 && IsOperator(rls.at(i).name) == false)
                    {
                       painter.setPen(QPen(Qt::red, 4, Qt::DashLine));
                       painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() + 125, "op machine");
                    }
                    else
                    {
                        painter.setPen(QPen(Qt::blue, 4, Qt::DashLine));
                    }
                }
                painter.drawRect(rls.at(i).rect);
            }
            if(idx == 1)
            {
                painter.setPen(QPen(Qt::red, 4, Qt::DashLine));
                painter.drawConvexPolygon(m1pts);
                painter.drawConvexPolygon(m2pts);
            }

        }
        /*if(idx == 1)
            img.save("/tmp/0.jpg");
        if(idx == 2)
            img.save("/tmp/1.jpg");
        if(idx == 3)
            img.save("/tmp/2.jpg");*/
    }
    mutex.unlock();


    QLabel *cam[4] = {ui->label_CameraA, ui->label_CameraB, ui->label_CameraC, ui->label_CameraD};
    int w = ui->label_CameraA->width();
    int h = ui->label_CameraA->height();
    QPixmap mp = QPixmap::fromImage(img);
    QPixmap fitpixmap = mp.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 饱满填充
    //ui->label_CameraA->setPixmap(fitpixmap);
    cam[idx-1]->setPixmap(fitpixmap);
}
void Widget::DealShowObjs(ObjdectRls rls)
{
    int count = showobjs.count();
    rls.captime = QDateTime::currentDateTime();
    for(int i = count - 1; i >=0; i--)
    {
        if((rls.CAMID == showobjs[i].CAMID) && (rls.ID == showobjs[i].ID))
        {
            showobjs.removeAt(i);
            break;
        }
    }

    showobjs.append(rls);
    if(showobjs.count() > 5)
        showobjs.removeFirst();

    LAB *plab[5] = {ui->lAB, ui->lAB_2, ui->lAB_3, ui->lAB_4, ui->lAB_5};

    count = showobjs.count();
    int ma = min(count, 5);
    for(int i = 0; i < ma; i++)
    {
        showmsg msg;
        msg.camid = "工位:" + QString::number(showobjs[i].CAMID);
        if(showobjs[i].withHat == 0)
        {
            msg.status = "未戴安全帽";
            msg.warning = true;
        }
        if(showobjs[i].OPFrame >= 45)
        {
            msg.status = "操作仪器";
        }
        msg.ttime = showobjs[i].captime.toString("HH:mm::ss");
        msg.img = showobjs[i].img;
        msg.id = "姓名:" + showobjs[i].name;
        plab[i]->SetMsg(msg);
    }

    /*showmsg msg;
    msg.camid="1";
    msg.status="背景颜色";
    msg.warning = true;
    ui->lAB->SetMsg(msg);*/

}
void Widget::recvObjDect(QList<ObjdectRls> list, int idx)
{
    if(idx < 0 || idx > 3)
        return;
    mutex.lock();
    int count = list.count();

    objdects[idx].clear();
    for(int i = 0; i < count; i++)
    {
        objdects[idx].append(list[i]);
        DealShowObjs(list[i]);
    }
    mutex.unlock();
    //int count = objdects[idx].count();
    /*int comcount = list.count();
    if(count == 0)
    {
        for(int i = 0; i < list.count(); i++)
        {
            list[i].ID = currentObjID[idx];
            objdects[idx].append(list.at(i));
            currentObjID[idx]++;
        }
    }
    else
    {
        if(count <= comcount)
        {
            for(int i = 0; i < count; i++)
            {
                double coo =  0;
                int iddx = 0;
                for(int j = 0; j < list.count(); j++)
                {
                    double co = compareHist( objdects[idx][i].Hist, list[j].Hist, CV_COMP_CORREL );
                    if(co > coo)
                    {
                        coo = co;
                        iddx = j;
                    }
                }
                objdects[idx][i].Hist = list[iddx].Hist;
                objdects[idx][i].rect = list[iddx].rect;
                objdects[idx][i].img = list[iddx].img;
                objdects[idx][i].leaveframe = 0;
                list.removeAt(iddx);
            }
            for(int i = 0; i < list.count(); i++)
            {
                list[i].ID = currentObjID[idx];
                list[i].leaveframe = 0;
                objdects[idx].append(list[i]);
                currentObjID[idx]++;
            }
        }
        else
        {
            int corr[count];
            for(int i = 0; i < count; i++)
            {
                corr[i] = -1;
            }
            for(int i = 0; i < comcount; i++)
            {
                double coo =  0;
                int iddx = 0;
                for(int j = 0; j < count; j++)
                {
                    double co = compareHist(list[i].Hist, objdects[idx][j].Hist, CV_COMP_CORREL );
                    if(co > coo)
                    {
                        coo = co;
                        iddx = j;
                    }
                }
                corr[iddx] = i;
            }

            for(int i = 0; i < count; i++)
            {
                if(corr[i] == -1)
                {
                    objdects[idx][i].leaveframe++;
                }
                else
                {
                    objdects[idx][i].Hist = list[corr[i]].Hist;
                    objdects[idx][i].rect = list[corr[i]].rect;
                    objdects[idx][i].img = list[corr[i]].img;
                    objdects[idx][i].leaveframe = 0;
                }
            }
            QMutableListIterator<ObjdectRls> it(objdects[idx]);
            while (it.hasNext())
            {
                ObjdectRls rls = it.next();
                if(rls.leaveframe > 2)
                {
                    it.remove();
                }
            }
        }
    }
    mutex.unlock();*/
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
    menu->addAction("register", this, SLOT(sysRegister()));
    menu->addAction("querry", this, SLOT(sysQuery()));
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
         //play_thd[i]->setRealPlay("132.120.136.54",8000,"admin","sipai_lab",1+i,false,0);  //CAMEREA ID 1234
         //play_thd[i]->start();
    }
    play_thd[0]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180827/ch01_20180827125633.mp4", 1);
    play_thd[1]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180827/ch02_20180827125633.mp4", 2);
    play_thd[2]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180827/ch03_20180827125633.mp4", 3);
    play_thd[3]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180827/ch04_20180827125633.mp4", 4);
    play_thd[0]->start();
    play_thd[1]->start();
    play_thd[2]->start();
    play_thd[3]->start();


    human_thd = new CHuamDectThd(0);
    for(int i = 0; i < 3; i++)
    {
        //human_thd[i] = new CHuamDectThd(i);
        connect(play_thd[i], &PlayLocalM4::message,human_thd, &CHuamDectThd::recvImg);

    }
    connect(human_thd, &CHuamDectThd::message, this, &Widget::recvObjDect);
    human_thd->start();
    //face_thd = new CFaceClsThread();
    //connect(play_thd[1], &PlayLocalM4::message, face_thd, &CFaceClsThread::recvImg);
    //face_thd->start();
}
void Widget::sysRegister()
{
    RegisterDlg *regdlg = new RegisterDlg();
    regdlg->show();
}
void Widget::sysQuery()
{
    QueryDlg *querydlg = new QueryDlg();
    querydlg->show();
}
