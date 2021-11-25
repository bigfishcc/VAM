#ifndef VAMSTAT_H
#define VAMSTAT_H

#include <QObject>
#include <QFileInfo>
#include <QVector>
#include <QListWidget>
#include "def.h"
#include "zip.h"
#include "dbvam.h"

class VamStat : public QObject
{
    Q_OBJECT
public:
    explicit VamStat(QObject *parent = nullptr);
    void StatVamType(QFileInfo& fileInfo);
    void StatVamType(QVector<FILE_IN_VAR>& list, int nIndex);
    void FillList(int nTypeIndex, int nGender);
    void FillListWithQuery(QString sSQL, int nList);
    void AnalyseVarList();
    QVector<VAM_ITEM> m_VamItemList[VAM_TYPE_TOTAL];
    bool* m_pbThread;
    void CheckReserved();
    void Database(bool bStart);
    void StartStat(bool bStart);
    void StartZip(bool bStart);
    void QueryContent(QString sVarFullFilePath);
    bool HaveStat();
    QString m_sGamePath;
    QString m_sWorkPath;
    QString m_sDataPath;
    int m_nGameId;
    void ViewItemPic(int nItemId);

signals:
    void setProgress(int nTotal, int nCur);
    void showToList(QListWidgetItem*,int);
    void showPic(QString picPath);

public slots:

protected:
    Zip* m_pZip;
    DBVam* m_pDB;
    void AddToVamTypeList(QString ext, VAM_ITEM& vt);
    QString VamTypeName(int nVamType);
};

#endif // VAMSTAT_H
