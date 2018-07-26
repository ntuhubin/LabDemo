#include "cfacefeat.h"

CFaceFeat::CFaceFeat()
{

}
PyObject* CFaceFeat::load_model() {

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
void CFaceFeat::GetGIL()
{
    //int nHold = PyGILState_Check();   //检测当前线程是否拥有GIL
    //PyGILState_STATE gstate;
    //if ( !nHold )
    {
        gstate = PyGILState_Ensure();   //如果没有GIL，则申请获取GIL
    }
    //Py_BEGIN_ALLOW_THREADS;
    //Py_BLOCK_THREADS;
}
void CFaceFeat::ReleaseGIL()
{
   // Py_UNBLOCK_THREADS;
    //Py_END_ALLOW_THREADS;
    //if (!nHold)
    {
        PyGILState_Release(gstate);    //释放当前线程的GIL
    }
}
int CFaceFeat::Init()
{
    /*Py_Initialize();
    if (!Py_IsInitialized())
    {
            //printf("init error\n");
       return 1;
    }*/
    /* import */
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");
    /* model loading */
    pConfigReturn = load_model();
    if (!pConfigReturn) {
            //printf("Cant open python file!\n");
         return 2;
    }
    pModule = PyImport_ImportModule("feature_extractor");
    if (!pModule) {
              //printf("Cant open python file!\n");
        return 3;
    }
    pFunc = PyObject_GetAttrString(pModule, "feature_extraction");
    return 0;
}
int CFaceFeat::GetFeature(char *imgpath)
{
    qDebug(imgpath);
    PyObject *pArgs = NULL;
    PyObject *pReturn = NULL;
    pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(imgpath));
    PyTuple_SetItem(pArgs, 1, pConfigReturn);

    pReturn = PyObject_CallObject(pFunc,pArgs);

    if(pReturn == 0x00)
    {
        qDebug(" no face detectd ");
        return 1;
    }

    if(PyList_Check(pReturn))
    { //check list
       int SizeOfList = PyList_Size(pReturn);
       if(SizeOfList < 1)
       {
           return 3;
       }
       for(int j = 0; j < 2; j++)
       {
          PyObject *ListItem = PyList_GetItem(pReturn, j);
          if(j == 0)
          {
             int NumOfItems = PyList_Size(ListItem);
             if(NumOfItems != 128)
             {
                 return 4;
             }
             for(int m = 0; m < 128; m++)
             {
                 PyObject *item = PyList_GetItem(ListItem, m);
                 //info.features.append(PyFloat_AsDouble(item3));
                 feat[m] = PyFloat_AsDouble(item);
              }
          }
          else if(j == 1)
          {
              for(int k = 0; k < 4; k++)
              {
                  PyObject *item = PyList_GetItem(ListItem, k);
                  //info.pos[k] = PyFloat_AsDouble(item4);
                  pos[k] = PyFloat_AsDouble(item);
               }
           }
         }
       }

       else
       {
           return 2;
       }

    if(pos[0] < 0 || pos[1] < 0 || pos[2] < 0 || pos[3] <0)
        return 4;

    return 0;
}
int CFaceFeat::train_classifier(char *feature_dir)
{
    PyObject *pModel = NULL;
    PyObject *pConfig = NULL;
    PyObject *pArgs = NULL;

    pModel = PyImport_ImportModule("classification");
    if (!pModel) {
            //printf("Cant open python file!\n");
        return -2;
    }

    /* classification.train */
    pConfig = PyObject_GetAttrString(pModel, "train");

    /* build args */
    pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(feature_dir));
    PyTuple_SetItem(pArgs, 1, PyUnicode_FromString("model"));

    /* print classifier path */
    PyObject_CallObject(pConfig, pArgs);

    return 0;
}
void CFaceFeat::deInit()
{
    /*Py_DECREF(pFunc);
    Py_DECREF(pModule);
    Py_DECREF(pFunc);
    Py_DECREF(pConfigReturn);*/
    Py_Finalize();
}
