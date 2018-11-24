#include "widget.h"
#include "ui_widget.h"
#include <QMenuBar>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //ui->label_title->setText("数字车间智能监控管理平台");
    ui->label_title->setText("                       ");
    ui->label_gap->setText("                                                   ");
    //ui->pushButtonmMenu->setText("Menu");
    int w = ui->label_LOGO->width() * 1.5;
    int h = ui->label_LOGO->height();


    //QPixmap icon1(tr("/home/proj/lab/GUI0913/menu.png"));
    ui->pushButtonmMenu->setIcon((QPixmap("/home/proj/lab/GUI0913/menu.png")));
    ui->pushButtonmMenu->setIconSize(QSize(80, 30));
    //ui->pushButtonmMenu->setFixedSize(icon1.size());

    ui->btnup->setText("");
    QPixmap icon2(tr("/home/proj/lab/GUI0913/ArrowUp.png"));
    ui->btnup->setIcon(icon2);
    ui->btnup->setIconSize(icon2.rect().size());

    ui->btndown->setText("");
    QPixmap icon3(tr("/home/proj/lab/GUI0913/ArrowDown.png"));
    ui->btndown->setIcon(icon3);
    ui->btndown->setIconSize(icon3.rect().size());

    //QPixmap mp;
    //mp.load("./LOGO.png");
    //QPixmap fitpixmap = mp.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 饱满填充
    //ui->label_LOGO->setPixmap(fitpixmap);
    ui->label_LOGO->setText("");

    connect(ui->pushButtonmMenu, &QPushButton::clicked, this, &Widget::frmMenu);
    connect(ui->btnup,&QPushButton::clicked,this,&Widget::upShow);
    connect(ui->btndown,&QPushButton::clicked,this,&Widget::downShow);
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
    m2pts.setPoints(4, 1100,560, 1300,640,950,950,600,880);
    //m1pts.setPoints(4, 1140,410, 1500,410,1500,650,1140,650);
    //m2pts.setPoints(4, 850,600, 1300,600,1300,1000,850,1000);

    showindex = 0;
    lastshowcount = 0;
    QString filename = "/home/proj/lab/GUI0918/Background.jpg";
    QPixmap pixmap(filename);
    QPalette pal;
    pal.setBrush(QPalette::Window,QBrush(pixmap));
    setPalette(pal);

    hatColor[0] = "黑色";
    hatColor[1] = "蓝色";
    hatColor[2] = "棕色";
    hatColor[3] = "灰色";
    hatColor[4] = "绿色";
    hatColor[5] = "橘色";
    hatColor[6] = "粉红色";
    hatColor[7] = "紫色";
    hatColor[8] = "红色";
    hatColor[9] = "白色";
    hatColor[10] = "黄色";
    curtraceid = -1;
    currentframes = 0;
    Machine_Violation = false;
}

