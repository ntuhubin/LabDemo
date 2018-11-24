#ifndef PUBLICFUN_H
#define PUBLICFUN_H
#include <QtCore>
#include <QtGui>
#include <QList>
#include <QDir>
#include <QFile>
#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Python.h"

#include <QDebug>
using namespace cv;
using namespace std;
class publicFun: public QObject
{
public:
    static QList<QString> GetImagePath()
    {
        QString filepath = "/home/cloud2/matou/4";
        QStringList filters;
        filters<<QString("*.jpeg")<<QString("*.jpg")<<QString("*.png")<<QString("*.tiff")<<QString("*.gif")<<QString("*.bmp");
        QDir d(filepath);
        d.setFilter( QDir::NoSymLinks | QDir::Files);
        d.setNameFilters(filters);
        return d.entryList();
    }
    static QImage MatToQImage(const Mat &mat)
    {
        if(mat.type() == CV_8UC1)
        {
            QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
            // Set the color table (used to translate colour indexes to qRgb values)
            image.setColorCount(256);
            for(int i = 0; i < 256; i++)
            {
                image.setColor(i, qRgb(i, i, i));
            }
            // Copy input Mat
            uchar *pSrc = mat.data;
            for(int row = 0; row < mat.rows; row ++)
            {
                uchar *pDest = image.scanLine(row);
                memcpy(pDest, pSrc, mat.cols);
                pSrc += mat.step;
            }
            return image;
        }
            // 8-bits unsigned, NO. OF CHANNELS = 3
         else if(mat.type() == CV_8UC3)
         {
            // Copy input Mat
           const uchar *pSrc = (const uchar*)mat.data;
           // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            return image.rgbSwapped();
         }
         else if(mat.type() == CV_8UC4)
         {
            //qDebug() << "CV_8UC4";
            // Copy input Mat
            const uchar *pSrc = (const uchar*)mat.data;
           // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
             return image.copy();
         }
         else
         {
             //qDebug() << "ERROR: Mat could not be converted to QImage.";
             return QImage();
         }
    }
    static cv::Mat QImageToMat(QImage image)
    {
        cv::Mat mat;
        /*mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        return mat;*/
        switch (image.format())
        {
            case QImage::Format_ARGB32:
            case QImage::Format_RGB32:
            case QImage::Format_ARGB32_Premultiplied:
                mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
                break;
            case QImage::Format_RGB888:
                mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
                cv::cvtColor(mat, mat, CV_BGR2RGB);
                break;
            case QImage::Format_Indexed8:
                mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
                break;
        }
        cv::Mat m = cv::Mat(image.height(), image.width(), CV_8UC3);
        cv::cvtColor(mat, m, CV_RGBA2RGB);
        //imwrite("test.jpg",m);
        return m;
    }
    static void InitPython()
    {
        Py_Initialize();
        if( !Py_IsInitialized())
        {
            return;
        }
        PyEval_InitThreads();
        int nInit = PyEval_ThreadsInitialized();
        if(nInit)
        {
            PyEval_SaveThread();
        }
    }
    static void SendSMS(char* workerid, char* camid)
    {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('/home/proj/qcloud/')");
        PyObject *pModule = PyImport_ImportModule("sendsms");
        PyObject *pFunc = PyObject_GetAttrString(pModule, "sendSMS");
        PyObject *pArgs;
        pArgs = PyTuple_New(5);

        PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", 1400111693));
        PyTuple_SetItem(pArgs, 1, PyUnicode_FromString("05a22769fd49fd9860b6cf5c196054bc"));
        PyTuple_SetItem(pArgs, 2, Py_BuildValue("i", 0));
        PyTuple_SetItem(pArgs, 3, PyUnicode_FromString(workerid));
        PyTuple_SetItem(pArgs, 4, PyUnicode_FromString(camid));

        PyObject_CallObject(pFunc, pArgs);
        PyGILState_Release(gstate);    //释放当前线程的GIL
    }
    static Mat GetW2CM()
    {
        //读取w2cM

        FileStorage fs("w2cM.xml",FileStorage::READ);
        Mat w2cM;
        fs["w2cM"]>>w2cM;
        fs.release();
        return w2cM;
    }
    static int GetColorValues(Mat image, Mat w2cM)
    {
        if(!image.data)
            return -1;

        //判断是否是彩色图片
        if( image.channels() != 3)
            return -1;

        //cvtColor( image, image,  COLOR_RGB2BGR);
        int rows = image.rows;  //h
        int cols = image.cols;   //w
        int areas = rows*cols;

        if(areas < 80*80)
            return -1;

        cv::Rect rct(cols/3,rows/8,cols*2/3,2*rows/8);
        image = image(rct);
        rows = image.rows;
        cols = image.cols;
        areas = rows * cols;

        //分离通道
        vector<Mat> bgr_planes;
        split(image,bgr_planes);

        //把各通道转为64F
        Mat bplanes,gplanes,rplanes;
        bgr_planes[0].convertTo(bplanes,CV_64FC1);
        bgr_planes[1].convertTo(gplanes,CV_64FC1);
        bgr_planes[2].convertTo(rplanes,CV_64FC1);

        //floor(各通道/8.0)
        Mat fbplanes,fgplanes,frplanes;
        fbplanes = floorMat(bplanes);
        fgplanes = floorMat(gplanes);
        frplanes = floorMat(rplanes);
        Mat index_im = frplanes+32*fgplanes+32*32*fbplanes;//index_im最大值可能为:31+31*32+32*32*31=32767

        Mat index_im_col;
        index_im_col = index_im.reshape(1,areas);

        //读取w2cM

        /*FileStorage fs("w2cM.xml",FileStorage::READ);
        Mat w2cM;
        fs["w2cM"]>>w2cM;*/
        Mat index_col(areas,1,CV_32SC1);
        int tempIndex;
        for(int r = 0;r< areas;r++)
        {
            tempIndex = index_im_col.at<int>(r,0);
            index_col.at<int>(r,0) = w2cM.at<int>(tempIndex,0);
        }
        //reshape
        Mat out2 = index_col.reshape(1,rows);
        /* -------------------11种颜色值-----------------------*/
        Mat color_values(11,1,CV_64FC3);

        //black-黑色 [0 0 0]
        color_values.at<Vec3d>(0,0) = Vec3d(0,0,0);

            //blue-蓝色 [1 0 0]
        color_values.at<Vec3d>(1,0) = Vec3d(1,0,0);

            //brown-棕色(褐色) [0.25 0.4 0.5]
        color_values.at<Vec3d>(2,0) = Vec3d(0.25,0.4,0.5);

            //grey-灰色[0.5 0.5 0.5]
        color_values.at<Vec3d>(3,0) = Vec3d(0.5,0.5,0.5);

            //green-绿色[0 1 0]
        color_values.at<Vec3d>(4,0) = Vec3d(0,1,0);

            //orange-橘色[0 0.8 1]
         color_values.at<Vec3d>(5,0) = Vec3d(0,0.8,1);

            //pink-粉红色[1 0.5 1]
         color_values.at<Vec3d>(6,0) = Vec3d(1,0.5,1);

            //purple-紫色[1 0 1]
         color_values.at<Vec3d>(7,0) = Vec3d(1,0,1);

            //red-红色 [0 0 1]
         color_values.at<Vec3d>(8,0) = Vec3d(0,0,1);

            //white-白色 [1 1 1]
         color_values.at<Vec3d>(9,0) = Vec3d(1,1,1);

            //yellow-黄色[0 1 1]
         color_values.at<Vec3d>(10,0) = Vec3d(0,1,1);

            /*--------------------------------------------------------*/
          //Mat out(rows,cols,CV_64FC3);
          int colors[11]={0};
          for(int r = 0 ;r<rows;r++)
          {
              for(int c =0;c<cols;c++)
              {
                  int tindex = out2.at<int>(r,c);
                  colors[tindex]++;
                  //out.at<Vec3d>(r,c) = color_values.at<Vec3d>(tindex,0)*255;
              }
           }
          //Mat colorMap;
          //out.convertTo(colorMap,CV_8UC3);

          int max=colors[0];
          int idx=0;
          for(int i=0;i<=11;i++)
          {
              if(max<colors[i])
              {
                  max=colors[i];
                  idx=i;
              }
          }
          return idx;
    }
    //注意参数只为 CV_64FC1
    static Mat floorMat(const Mat & doubleMat)
    {
        int rows = doubleMat.rows;
        int cols = doubleMat.cols;
        Mat flo(rows,cols,CV_32SC1);

        for(int r = 0;r < rows;r++)
        {
            for(int c = 0;c < cols ;c++)
            {
                flo.at<int>(r,c)= floor(doubleMat.at<double>(r,c)/8.0);
            }
        }
        return flo;
    }
};
#endif // PUBLICFUN_H
