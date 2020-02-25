#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 * Function:读取配置
*/
void MainWindow::readConfig()
{
    //回车风格
    if(Config::getEnterStyle() == EnterStyle_e::WinStyle){
        ui->action_winLikeEnter->setChecked(true);
        ui->action_unixLikeEnter->setChecked(false);
    }else if (Config::getEnterStyle() == EnterStyle_e::UnixStyle) {
        ui->action_winLikeEnter->setChecked(false);
        ui->action_unixLikeEnter->setChecked(true);
    }else {
        ui->action_winLikeEnter->setChecked(true);
        ui->action_unixLikeEnter->setChecked(false);
        QMessageBox::warning(this, "警告", "未知的回车风格");
    }

    //编码规则
    if(Config::getCodeRule() == CodeRule_e::UTF8){
        ui->actionUTF8->setChecked(true);
    }else {
        ui->actionUTF8->setChecked(true);
        QMessageBox::warning(this, "警告", "未支持的编码格式");
    }
    //时间戳
    ui->timeStampDisplayCheckBox->setChecked(Config::getTimeStampState());
    //发送间隔
    ui->sendInterval->setText(QString::number(Config::getSendInterval()));
    //hex发送
    ui->hexSend->setChecked(Config::getHexSendState());
    //hex显示
    ui->hexDisplay->setChecked(Config::getHexShowState());
    //波特率
    ui->baudrateList->setCurrentText(QString::number(Config::getBaudrate()));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    connect(&continuousWriteTimer, SIGNAL(timeout()), this, SLOT(continuousWriteSlot()));
    connect(&serial, SIGNAL(readyRead()), this, SLOT(readSerialPort()));

    readConfig();

    //搜寻可用串口
    on_refreshCom_clicked();

    //显示收发统计
    serial.resetCnt();
    ui->statusBar->showMessage(serial.getTxRxString());

    //设置波特率框和发送间隔框的合法输入范围
    ui->baudrateList->setValidator(new QIntValidator(0,9999999,this));
    ui->sendInterval->setValidator(new QIntValidator(0,99999,this));

    //只存在一个串口时自动打开
    if(ui->comList->count()==1){
        ui->comSwitch->setChecked(true);
        on_comSwitch_clicked(true);
    }

}

MainWindow::~MainWindow()
{
    Config::setBaudrate(serial.baudRate());
    Config::setDataBits(serial.dataBits());
    Config::setStopBits(serial.stopBits());
    Config::setParity(serial.parity());
    Config::setFlowControl(serial.flowControl());

    if(ui->actionUTF8->isChecked()){
        Config::setCodeRule(CodeRule_e::UTF8);
    }
    if(ui->action_winLikeEnter->isChecked()){
        Config::setEnterStyle(EnterStyle_e::WinStyle);
    }else if(ui->action_unixLikeEnter->isChecked()){
        Config::setEnterStyle(EnterStyle_e::UnixStyle);
    }

    Config::setHexSendState(ui->hexSend->isChecked());
    Config::setHexShowState(ui->hexDisplay->isChecked());
    Config::setSendInterval(ui->sendInterval->text().toInt());
    Config::setTimeStampState(ui->timeStampDisplayCheckBox->isChecked());

    delete ui;
}

/*
 * Function:刷新串口按下
*/
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

/*
 * Function:清空接收区按钮按下，清空接收区
*/
void MainWindow::on_clearRecvAreaButton_clicked()
{
    ui->textBrowser->clear();
    RxBuff.clear();
    serial.resetRxCnt();

    //更新收发统计
    ui->statusBar->showMessage(serial.getTxRxString());
}

/*
 * Function:清空发送区按钮按下，清空发送区
*/
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
        if(serial.open(com,baud)){
            ui->comSwitch->setText("关闭串口");
            ui->refreshCom->setEnabled(false);
        }
        else {
            ui->comSwitch->setChecked(false);
            ui->refreshCom->setEnabled(true);
            QMessageBox::critical(this, "串口打开失败!", "该串口设备不存在或已被占用", QMessageBox::Ok);
        }
    }
    else
    {
        //关闭定时器
        if(continuousWriteTimer.isActive()){
            continuousWriteTimer.stop();
            ui->TimerSendCheck->setChecked(false);
        }

        serial.close();
        ui->comSwitch->setText("打开串口");
        ui->refreshCom->setEnabled(true);
    }
}

