#ifndef DBVAM_H
#define DBVAM_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QVector>
#include "def.h"

struct FIELD_VALUE
{
    QString field;
    QString value;
};

struct WHERE_STR
{
    QString whereStr;
    bool bAnd;
};

class DBVam : public QObject
{
    Q_OBJECT
public:
    explicit DBVam(QObject *parent = nullptr);
    bool openDB();
    void closeDB();
    bool createTable();
    bool exec(QString sql);
    bool query(QString sql);
    bool query_movenext();
    void queryTest();
    QVariant query_value(int nField);
    void clear();
    void setTable(QString tablename);
    void setValue(QString field, QString value);
    void addWhere(QString whereStr, bool bAnd);
    void addLimit(int limit, int offset);
    void setSelectField(QString sel);
    bool getSQLString(QString& sql, int st);
    void Test();
    QString stringValid(QString& s);

signals:

public slots:

protected:
    QString m_sDBName;
    QSqlDatabase* m_pDatabase;
    QSqlQuery* m_pQuery;
    QString m_scurTable;
    QVector<FIELD_VALUE> m_FieldValue;
    QVector<WHERE_STR> m_Where;
    QString m_sSel;
    QString m_Limit;
    bool getWhereString(QString& where);
};

#endif // DBVAM_H
