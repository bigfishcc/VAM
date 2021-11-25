#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTreeWidget>
#include <QDebug>
#include <QTextStream>
#include <QFileDialog>
#include <QSettings>
#include "zip.h"
#include "dbvam.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings(true);
    initConnect();
    initWindow(true);
    m_bThread=false;
    m_bGameComboOK=false;
}

MainWindow::~MainWindow()
{
    settings(false);
    delete ui;
}


void MainWindow::initConnect()
{
//    connect(&m_thCheck,SIGNAL(showInfo(QString)),this,SLOT(on_showInfo(QString)));
    connect(m_thCheck.m_pVamStat,SIGNAL(setProgress(int,int)),this,SLOT(on_Progress(int,int)));
    connect(&m_thCheck,SIGNAL(fileCounter(int)),this,SLOT(on_fileCounter(int)));
    connect(&m_thCheck,SIGNAL(showToList(QListWidgetItem*,int)),this,SLOT(on_showToList(QListWidgetItem*,int)));
    connect(m_thCheck.m_pVamStat,SIGNAL(showToList(QListWidgetItem*,int)),this,SLOT(on_showToList(QListWidgetItem*,int)));
    connect(m_thCheck.m_pVamStat,SIGNAL(showPic(QString)),this,SLOT(on_showPic(QString)));
}

void MainWindow::on_showInfo(QString info)
{
    int count = ui->listWidget_Cont->count();

    info+="("+QString::number(count)+")";
    QMessageBox::information(this,"Info",info);
}

void MainWindow::initWindow(bool bInit)
{
    if(bInit)
    {
        int i;
        QListWidget* pList;
        for(i=0;i<2;i++)
        {
            if(i==0)
                pList=ui->listWidget_Cont;
            else {
                pList=ui->listWidget_VarContent;
            }
            //设置QListWidget的显示模式
            pList->setViewMode(QListView::IconMode);
            //设置QListWidget中单元项的图片大小
            pList->setIconSize(QSize(256,256));
            //设置QListWidget中单元项的间距
            pList->setSpacing(10);
            //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
            pList->setResizeMode(QListWidget::Adjust);
            //设置不能移动
            pList->setMovement(QListWidget::Static);
            pList->setWordWrap(true);
        }
        ui->progressBar_VarAna->setRange(0,10);
        ui->progressBar_VarAna->setValue(0);
        m_thCheck.m_pbThread=&m_bThread;
        m_thCheck.m_pVamStat->m_pbThread=&m_bThread;
        initGameList();
        initQuery();
    }
}

void MainWindow::initQuery()
{
    QStringList typeTxt;
    typeTxt<<" "<<"Scene"<<"Asset"<<"Clothing"<<"Hair"<<"Look"<<"Pose"<<"Script"<<"Texture";
    int i;
    for(i=0;i<typeTxt.count();i++)
    {
        ui->comboBox_QType->addItem(typeTxt[i],i);
    }
    ui->comboBox_QType->setCurrentIndex(0);
    QStringList genderTxt;
    genderTxt<<" "<<"Female"<<"Male";
    for(i=0;i<genderTxt.count();i++)
    {
        ui->comboBox_QGender->addItem(genderTxt[i],i);
    }
    ui->comboBox_QGender->setCurrentIndex(0);
}
void MainWindow::on_pushButton_ScanToDB_clicked()
{
    if(m_bThread)
    {
        QMessageBox::information(this,"Info","Thread Running, press Stop to end it");
        return;
    }
    if(QMessageBox::question(this,"Question","Scan To DB Will Spend Some Time (But You Can Terminate It as You Want), Do You Want DO THIS?")!=QMessageBox::Yes)
        return;
    m_thCheck.m_nRunType=RUN_STAT;
    m_thCheck.start();
}


void MainWindow::on_pushButton_InitDB_clicked()
{
    DBVam db;
    db.openDB();
    db.createTable();
    db.closeDB();
    return;
    QString newfn;
    m_zip.ExtractAFile("F:/Ent/Virt A Mate/AddonPackages/Oronan.Juri.1.var","Saves/scene/Oronan/Oronan_Juri.jpg","",newfn);
    m_zip.WaitFinished();
    qDebug()<<newfn;
    return;
    m_zip.PackFileList("F:/Ent/Virt A Mate/AddonPackages/AcidBubbles.ColliderEditor.29.var");
//    qDebug()<<"======";
    QVector<FILE_IN_VAR> flist;
    m_zip.GetFileList(flist,"");
//    qDebug()<<"total:"<<flist.count();
//    qDebug()<<flist[0];
}

void MainWindow::settings(bool bLoad)
{
    m_settFile=QCoreApplication::applicationDirPath()+"/set.ini";
    QSettings sett(m_settFile,QSettings::IniFormat);
    if(bLoad)
    {

    }
    else {

    }
}

void MainWindow::on_pushButton_Stop_clicked()
{
    m_bThread=false;
}

