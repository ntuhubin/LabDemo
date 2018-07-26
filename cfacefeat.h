#ifndef CFACEFEAT_H
#define CFACEFEAT_H
#include <string>
#include <iostream>
#include <vector>
#include "Python.h"
//#include <numpy/arrayobject.h>
#include <QDebug>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

class CFaceFeat
{
public:
    CFaceFeat();
    PyObject* load_model();
    int Init();
    int GetFeature(char *imgpath);
    void deInit();
    int train_classifier(char* feature_dir);
    double feat[128];
    double pos[4];
private:
    //PyObject *pArgs;
    PyObject *pConfigReturn;
    PyObject *pModule;
    PyObject *pFunc;
    //PyObject *pReturn;
public:
    PyGILState_STATE gstate;
    void GetGIL();

    void ReleaseGIL();

};

#endif // CFACEFEAT_H
