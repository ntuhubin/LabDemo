#ifndef CPERSONREIDTHD_H
#define CPERSONREIDTHD_H
#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/image_ops.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/init_main.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/time.h>
#include <vector>
#include <string>
#include <QThread>
#include <QMetaType>
#include <QList>
#include <QMutex>
#include "g.h"
#include "publicfun.h"
using tensorflow::Tensor;
using tensorflow::Status;
using namespace std;
using namespace cv;


class CPersonReIDThd:public QThread
{
    Q_OBJECT
public slots:
    void ReidList(QList<ObjdectRls> rls[3]);
signals:
    void SendReid(QList<ObjdectRls> list1, QList<ObjdectRls> list2);
    void sendreid(ObjdectRls rls1, ObjdectRls rls2);
private:
    int input_width;
    int input_height;
    string input_layer;
    string output_layer;
    string graph;
    string root_dir;
    std::unique_ptr<tensorflow::Session> session;
    QList<ObjdectRls> reidlist[3];
    volatile bool newcome;
    volatile bool stop_flag;
    QMutex mutex;
public:
    CPersonReIDThd();
    void InitTF();
    float GetSocre(Mat img1, Mat img2);
    void ComparePerson(QList<ObjdectRls> &list, QList<ObjdectRls> cplist, int camid);
    ObjdectRls ComparePerson(QList<ObjdectRls> list, ObjdectRls rls);
    void StopRun();
private:
    void run();
};

#endif // CPERSONREIDTHD_H
