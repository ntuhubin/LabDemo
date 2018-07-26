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
    for(int i = 0; i < maintainhuman[1].count(); i++)
    {
        if(dectresult[0].tarckid == maintainhuman[1][i].ID)
        {
            maintainhuman[1][i].name = dectresult[0].lable;
            break;
        }
    }
    mutex.unlock();
}
int CHuamDectThd::DetectHuman(Mat img, int *rect, QImage qimg)
{
    DetectedObjectGroup detected_object_group;
    const float score_threshold = 0.6f;
    const int max_num_detections = 4;
    string frozen_graph_path = "helmet_model_v2.pb.quantize";
    const DetectorType detector_type = HUMAN;
    if (!Detection(img, &detected_object_group, frozen_graph_path,
                       detector_type, score_threshold, max_num_detections)) {
        return -1;
    }
    list[dectindex].clear();
    hatlist[dectindex].clear();
    for (int num = 0; num < detected_object_group.num_of_object; num++) {
        DetectedObject detected_object;
        detected_object = detected_object_group.detected_objects[num];

        Point2f top_left(detected_object.left,detected_object.top);
        Point2f bottom_right(detected_object.width+detected_object.left,
                             detected_object.height+detected_object.top);
        /*rectangle(img, top_left, bottom_right, CV_RGB(255,0,0), 2);*/
        rect[0] = detected_object.left;
        rect[1] = detected_object.top;
        rect[2] = bottom_right.x;
        ObjdectRls objrls;
        objrls.CAMID = dectindex;
        objrls.ObjID = detected_object.object_id;
        objrls.withHat = 0;
        /*objrls.rect[0] = detected_object.left;
        objrls.rect[1] = detected_object.top;
        objrls.rect[2] = bottom_right.x;
        objrls.rect[3] = bottom_right.y;*/
        objrls.rect = QRectF(detected_object.left,detected_object.top,detected_object.width,detected_object.height);
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
            int h_bins = 30; int s_bins = 32;
            int histSize[] = { h_bins, s_bins };
            // hue的取值范围从0到256, saturation取值范围从0到180
            float h_ranges[] = { 0, 180 };
            float s_ranges[] = { 0, 256 };
            const float* ranges[] = { h_ranges, s_ranges };
            // 使用第0和第1通道
            int channels[] = { 0, 1 };
            calcHist( &ROIImg, 1, channels, Mat(), objrls.Hist, 2, histSize, ranges, true, false );
            normalize( objrls.Hist, objrls.Hist, 0, 1, NORM_MINMAX);
            objrls.ID = 0;
            list[dectindex].append(objrls);
        }
        else if(objrls.ObjID == 3)  //an quan mao
            hatlist[dectindex].append(objrls);
      }
    return 0;
}
void CHuamDectThd::run()
{
    int rect[4];
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

            DetectHuman(mat, rect, qimg);
            //qDebug()<<time.elapsed()/1000.0<<"s";
            CheckHat();
            MaintainObj();


           emit message(maintainhuman[dectindex], dectindex);

            dectindex++;
            if(dectindex == 3)
                dectindex = 0;
        }

        if(b_stop == true)
        {
            break;
        }
        msleep(30);
    }
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
                double coo =  0;
                int iddx = 0;
                for(int j = 0; j < list[dectindex].count(); j++)
                {
                    double co = compareHist( maintainhuman[dectindex][i].Hist, list[dectindex][j].Hist, CV_COMP_CORREL );
                    if(co > coo)
                    {
                        coo = co;
                        iddx = j;
                    }
                }
                maintainhuman[dectindex][i].Hist = list[dectindex][iddx].Hist;
                maintainhuman[dectindex][i].rect = list[dectindex][iddx].rect;
                maintainhuman[dectindex][i].img = list[dectindex][iddx].img;
                maintainhuman[dectindex][i].leaveframe = 0;
                list[dectindex].removeAt(iddx);
            }
            for(int i = 0; i < list[dectindex].count(); i++)
            {
                list[dectindex][i].ID = currentObjID[dectindex];
                list[dectindex][i].leaveframe = 0;
                maintainhuman[dectindex].append(list[i]);
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
                double coo =  0;
                int iddx = 0;
                for(int j = 0; j < count; j++)
                {
                    double co = compareHist(list[dectindex][i].Hist, maintainhuman[dectindex][j].Hist, CV_COMP_CORREL );
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
                    maintainhuman[dectindex][i].leaveframe++;
                }
                else
                {
                    maintainhuman[dectindex][i].Hist = list[dectindex][corr[i]].Hist;
                    maintainhuman[dectindex][i].rect = list[dectindex][corr[i]].rect;
                    maintainhuman[dectindex][i].img = list[dectindex][corr[i]].img;
                    maintainhuman[dectindex][i].leaveframe = 0;
                }
            }
            QMutableListIterator<ObjdectRls> it(maintainhuman[dectindex]);
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

    for(int i = 0; i < maintainhuman[1].count(); i++)
    {
        if(maintainhuman[1][i].name == "")  //process one img per time
        {
            face_thd->SetParam(maintainhuman[1][i].img, maintainhuman[1][i].ID);
        }
    }
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
void CHuamDectThd::CheckHat()
{
    int humamCount = list[dectindex].count();
    int hatCount = hatlist[dectindex].count();
    if(hatCount == 0)
        return;
    if(humamCount == hatCount)
    {
        for(int i = 0; i < humamCount; i++)
        {
            list[dectindex][i].withHat = 1;
        }
        return;
    }
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
void CHuamDectThd::StopRun()
{
    face_thd->setStop();
    b_stop = true;
}
