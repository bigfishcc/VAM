#ifndef ZIP_H
#define ZIP_H

#include <QObject>
#include <QProcess>
#include <QVector>
#include "def.h"

class Zip : public QObject
{
    Q_OBJECT
public:
    explicit Zip(QObject *parent = nullptr);
    void PackFileList(QString sPackFile);
    void ExtractAFile(QString sPackFile, QString sExtractFilePath, QString destFilePath, QString& newFilePath);
    QString m_s7Zip;
    QString m_sTempPath;
    void GetFileList(QVector<FILE_IN_VAR>& list,QString sPackFile);
    void WaitFinished();

signals:

public slots:
    void on_ProcessResult();

protected:
    QProcess* m_caller;
    QString m_sExeResult;
};

#endif // ZIP_H
