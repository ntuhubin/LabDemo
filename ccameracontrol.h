#ifndef CCAMERACONTROL_H
#define CCAMERACONTROL_H


#include "includeCn/HCNetSDK.h"
#include "includeCn/LinuxPlayM4.h"
#include "includeCn/PlayM4.h"
#include <QObject>
#include <string>
using namespace std;


class CCameraControl: public QObject
{
    Q_OBJECT
//signals:
    //void sendrealpic(BYTE *pbuf, unsigned int realsize);
private:
    LONG lUserID;
    LONG lRealPlayHandle;
    LONG lport;
public:
    int imgw;
    int imgh;
    //BYTE *realBuf;
    string filesavename;
    int camid;
    bool b_savevideo;
public:
    explicit CCameraControl(QObject *parent = 0);
    void SetParams(int id);
    LONG Login(char *ip, int port, char *name, char *pwd);
    LONG StartRealPlay(bool b_save, int stream);
    void StopRealPlay();
    string SetFileName();
    friend void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lID, LONG lHandle, void *pUser);
    friend void CALLBACK g_RealDataCallBack(LONG lRealHandle, DWORD dwType, BYTE *buf, DWORD BufSize, void *dwuser);
public:
    BOOL openM4File(char *filename,int &w, int &h);
    void CapImage(BYTE *buf, unsigned int dwsize, unsigned int &capsize);
    void closeM4();
    friend void CALLBACK g_DisplayCBFun(LONG nport, char *buf, LONG size, LONG w, LONG h, LONG nStamp, LONG nType, void *nReserved);
};

#endif // CCAMERACONTROL_H
