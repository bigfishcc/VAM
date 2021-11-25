#include "zip.h"
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>

Zip::Zip(QObject *parent) : QObject(parent)
{
    QString settFile=QCoreApplication::applicationDirPath()+"/set.ini";
    QSettings sett(settFile,QSettings::IniFormat);
    m_s7Zip=sett.value("Settings/zip").toString();
    m_sTempPath=sett.value("Settings/WorkPath").toString()+"/~tmp";
    QDir dir(m_sTempPath);
    if(!dir.exists())
    {
        dir.mkpath(m_sTempPath);
    }
    m_caller=nullptr;
}

void Zip::ExtractAFile(QString sPackFile, QString sExtractFilePath, QString destFilePath, QString& newFilePath)
{
    if(m_caller==nullptr)
        m_caller = new QProcess(this);//创建对象，指定父类指针
//    connect(caller, SIGNAL(finished()),this,SLOT(on_ProcessResult()));//连接信号
    //    connect(caller, SIGNAL(readyReadStandardError()),this,SLOT(on_ProcessResult()));
    disconnect(m_caller, SIGNAL(readyReadStandardOutput()),this,SLOT(on_ProcessResult()));
    if(destFilePath=="")
        destFilePath=m_sTempPath;
    int n=sExtractFilePath.lastIndexOf("/");
    newFilePath=destFilePath+"/"+sExtractFilePath.mid(n+1);
    m_sExeResult="";
    QStringList para;
    para<<"e";
    para<<("-o"+destFilePath)<<"-y";
    para<<sPackFile;
    para<<sExtractFilePath;
//    qDebug()<<para;
    m_caller->execute(m_s7Zip,para);//启动程序并传入参数
}

void Zip::PackFileList(QString sPackFile)
{
    if(m_caller==nullptr)
        m_caller = new QProcess(this);//创建对象，指定父类指针
    connect(m_caller, SIGNAL(readyReadStandardOutput()),this,SLOT(on_ProcessResult()),Qt::DirectConnection);
    m_sExeResult="";
    QStringList para;
    para<<"l"<<sPackFile;
    m_caller->start(m_s7Zip,para);//启动程序并传入参数
//    qDebug()<<m_s7Zip;
//    qDebug()<<sPackFile;
}

void Zip::on_ProcessResult()
{
    QByteArray res = m_caller->readAllStandardOutput(); //获取标准输出
//    int n = res.count();
//    qDebug()<<n;
    QString str=QString::fromLocal8Bit(res);
//    qDebug()<<str;
//    ui->textEdit->setText(str); //打印出来
    m_sExeResult+=str;
    if(!m_caller->canReadLine())  //output end
    {
/*        QString tmp=m_sExeResult;
        QFile file("d:/cmd.txt");
        file.open(QIODevice::WriteOnly);
        QTextStream outstr(&file);
        outstr<<tmp;
        file.flush();
        file.close();
*/
//        caller->close();
//        delete caller;
    }
}

void Zip::WaitFinished()
{
    m_caller->waitForFinished();
    m_caller->close();
}

void Zip::GetFileList(QVector<FILE_IN_VAR>& list,QString sPackFile)
{
    WaitFinished();
    int n=m_sExeResult.indexOf("------\r\n");
    QString tmp=m_sExeResult.right(m_sExeResult.length()-n-8);
    n=tmp.indexOf("\r\n------");
    tmp=tmp.left(n+2);
    QString fn;
    while(1)
    {
        n=tmp.indexOf("\r\n");
        if(n<0)break;
        fn=tmp.left(n);
        QString s=fn.mid(20,1);
        if(s!="D")
        {
//            qDebug()<<"***************"<<fn;
            FILE_IN_VAR fiv;
            fiv.parentVarName=sPackFile;
            fiv.fileTime=fn.left(19);
            QString fstmp=fn.mid(25,13);
            int m=fstmp.lastIndexOf(' ');
            fstmp=fstmp.right(fstmp.length()-m-1);
            bool ok;
            fiv.fileSize=fstmp.toLong(&ok, 10);
            fiv.fileName=fn.right(fn.length()-53);
            fiv.fileName.replace('\\','/');
//            qDebug()<<fiv.fileName<<"==="<<fiv.fileSize<<"==="<<fiv.fileTime;
            list.push_back(fiv);
        }
        tmp=tmp.right(tmp.length()-n-2);
    }
}
