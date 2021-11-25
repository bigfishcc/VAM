#include "vamstat.h"
#include <QDebug>
#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QSettings>
#include <QDir>

VamStat::VamStat(QObject *parent) : QObject(parent)
{
    m_sGamePath="";
    m_nGameId=-1;
    m_pZip=nullptr;
    m_pDB=nullptr;
    QString settFile=QCoreApplication::applicationDirPath()+"/set.ini";
    QSettings sett(settFile,QSettings::IniFormat);
    m_sWorkPath=sett.value("Settings/WorkPath").toString();
    m_sDataPath="";
    QDir dir(m_sWorkPath);
    if(!dir.exists())
        dir.mkpath(m_sWorkPath);
}


void VamStat::StatVamType(QFileInfo& fileInfo)
{
    VAM_ITEM vt;
    vt.name=fileInfo.fileName();
    if(vt.name=="meta.json")return;
    int nStart=m_sGamePath.length()+1;
    vt.author="";
    vt.orgFilePath=fileInfo.filePath();
    vt.filePath=fileInfo.path();
    QString ext=fileInfo.suffix().toLower();
    vt.inVar=false;
    vt.prevFilePath="";
    if(ext!="png"&&ext!="jpg")
    {
        int n=vt.orgFilePath.lastIndexOf('.');
        QString fs=vt.orgFilePath.left(n+1)+"jpg";
        QFileInfo fi(fs);
        if(fi.exists())
        {
            vt.prevFilePath=fs.mid(nStart);
        }
    }
    QDateTime dt = fileInfo.birthTime();
    vt.fileTime=dt.toString("yyyy-MM-dd hh:mm:ss");
    vt.fileSize = static_cast<long>(fileInfo.size());
    vt.orgFilePath=vt.orgFilePath.mid(nStart);
    vt.filePath=vt.filePath.mid(nStart);
    AddToVamTypeList(ext, vt);
}


void VamStat::StatVamType(QVector<FILE_IN_VAR>& list, int nIndex)
{
    VAM_ITEM vt;
    FILE_IN_VAR fiv=list[nIndex];
    if(fiv.fileName=="meta.json")return;
    int nStart=m_sGamePath.length()+1;
    vt.orgFilePath=fiv.parentVarName.mid(nStart);
    int n=fiv.fileName.lastIndexOf('/');
    if(n<0)
    {
        vt.name=fiv.fileName;
        vt.filePath="";
    }
    else {
        vt.name=fiv.fileName.mid(n+1);
        vt.filePath=fiv.fileName.left(n);
    }
    n=vt.name.lastIndexOf('.');
    if(n<0)return;
    QString ext=vt.name.mid(n+1).toLower();
    n=fiv.parentVarName.lastIndexOf('/');
    QString author=fiv.parentVarName.mid(n+1);
    n=author.indexOf('.');
    vt.author=author.left(n);
    vt.prevFilePath="";
    vt.inVar=true;
    if(ext!="png"&&ext!="jpg")
    {
        n=fiv.fileName.lastIndexOf('.');
        QString fs=fiv.fileName.left(n+1)+"jpg";
        int cnt=list.count();
        int i;
        for(i=0;i<cnt;i++)
        {
            if(fs.compare(list[i].fileName,Qt::CaseInsensitive)==0)
            {
                vt.prevFilePath=fs;
                break;
            }
        }
    }
    vt.fileSize=fiv.fileSize;
    vt.fileTime=fiv.fileTime;
    AddToVamTypeList(ext, vt);
}

