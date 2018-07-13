#include "chuamdectthd.h"

CHuamDectThd::CHuamDectThd(int idx):QThread()
{
    idd = idx;
    b_stop = false;
    dectindex = 0;
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
int CHuamDectThd::DetectHuman(Mat img, int *rect, QImage qimg)
{
    DetectedObjectGroup detected_object_group;
    const float score_threshold = 0.6f;
    const int max_num_detections = 4;
    string frozen_graph_path = "helmet_model.pb.quantize";
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
            list[dectindex].append(objrls);
            cv::Mat ROIImg = publicFun::QImageToMat(objrls.img);
            cvtColor( ROIImg, ROIImg,  COLOR_RGB2HSV);
            // 对hue通道使用30个bin,对saturatoin通道使用32个bin
            int h_bins = 64; int s_bins = 64;
            int histSize[] = { h_bins, s_bins };
            // hue的取值范围从0到256, saturation取值范围从0到180
            float h_ranges[] = { 0, 256 };
            float s_ranges[] = { 0, 180 };
            const float* ranges[] = { h_ranges, s_ranges };
            // 使用第0和第1通道
            int channels[] = { 0, 1 };
            calcHist( &ROIImg, 1, channels, Mat(), objrls.Hist, 2, histSize, ranges, true, false );
            normalize( objrls.Hist, objrls.Hist, 0, 1, NORM_MINMAX, -1, Mat() );
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

            //if(list[dectindex].count() != 0)
            {
                emit message(list[dectindex], dectindex);
            }
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
    int hatCount = list[dectindex].count();
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
    b_stop = true;
}