/*
 * Function:从串口读取数据
*/
void MainWindow::readSerialPort()
{
    QString timeString("");

    QByteArray readBuff;
    static QByteArray restBuff;

    //读取数据并衔接到上次未处理完的数据后面
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
    if(ui->timeStampDisplayCheckBox->isChecked()){
        timeString = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        timeString = "["+timeString+"]Rx<- ";
    }

    //打印数据
    if(!readBuff.isEmpty()){
        //移动光标
        ui->textBrowser->moveCursor(QTextCursor::End);
        //进制转换
        if(ui->hexDisplay->isChecked()){
            readBuff = toHexDisplay(readBuff).toUtf8();
        }
        //追加数据
        ui->textBrowser->insertPlainText(timeString + readBuff);
        if(!timeString.isEmpty())
            ui->textBrowser->insertPlainText("\r\n");

        //更新收发统计
        ui->statusBar->showMessage(serial.getTxRxString());
    }
}

/*
 * Function:连续发送定时器槽，执行数据发送
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

            //回车风格转换，win风格补上'\r'，默认unix风格
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

            //更新在显示区域
            QString timeString;
            if(ui->timeStampDisplayCheckBox->isChecked()){
                timeString = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
                timeString = "["+timeString+"]Tx-> ";
            }
            //若添加了时间戳则把发送的数据也显示在接收区
            if(!timeString.isEmpty()){
                //如果hex发送则把显示在接收区的发送数据转为hex模式
                if(ui->hexDisplay->isChecked())
                    tmp = toHexDisplay(tmp).toUtf8();
                ui->textBrowser->moveCursor(QTextCursor::End);
                ui->textBrowser->insertPlainText(timeString + tmp + "\r\n");
            }
        }
        else {
            //以hex发送数据
            //HexStringToByteArray函数必须传入格式化后的字符串，如"02 31"
            QByteArray tmp;
            bool ok;
            tmp = HexStringToByteArray(ui->textEdit->toPlainText(),ok);
            if(ok)
                serial.write(tmp);
//            QMessageBox::information(this,"提示","此功能尚未完成。");
        }
        //更新收发统计
        ui->statusBar->showMessage(serial.getTxRxString());
    }else {
        QMessageBox::information(this,"提示","串口未打开");
    }
}

void MainWindow::on_clearStatistic_clicked()
{
    serial.resetCnt();

    //更新收发统计
    ui->statusBar->showMessage(serial.getTxRxString());
}

void MainWindow::on_TimerSendCheck_clicked(bool checked)
{
    if(ui->sendInterval->text().toInt() < 15 && checked){
        QMessageBox::warning(this,"警告","发送间隔较小可能不够准确");
    }

    if(!serial.isOpen()){
        QMessageBox::information(this,"提示","串口未打开");
        ui->TimerSendCheck->setChecked(false);
        return;
    }

    //启停定时器
    if(checked){
        ui->sendInterval->setEnabled(false);
        continuousWriteTimer.start(ui->sendInterval->text().toInt());
    }
    else {
        continuousWriteTimer.stop();
        ui->sendInterval->setEnabled(true);
    }
}

/*
 * Event:发送区文本变化
 * Function:检查十六进制发送模式下的发送区文本内容是否非法
*/
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
    }
}

/*
 * Event:十六进制格式发送按钮状态变化
 * Function:保存当前发送区的文本内容
*/
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

/*
 * Event:十六进制显示按钮状态改变
 * Function:将当前接收框的内容转换为十六进制格式重新显示
*/
void MainWindow::on_hexDisplay_clicked(bool checked)
{
    QMessageBox::StandardButton res;
    //如果启用时间戳则在hex转string时要求删除时间戳信息。
    if(ui->timeStampDisplayCheckBox->isChecked() && checked == false){
        res = QMessageBox::question(this, "询问", "该操作可能会删除时间戳信息");
        if(QMessageBox::No == res){
            ui->hexDisplay->setChecked(true);
            return;
        }
    }
    if(checked){
        QString tmp = ui->textBrowser->toPlainText();
        ui->textBrowser->clear();
        ui->textBrowser->setText(toHexDisplay(tmp));
    }else {
        QString unconverted = ui->textBrowser->toPlainText();
        bool ok;
        QString converted;
        //删除时间戳信息。
        if(ui->timeStampDisplayCheckBox->isChecked()){
            unconverted.replace(QRegExp("\\[.*\\]Tx->"), "");
            unconverted.replace(QRegExp("\\[.*\\]Rx<-"), "");
        }
        //转换且转换成功才显示
        converted = toStringDisplay(unconverted,ok);
        ui->textBrowser->clear();
        if(ok)
            ui->textBrowser->setText(converted);
        else {
            ui->textBrowser->setText(unconverted);
        }
    }
    ui->textBrowser->moveCursor(QTextCursor::End);
}

