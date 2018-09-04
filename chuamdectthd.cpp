#include "chuamdectthd.h"

CHuamDectThd::CHuamDectThd(int idx):QThread()
{
    idd = idx;
    b_stop = false;
    dectindex = 0;
    for(int i = 0; i < 3; i++)
    {
        currentObjID[i] = 1;
    }
    face_thd = new CFaceClsThread();
    connect(face_thd, &CFaceClsThread::sendcls,this,&CHuamDectThd::recvFace);
    face_thd->start();

    personReid = new CPersonReIDThd();
    connect(personReid, &CPersonReIDThd::SendReid, this, &CHuamDectThd::RecvReid);
    personReid->start();

    db = new CDbPro();

    qRegisterMetaType<QList<ObjdectRls>> ("QList<ObjdectRls>");
}
void CHuamDectThd::recvImg(QImage img, int idx)
{
    mutex.lock();
    if(idx != 4)
    {
        curImg[idx - 1] = img.copy();
    }
    mutex.unlock();
}
void CHuamDectThd::recvFace(QList<ClsResult> dectresult)
{
    int count = dectresult.count();
    if(count != 1)
    {
        return;
    }
    mutex.lock();
    int index = -10;
    ObjdectRls rls;
    for(int i = 0; i < maintainhuman[1].count(); i++)
    {
        if(maintainhuman[1][i].name != "")
        {
            continue;
        }
        if(dectresult[0].tarckid == maintainhuman[1][i].ID)
        {
            int eventid = 0;
            maintainhuman[1][i].name = dectresult[0].lable;
            index = i;
            rls = maintainhuman[1][i];
            if(rls.withHat == 0)
            {
                eventid = 1;
                char*  chname;
                QByteArray ba = rls.name.toLatin1(); // must
                chname = ba.data();

                char* chcamid;
                QString s = QString::number(rls.CAMID+1,10);
                QByteArray bcamid = s.toLatin1();
                chcamid = bcamid.data();
                //publicFun::SendSMS(chname, chcamid);
            }
            AddEventDB(2,dectresult[0].lable, eventid, dectresult[0].dectImg);

            break;
        }
    }
    mutex.unlock();
    if(index < 0)
    {
        return;
    }

    if(facePerson.count() == 0)
    {
        facePerson.append(rls);
        mutex.unlock();
        return;
    }
    bool newcomer = true;
    for(int i = 0; i < facePerson.count(); i++)
    {
        if(facePerson[i].name == rls.name)
        {
            facePerson[i] = rls;
            newcomer = false;
            break;
        }
    }
    if(newcomer == true)
    {
        facePerson.append(maintainhuman[1][index]);
    }
}
void CHuamDectThd::recvreid(ObjdectRls rls1, ObjdectRls rls2)
{
    if(rls1.name != "")
    {
        for(int n = 0; n < maintainhuman[0].count(); n++)
        {
            if(rls1.ID == maintainhuman[0][n].ID)
            {
                maintainhuman[0][n].name = rls1.name;
            }
        }
    }

    if(rls2.name != "")
    {
        for(int n = 0; n < maintainhuman[2].count(); n++)
        {
            if(rls2.ID == maintainhuman[2][n].ID)
            {
                maintainhuman[2][n].name = rls2.name;
            }
        }
    }
}
void CHuamDectThd::RecvReid(QList<ObjdectRls> list1, QList<ObjdectRls> list2)
{
    mutex.lock();
    for(int m = 0; m < list1.count(); m++)
    {
        for(int n = 0; n < maintainhuman[0].count(); n++)
        {
            if(list1[m].ID == maintainhuman[0][n].ID)
            {
               if(list1[m].name != "")
               {
                   maintainhuman[0][n].name = list1[m].name;
                   break;
               }

            }
        }
    }
    for(int m = 0; m < list2.count(); m++)
    {
        for(int n = 0; n < maintainhuman[2].count(); n++)
        {
            if(list2[m].ID == maintainhuman[2][n].ID)
            {
                if(list2[m].name != "")
                {
                    maintainhuman[2][n].name = list2[m].name;
                    break;
                }
            }
        }
    }
    mutex.unlock();
}
void CHuamDectThd::MultiPrepareReid()
{
    QList<ObjdectRls> rls[3];
    if(facePerson.count() == 0)
    {
        return;
    }
    for(int i = 0; i < facePerson.count(); i++)
    {
        rls[1].append(facePerson[i]);
    }
    for(int m = 0; m < maintainhuman[0].count(); m++)
    {
        if(maintainhuman[0][m].name == "")
        {
            rls[0].append(maintainhuman[0][m]);
        }
    }
    for(int m = 0; m < maintainhuman[2].count(); m++)
    {
        if(maintainhuman[2][m].name == "")
        {
            rls[2].append(maintainhuman[2][m]);
        }
    }
    if((rls[0].count() == 0) && (rls[2].count() == 0))
    {
        return;
    }
    else
    {
        personReid->ReidList(rls);
    }
}
void CHuamDectThd::PrepareReid(ObjdectRls Objrls)
{
    QList<ObjdectRls> rls[3];
    rls[1].append(Objrls);
    bool reid = true;
    for(int m = 0; m < maintainhuman[0].count(); m++)
    {
        if(maintainhuman[0][m].name == Objrls.name)  //exsit this person
        {
            reid = false;
        }
        if(maintainhuman[0][m].name == "")
        {
            rls[0].append(maintainhuman[0][m]);
        }
    }
    if(reid == false)
    {
        rls[0].clear();
    }
    reid = true;
    for(int m = 0; m < maintainhuman[2].count(); m++)
    {
        if(maintainhuman[2][m].name == Objrls.name)  //exsit this person
        {
            reid = false;
        }
        if(maintainhuman[2][m].name == "")
        {
            rls[2].append(maintainhuman[2][m]);
        }
    }
    if(reid == false)
    {
        rls[2].clear();
    }
    if((rls[0].count() == 0) && (rls[2].count() == 0))
    {
        return;
    }
    else
    {
        personReid->ReidList(rls);
    }
}
int CHuamDectThd::DetectHuman(Mat img, QImage qimg, cv::Rect rc)
{
    DetectedObjectGroup detected_object_group;
    const float score_threshold = 0.6f;
    const int max_num_detections = 6;
    string frozen_graph_path = "helmet_model_v3.pb.quantize";
    const DetectorType detector_type = HUMAN;
    //Mat imgage = img.clone();
    //Mat roi;
    //roi=img(rc);
    //Mat dectroi = roi.clone();
    /*img = img(rc);
    string name = std::to_string(dectindex) + ".jpg";
    imwrite(name, img);
    img = imread(name);*/
    if (!Detection(img, &detected_object_group, frozen_graph_path,
                       detector_type, score_threshold, max_num_detections)) {
        return -1;
    }
    list[dectindex].clear();
    hatlist[dectindex].clear();
    for (int num = 0; num < detected_object_group.num_of_object; num++) {
        DetectedObject detected_object;
        detected_object = detected_object_group.detected_objects[num];

        /*Point2f top_left(detected_object.left,detected_object.top);
        Point2f bottom_right(detected_object.width+detected_object.left,
                             detected_object.height+detected_object.top);
        rectangle(img, top_left, bottom_right, CV_RGB(255,0,0), 2);*/
        /*rect[0] = detected_object.left;
        rect[1] = detected_object.top;
        rect[2] = bottom_right.x;*/
        ObjdectRls objrls;
        objrls.CAMID = dectindex;
        objrls.ObjID = detected_object.object_id;
        objrls.withHat = 0;

        //objrls.rect = QRectF(detected_object.left + rc.x, detected_object.top + rc.y, detected_object.width,detected_object.height);
        objrls.rect = QRectF(detected_object.left, detected_object.top, detected_object.width,detected_object.height);
        objrls.img = qimg.copy(objrls.rect.toRect());
        if(objrls.ObjID == 1) //human
        {
            int rectsize = objrls.rect.width() * objrls.rect.height();
            if(rectsize < 160 * 80)
            {
                continue;
            }
            cv::Mat ROIImg = publicFun::QImageToMat(objrls.img);
            cvtColor( ROIImg, ROIImg,  COLOR_RGB2HSV);
            // 对hue通道使用30个bin,对saturatoin通道使用32个bin
            int h_bins = 60; int s_bins = 60;
            int histSize[] = { h_bins, s_bins };
            // hue的取值范围从0到256, saturation取值范围从0到180
            float h_ranges[] = { 0, 256 };
            float s_ranges[] = { 0, 180 };
            const float* ranges[] = { h_ranges, s_ranges };
            // 使用第0和第1通道
            int channels[] = { 0, 1 };
            calcHist( &ROIImg, 1, channels, Mat(), objrls.Hist, 2, histSize, ranges, true, false );
            normalize( objrls.Hist, objrls.Hist, 0, 1, NORM_MINMAX, -1, Mat());
            objrls.ID = 0;
            objrls.OPFrame = 0;
            objrls.leaveframe = 0;
            objrls.LeaOPFrame = 0;
            list[dectindex].append(objrls);
        }
        else if(objrls.ObjID == 3)  //an quan mao
            hatlist[dectindex].append(objrls);
      }
    return 0;
}
void CHuamDectThd::run()
{
    //int rect[4];
    cv::Rect rc[3];
    rc[0] = cv::Rect(574,129,1269,935);
    rc[1] = cv::Rect(306,158,1308,902);
    rc[2] = cv::Rect(536,223,801,845);
    while(true)
    {
        if(curImg[dectindex].isNull() == false)
        {
            mutex.lock();
            cv::Mat mat = publicFun::QImageToMat(curImg[dectindex]);
            QImage qimg = curImg[dectindex].copy();
            mutex.unlock();
            //QTime time;
            //time.start();

            DetectHuman(mat, qimg, rc[dectindex]);
            //qDebug()<<time.elapsed()/1000.0<<"s";
            CheckHat();
            mutex.lock();
            MaintainObj_3();

            for(int i = 0; i < maintainhuman[1].count(); i++)
            {
                if(maintainhuman[1][i].name == "")  //process one img per time
                {
                    face_thd->SetParam(maintainhuman[1][i].img, maintainhuman[1][i].ID);
                }
            }

            //ProcessOPArea();

            MultiPrepareReid();

            emit message(maintainhuman[dectindex], dectindex);
            mutex.unlock();

            dectindex++;
            if(dectindex >= 3)
            {
                dectindex = 0;
            }
        }

        if(b_stop == true)
        {
            break;
        }
        msleep(30);
    }
}
void CHuamDectThd::MaintainObj_3()
{
    int count = maintainhuman[dectindex].count();
    int comcount = list[dectindex].count();
    if(count == 0)
    {
        for(int i = 0; i < comcount; i++)
        {
            list[dectindex][i].ID = currentObjID[dectindex];
            list[dectindex][i].leaveframe = 0;
            maintainhuman[dectindex].append(list[dectindex][i]);
            currentObjID[dectindex]++;
        }
        return;
    }
    if(comcount == 0)
    {
        for(int i = count - 1; i >= 0; i--)
        {
            maintainhuman[dectindex][i].leaveframe++;
            if(maintainhuman[dectindex][i].leaveframe > 3)
            {
                maintainhuman[dectindex].removeAt(i);
            }
        }
        return;
    }
    int mc[count];
    int dc[comcount];
    std::vector<DisPair> pairvector;
    for(int i = 0; i < count; i++)
    {
        for (int j = 0; j < comcount; j++)
        {
            DisPair dp;
            dp.cm = i;
            dp.cd = j;
            double co = 100;
            if(maintainhuman[dectindex][i].Hist.type() == list[dectindex][j].Hist.type())
            {
                co = compareHist( maintainhuman[dectindex][i].Hist, list[dectindex][j].Hist, CV_COMP_BHATTACHARYYA );
            }
            dp.bhatdis = co;
            QRectF r = maintainhuman[dectindex][i].rect.intersected(list[dectindex][j].rect);
            dp.corssarea = r.width() * r.height();
            pairvector.push_back(dp);
            dc[j] = 0;
        }
        mc[i] = 0;
    }
    std::sort(pairvector.begin(), pairvector.end(), LessSort);
    for(auto it = pairvector.begin(); it != pairvector.end(); it++)
    {
        if(mc[it->cm] == 1)
        {
            continue;
        }
        if(dc[it->cd] == 1)
        {
            continue;
        }
        if(it->corssarea == 0)
        {
            continue;
        }
        if (it->bhatdis > 1)
        {
            continue;
        }

        maintainhuman[dectindex][it->cm].Hist = list[dectindex][it->cd].Hist;
        maintainhuman[dectindex][it->cm].rect = list[dectindex][it->cd].rect;
        maintainhuman[dectindex][it->cm].img = list[dectindex][it->cd].img;
        maintainhuman[dectindex][it->cm].leaveframe = 0;
        mc[it->cm] = 1;
        dc[it->cd] = 1;
    }
    for(int i = count - 1; i >= 0; i--)
    {
        if(mc[i] == 0)
        {
            maintainhuman[dectindex][i].leaveframe++;
            if(maintainhuman[dectindex][i].leaveframe > 3)
            {
                maintainhuman[dectindex].removeAt(i);
            }
        }
        /*if(dectindex == 1)
        {
            if(maintainhuman[1][i].name == "")  //process one img per time
            {
                face_thd->SetParam(maintainhuman[1][i].img, maintainhuman[1][i].ID);
            }
        }*/
    }
    for(int i = 0; i < comcount; i++)
    {
        if(dc[i] == 0)
        {
            list[dectindex][i].ID = currentObjID[dectindex];
            list[dectindex][i].leaveframe = 0;
            maintainhuman[dectindex].append(list[dectindex][i]);
            currentObjID[dectindex]++;
        }
    }
}
void CHuamDectThd::MaintainObj_2()
{
    int count = maintainhuman[dectindex].count();
    int comcount = list[dectindex].count();
    if(count == 0)
    {
        for(int i = 0; i < list[dectindex].count(); i++)
        {
            list[dectindex][i].ID = currentObjID[dectindex];
            maintainhuman[dectindex].append(list[dectindex].at(i));
            currentObjID[dectindex]++;
        }
    }
    else
    {
        for(int i = count - 1; i >= 0; i--)
        {
            bool update = false;
            int iddx = 0;
            for(int j = 0; j < list[dectindex].count(); j++)
            {
                double co = 1;
                if(maintainhuman[dectindex][i].Hist.type() == list[dectindex][j].Hist.type())
                {
                    co = compareHist( maintainhuman[dectindex][i].Hist, list[dectindex][j].Hist, CV_COMP_BHATTACHARYYA );
                }

                /*QFile f("coo.txt");
                if(f.open(QIODevice::Append | QIODevice::Text))
                {
                    QTextStream txtOutput(&f);
                    txtOutput << co << endl;
                    f.close();
                }*/

                if(co < 0.4) //same
                {
                    maintainhuman[dectindex][i].Hist = list[dectindex][j].Hist;
                    maintainhuman[dectindex][i].rect = list[dectindex][j].rect;
                    maintainhuman[dectindex][i].img = list[dectindex][j].img;
                    maintainhuman[dectindex][i].leaveframe = 0;
                    iddx = j;
                    update = true;
                    break;
                }


            }
            if(update == true)
            {
                list[dectindex].removeAt(iddx);
            }
            else
            {
                maintainhuman[dectindex][i].leaveframe++;
                int lf = maintainhuman[dectindex][i].leaveframe;
                if(lf > 4)
                {

                    /*if(dectindex == 1)
                    {
                        for(int m = 0; m < facePerson.count(); m++)
                        {
                            if(facePerson[m].name == maintainhuman[dectindex][i].name)
                            {
                                facePerson.removeAt(m);
                                break;
                            }
                        }
                    }*/
                    maintainhuman[dectindex].removeAt(i);
                }
            }
        }



        for(int i = 0; i < list[dectindex].count(); i++)
        {
            list[dectindex][i].ID = currentObjID[dectindex];
            list[dectindex][i].leaveframe = 0;
            maintainhuman[dectindex].append(list[dectindex][i]);
            currentObjID[dectindex]++;
        }
    }



    for(int i = 0; i < maintainhuman[1].count(); i++)
    {
        if(maintainhuman[1][i].name == "")  //process one img per time
        {
            face_thd->SetParam(maintainhuman[1][i].img, maintainhuman[1][i].ID);
        }
    }
    ProcessOPArea();
}
void CHuamDectThd::MaintainObj()
{
    int count = maintainhuman[dectindex].count();
    int comcount = list[dectindex].count();
    if(count == 0)
    {
        for(int i = 0; i < list[dectindex].count(); i++)
        {
            list[dectindex][i].ID = currentObjID[dectindex];
            maintainhuman[dectindex].append(list[dectindex].at(i));
            currentObjID[dectindex]++;
        }
    }
    else
    {
        if(count <= comcount)  //new comer
        {
            for(int i = 0; i < count; i++)
            {
                double coo =  1;
                int iddx = -1;
                for(int j = 0; j < list[dectindex].count(); j++)
                {
                    double co = 100;
                    if(maintainhuman[dectindex][i].Hist.type() == list[dectindex][j].Hist.type())
                    {
                        co = compareHist( maintainhuman[dectindex][i].Hist, list[dectindex][j].Hist, CV_COMP_BHATTACHARYYA );
                    }
                    if(co < coo)
                    {
                        coo = co;
                        iddx = j;
                    }
                }
                if(iddx != -1)
                {
                    maintainhuman[dectindex][i].Hist = list[dectindex][iddx].Hist;
                    maintainhuman[dectindex][i].rect = list[dectindex][iddx].rect;
                    maintainhuman[dectindex][i].img = list[dectindex][iddx].img;
                    maintainhuman[dectindex][i].leaveframe = 0;
                    list[dectindex].removeAt(iddx);
                }

            }
            for(int m = 0; m < list[dectindex].count(); m++)
            {
                list[dectindex][m].ID = currentObjID[dectindex];
                list[dectindex][m].leaveframe = 0;
                maintainhuman[dectindex].append(list[dectindex][m]);

                currentObjID[dectindex]++;
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
                double coo =  1;
                int iddx = -1;
                for(int j = 0; j < count; j++)
                {
                    double co = 100;
                    if(maintainhuman[dectindex][j].Hist.type() == list[dectindex][i].Hist.type())
                    {
                        co = compareHist( maintainhuman[dectindex][j].Hist, list[dectindex][i].Hist, CV_COMP_BHATTACHARYYA );
                    }
                    if(co < coo)
                    {
                        coo = co;
                        iddx = j;
                    }
                }
                if(iddx != -1)
                {
                    corr[iddx] = i;
                }

            }

            for(int i = count - 1; i >= 0; i--)
            {
                if(corr[i] == -1)
                {
                    maintainhuman[dectindex][i].leaveframe++;
                    if(maintainhuman[dectindex][i].leaveframe > 3)
                    {
                        maintainhuman[dectindex].removeAt(i);
                    }
                }
                else
                {
                    maintainhuman[dectindex][i].Hist = list[dectindex][corr[i]].Hist;
                    maintainhuman[dectindex][i].rect = list[dectindex][corr[i]].rect;
                    maintainhuman[dectindex][i].img = list[dectindex][corr[i]].img;
                    maintainhuman[dectindex][i].leaveframe = 0;
                }
            }
        }
    }

    for(int i = 0; i < maintainhuman[1].count(); i++)
    {
        if(maintainhuman[1][i].name == "")  //process one img per time
        {
            face_thd->SetParam(maintainhuman[1][i].img, maintainhuman[1][i].ID);
        }
    }
    ProcessOPArea();
}
int CHuamDectThd::isCross(ObjdectRls rls1, ObjdectRls rls2)
{
    QRectF r = rls1.rect.intersected(rls2.rect);
    double rsize = r.width() * r.height();
    double hatsize = rls2.rect.width() * rls2.rect.height();
    if(rsize / hatsize >= 0.5)
        return 1;
    else
        return 0;
}
double CHuamDectThd::xmulti(Point pa, Point pb, Point pc)
{
    return (pb.x-pa.x)*(pc.y-pa.y) - (pb.y-pa.y)*(pc.x-pa.x);
}
bool CHuamDectThd::isINOpArea(int x, int y)
{
    bool ret = true;
    bool retb = true;
    float eps = 0.0000000001;
    cv::Point pt;
    pt.x = x;
    pt.y = y;
    cv::Point rectA[4];
    rectA[0].x = 1374;
    rectA[0].y = 427;
    rectA[1].x = 1550;
    rectA[1].y = 471;
    rectA[2].x = 1329;
    rectA[2].y = 630;
    rectA[3].x = 1157;
    rectA[3].y = 555;
    for(int i = 3; i >=1; i--)
    {
        if (xmulti(pt, rectA[i], rectA[i-1]) > 0)
        {
            ret = false;
            break;
        }
    }
    if(ret == true)
        return ret;
    rectA[0].x = 1139;
    rectA[0].y = 628;
    rectA[1].x = 1273;
    rectA[1].y = 690;
    rectA[2].x = 1000;
    rectA[2].y = 950;
    rectA[3].x = 760;
    rectA[3].y = 880;
    for(int i = 3; i >=1; i--)
    {
        if (xmulti(pt, rectA[i], rectA[i-1]) > 0)
        {
            retb = false;
            break;
        }
    }
    if(ret == false && retb == false)
    {
        return false;
    }
    return true;


    /*int xl = 1140, xr = 1500, yt = 410, yd = 650;
    if(x > xl && x < xr && y > yt && y < yd)
    {
        return true;
    }
    xl = 850;
    xr = 1300;
    yt = 600;
    yd = 1000;
    if(x > xl && x < xr && y > yt && y < yd)
    {
        return true;
    }
    return false;*/
}
bool CHuamDectThd::isINOPLst(string name)
{
    foreach (auto record, recordlst) {
        if(record.staffid == name)
        {
            return true;
        }
    }
    return false;
}
void CHuamDectThd::ProcessOPArea()
{
    for(int i = 0; i < maintainhuman[0].count(); i++)
    {
        int lx = maintainhuman[0][i].rect.x();
        int ly = maintainhuman[0][i].rect.y() + maintainhuman[0][i].rect.height();
        if(isINOpArea(lx, ly))
        {
            maintainhuman[0][i].OPFrame++;
            maintainhuman[0][i].LeaOPFrame = 0;
            if(maintainhuman[0][i].OPFrame > 50)
            {
                if(isINOPLst(maintainhuman[0][i].name.toStdString()) == false)
                {
                    OPRecord record;
                    record.staffid = maintainhuman[0][i].name.toStdString();
                    record.starttime = QDateTime::currentDateTime();
                    record.dectimg = maintainhuman[0][i].img;
                    recordlst.append(record);
                }
            }
        }
        else
        {
            maintainhuman[0][i].LeaOPFrame++;
            if(maintainhuman[0][i].LeaOPFrame > 10)
            {
                maintainhuman[0][i].OPFrame = 0;
                int count = recordlst.count();
                for(int i = count - 1; i>=0; i--)
                {
                    auto record = recordlst.at(i);
                    if(record.staffid == maintainhuman[0][i].name.toStdString())
                    {
                        record.endtime = QDateTime::currentDateTime();
                        db->ConnectDB();
                        db->InsertOpRecord(record);
                        db->CloseDB();
                        recordlst.removeAt(i);
                        break;
                    }
                }
            }
        }
    }
}
void CHuamDectThd::CheckHat()
{
    int humamCount = list[dectindex].count();
    int hatCount = hatlist[dectindex].count();
    if(hatCount == 0)
        return;
    /*if(humamCount == hatCount)
    {
        for(int i = 0; i < humamCount; i++)
        {
            list[dectindex][i].withHat = 1;
        }
        return;
    }*/
    for(int i = 0; i < humamCount; i++)
    {
        for(int j = 0; j < hatCount; j++)
        {
            int ret = isCross(list[dectindex].at(i), hatlist[dectindex].at(j));
            if(ret == 1)
            {
                list[dectindex][i].withHat = 1;
                break;
            }
        }
    }
}
void CHuamDectThd::AddEventDB(int camid, QString staffid, int eventid, QImage img)
{
    EventInfo info;
    info.camid = camid;
    info.staffid = staffid.toStdString();
    info.eventid = eventid;
    info.dectimg = img;
    info.eventtime = QDateTime::currentDateTime();
    if (db->ConnectDB())
    {
        db->InsertEvent(info);
        db->CloseDB();
    }
}
void CHuamDectThd::StopRun()
{
    face_thd->setStop();
    personReid->StopRun();
    b_stop = true;
}
