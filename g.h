#ifndef G_H
#define G_H
#include <QRectF>
#include <QDate>
#include <QRectF>
#include <QImage>
#include "opencv2/imgproc/imgproc.hpp"
typedef struct
{
    int CAMID;
    int ObjID;   //obj type
    int withHat;   //如果是人，是否戴帽 0 不戴 1戴
    int ID;     //the same obj has a same ID from 1, trackid
    QString name;  //person's name or workid
    int leaveframe;
    QRectF rect;
    QImage img;   // capture image
    int OPFrame;    //站在操作区的帧数，超过一定帧则认为在操作机器
    int LeaOPFrame;  //离开操作区的帧数，超过一定帧则将OPFrame清0
    cv::MatND Hist;   //hist of human, used for match
}ObjdectRls;
typedef struct
{
    QString name;
    QImage img;
    int label;
    int realcount;
}SaveRegPerson, *pSaveRegPerson;
typedef struct
{
    QString name;
    QList<QString> imgs;
    QList<float> features;  //128*sizeof(imgs)
    int lable;
}RegisterPerson, *pRegisterPerson;
typedef struct
{
    QString name;
    QImage regImg;    //zhuce zhao pian
    QImage capImg;     //zhua tu
    QDateTime capDate;     //zhua tu shijian
    int label;
    double belief;
}ShowRegPerson, *pShowRegPerson;
typedef struct
{
    QString label;
    double belief;
    double pos[4];
    QList<float> features;  //128*sizeof(imgs)
}ClsInfo, *pClsInfo;
typedef struct
{
    int tarckid;    //对应行人检测中的行人ID
    QImage dectImg;
    //QList<float> features;  //128*sizeof(imgs)
    QString lable;   // 实际是注册人姓名
    double belief;
    QDateTime capDate;
    QRectF rect;     //脸部区域
}ClsResult,*pClsResult;

typedef struct
{
    std::string name;
    QList<float> features;  //128
    int showtimes;
    QDateTime Lastshow;
    QImage dectimg;
}StrangerInfo, *pStrangerInfo;

typedef struct
{
    std::string staffid;
    int eventid;   //0 nomal enter; 1 no hat
    QDateTime eventtime;
    int camid;    // 1-3
    QImage dectimg;
}EventInfo, *pEventInfo;

#endif // G_H
