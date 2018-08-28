#ifndef QUERYDLG_H
#define QUERYDLG_H

#include <QDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include "cdbpro.h"

namespace Ui {
class QueryDlg;
}

class QueryDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QueryDlg(QWidget *parent = 0);
    ~QueryDlg();

private slots:
    void on_btn_query_clicked();

    void on_tv_EventInfo_clicked(const QModelIndex &index);

    void on_btnSwitch_clicked();

    void on_tv_OPInfo_clicked(const QModelIndex &index);

private:
    CDbPro sqlpro;
    int searchcount;
    int currentindex;
private:
    void showEventInfos();
    void showOpRecords();

private:
    Ui::QueryDlg *ui;


};

#endif // QUERYDLG_H
