#include "playlocalm4.h"

//QObject* par
PlayLocalM4::PlayLocalM4():QThread()
{
    cam = new CCameraControl();
    stopflag = true;
    //qRegisterMetaType<vector<cv::Rect> > ("vector<cv::Rect>");
}
PlayLocalM4::~PlayLocalM4()
{
    //qDebug() << "finsih";
}
void PlayLocalM4::setPlayClient(char *path, int idx)
{
    cam->SetParams(idx);
    int width = 0;
    int height = 0;
    cam->openM4File(path, width, height);
    picsize = width * height *3;
    idd = idx;
    //unsigned int w = width;
    //unsigned int h = height;
    //unsigned int size = w * h * 3;
    //unsigned int dwsize = 0;
}

void PlayLocalM4::setRealPlay(char *ip, int port, char *name, char *pwd, int idx, bool save, int stream)
{
    cam->SetParams(idx);
    cam->Login(ip, port, name, pwd);
    cam->StartRealPlay(save, stream);
    idd = idx;
    picsize = 2048 * 1536 *2;

}
void PlayLocalM4::stopRealPlay()
{
    stopflag = false;
    cam->StopRealPlay();
}
void PlayLocalM4::run()
{
    BYTE *buf = new BYTE[picsize];
    unsigned int dwsize = 0;
    int num = 0;
    while(1)
    {
        memset(buf, 0, picsize);
        cam->CapImage(buf, picsize, dwsize);
        QImage img;
        if(dwsize > 0)
        {
            //std::vector<cv::Rect> faces;
            /*char *data = new char[dwsize];
            memcpy(data,buf,dwsize);
            Mat rawdata(1, dwsize, CV_8UC1, (void*)data);
            cv::Mat matimg = imdecode(rawdata, CV_LOAD_IMAGE_COLOR);
            delete data;*/


            img.loadFromData(buf,dwsize);
            //qDebug() << "load finish";
            if(img.isNull() == false)
            {
                //QString str = QString::number(num, 10);
                emit message(img,idd);
            }

        }

        if(stopflag == false)
        {
            break;
        }
        msleep(100);
    }

    delete buf;
}
void PlayLocalM4::stopLocalplay()
{
    stopflag = false;
    cam->closeM4();
}
