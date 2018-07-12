#include "cfaceclassification.h"

CFaceClassification::CFaceClassification()
{

}
PyObject* CFaceClassification::load_model() {

  PyObject *pModel = NULL;
  PyObject *pConfig = NULL;
  pModel = PyImport_ImportModule("model");  //model
  if (!pModel) {
          //printf("Cant open python file!\n");
          return NULL;
  }
  /* model.feature_extraction */
  pConfig = PyObject_GetAttrString(pModel, "config");  //config

  /* return model object */
  return PyObject_CallObject(pConfig, NULL);
}
void CFaceClassification::GetGIL()
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
void CFaceClassification::ReleaseGIL()
{
   // Py_UNBLOCK_THREADS;
    //Py_END_ALLOW_THREADS;
    //if (!nHold)
    {
        PyGILState_Release(gstate);    //释放当前线程的GIL
    }
}
int CFaceClassification::Init()
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
    /* classification.infer */
    pModule = PyImport_ImportModule("classification");
    pFunc = PyObject_GetAttrString(pModule, "infer");
}
void CFaceClassification::deInit()
{
      Py_Finalize();
}
void CFaceClassification::classifier(QList<float> feat)
{

}
void CFaceClassification::classfier(char *imgpath)
{
    //Init();
    ClsList.clear();
    char* classifier_path = "./model/classifier.pkl";
    /* build args */
    PyObject *pArgs;
    pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, PyString_FromString(classifier_path));
    PyTuple_SetItem(pArgs, 1, PyString_FromString(imgpath));
    PyTuple_SetItem(pArgs, 2, pConfigReturn);

    PyObject *pReturn = PyObject_CallObject(pFunc, pArgs);
    int SizeOfList = PyList_Size(pReturn);
    int personnum = SizeOfList / 4;
    if(personnum < 1)
        return;
    ClsInfo info;
    PyObject *ListItem = NULL;
    for(int i = 0; i < personnum; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            ListItem = PyList_GetItem(pReturn, i*4+j);
            if(j == 0)
            {
                PyObject *item1 = PyList_GetItem(ListItem, 0);
                info.label = PyString_AsString(item1);

            }
            else if(j == 1)
            {
                PyObject *item2 = PyList_GetItem(ListItem, 0);
                info.belief = PyFloat_AsDouble(item2);

            }
            else if(j == 2)
            {
                for(int m = 0; m < 128; m++)
                {
                    PyObject *item3 = PyList_GetItem(ListItem, m);
                    info.features.append(PyFloat_AsDouble(item3));
                }
            }
            else if(j == 3)
            {
                for(int k = 0; k < 4; k++)
                {
                    PyObject *item4 = PyList_GetItem(ListItem, k);
                    info.pos[k] = PyFloat_AsDouble(item4);
                }

            }

        }
        ClsList.append(info);
    }
    Py_DECREF(ListItem);
}
