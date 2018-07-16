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
    PyRun_SimpleString("sys.path.append('/home/proj/qcloud/')");
    PyObject *pModule = PyImport_ImportModule("sendsms");
    PyObject *pFunc = PyObject_GetAttrString(pModule, "sendSMS");
    PyObject *pArgs;
    pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", 1400111693));
    PyTuple_SetItem(pArgs, 1, PyUnicode_FromString("05a22769fd49fd9860b6cf5c196054bc"));
    PyTuple_SetItem(pArgs, 2, Py_BuildValue("i", 0));
    PyEval_CallObject(pFunc, pArgs);

    Py_Finalize();
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //PythonTest();
    Widget w;
    w.showMaximized();

    publicFun::InitPython();
    //publicFun::SendSMS();

    return a.exec();
}
