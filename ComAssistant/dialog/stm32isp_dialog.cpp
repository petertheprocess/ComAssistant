#include "stm32isp_dialog.h"
#include "ui_stm32isp_dialog.h"
#include "QLabel"

//捕获线程结束后的main返回值
void STM32ISP_Dialog::finished(int exitCode,QProcess::ExitStatus exitStatus)
{
    qDebug()<<"Return "<<exitCode;// 被调用程序的main返回的int
    qDebug()<<exitStatus;// QProcess::ExitStatus(NormalExit)
    process->close();
}

//捕获命令行是否有新的标准输出
void STM32ISP_Dialog::readyRead()
{
    QString str=QString::fromLocal8Bit(process->readAll());
    if(str.right(2)=="\r\n")str=str.remove(str.length()-2,2);
    if(str.startsWith("Process:"))
    {
        ui->progressBar->setValue(str.mid(8,str.indexOf('%')-8).toInt());
    }
    ui->browser->append(str);
    qDebug()<<str;// "hello it is ok!"
}


STM32ISP_Dialog::STM32ISP_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::STM32ISP_Dialog)
{
    ui->setupUi(this);

    //一些准备工作
    serial = new QSerialPort;
    ui->progressBar->setValue(0);
    QSerialPortInfo::availablePorts();
//    ui->fileaddr->setText(tr("G:/Desktop/Bootloader.hex"));
    ui->fileaddr->setText(tr("hex文件路径"));

    //搜寻可用串口
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort TmpSerial;
        TmpSerial.setPort(info);
        if(TmpSerial.open(QSerialPort::ReadWrite))
        {
            TmpSerial.close();
            ui->com->addItem(TmpSerial.portName()+"("+"空闲:"+info.description()+")");
        }
        else
        {
            TmpSerial.close();
            ui->com->addItem(TmpSerial.portName()+"("+"占用:"+info.description()+")");
        }

    }
    if(ui->com->count()==0)
    {
        qDebug("Serial port doesn't exist! Please check the cable");
    }

    //准备一个线程
    process = new QProcess;
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),SLOT(finished(int,QProcess::ExitStatus)));
    connect(process,SIGNAL(readyRead()),this,SLOT(readyRead()));

    ui->browser->setText(
                tr("点击refresh按钮刷新串口\r\n")+
                tr("点击OpenFile按钮选择你的hex程序\r\n")+
                tr("点击Download下载文件\r\n")+
                tr("点击GetChipInfo获取芯片信息\r\n")+
                tr("--------------------------------------------------\r\n")+
                tr("文件路径请避免空格和中文，本上位机未作特别处理，可能导致问题。\r\n")+
                tr("确保STM32已经进入ISP Bootloader(或者BT0被置高后并复位)。\r\n")+
                tr("本程序调用stm32isp.exe实现ISP升级。\r\n")
                );
    ui->browser->append("欢迎访问：<a href=\"https://shop490276933.taobao.com\">https://shop490276933.taobao.com</a>\r\n");

        QLabel *permanent1=new QLabel(this);
        permanent1->setText(tr("<a href=\"https://shop490276933.taobao.com\">淘宝店铺</a>"));
        permanent1->setOpenExternalLinks(true);//设置可以打开网站链接
        ui->statusBar->addPermanentWidget(permanent1);//显示永久信息
        ui->statusBar->show();
}

STM32ISP_Dialog::~STM32ISP_Dialog()
{
    serial->close();
    process->close();
    delete ui;

}

//打开文件
void STM32ISP_Dialog::on_openfile_clicked()
{
    QString fileName,tmpFileName;
    tmpFileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",  tr("hex(*.hex)"));
    if(tmpFileName!="")
    {
        fileName=tmpFileName;
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)==true)
        {
            QByteArray fileBuff = file.readAll();
            file.close();

            ui->fileaddr->setText(fileName);
            ui->browser->setText(fileBuff);
        }
        else
            file.close();
    }

}

//刷新串口
void STM32ISP_Dialog::on_refresh_clicked()
{
    if(process->isOpen())
    {
        qDebug()<<"There is process running.";
        return;
    }
    ui->com->clear();
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort TmpSerial;
        TmpSerial.setPort(info);
        if(TmpSerial.open(QSerialPort::ReadWrite))
        {
            TmpSerial.close();
            ui->com->addItem(TmpSerial.portName()+"("+"空闲:"+info.description()+")");
        }
        else
        {
            TmpSerial.close();
            ui->com->addItem(TmpSerial.portName()+"("+"占用:"+info.description()+")");
        }

    }
    if(ui->com->currentText().left(3)!="COM")
    {
        qDebug("Serial port doesn't exist! Please check the cable");
    }
}

//路径不允许有空格，否则要加双引号！
static QString CLIToolPath="stm32isp.exe";
//下载按钮
void STM32ISP_Dialog::on_download_clicked()
{
    if(process->isOpen())
    {
        qDebug()<<"There is process running.";
        return;
    }
    ui->browser->setText("");
    ui->progressBar->setValue(0);
    QString path;
    QString comPort="-com:";
    QString CMD="";

    //路径如果带空格必须以参数形式传递，以下方式无法正常启动
    //路径如果带中文也无法正常启动
    path="-path:"+ui->fileaddr->text();
    comPort="-com:"+ui->com->currentText().mid(0,ui->com->currentText().indexOf('('));
    CMD="-download";
    QStringList cmdLine;
    cmdLine.append("/c "+CLIToolPath+" "+path+" "+comPort+" "+CMD);
    qDebug()<<cmdLine;

    process->start("cmd",cmdLine);
}

//获取信息
void STM32ISP_Dialog::on_getchipinfo_clicked()
{
    if(process->isOpen())
    {
        qDebug()<<"There is process running.";
        return;
    }
    ui->browser->setText("");
    QString path;
    QString comPort="-com:";
    QString CMD="";

    path=ui->fileaddr->text();

    comPort="-com:"+ui->com->currentText().mid(0,ui->com->currentText().indexOf('('));
    CMD="-getinfo";
    QStringList cmdLine;
    cmdLine.append("/c "+CLIToolPath+" "+comPort+" "+CMD);
    qDebug()<<cmdLine;
    process->start("cmd",cmdLine);
}
