#ifndef CDBPRO_H
#define CDBPRO_H

#include <mysql/mysql.h>
#include <stdio.h>
#include <QDebug>
#include "g.h"
using namespace std;


typedef struct
{
    std::string staffid;
    int eventid;   //0 nomal enter; 1 no hat
    std::string eventtime;
    std::string camid;    // 1-3
    QImage dectimg;
}EventQuery, *pEventQuery;

class CDbPro
{
public:
    CDbPro();
    bool ConnectDB();
    void CloseDB();
    void InsertEvent(EventInfo info);
    void GetEntryPerson(string begintime, string endtime);
public:
    QList<EventQuery> eq_list;
private:
    MYSQL mysql;
    MYSQL_RES *result;
    MYSQL_ROW sql_row;
};

#endif // CDBPRO_H
