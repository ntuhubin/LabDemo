#include "querydlg.h"
#include "ui_querydlg.h"

QueryDlg::QueryDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryDlg)
{
    ui->setupUi(this);

    ui->dte_Begin->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    ui->dte_End->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    ui->dte_Begin->setDateTime(QDateTime::currentDateTime());
    ui->dte_End->setDateTime(QDateTime::currentDateTime());
    ui->tv_EventInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tv_EventInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tv_OPInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tv_OPInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    searchcount = 0;
    currentindex = 0;
    ui->stackedWidget->setCurrentIndex(0);
}

QueryDlg::~QueryDlg()
{
    delete ui;
}
void QueryDlg::showEventInfos()
{
    QStandardItemModel  *model;
    model = new QStandardItemModel();
    model->setColumnCount(4);
    model->setHeaderData(0,Qt::Horizontal,"ID");
    model->setHeaderData(1,Qt::Horizontal,"Event");
    model->setHeaderData(2,Qt::Horizontal,"date");
    model->setHeaderData(3,Qt::Horizontal,"CAMID");
    ui->tv_EventInfo->setModel(model);
    ui->tv_EventInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    for(int i = 0; i < sqlpro.eq_list.count(); i++)
    {
        QString id = QString::fromStdString(sqlpro.eq_list[i].staffid);
        model->setItem(i,0,new QStandardItem(id));
        if(sqlpro.eq_list[i].eventid == 0)
            model->setItem(i,1,new QStandardItem("正常进入"));
        else {
            model->setItem(i,1,new QStandardItem("未戴安全帽"));
        }
        QString dt = QString::fromStdString(sqlpro.eq_list[i].eventtime);
        model->setItem(i, 2, new QStandardItem(dt));
        QString cid = QString::fromStdString(sqlpro.eq_list[i].camid);
        model->setItem(i,3,new QStandardItem(cid));
        ui->tv_EventInfo->setColumnWidth(0,140);
        ui->tv_EventInfo->setColumnWidth(1,140);
        ui->tv_EventInfo->setColumnWidth(2,140);
        ui->tv_EventInfo->setColumnWidth(3,140);
    }
}
void QueryDlg::showOpRecords()
{
    QStandardItemModel  *model;
    model = new QStandardItemModel();
    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal,"ID");
    model->setHeaderData(1,Qt::Horizontal,"starttime");
    model->setHeaderData(2,Qt::Horizontal,"leavetime");
    ui->tv_OPInfo->setModel(model);
    ui->tv_OPInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    for(int i = 0; i < sqlpro.op_lst.count(); i++)
    {
        QString id = QString::fromStdString(sqlpro.op_lst[i].staffid);
        model->setItem(i,0,new QStandardItem(id));

        QString dt = QString::fromStdString(sqlpro.op_lst[i].starttime);
        model->setItem(i, 1, new QStandardItem(dt));
        QString cid = QString::fromStdString(sqlpro.op_lst[i].endtime);
        model->setItem(i,2,new QStandardItem(cid));
        ui->tv_OPInfo->setColumnWidth(0,180);
        ui->tv_OPInfo->setColumnWidth(1,180);
        ui->tv_OPInfo->setColumnWidth(2,180);

    }
}
void QueryDlg::on_btn_query_clicked()
{
    QDateTime begin = ui->dte_Begin->dateTime();
    QDateTime end = ui->dte_End->dateTime();
    qint64 secs = begin.secsTo(end);
    if(secs < 0)
    {
        QMessageBox::about(NULL, "warning", "起始时间大于结束时间");
        return;
    }
    bool ret = sqlpro.ConnectDB();
    if(ret == false)
    {
        QMessageBox::about(NULL, "warning", "DB ERROR");
        return;
    }
    if(currentindex == 0)
    {
        sqlpro.GetEntryPerson(begin.toString("yyyy-MM-dd hh:mm:ss").toStdString(),
                              end.toString("yyyy-MM-dd hh:mm:ss").toStdString());

        showEventInfos();
        searchcount = sqlpro.eq_list.count();
    }
    else {
        sqlpro.GetOpRecord(begin.toString("yyyy-MM-dd hh:mm:ss").toStdString(),
                              end.toString("yyyy-MM-dd hh:mm:ss").toStdString());

        showOpRecords();
        searchcount = sqlpro.op_lst.count();
    }
    sqlpro.CloseDB();
}

void QueryDlg::on_tv_EventInfo_clicked(const QModelIndex &index)
{
    int row = ui->tv_EventInfo->currentIndex().row();
    if(row < 0)
       return;
    if(row >= searchcount)
       return;
    if(searchcount != 0)
    {


         QPixmap mp = QPixmap::fromImage(sqlpro.eq_list[row].dectimg);
         ui->label_pic->setScaledContents(true);
         ui->label_pic->setPixmap(mp);

    }
}

void QueryDlg::on_btnSwitch_clicked()
{
    currentindex = 1 - currentindex;
    ui->stackedWidget->setCurrentIndex(currentindex);
}

void QueryDlg::on_tv_OPInfo_clicked(const QModelIndex &index)
{
    int row = ui->tv_OPInfo->currentIndex().row();
    if(row < 0)
       return;
    if(row >= searchcount)
       return;
    if(searchcount != 0)
    {

        QPixmap mp = QPixmap::fromImage(sqlpro.op_lst[row].dectimg);
        ui->label_oppic->setScaledContents(true);
        ui->label_oppic->setPixmap(mp);


    }
}
