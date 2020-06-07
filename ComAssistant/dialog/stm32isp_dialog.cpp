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
                tr("- 【确保STM32可正确进入ISP模式\r\n")+
                tr("- 【如开发板有自动下载电路，可勾选“使用自动下载电路”进入ISP模式】\r\n")+
                tr("- 【如开发板无自动下载电路，可拉高BOOT0后复位单片机进入ISP模式，也可通过以下函数进行软件跳转】\r\n")+
                tr("- 【仅支持通过DTR低电平复位，RTS高电平控制自动下载电路进ISP模式】\r\n")+
                tr("- 【【下载程序可能会被报毒，请对stm32isp.exe进行放行】】\r\n")
                );
    ui->browser->append("STM32F1/F4系列可使用以下函数跳转至ISP Bootloader：\n");
    ui->browser->append("<pre style=\"background-color: rgb(235, 235, 235);\">"
                        "    void JumpToISP(void)\n"
                        "    {\n"
                        "        //根据芯片修改地址,参阅“芯片参考手册——Flash——系统存储区”部分\n"
                        "        //stm32f10x大中小型芯片均为0x1FFFF000\n"
                        "        //stm32f1系列的互联型芯片为0x1FFFB000\n"
                        "        //stm32f40x为0x1FFF0000\n"
                        "        //stm32f41x为0x1FFF0000\n"
                        "        #define ISPAddress 0x1FFFF000 \n"
                        "\n"
                        "        //程序跳转不会复位外设，建议对一些可能导致问题的外设进行反初始化DeInit，如SysTick。\n"
                        "        //看门狗一旦初始化不能被关闭，因此如果使用了看门狗，则在调用该函数前，必须先复位系统，并在初始化看门狗之前调用该函数。\n"
                        "        uint32_t ispJumpAddr;//ISP程序的跳转地址，既程序入口\n"
                        "        uint32_t ispSpValue;//ISP程序的SP初值，即栈指针\n"
                        "        void (*Jump_To_ISP)(void);//定义一个函数指针\n"
                        "\n"
                        "        printf(\"\\r\\nJumping to ISP...\");\n"
                        "        SysTick->CTRL=0x00;       //关闭计数器\n"
                        "        SysTick->VAL =0X00;       //清空计数器\n"
                        "\n"
                        "        if (((*(__IO uint32_t*)ISPAddress) & 0x2FFE0000 ) == 0x20000000)//SP->RAM,RAM => 0x20000000\n"
                        "        { \n"
                        "            ispSpValue  = *(__IO uint32_t*)ISPAddress;\n"
                        "            ispJumpAddr = *(__IO uint32_t*)(ISPAddress+4);\n"
                        "\n"
                        "            /* 初始化 Stack Pointer */\n"
                        "            __set_MSP(ispSpValue);\n"
                        "\n"
                        "            /* Jump to isp */\n"
                        "            Jump_To_ISP = (void (*)(void))ispJumpAddr;\n"
                        "            Jump_To_ISP();\n"
                        "        }\n"
                        "    }\n"
                        "</pre>");
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

            //把路径格式改成cmd支持的格式，并在前后加双引号
            while (fileName.indexOf('/')!=-1) {
                fileName.replace('/','\\');
            }
            if(fileName.startsWith('"')==false)
                fileName.push_front('"');
            if(fileName.endsWith('"')==false)
                fileName.push_back('"');

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
        ui->browser->append("检测到stm32isp正在运行，已停止该进程。");
        process->close();
//        return;
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
        ui->browser->append("检测到stm32isp正在运行，已停止该进程。");
        process->close();
//        return;
    }
    ui->browser->setText("");
    ui->progressBar->setValue(0);
    QString path;
    QString comPort="-com:";
    QString CMD="";

    //准备构建需要传入的参数
    path="-path:"+ui->fileaddr->text();
    comPort="-com:"+ui->com->currentText().mid(0,ui->com->currentText().indexOf('('));
    CMD="-download";
    QStringList cmdLine;
    cmdLine<<path<<comPort<<CMD;
    if(ui->useAutoDownloadCircuit->isChecked())
        cmdLine<<"-adc";
    if(ui->eraseAll->isChecked())
        cmdLine<<"-true";
    qDebug()<<cmdLine;

    //调用命令行程序
    process->setProgram(CLIToolPath);
    process->setArguments(cmdLine);
    process->start();
}

//获取信息
void STM32ISP_Dialog::on_getchipinfo_clicked()
{
    if(process->isOpen())
    {
        qDebug()<<"There is process running.";
        ui->browser->append("检测到stm32isp正在运行，已停止该进程。");
        process->close();
//        return;
    }
    ui->browser->setText("");
    QString comPort="-com:";
    QString CMD="";

    comPort="-com:"+ui->com->currentText().mid(0,ui->com->currentText().indexOf('('));
    CMD="-getinfo";

    QStringList cmdLine;
    cmdLine<<comPort<<CMD;
    if(ui->useAutoDownloadCircuit->isChecked())
        cmdLine<<"-adc";
    qDebug()<<cmdLine;

    //调用命令行程序
    process->setProgram(CLIToolPath);
    process->setArguments(cmdLine);
    process->start();
}
