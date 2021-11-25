#ifndef CTHREADCHECK_H
#define CTHREADCHECK_H

#include <QThread>
#include <QFile>
#include <QVector>
#include <QDir>
#include <QListWidget>
#include "def.h"
#include "vamstat.h"

class CThreadCheck : public QThread
{
    Q_OBJECT
public:
    CThreadCheck();
    int m_nRunType;
    int m_nVamType;
    int m_nGender;
    VamStat* m_pVamStat;
    bool* m_pbThread;
    int m_fileCounter;
    QString m_sQuerySql;
    int m_nItemId;
    QString m_sVarFullFilePath;

protected:
    void run();
    bool SearchPath(QString path);
    QVector<extType> m_fileType;
    QVector<VAR_INFO> m_varList;
    void SortExt(QVector<extType>& et);
    void checkVar(int nFunc);
    void fileFound(QFileInfo& fileInfo);

signals:
    void showInfo(QString);
    void fileCounter(int fc);
    void showToList(QListWidgetItem*,int);

protected slots:
};

#endif // CTHREADCHECK_H
