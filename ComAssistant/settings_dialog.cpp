#include "settings_dialog.h"
#include "ui_settings_dialog.h"

settings_dialog::settings_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings_dialog)
{
    ui->setupUi(this);
}

settings_dialog::~settings_dialog()
{
    delete ui;
}

QSerialPort::StopBits settings_dialog::getStopBits()
{
    if("1" == ui->comboBox_stopBit->currentText())
        return QSerialPort::OneStop;
    else if("1.5" == ui->comboBox_stopBit->currentText())
        return QSerialPort::OneAndHalfStop;
    else if("2" == ui->comboBox_stopBit->currentText())
        return QSerialPort::TwoStop;
    else
        return QSerialPort::OneStop;
}

QSerialPort::DataBits settings_dialog::getDataBits()
{
    if("5" == ui->comboBox_dataBit->currentText())
        return QSerialPort::Data5;
    else if("6" == ui->comboBox_dataBit->currentText())
        return QSerialPort::Data6;
    else if("7" == ui->comboBox_dataBit->currentText())
        return QSerialPort::Data7;
    else if("8" == ui->comboBox_dataBit->currentText())
        return QSerialPort::Data8;
    else
        return QSerialPort::Data8;
}

QSerialPort::Parity settings_dialog::getParity()
{
    if("None" == ui->comboBox_parity->currentText())
        return QSerialPort::NoParity;
    else if("Odd" == ui->comboBox_parity->currentText())
        return QSerialPort::OddParity;
    else if("Even" == ui->comboBox_parity->currentText())
        return QSerialPort::EvenParity;
    else if("Mark" == ui->comboBox_parity->currentText())
        return QSerialPort::MarkParity;
    else if("Space" == ui->comboBox_parity->currentText())
        return QSerialPort::SpaceParity;
    else
        return QSerialPort::NoParity;
}

QSerialPort::FlowControl settings_dialog::getFlowControl()
{
    if("None" == ui->comboBox_flowControl->currentText())
        return QSerialPort::NoFlowControl;
    else if("Odd" == ui->comboBox_flowControl->currentText())
        return QSerialPort::HardwareControl;
    else if("Even" == ui->comboBox_flowControl->currentText())
        return QSerialPort::SoftwareControl;
    else
        return QSerialPort::NoFlowControl;
}

void settings_dialog::setStopBits(QSerialPort::StopBits stopbits)
{
    if(stopbits == QSerialPort::OneStop)
        ui->comboBox_stopBit->setCurrentIndex(0);
    else if(stopbits == QSerialPort::OneAndHalfStop)
        ui->comboBox_stopBit->setCurrentIndex(1);
    else if(stopbits == QSerialPort::TwoStop)
        ui->comboBox_stopBit->setCurrentIndex(2);
    else
        ui->comboBox_stopBit->setCurrentIndex(0);
    qDebug()<<"stop"<<stopbits;
}

void settings_dialog::setDataBits(QSerialPort::DataBits databits)
{
    if(databits == QSerialPort::Data5)
        ui->comboBox_dataBit->setCurrentIndex(0);
    else if(databits == QSerialPort::Data6)
        ui->comboBox_dataBit->setCurrentIndex(1);
    else if(databits == QSerialPort::Data7)
        ui->comboBox_dataBit->setCurrentIndex(2);
    else if(databits == QSerialPort::Data8)
        ui->comboBox_dataBit->setCurrentIndex(3);
    else
        ui->comboBox_dataBit->setCurrentIndex(3);
    qDebug()<<"data"<<databits;
}

void settings_dialog::setParity(QSerialPort::Parity parity)
{
    if(parity == QSerialPort::NoParity)
        ui->comboBox_parity->setCurrentIndex(0);
    else if(parity == QSerialPort::OddParity)
        ui->comboBox_parity->setCurrentIndex(1);
    else if(parity == QSerialPort::EvenParity)
        ui->comboBox_parity->setCurrentIndex(2);
    else if(parity == QSerialPort::MarkParity)
        ui->comboBox_parity->setCurrentIndex(3);
    else if(parity == QSerialPort::SpaceParity)
        ui->comboBox_parity->setCurrentIndex(4);
    else
        ui->comboBox_parity->setCurrentIndex(0);
}

void settings_dialog::setFlowControl(QSerialPort::FlowControl flowcontrol)
{
    if(flowcontrol == QSerialPort::NoFlowControl)
        ui->comboBox_flowControl->setCurrentIndex(0);
    else if(flowcontrol == QSerialPort::HardwareControl)
        ui->comboBox_flowControl->setCurrentIndex(1);
    else if(flowcontrol == QSerialPort::SoftwareControl)
        ui->comboBox_flowControl->setCurrentIndex(2);
    else
        ui->comboBox_flowControl->setCurrentIndex(0);
}

void settings_dialog::on_buttonBox_accepted()
{
    this->close();
}

void settings_dialog::on_buttonBox_rejected()
{
    this->close();
}
