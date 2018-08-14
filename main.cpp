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

PyObject*  loadmodel() {

  PyObject *pModel = NULL;
  PyObject *pConfig = NULL;
  pModel = PyImport_ImportModule("model");
  if (!pModel) {
          //printf("Cant open python file!\n");
          return NULL;
  }
  /* model.feature_extraction */
  pConfig = PyObject_GetAttrString(pModel, "config");

  /* return model object */
  return PyObject_CallObject(pConfig, NULL);
}

void facetest()
{
    /*Py_Initialize();
    if(!Py_IsInitialized())
    {
        qDebug()<<"11111111111111111111";
        return;
    }*/
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");

    PyObject *model = loadmodel();
    if (!model)
    {
        qDebug()<<"43333333";
        return;
    }
    PyObject *pArgs;
    PyObject *pModule = PyImport_ImportModule("feature_extractor");
    if (!pModule) {
        qDebug()<<"555555";
    }
    PyObject *pFunc = PyObject_GetAttrString(pModule, "feature_extraction");
    pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString("010.jpg"));
    PyTuple_SetItem(pArgs, 1, model);

    PyObject_CallObject(pFunc, pArgs);
    PyGILState_Release(gstate);    //释放当前线程的GIL
    //Py_Finalize();
}
void Facecls()
{
    CFaceClassification *face = new CFaceClassification();
    face->GetGIL();
    face->Init();
    face->classfier("/tmp/5.jpg");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    publicFun::InitPython();
    Widget w;
    w.showMaximized();


    //Facecls();
    //facetest();
    //publicFun::SendSMS();

    return a.exec();
}
