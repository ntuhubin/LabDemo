#ifndef CFACECLASSIFICATION_H
#define CFACECLASSIFICATION_H
#include <string>
#include <iostream>
#include <vector>
#include <Python.h>
#include <numpy/arrayobject.h>
#include "g.h"
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

class CFaceClassification
{
public:
    CFaceClassification();
public:
    PyObject *pConfigReturn;
    PyObject *pModule;
    PyObject *pFunc;
    int Init();
    void deInit();
    void classfier(char *imgpath);
    void classifier(QList<float> feat);
    QList<ClsInfo> ClsList;
    PyObject* load_model();
public:
    PyGILState_STATE gstate;
    void GetGIL();

    void ReleaseGIL();
};

#endif // CFACECLASSIFICATION_H