void VamStat::AddToVamTypeList(QString ext, VAM_ITEM& vt)
{
    int index=-1;
    if(ext=="var")
    {
        index=TYPE_VAR;
        m_VamItemList[index].push_back(vt);
        return;
    }
    vt.gender=GEN_ALL;
    QString path=vt.filePath;
    if(ext=="assetbundle")
        index=TYPE_ASSET;
    else if(ext=="vac")
        index=TYPE_SCENE;
    else if(ext=="png"||ext=="jpg")
    {
        if(path.indexOf("/Textures",0,Qt::CaseInsensitive)>=0)
            index=TYPE_TEXTURE;
    }
    else if(ext=="cs"||ext=="cslist")
        index=TYPE_SCRIPT;
    else if(ext=="json")
    {
        if(path.indexOf("/Scene",0,Qt::CaseInsensitive)>=0||
            path.indexOf("/Subscene",0,Qt::CaseInsensitive)>=0)
            index=TYPE_SCENE;
        else if(path.indexOf("/appearance",0,Qt::CaseInsensitive)>=0)
            index=TYPE_APPEAR;
        else if(path.indexOf("/pose",0,Qt::CaseInsensitive)>=0)
            index=TYPE_POSE;
        else if(path.indexOf("/scripts",0,Qt::CaseInsensitive)>=0)
            index=TYPE_SCRIPT;
        else {
            index=TYPE_UNKNOWN;
        }
    }
    else if(ext=="scene")
    {
        if(path.indexOf("/Scene",0,Qt::CaseInsensitive)>=0||
            path.indexOf("/Subscene",0,Qt::CaseInsensitive)>=0)
            index=TYPE_SCENE;
        else if(path.indexOf("/Asset",0,Qt::CaseInsensitive)>=0)
            index=TYPE_ASSET;
        else {
            index=TYPE_UNKNOWN;
        }
    }
    else if(ext=="vab")
    {
//        qDebug()<<path;
        if(path.indexOf("/Clothing",0,Qt::CaseInsensitive)>=0)
        {
            index=TYPE_CLOTH;
            if(path.indexOf("/Male",0,Qt::CaseInsensitive)>=0)
                vt.gender=GEN_MALE;
            else {
                vt.gender=GEN_FEMALE;
            }
        }
        else if(path.indexOf("/Hair",0,Qt::CaseInsensitive)>=0)
        {
            index=TYPE_HAIR;
            if(path.indexOf("/Male",0,Qt::CaseInsensitive)>=0)
                vt.gender=GEN_MALE;
            else {
                vt.gender=GEN_FEMALE;
            }
        }
        else {
            index=TYPE_UNKNOWN;
        }
//        qDebug()<<index;
    }
    else if(ext=="vap")
    {
        if(path.indexOf("/Clothing",0,Qt::CaseInsensitive)>=0)
        {
            index=TYPE_CLOTH;
            if(path.indexOf("/Male",0,Qt::CaseInsensitive)>=0)
                vt.gender=GEN_MALE;
            else {
                vt.gender=GEN_FEMALE;
            }
        }
        else if(path.indexOf("/Hair",0,Qt::CaseInsensitive)>=0)
        {
            index=TYPE_HAIR;
            if(path.indexOf("/Male",0,Qt::CaseInsensitive)>=0)
                vt.gender=GEN_MALE;
            else {
                vt.gender=GEN_FEMALE;
            }
        }
        else if(path.indexOf("/Pose",0,Qt::CaseInsensitive)>=0)
        {
            index=TYPE_POSE;
        }
        else
            index=TYPE_APPEAR;
    }
    if(index==TYPE_SCENE)
    {
        if(path.indexOf("Scripts",0,Qt::CaseInsensitive)>=0)
            index=TYPE_UNKNOWN;
    }
    if(vt.author==""&&index>TYPE_UNKNOWN)
    {
        int n=-1;
        QString findstr="";
        if(index==TYPE_SCENE)
            findstr="scene/";
        else if(index==TYPE_APPEAR)
            findstr="appearance/";
        else if(index==TYPE_CLOTH)
        {
            findstr="Clothing/Female/";
            n=vt.filePath.indexOf(findstr,Qt::CaseInsensitive);
            if(n<0)
            {
                findstr="Clothing/Male/";
                n=vt.filePath.indexOf(findstr,Qt::CaseInsensitive);
            }
        }
        if(findstr!=""||n>0)
        {
            if(n<0)
                n=vt.filePath.indexOf(findstr,Qt::CaseInsensitive);
            if(n>0)
            {
                QString tmp=vt.filePath.mid(n+findstr.length());
    //            qDebug()<<"tmp="<<tmp;
                n=tmp.indexOf('/');
                if(n>0)
                {
                    vt.author=tmp.left(n);
                }
                else {
                    vt.author=tmp;
                }
    //            qDebug()<<vt.author;
            }
        }
    }
    if(index>=TYPE_UNKNOWN)
    {
        m_VamItemList[index].push_back(vt);
    }
    if(index>TYPE_UNKNOWN)
    {
        if(vt.inVar)
        {
            if(vt.prevFilePath!="")
            {
                QString rel_path="image/"+QString::number(index);
                QString sDestPath=m_sDataPath+"/"+rel_path;
                QString prevfn=vt.prevFilePath;
                prevfn.replace("/","_");
                QString newfn=sDestPath+"/"+prevfn;
                QFileInfo fi(newfn);
                if(!fi.exists())
                {
                    QString iconfn;
                    m_pZip->ExtractAFile(m_sGamePath+"/"+vt.orgFilePath,
                                   vt.prevFilePath, sDestPath, iconfn);
                    QFile::rename(iconfn,newfn);
                }
                vt.prevFilePath=rel_path+"/"+prevfn;
            }
        }
        //database
        DBVam dbs;
        QString sql;
        sql="select count(*) from vam_item where vamgameid="+QString::number(m_nGameId)+" and orgfilepath='"+dbs.stringValid(vt.orgFilePath)+"'";
        sql+=" and name='"+dbs.stringValid(vt.name)+"' and filepath='"+dbs.stringValid(vt.filePath)+"'";
        bool bHave=true;
        if(m_pDB->query(sql))
        {
            m_pDB->query_movenext();
            bHave=(m_pDB->query_value(0).toInt()>0);
        }
        if(!bHave)
        {
            dbs.setTable("vam_item");
            dbs.setValue("vamgameid",QString::number(m_nGameId));
            dbs.setValue("name","'"+dbs.stringValid(vt.name)+"'");
            dbs.setValue("vamtype",QString::number(index));
            dbs.setValue("gender",QString::number(vt.gender));
            dbs.setValue("invar",vt.inVar?"1":"0");
            dbs.setValue("filesize",QString::number(vt.fileSize));
            dbs.setValue("filepath","'"+dbs.stringValid(vt.filePath)+"'");
            dbs.setValue("prevfilepath","'"+dbs.stringValid(vt.prevFilePath)+"'");
            dbs.setValue("prevfileloc",vt.inVar?"1":"0");
            dbs.setValue("orgfilepath","'"+dbs.stringValid(vt.orgFilePath)+"'");
            dbs.setValue("author","'"+dbs.stringValid(vt.author)+"'");
            dbs.setValue("vamflag","''");
            dbs.setValue("descript","''");
            dbs.setValue("score","0");
            dbs.setValue("birthTime","'"+vt.fileTime+"'");
            QDateTime ctm=QDateTime::currentDateTime();
            dbs.setValue("recordTime","'"+ctm.toString("yyyy-MM-dd hh:mm:ss")+"'");
            dbs.setValue("checked","1");
            dbs.getSQLString(sql, GET_INSERT);
//        qDebug()<<sql;
            m_pDB->exec(sql);
        }
    }
}

