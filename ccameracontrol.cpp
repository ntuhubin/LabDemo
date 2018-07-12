#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ccameracontrol.h"
#include <unistd.h>

using namespace std;
FILE *fp;

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lID, LONG lHandle, void *pUser)
{

}
void CALLBACK g_RealDataCallBack(LONG lRealHandle, DWORD dwType, BYTE *buf, DWORD BufSize, void *dwuser)
{
    CCameraControl *t = reinterpret_cast<CCameraControl*>(dwuser);
    LONG lPort= t->lport;
    switch (dwType) {
    case NET_DVR_SYSHEAD:
        if(lPort >= 0)
        {
            break;
        }
        if( !PlayM4_GetPort(&lPort))
        {
            break;
        }
        t->lport = lPort;
        if(BufSize > 0)
        {
            if( !PlayM4_SetStreamOpenMode(lPort, STREAME_REALTIME))
            {
                break;
            }
            if( !PlayM4_OpenStream(lPort, buf, BufSize, 1024*1024))
            {
                break;
            }
            if (!PlayM4_Play(lPort, 0))
            {
                break;
            }
            //
            if(t->b_savevideo == true)
            {
                fp = fopen(t->filesavename.c_str(),"wb");
                if(fp != 0)
                    fwrite(buf,BufSize,1,fp);
            }

        }
        break;
    case NET_DVR_STREAMDATA:
        if (BufSize >0 && lPort != -1)
        {
            if ( !PlayM4_InputData(lPort, buf, BufSize))
            {
                break;
            }
            if(t->b_savevideo == true)
            {
                if(fp != 0)
                    fwrite(buf,BufSize,1,fp);
            }
        }
        break;
    default:
        if (BufSize >0 && lPort != -1)
        {
            if ( !PlayM4_InputData(lPort, buf, BufSize))
            {
                break;
            }
        }
        break;
    }
}

void CALLBACK g_DisplayCBFun(LONG nport, char *buf, LONG size, LONG w, LONG h, LONG nStamp, LONG nType, void *nReserved)
{

}
CCameraControl::CCameraControl(QObject *parent):QObject(parent)
{

}
string CCameraControl::SetFileName()
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp,sizeof(tmp),"%Y%m%d%H%M%S.mp4",localtime(&timep));
    string s = tmp;
    return s;
}
void CCameraControl::SetParams(int id)
{

    NET_DVR_Init();
    NET_DVR_SetConnectTime(2000,1);
    NET_DVR_SetReconnect(10000,true);
    NET_DVR_SetExceptionCallBack_V30(0,NULL,g_ExceptionCallBack, NULL);
    lport= -1;
    camid = id;
}
LONG CCameraControl::Login(char *ip, int port, char *name, char *pwd)
{

    NET_DVR_USER_LOGIN_INFO logininfo = {0};
    logininfo.bUseAsynLogin = 0;
    strcpy(logininfo.sDeviceAddress, ip);
    logininfo.wPort = port;
    strcpy(logininfo.sUserName, name);
    strcpy(logininfo.sPassword, pwd);

    NET_DVR_DEVICEINFO_V40 DeviceInfo = {0};
    lUserID = NET_DVR_Login_V40(&logininfo,&DeviceInfo);
    if(lUserID < 0)
    {
        NET_DVR_Cleanup();
    }
    imgw = 2048;
    imgh = 1536;
    //int picsize = 2048*1536*2;
    //realBuf = new BYTE[picsize];
    return lUserID;
}
LONG CCameraControl::StartRealPlay(bool b_save, int stream)
{
    NET_DVR_PREVIEWINFO PlayInfo = {0};
    PlayInfo.hPlayWnd = NULL;
    PlayInfo.lChannel = 1;
    PlayInfo.dwStreamType = stream;  //0 main  1 zimaliu
    PlayInfo.dwLinkMode = 0;
    PlayInfo.bBlocked = 1;
    lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &PlayInfo, g_RealDataCallBack, this);
    if(lRealPlayHandle < 0)
    {
        int err = NET_DVR_GetLastError();
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
    }
    string s = SetFileName();
    char tmp[20];
    int id = camid + 1;
    sprintf(tmp,"camera%d/",id);
    string ss = tmp;
    filesavename = "./video/" + ss + s;
    b_savevideo = b_save;
    return lRealPlayHandle;
}
void CCameraControl::StopRealPlay()
{
    NET_DVR_StopRealPlay(lRealPlayHandle);
    PlayM4_Stop(lport);
    PlayM4_CloseStream(lport);
    PlayM4_FreePort(lport);
    NET_DVR_Logout(lUserID);
    NET_DVR_Cleanup();
    lport = -1;
}
BOOL CCameraControl::openM4File(char *filename,int &w, int &h)
{
    BOOL bFlag = FALSE;
    if(lport == -1)
    {
        bFlag = PlayM4_GetPort(&lport);
        if(bFlag == FALSE)
        {
            return FALSE;
        }
    }
    bFlag = PlayM4_OpenFile(lport, filename);
    //bFlag = PlayM4_SetDisplayCallBack(lport,g_DisplayCBFun);
    bFlag = PlayM4_Play(lport, NULL);
    sleep(1);
    PlayM4_GetPictureSize(lport, &w, &h);
    return bFlag;
}
void CCameraControl::CapImage(BYTE *buf, unsigned int dwsize, unsigned int &capsize)
{
    PlayM4_GetJPEG(lport, buf, dwsize, &capsize);
}
void CCameraControl::closeM4()
{
    PlayM4_Stop(lport);
    PlayM4_CloseFile(lport);
    PlayM4_FreePort(lport);
    lport = -1;
    if(fp != 0)
        fclose(fp);
}
