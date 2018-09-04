#ifndef REGISTERDLG_H
#define REGISTERDLG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDir>
#include <QFile>
#include <QBuffer>
#include <QMessageBox>
#include <QDebug>

#include "g.h"
#include "cfacefeat.h"
#include "playlocalm4.h"

namespace Ui {
class RegisterDlg;
}

class RegisterDlg : public QDialog
{
    Q_OBJECT
private slots:
    void LoadImage();
    void Classify();

    void on_btnEixt_clicked();

public:
    explicit RegisterDlg(QWidget *parent = 0);
    ~RegisterDlg();
private:
    QStandardItemModel  *model;
    CFaceFeat *face;
private:
    Ui::RegisterDlg *ui;
private:
    void InitTable();
    QList<QString> GetALLFiles(QString folder);
    QList<SaveRegPerson> CapList;
};

#endif // REGISTERDLG_H
