#include "cdbpro.h"

CDbPro::CDbPro()
{
    mysql_init(&mysql);
}
bool CDbPro::ConnectDB()
{
    if (mysql_real_connect(&mysql, "localhost", "root", "pwd123", "spDB", 3306, NULL, 0))
    {
        return true;
    }
    return false;
}
void CDbPro::CloseDB()
{
    mysql_close(&mysql);
}
void CDbPro::InsertEvent(EventInfo info)
{
    MYSQL_STMT *stmt = mysql_stmt_init(&mysql);
    char *query = "insert into event_table(staffID,eventID,eventTime,CamID,Img) values(?,?,?,?,?)";
    if(mysql_stmt_prepare(stmt, query, strlen(query)))
    {
        qDebug() << mysql_stmt_error(stmt);
        return;
    }
    MYSQL_BIND params[5];
    memset(params, 0, sizeof(params));
    int idx = 0;

    unsigned long namelen = strlen(info.staffid.c_str());
    char *c = new char[namelen];
    strcpy(c,info.staffid.c_str());

    params[idx].buffer_type = MYSQL_TYPE_STRING;
    params[idx].buffer = c;
    params[idx].buffer_length = 0;
    params[idx].is_null= 0;
    params[idx].length= &namelen;
    idx++;

    params[idx].buffer_type = MYSQL_TYPE_LONG;
    params[idx].buffer = (char *)&(info.eventid);
    params[idx].is_null= 0;
    params[idx].length= 0;
    idx++;

    MYSQL_TIME  ts;
    ts.year= info.eventtime.date().year();
    ts.month= info.eventtime.date().month();
    ts.day= info.eventtime.date().day();
    ts.hour= info.eventtime.time().hour();
    ts.minute= info.eventtime.time().minute();
    ts.second= info.eventtime.time().second();

    ts.neg = 0;
    ts.second_part = 0;
    params[idx].buffer_type= MYSQL_TYPE_DATETIME;
    params[idx].buffer= (char *)&ts;
    params[idx].is_null= 0;
    params[idx].length= 0;
    idx++;

    params[idx].buffer_type = MYSQL_TYPE_LONG;
    params[idx].buffer = (char *)&(info.camid);
    params[idx].is_null= 0;
    params[idx].length= 0;
    idx++;

    info.dectimg.save("/tmp/ttt.jpg");
    unsigned long pl = info.dectimg.byteCount() + 1024;
    char *p = new char[pl];
    FILE *fp = NULL;
    fp = fopen("/tmp/ttt.jpg", "r");
    unsigned long count = fread(p, 1, pl, fp);
    fclose(fp);

    params[idx].buffer_type= MYSQL_TYPE_LONG_BLOB;
    params[idx].buffer= p;
    params[idx].is_null= 0;
    params[idx].length= &count;



    mysql_stmt_bind_param(stmt, params);
    mysql_stmt_send_long_data(stmt,4,p,pl);
    int ret = mysql_stmt_execute(stmt);
    if(ret != 0)
    {
        delete c;
        delete p;
        qDebug() << mysql_stmt_error(stmt);
        return;
    }
    mysql_stmt_close(stmt);

    delete c;
    delete p;

}
void CDbPro::GetEntryPerson(string begintime, string endtime)
{
    eq_list.clear();
    std::string str_sqls; // = "select personname,capDate,cappic,belief from persontrace";
    char sqlchar[200];
    sprintf(sqlchar,"select staffID,eventID,eventTime,CamID,Img from event_table where eventTime between '%s' and '%s'",begintime.c_str(),endtime.c_str());
    str_sqls = sqlchar;
    int res = 0;
    const char *p = str_sqls.c_str();
    res = mysql_real_query(&mysql, p, str_sqls.size());
    if (res != 0)
    {
        return;
    }
    //
   MYSQL_RES *ms_res = mysql_store_result(&mysql);
   unsigned long *lengths;
   if (ms_res != NULL)
   {
       unsigned int field_num = mysql_num_fields(ms_res);
       //
       MYSQL_FIELD* field_info = mysql_fetch_field(ms_res);
       //assert(field_info != NULL);
       MYSQL_ROW row_data = NULL;
       while (1) {
           row_data = mysql_fetch_row(ms_res);    //
           if (row_data == NULL)
               break;
           EventQuery info;
           lengths = mysql_fetch_lengths(ms_res);
           info.staffid = row_data[0];
           info.eventid =  std::stoi(row_data[1]);
           info.eventtime = row_data[2];
           info.camid = row_data[3];
           unsigned int imglen = lengths[4];
           unsigned char *img = new unsigned char[imglen];
           //qDebug << row_data[0];
           //qDebug << row_data[1];
           memcpy(img, row_data[4],imglen);
           QImage qimg;
           qimg.loadFromData(img, imglen);
           info.dectimg = qimg;

           //result.capimg.save("./a.jpeg");
           eq_list.append(info);
           /*FILE *fp = NULL;
           fp = fopen("./res.jpeg","wb");
           fwrite(img,imglen,1,fp);
           fclose(fp);*/
           delete img;
           //qDebug << row_data[3];
           /*for (int i = 0; i < field_num; ++i)
           {
               if (row_data[i] == NULL)
               {
                    //qDebug<<field_info[i];
               }
           }*/
       }
   }
   mysql_free_result(ms_res);
   ms_res = NULL;
}
