#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial()    //初始化串口对象
{
    ui->setupUi(this);

    connect(&continuousWriteTimer, SIGNAL(timeout()), this, SLOT(continuousWriteSlot()));
    connect(&serial, SIGNAL(readyRead()), this, SLOT(readSerialPort()));

    //搜寻可用串口
    on_refreshCom_clicked();

    //显示收发统计
    serial.resetCnt();
    ui->statusBar->showMessage(serial.getTxRxString());

    //设置回车风格
    ui->action_winLikeEnter->setChecked(true);

    //设置编码格式
    ui->actionUTF8->setChecked(true);

    //设置默认波特率
    ui->baudrateList->setCurrentText("115200");

    //只存在一个串口时自动打开
    if(ui->comList->count()==1){
        ui->comSwitch->setChecked(true);
        on_comSwitch_clicked(true);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_refreshCom_clicked()
{
    QList<QString> tmp;
    //清空下拉列表
    ui->comList->clear();

    //搜索串口
    tmp = serial.refreshSerialPort();
    foreach(const QString &info, tmp)
    {
        ui->comList->addItem(info);
    }

    if(ui->comList->count() == 0)
        ui->comList->addItem("未找到可用串口!");
}

void MainWindow::on_clearRecvAreaButton_clicked()
{
    ui->textBrowser->clear();
    RxBuff.clear();
    serial.resetRxCnt();

    //更新收发统计
    ui->statusBar->showMessage(serial.getTxRxString());
}

void MainWindow::on_clearSendAreaButton_clicked()
{
    ui->textEdit->clear();
    serial.resetTxCnt();
    //更新收发统计
    ui->statusBar->showMessage(serial.getTxRxString());
}

//串口开关
void MainWindow::on_comSwitch_clicked(bool checked)
{
    QString com = ui->comList->currentText().mid(0,ui->comList->currentText().indexOf('('));
    int baud = ui->baudrateList->currentText().toInt();

    if(checked)
    {
        if(serial.open(com,baud))
            ui->comSwitch->setText("关闭串口");
        else {
            ui->comSwitch->setChecked(false);
            QMessageBox::critical(this, "串口打开失败!", "该串口设备不存在或已被占用", QMessageBox::Ok);
        }
    }
    else
    {
        //关闭定时器
        if(continuousWriteTimer.isActive())
            continuousWriteTimer.stop();

        serial.close();
        ui->comSwitch->setText("打开串口");
    }
}

/*
 * Function:读取数据
*/
void MainWindow::readSerialPort()
{
    QString timeString("");

    QByteArray readBuff;
    static QByteArray restBuff;

    readBuff = serial.readAll();
    readBuff = restBuff + readBuff;
    restBuff.clear();
    RxBuff.append(readBuff);

    //'\r'不允许单独出现
    if(readBuff.at(readBuff.size()-1) == '\r'){
        restBuff.append(readBuff.at(readBuff.size()-1));
        readBuff.remove(readBuff.size()-1,1);
    }
    //寻找中文等拓展字符,主要是数据要成组（3个char对应一个中文），因此对不是3的倍数的readBuff的末尾数据进行处理
    if(readBuff.size()%3!=0){
        //余1和余2
        if(readBuff.size()%3==1){
            if(readBuff.at(readBuff.size()-1)&0x80){
                restBuff.append(readBuff.at(readBuff.size()-1));
                readBuff.remove(readBuff.size()-1,1);
            }
        }
        if(readBuff.size()%3==2){
            if(readBuff.at(readBuff.size()-2)&0x80){
                restBuff.append(readBuff.mid(readBuff.size()-2));
                readBuff.remove(readBuff.size()-2,2);
            }
        }
    }

    //时间戳变量
    if(ui->timeStampDisplayCheckBox->isChecked())
        timeString = QDateTime::currentDateTime().toString("hh:mm::ss.zzz "); //补空格美观

    if(!readBuff.isEmpty()){
        //移动光标
        ui->textBrowser->moveCursor(QTextCursor::End);
        //追加数据
        ui->textBrowser->insertPlainText(timeString + readBuff);
        if(!timeString.isEmpty())
            ui->textBrowser->insertPlainText("\r\n");

        //更新收发统计
        ui->statusBar->showMessage(serial.getTxRxString());
    }
}

/*
 * Function:连续发送定时器槽
*/
void MainWindow::continuousWriteSlot()
{
    on_sendButton_clicked();
}

/*
 * Function:发送数据
*/
void MainWindow::on_sendButton_clicked()
{
    if(serial.isOpen()){
        //十六进制检查
        if(!ui->hexSend->isChecked()){

            //回车风格转换
            QByteArray tmp = ui->textEdit->toPlainText().toUtf8();
            if(ui->action_winLikeEnter->isChecked()){
                //win风格
                while (tmp.indexOf('\n') != -1) {
                    tmp = tmp.replace('\n', '\t');
                }
                while (tmp.indexOf('\t') != -1) {
                    tmp = tmp.replace('\t', "\r\n");
                }
            }
            else{
                //unix风格
                while (tmp.indexOf('\r') != -1) {
                    tmp = tmp.remove(tmp.indexOf('\r'),1);
                }

            }

            //utf8编码
            serial.write(tmp);
//            qDebug()<<"T:" + (tmp);
        }
        else {
            QMessageBox::information(this,"提示","此功能尚未完成。");
        }
        //更新收发统计
        ui->statusBar->showMessage(serial.getTxRxString());
    }
}

void MainWindow::on_clearStatistic_clicked()
{
    serial.resetCnt();

    //更新收发统计
    ui->statusBar->showMessage(serial.getTxRxString());
}

void MainWindow::on_TimerSendCheck_stateChanged(int arg1)
{
    //屏蔽警告
    arg1 = 0;

    if(ui->TimerSendCheck->isChecked()){
        ui->sendInterval->setEnabled(false);
        continuousWriteTimer.start(ui->sendInterval->text().toInt());
    }
    else {
        continuousWriteTimer.stop();
        ui->sendInterval->setEnabled(true);
    }
}

void MainWindow::on_textEdit_textChanged()
{
    //十六进制发送下的输入格式检查
    static QString lastText;
    if(ui->hexSend->isChecked()){
        if(!hexFormatCheck(ui->textEdit->toPlainText())){
            QMessageBox::warning(this, "警告", "存在非法的十六进制格式。");
            ui->textEdit->clear();
            ui->textEdit->insertPlainText(lastText);
            return;
        }
        //不能记录非空数据，因为clear操作也会触发本事件
        if(!ui->textEdit->toPlainText().isEmpty())
            lastText = ui->textEdit->toPlainText();
        ui->textBrowser->append(hexFormat(ui->textEdit->toPlainText()));
    }
}

void MainWindow::on_hexSend_stateChanged(int arg1)
{
    arg1 = 0;
    static QString lastAsciiText, lastHexText;
    if(ui->hexSend->isChecked()){
        lastAsciiText = ui->textEdit->toPlainText();
        ui->textEdit->clear();
        ui->textEdit->insertPlainText(lastHexText);
    }
    else{
        lastHexText = ui->textEdit->toPlainText();
        ui->textEdit->clear();
        ui->textEdit->insertPlainText(lastAsciiText);
    }
}

void MainWindow::on_hexDisplay_stateChanged(int arg1)
{
    if(ui->hexDisplay->isChecked())
        ui->textBrowser->setText(toHexDisplay(ui->textBrowser->toPlainText()));
}

void MainWindow::on_action_winLikeEnter_triggered(bool checked)
{
    if(checked)
        ui->action_unixLikeEnter->setChecked(false);
}

void MainWindow::on_action_unixLikeEnter_triggered(bool checked)
{
    if(checked)
        ui->action_winLikeEnter->setChecked(false);
}

void MainWindow::on_actionUTF8_triggered(bool checked)
{
    if(!checked)
        ui->actionUTF8->setChecked(!checked);
}

void MainWindow::on_actionSaveData_triggered()
{

    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "保存数据-选择文件路径",
                                                    QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".txt",
                                                    "Text File(*.txt);;All File(*.*)");

    if(!savePath.endsWith("txt")){
        QMessageBox::information(this,"尚未支持的文件格式","请选择txt文本文件。");
        return;
    }

    QFile file(savePath);
    QTextStream stream(&file);
    //删除旧数据形式写文件
    if(file.open(QFile::WriteOnly|QFile::Text|QFile::Truncate)){
        stream<<ui->textBrowser->toPlainText();
        file.close();
    }
}

void MainWindow::on_actionReadData_triggered()
{
    QString readPath = QFileDialog::getOpenFileName(this,
                                                    "读取数据-选择文件路径",
                                                    QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".txt",
                                                    "Text File(*.txt);;All File(*.*)");
    if(!readPath.endsWith("txt")){
        QMessageBox::information(this,"尚未支持的文件格式","请选择txt文本文件。");
        return;
    }

    QFile file(readPath);
    QTextStream stream(&file);
    //读文件
    if(file.open(QFile::ReadOnly|QFile::Text)){
        QMessageBox::information(this,"开发者提示","读取的文件可能缺少 '\\r' 字符");
        RxBuff.clear();
        RxBuff.append(stream.readAll());
        ui->textBrowser->setText(RxBuff);
        file.close();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    aboutMe.show();
}
