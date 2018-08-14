#include "cfaceclsthread.h"

CFaceClsThread::CFaceClsThread():QThread()
{
    face = new CFaceClassification();
    //face->Init();
    isGet = false;
    stopflag = false;
    qRegisterMetaType<QList<ClsResult> > ("QList<ClsResult>");
}
CFaceClsThread::~CFaceClsThread()
{

}
void CFaceClsThread::SetParam(QImage img,  int devID)
{
    mutex.lock();
    //RecvImg = img;
    //devid = devID;
    ImgQue.append(img);
    humanidQue.append(devID);
    isGet = true;
    mutex.unlock();

}
/*void CFaceClsThread::recvImg(QImage img, int idx)
{
    mutex.lock();
    //RecvImg = img.copy();
    //devid = idx;
    ImgQue.append(img);
    humanid
    mutex.unlock();
}*/
void CFaceClsThread::setStart()
{
    stopflag = false;
}
void CFaceClsThread::setStop()
{
    stopflag = true;
}
void CFaceClsThread::run()
{
    int first = 0;
    face->GetGIL();
    face->Init();
    while(true)
    {
        /*if(isGet == false)
        {
            msleep(200);
            continue;
        }*/
        if (ImgQue.count() == 0)
        {
            msleep(200);
            continue;
        }

        //qDebug() << "HERE1";
        mutex.lock();
        QImage img = ImgQue.dequeue();
        int humanid = humanidQue.dequeue();
        mutex.unlock();
        if(img.isNull() == true)
        {
            msleep(200);
            continue;
        }
        if(stopflag == true)
        {
            break;
        }

        if(first != 0)
        {
            face->GetGIL();
        }
        else
        {
            first = 1;
        }
        QList<ClsResult> list;
        QString filename = "/tmp/" + QString::number(humanid,10)+".jpg";
        img.save(filename);

        face->classfier(filename.toLatin1().data());
        //face->classfier("./register/hu/1.jpg");
        int personnum = face->ClsList.count();
        face->ReleaseGIL();
        //qDebug() << personnum;
        if(personnum == 0)
        {
            msleep(200);
            continue;
        }
        for(int i = 0; i < personnum; i++)
        {
            ClsResult cr;
            double pos[4];
            for(int j = 0; j < 4; j++)
            {
                pos[j] = face->ClsList[i].pos[j];
            }
            cr.dectImg = img.copy((pos[0] - 20) < 0 ? 0 : (pos[0] - 20),
                    (pos[1] - 20) < 0 ? 0 : (pos[1] - 20),
                    (pos[2] - pos[0] + 20 ) > img.width()? img.width() : (pos[2] - pos[0] + 20 ) ,
                    (pos[3] - pos[1] + 20 ) > img.height()? img.height() : (pos[3] - pos[1] + 20 ));
            cr.lable = face->ClsList[i].label;
            cr.belief = face->ClsList[i].belief;
            //cr.features = face->ClsList[i].features;
            cr.rect = QRectF(pos[0],pos[1], pos[2]-pos[0],pos[3]-pos[1]);
            cr.capDate = QDateTime::currentDateTime();
            cr.tarckid = humanid;
            list.append(cr);
        }

        emit sendcls(list);
        //isGet = false;
        msleep(200);
    }
}
/*int CFaceClsThread::UpdateCapList(ClsInfo clsinfo,  QImage dimg)
{
    int capnum = capList.count();
    if(capnum == 0)
    {
        CapInfo cinfo;
        cinfo.capImg = dimg;
        cinfo.label = clsinfo.label;
        cinfo.capDate = QDateTime::currentDateTime();
        capList.append(cinfo);
        return 0;
    }
    bool newperson = true;
    for(int i = 0; i < capnum; i++)
    {
        if(capList[i].label == clsinfo.label)
        {
            newperson = false;
            QDateTime dt = QDateTime::currentDateTime();
            qint64 msecs = dt.msecsTo(capList[i].capDate);
            if(msecs < 1000*60)  //60s one minute, do not need to update
            {
                capList[i].capDate = dt;
                return 1;
            }
            else {
                CapInfo cinfo;
                cinfo.capImg = dimg;
                cinfo.label = clsinfo.label;
                cinfo.capDate = QDateTime::currentDateTime();
                capList.append(cinfo);
                return 0;
            }
        }
    }
    if(newperson == true)
    {
        CapInfo cinfo;
        cinfo.capImg = dimg;
        cinfo.label = clsinfo.label;
        cinfo.capDate = QDateTime::currentDateTime();
        capList.append(cinfo);
        return 0;
    }
}*/
