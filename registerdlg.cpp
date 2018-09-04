#include "registerdlg.h"
#include "ui_registerdlg.h"

RegisterDlg::RegisterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDlg)
{
    ui->setupUi(this);
    InitTable();
    connect(ui->btnLoadReg, &QPushButton::clicked, this,&RegisterDlg::LoadImage);
    connect(ui->btnReg, &QPushButton::clicked, this,&RegisterDlg::Classify);
    face = new CFaceFeat();
}

RegisterDlg::~RegisterDlg()
{
    delete ui;
}
void RegisterDlg::InitTable()
{
    model = new QStandardItemModel();
    model->setColumnCount(2);
    model->setHeaderData(0,Qt::Horizontal,"姓名");
    model->setHeaderData(1,Qt::Horizontal,"照片数量");
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    //设置列宽不可变
    //ui->tableView->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);
    //ui->tableView->horizontalHeader()->setResizeMode(1,QHeaderView::Fixed);
    ui->tableView->setColumnWidth(0,350);
    ui->tableView->setColumnWidth(1,350);
}
QList<QString> RegisterDlg::GetALLFiles(QString folder)
{
    QString filepath = "./register/LabIDImg/"+folder;
    QStringList filters;
    filters<<QString("*.jpeg")<<QString("*.jpg")<<QString("*.png")<<QString("*.tiff")<<QString("*.gif")<<QString("*.bmp");
    QDir d(filepath);
    d.setFilter( QDir::NoSymLinks | QDir::Files);
    d.setNameFilters(filters);
    return d.entryList(); //返回这个目录中文件的名称的列表
}
void RegisterDlg::LoadImage()
{
    ui->btnLoadReg->setEnabled(false);

    face->GetGIL();
    int ret = face->Init();
    if(ret != 0)
    {
        QMessageBox::about(NULL, "about", "face init failed");
        return;
    }
    QDir d("./register/LabIDImg");
    d.setFilter( QDir::NoSymLinks | QDir::AllDirs);
    const QList<QString> list1 = d.entryList(); //返回这个目录中所有目录和文件的名称的列表
    int personcount = list1.count() - 2;
    if(personcount <= 0)
    {
        QMessageBox::about(NULL, "about", "注册目录下不存在注册照片");
        ui->btnLoadReg->setEnabled(true);
        face->ReleaseGIL();
        return;
    }
    CapList.clear();
    QString str = "注册人数一共:" + QString::number(personcount,10) + ",计算特征时间较长，请耐心等待！";
    QMessageBox::about(NULL, "about", str);
    QList<QString> writelabel;
    QList<QString> writefeat;
    QList<QString>::const_iterator it = list1.begin();
    SaveRegPerson srg;
    int personlabel = 1;
    while(it != list1.end())
    {
        if((*it) != "." && (*it) != "..")
        {
            QList<QString> filenames = GetALLFiles(*it);
            int filenum = filenames.count();
            if(filenum == 0)
            {
                continue;
            }
            int realpersonnum = 0;
            for(int i = 0; i < filenum; i++)
            {
                QString imgpath = "./register/LabIDImg/" + *it + "/"+ filenames.at(i);
                ret = face->GetFeature(imgpath.toLatin1().data());
                if (ret == 0)
                {
                    //QString strlabel = QString::number(personlabel) + ",register/" + *it + "/"+ filenames.at(i) + +"\n";
                    QString strlabel = "1,LabIDImg/" + *it + "/"+ filenames.at(i) + +"\n";
                    writelabel.append(strlabel);
                    QString featstr;
                    for(int k = 0; k < 127; k++)
                    {
                        double dd = face->feat[k];
                        featstr += QString("%1").arg(dd) + ",";
                    }
                    double d = face->feat[127];
                    featstr += QString("%1").arg(d) + +"\n";
                    writefeat.append(featstr);
                    realpersonnum++;
                    if (realpersonnum == 1)
                    {
                        QImage src = QImage(imgpath);
                        srg.img = src.copy(( face->pos[0] - 20) < 0 ? 0 : ( face->pos[0] - 20),
                                ( face->pos[1] - 20) < 0 ? 0 : ( face->pos[1] - 20),
                                ( face->pos[2] -  face->pos[0] + 20 ) > src.width()? src.width() : ( face->pos[2] -  face->pos[0] + 20 ) ,
                                ( face->pos[3] -  face->pos[1] + 20 ) > src.height()? src.height() : ( face->pos[3] -  face->pos[1] + 20 ));
                        srg.label = personlabel;
                        srg.name = *it;
                    }

                }
            }
            personlabel++;
            if(realpersonnum != 0)
            {
                srg.realcount = realpersonnum;
                CapList.append(srg);
            }
        }
        it++;
    }
    face->ReleaseGIL();
    QFile file;
    if(file.exists("./csv/labels.csv"))
    {
        file.remove("./csv/labels.csv");
        //file.copy("./unkonw/labels.csv","./csv/labels.csv");
    }
    if(file.exists("./csv/reps.csv"))
    {
        file.remove("./csv/reps.csv");
        //file.copy("./unkonw/reps.csv","./csv/reps.csv");
    }
    QFile csvFile("./csv/labels.csv");
    QFile featFile("./csv/reps.csv");
    csvFile.open(QIODevice::Append);
    featFile.open(QIODevice::Append);
    for(int i = 0; i < writelabel.count(); i++)
    {
        csvFile.write(writelabel[i].toLatin1());
        featFile.write(writefeat[i].toLatin1());
    }
    csvFile.close();
    featFile.close();
    QMessageBox::about(NULL, "about", "特征提取完成，接下来请点击注册！");
    for(int i = 0; i < CapList.count();i++)
    {
        model->setItem(i,0,new QStandardItem(CapList[i].name));
        model->setItem(i,1,new QStandardItem(QString::number(CapList[i].realcount,10)));
    }
    ui->btnLoadReg->setEnabled(true);
}
void RegisterDlg::Classify()
{
    int num = CapList.count();
    QFile fileWrite("./model/r.bin");
    fileWrite.open(QIODevice::WriteOnly);
    QDataStream  writeDataStream(&fileWrite);
    writeDataStream<<num;
    for(int i = 0; i < num; i++)
    {
        writeDataStream<<CapList[i].name<<CapList[i].img<<CapList[i].label;
    }
    fileWrite.close();
    face->GetGIL();
    face->Init();
    face->train_classifier("./csv");
    face->ReleaseGIL();
    QMessageBox::about(NULL, "about", "注册完成");

}

void RegisterDlg::on_btnEixt_clicked()
{
    this->close();
}