Widget::~Widget()
{

    //play_thd[3]->cam->AutoPan(1);
    human_thd->StopRun();
    for(int i = 0; i < 4; i++)
    {
        play_thd[i]->stopRealPlay();
        //play_thd[i]->stopLocalplay();
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


        QPainter painter(&img);

        QFont font;
        font.setPointSize(35);
        painter.setFont(font);
        for(int i = 0; i < count; i++)
        {
            painter.setPen(QPen(Qt::blue, 4, Qt::DashLine));
            if(rls.at(i).withHat == 0)
            {
                QString str = "NO HAT";
                painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() - 25,str);
            }
            else
            {
                int coloridx = rls[i].HatColor;
                if(coloridx != -1)
                {
                    painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() - 25, hatColor[coloridx]);
                }
            }
            painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() + 25, QString::number(rls[i].ID, 10));
            painter.drawText(rls.at(i).rect.x() + 105,rls.at(i).rect.y() + 25, rls[i].name);
            if(idx == 1)
            {
                if(rls.at(i).OPFrame >= 20  )
                {
                   if(IsOperator(rls.at(i).name) == false)
                   {
                       painter.setPen(QPen(Qt::red, 4, Qt::DashLine));
                       painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() + 125, "违规操作");
                   }
                   else
                   {
                       painter.setPen(QPen(Qt::green, 4, Qt::DashLine));
                       painter.drawText(rls.at(i).rect.x() + 25,rls.at(i).rect.y() + 125, "操作CNC");
                   }


                }
             }
            if(idx == 2)
            {
                if(rls[i].name == "")
                {
                    painter.setPen(QPen(Qt::yellow, 4, Qt::DashLine));
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
        /*if(idx == 1)
            img.save("/tmp/0.jpg");
        if(idx == 2)
            img.save("/tmp/1.jpg");
        if(idx == 3)
            img.save("/tmp/2.jpg");*/
       mutex.unlock();
    }
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
            rls.img = showobjs[i].img;
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

        if(showobjs[i].OPFrame >= 20)
        {
            if(IsOperator(showobjs[i].name) == false)
            {
                msg.warning = true;
                msg.status = "违规操作";
            }
            else
            {
                msg.status = "操作CNC";
                msg.warning = false;
            }

        }
        if(showobjs[i].CAMID == 1)
        {
            msg.status = "进入车间";
        }
        if(showobjs[i].withHat == 0)
        {
            msg.status = "未戴安全帽";
            msg.warning = true;
        }
        msg.ttime = showobjs[i].captime.toString("HH:mm::ss");
        msg.img = showobjs[i].img;
        msg.id = showobjs[i].name;
        plab[i]->SetMsg(msg);
    }

    /*showmsg msg;
    msg.camid="1";
    msg.status="背景颜色";
    msg.warning = true;
    ui->lAB->SetMsg(msg);*/

}
void Widget::DealShowobjs(QList<ObjdectRls> list, int index)
{
    int count = showlst[index].count();
    int comecount = list.count();
    if(comecount == 0)
    {
        showlst[index].clear();
        return;
    }
    if(count == 0)
    {
        for(int i = 0; i < comecount; i++)
        {
            if(list[i].name != "")
            {
                showlst[index].append(list[i]);
            }
        }
        return;
    }

    int rmv[count];
    for(int i = 0; i <count ;i++)
    {
        rmv[i] = 0;
    }
    QList<ObjdectRls> addlst;
    for(int i = 0; i < comecount; i++)
    {
        if(list[i].name != "")
        {
            bool newc = true;
            for(int j = 0; j < count; j++)
            {
                if(list[i].name == showlst[index][j].name)
                {
                    if(showlst[index][j].withHat != list[i].withHat)
                    {
                        showlst[index][j].img = list[i].img;
                        showlst[index][j].withHat = list[i].withHat;
                    }
                    if(showlst[index][j].OPFrame <= 19)
                    {
                        if(list[i].OPFrame >= 20)
                        {
                            showlst[index][j].img = list[i].img;
                        }
                    }
                    showlst[index][j].OPFrame = list[i].OPFrame;
                    newc = false;
                    rmv[j] = 1;
                    break;
                }
            }
            if(newc == true)
            {
                addlst.append(list[i]);
            }
        }
    }
    for(int j = showlst[index].count() - 1; j >=0; j--)
    {
        if(rmv[j] == 0)
        {
            showlst[index].removeAt(j);
        }
    }
    showlst[index].append(addlst);

    if(index == 1)
    {
        for(int i = 0; i < showlst[index].count(); i++)
        {
            int ret = IsInList(showlst[index][i].name);
            if(ret == -1)
            {
                showobjs.push_front(showlst[index][i]);
            }
            else
            {
                showobjs[ret] = showlst[index][i];
            }
        }
    }
    else if(index == 0)
    {
        for(int i = 0; i < showlst[index].count(); i++)
        {
            if(showlst[index][i].OPFrame >= 20)
            {
                int ret = IsInList(showlst[index][i].name);
                if(ret == -1)
                {
                    showobjs.push_front(showlst[index][i]);
                }
                else
                {
                    if(showobjs[ret].CAMID != 0)
                    {
                        showobjs.push_front(showlst[index][i]);
                    }
                    else
                    {
                        showobjs[ret] = showlst[index][i];
                    }
                }
            }
        }
    }


    LAB *plab[5] = {ui->lAB, ui->lAB_2, ui->lAB_3, ui->lAB_4, ui->lAB_5};
    /*showobjs.clear();
    showobjs.append(showlst[0]);
    for(int i = 0; i < showlst[1].count(); i++)
    {
        if(IsInList(showlst[1][i].name) == false)
        {
            showobjs.append(showlst[1][i]);
        }
    }
    for(int i = 0; i < showlst[2].count(); i++)
    {
        if(IsInList(showlst[2][i].name) == false)
        {
            showobjs.append(showlst[2][i]);
        }
    }*/
    count = showobjs.count();
    if(lastshowcount == count)
        return;
    lastshowcount = count;
    //if(showobjs.count() > 5)
        //showobjs.removeFirst();
    int ma = min(count, 5);
    showindex = 0;
    for(int i = 0; i < ma; i++)
    {
        showmsg msg;
        msg.camid = "工位:" + QString::number(showobjs[i].CAMID);
        msg.warning = false;
        if(showobjs[i].withHat == 0)
        {
            msg.status = "未戴安全帽";
            msg.warning = true;
        }
        if(showobjs[i].OPFrame >= 20)
        {
            if(IsOperator(showobjs[i].name) == false)
            {
                msg.warning = true;
                msg.status = "违规操作";
                Machine_Violation = true;
                GotoMachine();
            }
            else
            {
                msg.status = "操作CNC";
                msg.warning = false;
                Machine_Violation = false;
            }

        }
        if(showobjs[i].CAMID == 1)
        {
            msg.status = "进入车间";
        }
        msg.ttime = showobjs[i].captime.toString("HH:mm::ss");
        msg.img = showobjs[i].img;
        msg.id = showobjs[i].name;
        plab[i]->SetMsg(msg);
    }

}
int Widget::IsInList(QString name)
{
    for(int i = 0; i < showobjs.count(); i++)
    {
        if(showobjs[i].name == name)
        {
            return i;
        }
    }
    return -1;
}
void Widget::DealTraceLst(ObjdectRls rls)
{

    bool exsit = false;
    for(int m = 0; m < tracelst.count(); m++)
    {
        if(rls.ID == tracelst[m].objID)
        {
            exsit = true;
            break;
        }
    }
    if(exsit == false)
    {
        TraceInfo info;
        info.objID = rls.ID;
        info.frames = 0;
        tracelst.append(info);
    }

}
int Widget::GetCurrentTraceObj(QList<ObjdectRls> list)
{
    int count = list.count();
    if(count == 0)
    {
        currentframes = 0;
        return -1;
    }
    if(count == 1)
    {
        curtraceid = 0;
        currentframes++;
        return 0;
    }
    if(currentframes > 100)
    {
        curtraceid++;
        if(curtraceid >= count)
        {
            curtraceid = 0;
        }
        currentframes = 0;
    }
    currentframes++;
    return curtraceid;
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
        list[i].captime = QDateTime::currentDateTime();
        objdects[idx].append(list[i]);
        //DealShowObjs(list[i]);
    }

    if(idx == 2 && Machine_Violation == false)
    {
        int index = GetCurrentTraceObj(list);
        qDebug() << "track id:"<< index <<endl;
        if(index != -1)
        {
            int x = list[index].rect.x() + list[index].rect.width()/2;
            int y = list[index].rect.y() + list[index].rect.height()/8;
            int p = 832 + (x-480)*0.45625;
            int t = -135 + (y - 270)*0.4259;
            if( t < 0)
            {
                t = t + 3600;
            }
            NET_DVR_PTZPOS pos;
            pos.wAction = 1;
            pos.wPanPos = D2H(p);
            pos.wTiltPos = D2H(t);
            if(y < 500)
                pos.wZoomPos = 0x40;
            else
                pos.wZoomPos = 0x30;
            play_thd[3]->cam->SetPTZ(pos);
        }
    }

    DealShowobjs(list,idx);
    mutex.unlock();
    /*if(idx == 2)
    {

        if(count != 0)
        {
            int x = list[0].rect.x() + list[0].rect.width()/2;
            int y = list[0].rect.y() + list[0].rect.height()/8;
            int p = 832 + (x-480)*0.45625;
            int t = -135 + (y - 270)*0.4259;
            if( t < 0)
            {
                t = t + 3600;
            }
            NET_DVR_PTZPOS pos;
            pos.wAction = 1;
            pos.wPanPos = D2H(p);
            pos.wTiltPos = D2H(t);
            pos.wZoomPos = 0x30;
            play_thd[3]->cam->SetPTZ(pos);
        }
    }*/
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
    //ui->pushButtonmMenu->setVisible(false);
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

    //play_thd[3]->cam->AutoPan(0);

    /*play_thd[0]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180905/ch01_20180905162901.mp4", 1);
    play_thd[1]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180905/ch02_20180905162901.mp4", 2);
    play_thd[2]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180905/ch03_20180905162901.mp4", 3);
    play_thd[3]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180905/ch04_20180905162901.mp4", 4);
    //play_thd[0]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180910/ch01_20180910152525.mp4", 1);
    //play_thd[1]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180910/ch02_20180910152525.mp4", 2);
    //play_thd[2]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180910/ch03_20180910152524.mp4", 3);
    //play_thd[3]->setPlayClient("/home/proj/lab/cam_data/SampleVideo/20180910/ch04_20180910152524.mp4", 4);
    play_thd[0]->start();
    play_thd[1]->start();
    play_thd[2]->start();
    play_thd[3]->start();*/



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
void Widget::upShow()
{
    int count = showobjs.count();
    if(showindex + 5 >= count)
        return;
    LAB *plab[5] = {ui->lAB, ui->lAB_2, ui->lAB_3, ui->lAB_4, ui->lAB_5};
    showindex++;
    for(int i = showindex; i < showindex + 5; i++)
    {
        showmsg msg;
        msg.camid = "工位:" + QString::number(showobjs[i].CAMID);
        msg.warning = false;
        if(showobjs[i].withHat == 0)
        {
            msg.status = "未戴安全帽";
            msg.warning = true;
        }
        if(showobjs[i].OPFrame >= 20)
        {
            if(IsOperator(showobjs[i].name) == false)
            {
                msg.warning = true;
                msg.status = "违规操作";
            }
            else
            {
                msg.status = "操作CNC";
                msg.warning = false;
            }

        }
        if(showobjs[i].CAMID == 1)
        {
            msg.status = "进入车间";
        }
        msg.ttime = showobjs[i].captime.toString("HH:mm::ss");
        msg.img = showobjs[i].img;
        msg.id = showobjs[i].name;
        plab[i - showindex]->SetMsg(msg);
    }

}
void Widget::downShow()
{
    if(showindex == 0)
        return;
    LAB *plab[5] = {ui->lAB, ui->lAB_2, ui->lAB_3, ui->lAB_4, ui->lAB_5};
    showindex--;
    for(int i = showindex; i < showindex + 5; i++)
    {
        showmsg msg;
        msg.camid = "工位:" + QString::number(showobjs[i].CAMID);
        msg.warning = false;
        if(showobjs[i].withHat == 0)
        {
            msg.status = "未戴安全帽";
            msg.warning = true;
        }
        if(showobjs[i].OPFrame >= 20)
        {
            if(IsOperator(showobjs[i].name) == false)
            {
                msg.warning = true;
                msg.status = "违规操作";
            }
            else
            {
                msg.status = "操作CNC";
                msg.warning = false;
            }

        }
        if(showobjs[i].CAMID == 1)
        {
            msg.status = "进入车间";
        }
        msg.ttime = showobjs[i].captime.toString("HH:mm::ss");
        msg.img = showobjs[i].img;
        msg.id = showobjs[i].name;
        plab[i - showindex]->SetMsg(msg);
    }
}
int Widget::D2H(int d)
{
    int ret = 0;
    int index = 0;
    while(d != 0)
    {
        int x = d % 10;
        ret += x * qPow(16, index);
        d = d / 10;
        index++;
    }
    return ret;
}
void Widget::GotoMachine()
{
    NET_DVR_PTZPOS pos;
    pos.wAction = 1;
    pos.wPanPos = 0x910;

    pos.wTiltPos = 0x3500;
    pos.wZoomPos = 0x30;
    play_thd[3]->cam->SetPTZ(pos);
}
