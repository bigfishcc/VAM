#include "dbvam.h"
#include <QtSql/QSqlError>
#include <QDebug>
#include <QCoreApplication>
#include <QSettings>

DBVam::DBVam(QObject *parent) : QObject(parent)
{
    QString settFile=QCoreApplication::applicationDirPath()+"/set.ini";
    QSettings sett(settFile,QSettings::IniFormat);
    QString sWorkPath=sett.value("Settings/WorkPath").toString();
    m_sDBName=sWorkPath+"/vamdata.db";
//    qDebug()<<m_sDBName;
    clear();
}

bool DBVam::openDB()
{
    m_pDatabase=new QSqlDatabase;
    if(QSqlDatabase::contains("vam_database"))
      (*m_pDatabase) = QSqlDatabase::database("vam_database");
    else
      (*m_pDatabase) = QSqlDatabase::addDatabase("QSQLITE","vam_database");
    m_pDatabase->setDatabaseName(m_sDBName);
    if(!m_pDatabase->open())
    {
        qDebug()<<"database open error: "<<m_pDatabase->lastError();
        return false;
    }
//    qDebug()<<"database open OK!";
    m_pQuery=new QSqlQuery(*m_pDatabase);
    return true;
}

void DBVam::closeDB()
{
    m_pQuery->clear();
    m_pQuery->finish();
    delete m_pQuery;
    m_pDatabase->close();
    delete m_pDatabase;
    QSqlDatabase::removeDatabase("vam_database");
}

bool DBVam::exec(QString sql)
{
    QSqlQuery qry(*m_pDatabase);
    if(!qry.exec(sql))
    {
        qDebug()<<sql;
        qDebug()<<"exec error: "<<qry.lastError();
        return false;
    }
    return true;
}

bool DBVam::createTable()
{
    QString sql;
    sql="create table IF NOT EXISTS vam_game(id integer primary key autoincrement, gamepath text, score int, descript text, gamesize long, timecreate datetime, checked int)";
    if(!exec(sql))
        return false;
    sql="create table IF NOT EXISTS vam_item(id integer primary key autoincrement, vamgameid int, name text, vamtype int, gender int, invar int, filesize long,"
            "filepath text, prevfilepath text, prevfileloc int, orgfilepath text, author text, vamflag text, descript text, score int, birthTime datetime, recordTime datetime, checked int)";
    if(!exec(sql))
        return false;
    return true;
}

bool DBVam::query(QString sql)
{
    m_pQuery->clear();
    if(!m_pQuery->exec(sql))
    {
        qDebug()<<m_pQuery->lastError();
        return false;
    }
    return true;
}

bool DBVam::query_movenext()
{
    return m_pQuery->next();
}

QVariant DBVam::query_value(int nField)
{
    return m_pQuery->value(nField);
}

void DBVam::queryTest()
{
    QSqlQuery sql_query(*m_pDatabase);
    if(!sql_query.exec("select * from vam_game"))
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        int i=0;
        int c;
        while(sql_query.next())
        {
            for(c=0;c<7;c++)
                qDebug()<<i<<":"<<c<<"="<<sql_query.value(c).toString();
            i++;
        }
    }
}

void DBVam::clear()
{
    m_FieldValue.clear();
    m_Where.clear();
    m_sSel="";
    m_Limit="";
}

void DBVam::setTable(QString tablename)
{
    m_scurTable=tablename;
}

void DBVam::setValue(QString field, QString value)
{
    int i;
    int n=m_FieldValue.count();
    for(i=0;i<n;i++)
    {
        if(m_FieldValue.at(i).field==field)
        {
            m_FieldValue[i].value=value;
            return;
        }
    }
    FIELD_VALUE fv;
    fv.field=field;
    fv.value=value;
    m_FieldValue.push_back(fv);
}

void DBVam::addWhere(QString whereStr, bool bAnd)
{
    WHERE_STR ws;
    ws.whereStr=whereStr;
    ws.bAnd=bAnd;
    m_Where.push_back(ws);
}

bool DBVam::getSQLString(QString& sql, int st)
{
    int i;
    if(st==GET_INSERT)
    {   //is insert
        int n=m_FieldValue.count();
        if(n<=0)return false;
        sql="insert into "+m_scurTable+" (";
        for(i=0;i<n;i++)
        {
            sql+=m_FieldValue.at(i).field;
            if(i<n-1)sql+=",";
        }
        sql+=") values (";
        for(i=0;i<n;i++)
        {
            sql+=m_FieldValue.at(i).value;
            if(i<n-1)sql+=",";
        }
        sql+=")";
    }
    else if(st==GET_SET)
    {  //is update set
        int n=m_FieldValue.count();
        if(n<=0)return false;
        sql="update "+m_scurTable+" set ";
        for(i=0;i<n;i++)
        {
            sql+=m_FieldValue.at(i).field+"="+m_FieldValue.at(i).value;
            if(i<n-1)sql+=",";
        }
        QString where="";
        if(getWhereString(where))
            sql+=" "+where;
    }
    else if(st==GET_SELECT)
    {
        sql="select ";
        if(m_sSel=="")sql+="* ";
        else sql+=m_sSel;
        sql+=" from "+m_scurTable + " ";
        QString where="";
        if(getWhereString(where))
            sql+=" "+where;
        if(m_Limit!="")
            sql+=" "+m_Limit;
    }
    return true;
}

bool DBVam::getWhereString(QString& where)
{
    int n;
    n=m_Where.count();
    if(n<=0)
        return true;
    int i;
    where+=" where ";
    QString tmp;
    for(i=0;i<n;i++)
    {
        if(i>0)
            where+=(m_Where.at(i).bAnd?" and ":" or ");
        where+="("+m_Where.at(i).whereStr+") ";
    }
    return true;
}

void DBVam::Test()
{
    QSqlQuery qry(*m_pDatabase);
    for(int i=0;i<5;i++)
    {
    QString sql="insert into vam_test values(1)";
    if(!qry.exec(sql))
        qDebug()<<"ERROR:"<<qry.lastError();
    }
    if(qry.exec("select * from vam_test"))
    {
        while(qry.next())
        {
            qDebug()<<"2";
        }
    }
    else {
        qDebug()<<"ERROR:"<<qry.lastError();

    }
}

QString DBVam::stringValid(QString& s)
{
    return s.replace("'","''");
}

void DBVam::setSelectField(QString sel)
{
    m_sSel=sel;
}

void DBVam::addLimit(int limit, int offset)
{
    m_Limit=" LIMIT "+QString::number(limit);
    if(offset>-1)
        m_Limit+=" OFFSET "+QString::number(offset);
}

