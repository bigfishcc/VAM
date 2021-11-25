#include "cthreadcheck.h"
#include <QDir>
#include <QTextStream>
#include <QTime>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>

CThreadCheck::CThreadCheck() :
    QThread()
{
    m_nRunType=RUN_NONE;
    m_nVamType=-1;
    m_pVamStat=new VamStat;
    m_varList.clear();
}

void CThreadCheck::run()
{
    if(m_nRunType==RUN_NONE)
        return;
    *m_pbThread=true;
    if(m_pVamStat==nullptr)
        m_pVamStat=new VamStat;
    switch(m_nRunType)
    {
    case RUN_LIST:
        m_pVamStat->FillList(m_nVamType, m_nGender);
        break;
    case RUN_QUERY_LIST:
        m_pVamStat->Database(true);
        m_pVamStat->FillListWithQuery(m_sQuerySql,0);
        m_pVamStat->Database(false);
        break;
    case RUN_STAT:
        m_pVamStat->StartStat(true);
        m_pVamStat->Database(true);
        m_pVamStat->StartZip(true);
        m_fileCounter=0;
        SearchPath(m_pVamStat->m_sGamePath);
        m_pVamStat->AnalyseVarList();
        //    m_pVamStat->CheckReserved();
        m_pVamStat->StartZip(false);
        m_pVamStat->Database(false);
        m_pVamStat->StartStat(false);
        break;
    case RUN_VAR_VALID:
        checkVar(0);
        break;
    case RUN_VAR_SAME:
        checkVar(1);
        break;
    case RUN_ITEM_PIC:
        m_pVamStat->StartZip(true);
        m_pVamStat->Database(true);
        m_pVamStat->ViewItemPic(m_nItemId);
        m_pVamStat->Database(false);
        m_pVamStat->StartZip(false);
        break;
    case RUN_VIEW_IN_VAR:
        m_pVamStat->Database(true);
        m_pVamStat->QueryContent(m_sVarFullFilePath);
        m_pVamStat->Database(false);
        break;
    }
//  emit showInfo("yes");
    *m_pbThread=false;
}

bool CThreadCheck::SearchPath(QString path)
{
    QDir dir(path);
    if(!dir.exists())
        return false;
    dir.setFilter(QDir::Dirs|QDir::Files);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list=dir.entryInfoList();
    int i=0;
    QVector<extType> et;
    et.clear();
    do{
        QFileInfo fileInfo=list.at(i);
        bool bIsDir=fileInfo.isDir();
        if(bIsDir)
        {
            QString file_name = fileInfo.fileName();
            if(file_name!="."&&file_name!="..")
                SearchPath(fileInfo.filePath());
        }
        else {
            fileFound(fileInfo);
            m_fileCounter++;
            emit fileCounter(m_fileCounter);
        }
        i++;
    }while(i<list.size()&&(*m_pbThread));
    return true;
}

void CThreadCheck::fileFound(QFileInfo& fileInfo)
{
    if(m_nRunType==RUN_VAR_VALID||m_nRunType==RUN_VAR_SAME)
    {
        QString ext=fileInfo.suffix().toLower();
        if(ext=="var"||ext=="rar"||ext=="zip"||ext=="7z")
        {
            VAR_INFO vi;
            vi.path=fileInfo.absolutePath();
            vi.ftitle=fileInfo.completeBaseName();
            vi.fext=ext;
            vi.size=static_cast<int>(fileInfo.size());
            m_varList.push_back(vi);
        }
    }
    else {
        //文件名
        //文件后缀
        QString file_suffix = fileInfo.suffix();
        //绝对路径
        QString file_path = fileInfo.absolutePath();
        m_pVamStat->StatVamType(fileInfo);
    }
}

void CThreadCheck::SortExt(QVector<extType>& et)
{
    int total=et.count();
    int i, j;
    for(i=0;i<total-1;i++)
        for(j=i;j<total;j++)
        {
            if(et[i].sum<et[j].sum)
            {
                extType tmp=et[i];
                et[i]=et[j];
                et[j]=tmp;
            }
        }
}

void CThreadCheck::checkVar(int nFunc)
{
    if(m_varList.count()<=0)
    {
        m_fileCounter=0;
        SearchPath(m_pVamStat->m_sGamePath);
    }
    int cnt=m_varList.count();
    QListWidgetItem* pItem=new QListWidgetItem(m_pVamStat->m_sGamePath);
    pItem->setToolTip(m_pVamStat->m_sGamePath);
    emit showToList(pItem,2);
//    delete pItem;
    if(nFunc==0)        //check none var
    {
        int i;
        bool bShow;
        for(i=0;i<cnt;i++)
        {
            bShow=false;
            VAR_INFO vi=m_varList[i];
            if(vi.fext=="var")
            {
                QFile fn(vi.path+"/"+vi.ftitle+"."+vi.fext);
                if(!fn.exists())continue;
                if(!fn.open(QIODevice::ReadOnly))continue;
                char flag[2];
                fn.read(flag,2);
                fn.close();
                if(flag[0]!='P'&&flag[1]!='Z')
                    bShow=true;
            }
            else {
                bShow=true;
            }
            if(bShow)
            {
                QListWidgetItem* pItem=new QListWidgetItem(vi.ftitle+"."+vi.fext);
                pItem->setToolTip(vi.path+"/"+vi.ftitle+"."+vi.fext);
                emit showToList(pItem,2);
//                delete pItem;
            }
        }
    }
    else if(nFunc==1){  //check same
        int i;
        bool bShow;
        for(i=0;i<cnt;i++)
        {
            m_varList[i].flag=0;
        }
        int j;
        bool bFirst=true;
        for(i=0;i<cnt-1;i++)
        {
            bFirst=true;
            for(j=i+1;j<cnt;j++)
            {
                if(m_varList[i].flag==1)continue;
                if(m_varList[i].size==0)continue;
                if(m_varList[i].size==m_varList[j].size)
                {
                    m_varList[j].flag=1;
                    QListWidgetItem* pItem=new QListWidgetItem(">>"+m_varList[i].ftitle+"."+m_varList[i].fext);
                    if(bFirst)
                    {
                        bFirst=false;
                        pItem->setToolTip(m_varList[i].path+"/"+m_varList[i].ftitle+"."+m_varList[i].fext);
                        emit showToList(pItem,2);
                    }
                    QListWidgetItem* pItem2=new QListWidgetItem("===="+m_varList[j].ftitle+"."+m_varList[j].fext);
                    pItem2->setToolTip(m_varList[j].path+"/"+m_varList[j].ftitle+"."+m_varList[j].fext);
                    emit showToList(pItem2,2);
                    m_varList[j].flag=1;
                }
            }
        }
    }
}
