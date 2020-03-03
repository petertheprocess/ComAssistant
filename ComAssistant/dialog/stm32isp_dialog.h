#ifndef STM32ISP_DIALOG_H
#define STM32ISP_DIALOG_H

#include <QDialog>
#include <QtCore/QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtDebug>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

namespace Ui {
class STM32ISP_Dialog;
}

class STM32ISP_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit STM32ISP_Dialog(QWidget *parent = nullptr);
    ~STM32ISP_Dialog();
    QSerialPort *serial;
    QProcess *process;

private slots:
    void on_openfile_clicked();
    void on_refresh_clicked();
    void on_getchipinfo_clicked();
    void on_download_clicked();
    void finished(int exitCode,QProcess::ExitStatus exitStatus);
    void readyRead();
//    void readyReadStandardOutput();

private:
    Ui::STM32ISP_Dialog *ui;

};

#endif // STM32ISP_DIALOG_H
