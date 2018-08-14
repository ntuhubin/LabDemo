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
};
#endif // PUBLICFUN_H
