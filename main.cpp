#include "widget.h"
#include <QApplication>
#include "publicfun.h"

void PythonTest()
{
    Py_Initialize();
    if(!Py_IsInitialized())
    {
        qDebug()<<"11111111111111111111";
        return;
    }
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");
    PyObject *pModule = PyImport_ImportModule("model");
    if(!pModule)
    {
        qDebug()<<"2222222222222";
        return;
    }
    PyObject * pFuncHello = PyObject_GetAttrString(pModule, "config");
    if(!pFuncHello)
    {
        qDebug()<<"33333333333";
        return;
    }
    PyObject_CallObject(pFuncHello, NULL);
    Py_Finalize();
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //PythonTest();
    Widget w;
    w.showMaximized();

    publicFun::InitPython();

    return a.exec();
}
