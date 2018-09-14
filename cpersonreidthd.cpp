#include "cpersonreidthd.h"

CPersonReIDThd::CPersonReIDThd():QThread()
{
    InitTF();
    stop_flag = false;
    newcome = false;
    saveidx0=0;
    saveidx2=0;
    qRegisterMetaType<QList<ObjdectRls>> ("QList<ObjdectRls>");
    //qRegisterMetaType<ObjdectRls> ("ObjdectRls");
}
void CPersonReIDThd::ReidList(QList<ObjdectRls> rls[3], QList<QString> elst[2])
{
    mutex.lock();
    for(int i = 0; i < 3; i++)
    {
        reidlist[i].clear();
        reidlist[i].append(rls[i]);
    }
    for(int i =0; i < 2; i++)
    {
        ExsitLst[i].clear();
        ExsitLst[i].append(elst[i]);
    }
    newcome = true;
    mutex.unlock();

}
void CPersonReIDThd::ComparePerson(QList<ObjdectRls> &list, QList<ObjdectRls> cplist, int camid, QList<QString> elst)
{
    QList<ObjdectRls> tmp;
    int count = cplist.count();
    for(int i = 0; i< count; i++)
    {
        if(! elst.contains(cplist[i].name))
            tmp.append(cplist.at(i));
    }
    float score = -1000;
    int index = -1;
    for(int i = 0; i < list.count(); i++)
    {
        list[i].name = "";
        Mat img1 = publicFun::QImageToMat(list[i].img);
        for(int j = 0; j < tmp.count(); j++)
        {
            Mat img2 = publicFun::QImageToMat(tmp[j].img);
            float ss = GetSocre(img1, img2);
            if(ss > score)
            {
                score = ss;
                index = j;
            }
        }
        /*if(camid == 1)
        {
            QString savepath = "/home/proj/lab/data/reid0/" + QString::number(saveidx0,10)+".jpg";
            list[i].img.save(savepath);
            saveidx0++;
        }
        else if(camid == 3)
        {
            QString savepath = "/home/proj/lab/data/reid2/" + QString::number(saveidx2,10)+".jpg";
            list[i].img.save(savepath);
            saveidx2++;
        }*/
        /*if(camid == 3)
        {
            QFile f("reid.txt");
            if(f.open(QIODevice::Append | QIODevice::Text))
            {
                QTextStream txtOutput(&f);
                txtOutput << score << endl;
                f.close();
            }
        }*/
        if(score > 0.7)  //match
        {
            list[i].name = tmp[index].name;
            tmp.removeAt(index);
        }
        score = -100;
        index = -1;
    }
}
ObjdectRls CPersonReIDThd::ComparePerson(QList<ObjdectRls> list, ObjdectRls rls)
{
    float score = -1000;
    int index = -1;
    for(int i = 0; i < list.count(); i++)
    {
        Mat img1 = publicFun::QImageToMat(list[i].img);
        Mat img2 = publicFun::QImageToMat(rls.img);
        float ss = GetSocre(img1, img2);
        if(ss > score)
        {
            score = ss;
            index = i;
        }
    }
    QFile f("reid.txt");
    if(f.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream txtOutput(&f);
        txtOutput << score << endl;
        f.close();
    }
    if(score > 0.9)  //match
    {
        list[index].name = rls.name;
        return list[index];
    }
    else
    {
        return list[0];   //bug
    }
}
void CPersonReIDThd::InitTF()
{
    //tensorflow::port::InitMain()
    input_width = 60;
    input_height = 160;
    input_layer = "images:0";
    output_layer = "softmax:0";
    graph = "person-reid.pb";
    root_dir = "";
    tensorflow::GraphDef graph_def;
    if (!ReadBinaryProto(tensorflow::Env::Default(), graph, &graph_def).ok()) {
        //LOG(ERROR) << "Read proto";
        return ;
      }


    tensorflow::SessionOptions sess_opt;
    sess_opt.config.mutable_gpu_options()->set_per_process_gpu_memory_fraction(0.2);
    sess_opt.config.mutable_gpu_options()->set_allow_growth(true);
    (&session)->reset(tensorflow::NewSession(sess_opt));
    if (!session->Create(graph_def).ok()) {
        //LOG(ERROR) << "Create graph";
        return ;
    }
}
float CPersonReIDThd::GetSocre(Mat img1, Mat img2)
{

    img1.convertTo(img1, CV_32FC1);
    resize(img1, img1, cv::Size(input_width,input_height), 0, 0, CV_INTER_CUBIC);
    //cvtColor(img1, img1, COLOR_BGR2RGB);
    img2.convertTo(img2, CV_32FC1);
    resize(img2, img2, cv::Size(input_width,input_height), 0, 0, CV_INTER_CUBIC);
    //cvtColor(img2, img2, COLOR_BGR2RGB);
    tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({2, 1, input_height,input_width, 3}));
    auto input_tensor_mapped = input_tensor.tensor<float, 5>();

    const float * source_data1 = (float*) img1.data;
    const float * source_data2 = (float*) img2.data;

      // copying the data into the corresponding tensor
    for (int y = 0; y < input_height; ++y)
    {
        const float* source_row1 = source_data1 + (y * input_width * 3);
        const float* source_row2 = source_data2 + (y * input_width * 3);
        for (int x = 0; x < input_width; ++x)
        {
            const float* source_pixel1 = source_row1 + (x * 3);
            const float* source_pixel2 = source_row2 + (x * 3);
            for (int c = 0; c < 3; ++c)
            {
                const float* source_value1 = source_pixel1 + c;
                const float* source_value2 = source_pixel2 + c;
                input_tensor_mapped(0, 0, y, x, c) = *source_value1;
                input_tensor_mapped(1, 0, y, x, c) = *source_value2;
          }
        }
      }
      std::vector<Tensor> outputs;
      if (!session->Run({{input_layer, input_tensor}},{output_layer}, {}, &outputs).ok())
      {
          //LOG(ERROR) << "Running model failed";
          return -1;
      }

      tensorflow::Tensor output = std::move(outputs.at(0));
      auto scores = output.flat<float>();
      //return (scores(0) - scores(1));
      return scores(0);
        //cout << "prob of same person:" << scores(0) << "  prob of different person:" << scores(1) << endl;
}
void CPersonReIDThd::run()
{
    while(true)
    {
        if(newcome == false)
        {
            msleep(100);
            continue;
        }
        mutex.lock();
        QList<ObjdectRls> rlist[3];
        for(int i = 0; i < 3; i++)
        {
            rlist[i].append(reidlist[i]);
        }
        QList<QString> elst[2];
        for(int i = 0; i < 2; i++)
        {
            elst[i].append(ExsitLst[i]);
        }
        newcome = false;
        mutex.unlock();
        //ObjdectRls rls1 = ComparePerson(rlist[0], rlist[1][0]);
        //ObjdectRls rls2 = ComparePerson(rlist[2], rlist[1][0]);
        ComparePerson(rlist[0], rlist[1], 1, elst[0]);
        ComparePerson(rlist[2], rlist[1], 3, elst[1]);
        emit SendReid(rlist[0], rlist[2]);

        msleep(100);
        if(stop_flag == true)
        {
            break;
        }
    }
}
void CPersonReIDThd::StopRun()
{
    stop_flag = true;
}