void MainWindow::on_Progress(int nTotal, int nCur)
{
    if(nCur==0)
    {
        ui->progressBar_VarAna->setRange(0,nTotal-1);
    }
    ui->progressBar_VarAna->setValue(nCur);
    QString msg;
    msg.sprintf("%d/%d",nCur+1,nTotal);
    ui->lineEdit_Ind->setText(msg);
}

void MainWindow::on_pushButton_InsertGame_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this);
    if(path=="")return;
    qDebug()<<path;
    DBVam db;
    db.openDB();
    db.setTable("vam_game");
    db.setValue("gamepath","'"+path+"'");
    QString sql;
    db.getSQLString(sql,GET_INSERT);
    db.exec(sql);
    db.closeDB();
    initGameList();
}

void MainWindow::on_pushButton_ExecSQL_clicked()
{
    DBVam db;
    db.openDB();
    QString sql=ui->textEdit_SQL->toPlainText();
    db.exec(sql);
    qDebug()<<sql;
    db.closeDB();
}

void MainWindow::initGameList()
{
    ui->comboBox_Game->clear();
    DBVam db;
    db.openDB();
    if(db.query("select * from vam_game"))
    {
        while(db.query_movenext())
        {
            ui->comboBox_Game->addItem(db.query_value(1).toString(), db.query_value(0).toInt());
        }
    }
    db.closeDB();
    m_bGameComboOK=true;
    on_pushButton_Open_clicked();
}

void MainWindow::on_pushButton_Open_clicked()
{
    QString vamgameid=ui->comboBox_Game->currentData().toString();
    DBVam db;
    db.openDB();
    QString sql="select gamepath from vam_game where id="+vamgameid;
    qDebug()<<sql;
    if(!db.query(sql))
    {
        QMessageBox::information(this,"info","select game error, perhaps need 'database table' and 'insert game'!");
    }
    else {
        if(db.query_movenext())
        {
            m_thCheck.m_pVamStat->m_sGamePath=db.query_value(0).toString();
            m_thCheck.m_pVamStat->m_nGameId=vamgameid.toInt();
            m_thCheck.m_pVamStat->m_sDataPath=m_thCheck.m_pVamStat->m_sWorkPath+"/"+vamgameid;
            qDebug()<<m_thCheck.m_pVamStat->m_sGamePath;
            qDebug()<<m_thCheck.m_pVamStat->m_sDataPath;
            qDebug()<<m_thCheck.m_pVamStat->m_nGameId;
        }
    }
    sql="select count(*) from vam_item where vamgameid="+vamgameid;
    if(db.query(sql))
    {
        db.query_movenext();
        qDebug()<<"record count: "<<db.query_value(0);
    }
    db.closeDB();

}

void MainWindow::on_fileCounter(int fc)
{
    ui->lineEdit_fileCounter->setText(QString::number(fc));
}

void MainWindow::on_pushButton_ExecQuery_clicked()
{
    ui->listWidget_Cont->clear();
    m_thCheck.m_sQuerySql=ui->textEdit_SQL->toPlainText();
    m_thCheck.m_nRunType=RUN_QUERY_LIST;
    m_thCheck.start();
}

void MainWindow::on_pushButton_Query_clicked()
{
    DBVam db;
    db.setTable("vam_item");
    int nType=ui->comboBox_QType->currentData().toInt();
    if(nType>0)
    {
        db.addWhere("vamtype="+QString::number(nType),true);
    }
    int nGender=ui->comboBox_QGender->currentData().toInt();
    if(nGender>0)
    {
        db.addWhere("gender="+QString((nGender==1?"0":"1")),true);
    }
    QString name=ui->lineEdit_QName->text();
    if(name!="")
    {
        if(name.indexOf('*')<0)
            db.addWhere("name='"+name+"' COLLATE NOCASE",true);
        else {
            name.replace('*','%');
            db.addWhere("name like '"+name+"' COLLATE NOCASE",true);
        }
    }
    QString author=ui->lineEdit_QAuthor->text();
    if(author!="")
    {
        if(author.indexOf('*')<0)
            db.addWhere("author='"+author+"' COLLATE NOCASE",true);
        else {
            author.replace('*','%');
            db.addWhere("author like '"+author+"' COLLATE NOCASE",true);
        }
    }
    QString count=ui->lineEdit_QCount->text();
    if(count!="")
    {
        QString offset=ui->lineEdit_QOffset->text();
        if(offset!="")
        {
            db.addLimit(count.toInt(),offset.toInt());
        }
        else {
            db.addLimit(count.toInt(),-1);
        }
    }
    db.addWhere("vamgameid="+QString::number(m_thCheck.m_pVamStat->m_nGameId),true);
    QString sql;
    db.getSQLString(sql, GET_SELECT);
    ui->textEdit_SQL->setText(sql);
    on_pushButton_GenRecCount_clicked();
    int nRecCount = ui->lineEdit_RecCount->text().toInt();
    int nWantCount = ui->lineEdit_QCount->text().toInt();
    if(nWantCount>0&&nWantCount<nRecCount)
        nRecCount=nWantCount;
    if(nRecCount>5000)
    {
        if(QMessageBox::question(this, "Query", "Record Count over 5000, Continue?")!=QMessageBox::Yes)
            return;
    }
    on_pushButton_ExecQuery_clicked();
}