void VamStat::AnalyseVarList()
{
    int n=m_VamItemList[TYPE_VAR].count();
    if(n<=0)return;
    int i;
    QVector<FILE_IN_VAR>list;
    int j;
    for(i=0;i<n;i++)
    {
        if((*m_pbThread)==false)break;
        emit setProgress(n,i);
        list.clear();
        QString ofn=m_sGamePath+"/"+m_VamItemList[TYPE_VAR][i].orgFilePath;
        m_pZip->PackFileList(ofn);
        m_pZip->GetFileList(list,ofn);
        int nList=list.count();
//        qDebug()<<"AnalyseVamList: "<<i<<"/"<<n<<";file cnt"<<nList;
        for(j=0;j<nList;j++)
        {
            StatVamType(list,j);
        }
    }
    list.clear();
}

void VamStat::FillList(int nTypeIndex, int nGender)
{
    if(nTypeIndex<0)return;
    int n=m_VamItemList[nTypeIndex].count();
    int i;
    int n2=n;
    if(nGender!=GEN_ALL)
    {
        n2=0;
        for(i=0;i<n;i++)
        {
            if(nTypeIndex==TYPE_CLOTH||nTypeIndex==TYPE_HAIR)
            {
                if(nGender!=m_VamItemList[nTypeIndex].at(i).gender)
                    continue;
            }
            n2++;
        }
    }
//    if(m_pZip!=nullptr)
//        delete m_pZip;
//    m_pZip=new Zip;
    int i2;
    for(i=0,i2=0;i<n;i++)
    {
        if((*m_pbThread)==false)break;
        emit setProgress(n2,i2);
        if(nGender!=GEN_ALL)
        {
            if(nTypeIndex==TYPE_CLOTH||nTypeIndex==TYPE_HAIR)
            {
                if(nGender!=m_VamItemList[nTypeIndex].at(i).gender)
                    continue;
            }
        }
        i2++;
        QString txt;
        txt=m_VamItemList[nTypeIndex].at(i).name;
        if(m_VamItemList[nTypeIndex].at(i).author!="")
            txt="【"+m_VamItemList[nTypeIndex].at(i).author+"】"+txt;
        QString iconfn=m_VamItemList[nTypeIndex].at(i).prevFilePath;
        if(iconfn=="")
            iconfn=QCoreApplication::applicationDirPath()+"/image/nopic.jpg";
        else {
            iconfn=m_sGamePath+"/"+iconfn;
        }
        if(m_VamItemList[nTypeIndex].at(i).inVar)
        {
            if(m_VamItemList[nTypeIndex].at(i).prevFilePath=="")
                iconfn=QCoreApplication::applicationDirPath()+"/image/nopic_var.jpg";
            else {
                iconfn=m_sDataPath+"/"+m_VamItemList[nTypeIndex].at(i).prevFilePath;
//                qDebug()<<iconfn;
            }
//            m_pZip->ExtractAFile(m_scurGamePath+m_VamItemList[nTypeIndex].at(i).orgFilePath,
//                               m_VamItemList[nTypeIndex].at(i).prevFilePath, "", iconfn);
//            m_zip.WaitFinished();
/*            if(m_Vam[nTypeIndex].at(i).name=="Ryan.json")
            {
                QFile::copy(iconfn,"d:/2.jpg");
            }
*/        }
        QString tp=m_VamItemList[nTypeIndex].at(i).filePath;
        if(m_VamItemList[nTypeIndex].at(i).inVar)
        {
            tp=m_VamItemList[nTypeIndex].at(i).orgFilePath+" : "+m_VamItemList[nTypeIndex].at(i).filePath;
        }
        txt+=" ("+tp+")";
        QListWidgetItem* item=new QListWidgetItem(QIcon(iconfn),txt);
        item->setToolTip(tp);
        emit showToList(item,0);//pList->addItem(item);

//        if(m_VamItemList[nTypeIndex].at(i).inVar)
//            QFile::remove(iconfn);
    }
//    delete m_pZip;
//    m_pZip=nullptr;
}

