#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "cthreadcheck.h"
#include "zip.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:

private slots:
    void on_showInfo(QString);
    void on_Progress(int nTotal, int nCur);
    void on_fileCounter(int fc);
    void on_showToList(QListWidgetItem* aitem, int nList);
    void on_showPic(QString picPath);
    void on_pushButton_ScanToDB_clicked();
    void on_pushButton_InitDB_clicked();
    void on_pushButton_Stop_clicked();

    void on_pushButton_InsertGame_clicked();

    void on_pushButton_ExecSQL_clicked();

    void on_pushButton_Open_clicked();

    void on_pushButton_ExecQuery_clicked();

    void on_pushButton_Query_clicked();

    void on_pushButton_GenRecCount_clicked();

    void on_pushButton_CheckVarValid_clicked();

    void on_pushButton_CheckVarSame_clicked();

    void on_listWidget_Cont_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_Cont_itemSelectionChanged();

    void on_pushButton_QueryAtSameVar_clicked();

    void on_listWidget_VarContent_itemSelectionChanged();

    void on_pushButton_QueryAtSameVar_selVarFile_clicked();

    void on_listWidget_CheckVar_itemSelectionChanged();

    void on_listWidget_CheckVar_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    void initWindow(bool bInit);
    void initConnect();
    CThreadCheck m_thCheck;
    void settings(bool bLoad);
    Zip m_zip;
    bool m_bThread;
    QString m_settFile;
    void initGameList();
    void initQuery();
    QImage m_Img;
    bool m_bGameComboOK;

};

#endif // MAINWINDOW_H