void MainWindow::on_pushButton_GenRecCount_clicked()
{
    QString sql=ui->textEdit_SQL->toPlainText();
    bool bCheckOK=false;
    if(sql!="")
    {
        int n;
        n=sql.indexOf("from",Qt::CaseInsensitive);
        if(n>=0)
        {
            QString sql2;
            sql2="select count(*) "+sql.mid(n);
            DBVam db;
            db.openDB();
            if(db.query(sql2))
            {
                if(db.query_movenext())
                {
                    bCheckOK=true;
                    ui->lineEdit_RecCount->setText(db.query_value(0).toString());
                }
            }
            db.closeDB();
        }
    }
    if(!bCheckOK)
        ui->lineEdit_RecCount->setText("0");

}

void MainWindow::on_pushButton_CheckVarValid_clicked()
{
    if(m_bThread)
    {
        QMessageBox::information(this,"Info","Thread Running, press Stop to end it");
        return;
    }
    ui->listWidget_CheckVar->clear();
    m_thCheck.m_nRunType=RUN_VAR_VALID;
    m_thCheck.start();
}

void MainWindow::on_pushButton_CheckVarSame_clicked()
{
    if(m_bThread)
    {
        QMessageBox::information(this,"Info","Thread Running, press Stop to end it");
        return;
    }
    ui->listWidget_CheckVar->clear();
    m_thCheck.m_nRunType=RUN_VAR_SAME;
    m_thCheck.start();
}

void MainWindow::on_showToList(QListWidgetItem* aitem, int nList)
{
    if(nList==0)
        ui->listWidget_Cont->addItem(aitem);
    else if(nList==1)
        ui->listWidget_VarContent->addItem(aitem);
    else if(nList==2)
        ui->listWidget_CheckVar->addItem(aitem);
}


void MainWindow::on_listWidget_Cont_itemDoubleClicked(QListWidgetItem *item)
{
    QString fn=item->toolTip();
    fn.replace('/','\\');
    QProcess proc(this);
    proc.start("explorer /select," + fn);
    proc.waitForFinished();
}

void MainWindow::on_listWidget_Cont_itemSelectionChanged()
{
    QListWidgetItem* item=ui->listWidget_Cont->currentItem();
    ui->label_VarFullPath->setText(item->toolTip());
    m_thCheck.m_nRunType=RUN_ITEM_PIC;
    m_thCheck.m_nItemId=item->data(Qt::UserRole).toInt();
    m_thCheck.start();
}


void MainWindow::on_listWidget_VarContent_itemSelectionChanged()
{
    QListWidgetItem* item=ui->listWidget_VarContent->currentItem();
    ui->label_VarFullPath->setText(item->toolTip());
    m_thCheck.m_nRunType=RUN_ITEM_PIC;
    m_thCheck.m_nItemId=item->data(Qt::UserRole).toInt();
    m_thCheck.start();
}

void MainWindow::on_showPic(QString picPath)
{
    if(m_Img.load(picPath))
    {
        ui->label_Pic->setPixmap(QPixmap::fromImage(m_Img));
        ui->label_Pic->setScaledContents(true);
    }
}


void MainWindow::on_pushButton_QueryAtSameVar_clicked()
{
    ui->listWidget_VarContent->clear();
    QString ffp=ui->label_VarFullPath->text();
    if(ffp=="")return;
    int n=ffp.lastIndexOf('.');
    if(n<=0)return;
    QString ext=ffp.mid(n+1).toLower();
    if(ext=="var")
    {
        m_thCheck.m_nRunType=RUN_VIEW_IN_VAR;
        m_thCheck.m_sVarFullFilePath=ffp;
        m_thCheck.start();
    }
}


void MainWindow::on_pushButton_QueryAtSameVar_selVarFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("open a file."),
            m_thCheck.m_pVamStat->m_sGamePath+"/Addonpackages/",
            tr("Var Files(*.var)"));
    if (!fileName.isEmpty()) {
        ui->label_VarFullPath->setText(fileName);
    }
}

void MainWindow::on_listWidget_CheckVar_itemSelectionChanged()
{
    ui->label_VarFullPath->setText(ui->listWidget_CheckVar->currentItem()->toolTip());
    on_pushButton_QueryAtSameVar_clicked();
}

void MainWindow::on_listWidget_CheckVar_itemDoubleClicked(QListWidgetItem *item)
{
    QString fn=item->toolTip();
    fn.replace('/','\\');
    QProcess proc(this);
    proc.start("explorer /select," + fn);
    proc.waitForFinished();
}