void VamStat::FillListWithQuery(QString sSQL, int nList)
{
    int n=sSQL.indexOf("from");
    QString sql2="select count(*) "+sSQL.mid(n);
    if(!m_pDB->query(sql2))
    {
        qDebug()<<sql2;
        return;
    }
    m_pDB->query_movenext();
    int nTotal=m_pDB->query_value(0).toInt();
    if(!m_pDB->query(sSQL))
        return;
    int index=0;
    while(m_pDB->query_movenext()&&(*m_pbThread))
    {
        emit setProgress(nTotal, index);
        QString iconfn=m_pDB->query_value(8).toString();    //prevfilepath
        QString tp=m_pDB->query_value(7).toString();    //path
        if(m_pDB->query_value(3).toInt()==TYPE_TEXTURE) //vamtype
        {
            iconfn=QCoreApplication::applicationDirPath()+"/image/click.jpg";
        }
        else {
            if(m_pDB->query_value(5).toInt()==1)    //invar
            {
                if(iconfn=="")
                    iconfn=QCoreApplication::applicationDirPath()+"/image/nopic_var.jpg";
                else
                    iconfn=m_sDataPath+"/"+iconfn;
                tp=m_pDB->query_value(10).toString()+" : "+tp;    //orgfile
            }
            else
            {
                if(iconfn=="")
                    iconfn=QCoreApplication::applicationDirPath()+"/image/nopic.jpg";
                else
                    iconfn=m_sGamePath+"/"+iconfn;
            }
        }
        QString txt="【"+m_pDB->query_value(F_AUTHOR).toString()+"】 "+m_pDB->query_value(F_NAME).toString()+
                "《"+VamTypeName(m_pDB->query_value(F_VAM_TYPE).toInt())+"》";   //author, name
        txt+=" 【"+tp+"】";
        QListWidgetItem* item=new QListWidgetItem(QIcon(iconfn),txt);
        QString promptStr=m_sGamePath+"/"+m_pDB->query_value(10).toString(); //org file path
        item->setToolTip(promptStr);
        item->setData(Qt::UserRole,m_pDB->query_value(0).toInt());
        emit showToList(item,nList);//pList->addItem(item);
        index++;
    }
}

