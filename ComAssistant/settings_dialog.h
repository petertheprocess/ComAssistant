#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QString>
#include <QSerialPort>
#include <QtDebug>

namespace Ui {
class settings_dialog;
}

class settings_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit settings_dialog(QWidget *parent = nullptr);
    ~settings_dialog();
    QSerialPort::StopBits getStopBits(); //返回停止位
    QSerialPort::DataBits getDataBits(); //返回数据位
    QSerialPort::Parity getParity(); //返回校验
    QSerialPort::FlowControl getFlowControl(); //返回流控
    void setStopBits(QSerialPort::StopBits stopbits);
    void setDataBits(QSerialPort::DataBits databits);
    void setParity(QSerialPort::Parity parity);
    void setFlowControl(QSerialPort::FlowControl flowcontrol);
    bool clickedOK(){
        return ok;
    }
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::settings_dialog *ui;
    bool ok = false;
};

#endif // SETTINGS_DIALOG_H