/*
 * Action:激活使用win风格回车（\r\n）
 * Function:
*/
void MainWindow::on_action_winLikeEnter_triggered(bool checked)
{
    if(checked){
        ui->action_unixLikeEnter->setChecked(false);

    }else {
        ui->action_winLikeEnter->setChecked(false);
    }
}

/*
 * Action:激活使用unix风格回车（\n）
 * Function:
*/
void MainWindow::on_action_unixLikeEnter_triggered(bool checked)
{
    if(checked){
        ui->action_winLikeEnter->setChecked(false);
    }else {
        ui->action_unixLikeEnter->setChecked(false);
    }
}


/*
 * Action:激活使用UTF8编码
 * Function:暂未支持其他格式编码
*/
void MainWindow::on_actionUTF8_triggered(bool checked)
{
    if(!checked){
        ui->actionUTF8->setChecked(!checked);
    }
}

/*
 * Action:保存数据动作触发
 * Function:
*/
void MainWindow::on_actionSaveData_triggered()
{
    //如果追加时间戳则提示时间戳不会被保存
    if(ui->timeStampDisplayCheckBox->isChecked())
        QMessageBox::information(this,"提示","时间戳数据不会被保存！只保存接收到的原始数据。");

    //打开保存文件对话框
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "保存数据-选择文件路径",
                                                    QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".txt",
                                                    "Text File(*.txt);;All File(*.*)");
    //检查路径格式
    if(!savePath.endsWith("txt")){
        QMessageBox::information(this,"尚未支持的文件格式","请选择txt文本文件。");
        return;
    }

    //保存数据
    QFile file(savePath);
    QTextStream stream(&file);
    //删除旧数据形式写文件
    if(file.open(QFile::WriteOnly|QFile::Text|QFile::Truncate)){
        stream<<RxBuff;
        file.close();
    }
}

/*
 * Action:读取数据动作触发
 * Function:
*/
void MainWindow::on_actionReadData_triggered()
{   
    //打开文件对话框
    QString readPath = QFileDialog::getOpenFileName(this,
                                                    "读取数据-选择文件路径",
                                                    "",
                                                    "Text File(*.txt);;All File(*.*)");
    //检查文件路径结尾
    if(!readPath.endsWith("txt")){
        QMessageBox::information(this,"尚未支持的文件格式","请选择txt文本文件。");
        return;
    }

    //读取文件
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

/*
 * Action:触发“关于”按钮
 * Function:弹出关于对话框
*/
void MainWindow::on_actionAbout_triggered()
{
    //创建关于我对话框资源
    About_Me_Dialog* p = new About_Me_Dialog;
    //设置close后自动销毁
    p->setAttribute(Qt::WA_DeleteOnClose);
    //非阻塞式显示
    p->show();
}

void MainWindow::on_actionCOM_Config_triggered()
{
    //创建串口设置对话框
    settings_dialog* p = new settings_dialog(this);
    //对话框读取原配置
    p->setStopBits(serial.stopBits());
    p->setDataBits(serial.dataBits());
    p->setParity(serial.parity());
    p->setFlowControl(serial.flowControl());
    p->exec();
    //对话框返回新配置并设置
    if(!serial.moreSetting(p->getStopBits(),p->getParity(),p->getFlowControl(),p->getDataBits()))
        QMessageBox::information(this,"提示","串口设置失败");
    delete p;
}

/*
 * Function:波特率框文本变化，检查输入合法性并重置波特率
*/
void MainWindow::on_baudrateList_currentTextChanged(const QString &arg1)
{
    bool ok;
    int baud = arg1.toInt(&ok);
    if(ok){
        serial.setBaudRate(baud);
    }
    else {
        QMessageBox::information(this,"提示","请输入合法波特率");
    }
}

/*
 * Function:端口号文本变化，重新打开串口
*/
void MainWindow::on_comList_currentTextChanged(const QString &arg1)
{
    QString unused = arg1;//屏蔽警告
    //重新打开串口
    if(serial.isOpen()){
        on_comSwitch_clicked(false);
        on_comSwitch_clicked(true);
    }
}


void MainWindow::on_actionSaveShowedData_triggered()
{
    //打开保存文件对话框
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "保存数据-选择文件路径",
                                                    QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".txt",
                                                    "Text File(*.txt);;All File(*.*)");
    //检查路径格式
    if(!savePath.endsWith("txt")){
        QMessageBox::information(this,"尚未支持的文件格式","请选择txt文本文件。");
        return;
    }

    //保存数据
    QFile file(savePath);
    QTextStream stream(&file);
    //删除旧数据形式写文件
    if(file.open(QFile::WriteOnly|QFile::Text|QFile::Truncate)){
        stream<<ui->textBrowser->toPlainText();
        file.close();
    }
}