QString VamStat::VamTypeName(int nVamType)
{
    switch(nVamType)
    {
    case TYPE_SCENE: return "SCENE";break;
    case TYPE_ASSET: return "ASSET";break;
    case TYPE_CLOTH: return "CLOTHING";break;
    case TYPE_HAIR: return "HAIR";break;
    case TYPE_APPEAR: return "LOOK";break;
    case TYPE_POSE: return "POSE";break;
    case TYPE_SCRIPT: return "SCRIPT";break;
    case TYPE_TEXTURE: return "TEXTURE";break;
    case TYPE_VAR: return "VAR";break;
    case TYPE_OTHER: return "OTHER";break;
    case TYPE_UNKNOWN:
    default:
        return "";break;
    }
    return "";
}
void VamStat::StartStat(bool bStart)
{
    if(bStart)
    {
        int i;
        for(i=0;i<VAM_TYPE_TOTAL;i++)
            m_VamItemList[i].clear();
    }
    else {
    }
}

void VamStat::StartZip(bool bStart)
{
    if(bStart)
    {
        if(m_pZip!=nullptr)
            delete m_pZip;
        m_pZip=new Zip;
    }
    else {
        delete m_pZip;
        m_pZip=nullptr;
    }
}

void VamStat::CheckReserved()
{
    int i;
    int cnt=m_VamItemList[TYPE_UNKNOWN].count();
    for(i=0;i<cnt;i++)
    {
        qDebug()<<i<<":"<<m_VamItemList[TYPE_UNKNOWN].at(i).orgFilePath<<"----"<<m_VamItemList[TYPE_UNKNOWN].at(i).filePath<<"----"<<m_VamItemList[TYPE_UNKNOWN].at(i).name;
    }
    for(i=0;i<VAM_TYPE_TOTAL;i++)
    {
        int nTotal=m_VamItemList[i].count();
        qDebug()<<i<<":"<<nTotal;
    }
}

void VamStat::Database(bool bStart)
{
    if(bStart)
    {
        if(m_pDB!=nullptr)
            delete m_pDB;
        m_pDB=new DBVam;
        m_pDB->openDB();
    }
    else {
        m_pDB->closeDB();
        delete m_pDB;
        m_pDB=nullptr;
    }
}

bool VamStat::HaveStat()
{
    int i;
    for(i=0;i<VAM_TYPE_TOTAL;i++)
    {
        if(m_VamItemList[i].count()>0)
            return true;
    }
    return false;
}



void VamStat::ViewItemPic(int nItemId)
{
    QString sql="select * from vam_item where id="+QString::number(nItemId);
    if(!m_pDB->query(sql))
    {
        return;
    }
    if(!m_pDB->query_movenext())
        return;
    int invar=m_pDB->query_value(F_IN_VAR).toInt();    //invar
    QString prev=m_pDB->query_value(F_PREV_FILE_PATH).toString();  //prevfilepath
    QString sPath;
    int type=m_pDB->query_value(F_VAM_TYPE).toInt();     //vamtype
    if(type==TYPE_TEXTURE)
    {
        if(invar==1)
        {
            QString sDestPath=m_sWorkPath+"/~tmp";
            QString iconfn;
            QString name=m_pDB->query_value(F_NAME).toString();
            QString prev=m_pDB->query_value(F_FILE_PATH).toString()+"/"+name;
            int n=name.lastIndexOf('.');
            QString ext=name.mid(n);
            sPath=sDestPath+"/pic"+ext;
            QFile::remove(sPath);
            m_pZip->ExtractAFile(m_sGamePath+"/"+m_pDB->query_value(F_ORG_FILE_PATH).toString(),
                           prev, sDestPath, iconfn);
            QFile::rename(iconfn,sPath);
        }
        else {
            sPath=m_sGamePath+"/"+m_pDB->query_value(F_ORG_FILE_PATH).toString();
        }
    }
    else {
        if(invar==1)
        {
            if(prev=="")
                sPath=QCoreApplication::applicationDirPath()+"/image/nopic_var.jpg";
            else {
                sPath=m_sDataPath+"/"+prev;
            }
        }
        else
        {
            if(prev=="")
                sPath=QCoreApplication::applicationDirPath()+"/image/nopic.jpg";
            else
                sPath=m_sGamePath+"/"+prev;
        }
    }
    emit showPic(sPath);
}

void VamStat::QueryContent(QString sVarFullFilePath)
{
    int n=m_sGamePath.length();
    QString sPath=sVarFullFilePath.mid(n+1);
    QString sql="select * from vam_item where orgfilepath='"+m_pDB->stringValid(sPath)+"' and vamgameid="+QString::number(m_nGameId);
    qDebug()<<sql;
    FillListWithQuery(sql,1);
}
