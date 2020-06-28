#include "mainwindow.h"
#include "ui_mainwindow.h"

static QColor      g_background_color;
static QFont       g_font;
static bool        g_enableSumCheck;
static qint64      g_lastSecsSinceEpoch;
/*
 * Function:读取配置
*/
void MainWindow::readConfig()
{
    //先写入版本号和启动时间
    Config::setVersion();
    Config::setStartTime(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));

    //回车风格
    if(Config::getEnterStyle() == EnterStyle_e::WinStyle){
        enter = "\r\n";
        ui->action_winLikeEnter->setChecked(true);
        ui->action_unixLikeEnter->setChecked(false);
    }else if (Config::getEnterStyle() == EnterStyle_e::UnixStyle) {
        enter = "\n";
        ui->action_winLikeEnter->setChecked(false);
        ui->action_unixLikeEnter->setChecked(true);
    }else {
        ui->action_winLikeEnter->setChecked(true);
        ui->action_unixLikeEnter->setChecked(false);
        QMessageBox::warning(this, "警告", "读取到未知的回车风格");
    }

    //编码规则
    if(Config::getCodeRule() == CodeRule_e::UTF8){
        on_actionUTF8_triggered(true);
    }else if(Config::getCodeRule() == CodeRule_e::GBK){
        on_actionGBK_triggered(true);
    }

    //多字符串
    ui->actionMultiString->setChecked(Config::getMultiStringState());
    on_actionMultiString_triggered(Config::getMultiStringState());
    QStringList multi;
    multi = Config::getMultiString();
    while(multi.size()>0){
        ui->multiString->addItem(multi.at(0));
        multi.pop_front();
    }

    //文件对话框路径
    lastFileDialogPath = Config::getLastFileDialogPath();
    //加载高亮规则
    ui->actionKeyWordHighlight->setChecked(Config::getKeyWordHighlightState());
    on_actionKeyWordHighlight_triggered(Config::getKeyWordHighlightState());

    //时间戳
    ui->timeStampCheckBox->setChecked(Config::getTimeStampState());
    ui->timeStampTimeOut->setText(QString::number(Config::getTimeStampTimeOut()));
    if(!ui->timeStampCheckBox->isChecked())
        ui->timeStampTimeOut->setEnabled(false);
    //发送间隔
    ui->sendInterval->setText(QString::number(Config::getSendInterval()));
    //hex发送
    ui->hexSend->setChecked(Config::getHexSendState());
    //hex显示
    ui->hexDisplay->setChecked(Config::getHexShowState());
    //波特率
    ui->baudrateList->setCurrentText(QString::number(Config::getBaudrate()));
    //文本发送区，不能放在hex发送前面
    ui->textEdit->setText(Config::getTextSendArea());
    ui->textEdit->moveCursor(QTextCursor::End);
    //背景色
    int r,g,b;
    QColor color = Config::getBackGroundColor();
    if(color.isValid()){
        g_background_color = color;
        g_background_color.getRgb(&r,&g,&b);
    }else{
        r=g=b=255;
        g_background_color.setRed(r);
        g_background_color.setGreen(g);
        g_background_color.setBlue(b);
    }
    QString str = "background-color: rgb(RGBR,RGBG,RGBB);";
    str.replace("RGBR", QString::number(r));
    str.replace("RGBG", QString::number(g));
    str.replace("RGBB", QString::number(b));
    ui->textBrowser->setStyleSheet(str);

    //绘图器开关
    ui->actionPlotterSwitch->setChecked(Config::getPlotterState());
    on_actionPlotterSwitch_triggered(Config::getPlotterState());

    //协议类型
    if(Config::getPlotterType()==ProtocolType_e::Ascii||
       Config::getPlotterType()==ProtocolType_e::Ascii_SumCheck){
        on_actionAscii_triggered(true);
        if(Config::getPlotterType()==ProtocolType_e::Ascii_SumCheck){
            on_actionSumCheck_triggered(true);
        }
    }
    else if(Config::getPlotterType()==ProtocolType_e::Float||
            Config::getPlotterType()==ProtocolType_e::Float_SumCheck){
        on_actionFloat_triggered(true);
        if(Config::getPlotterType()==ProtocolType_e::Float_SumCheck){
            on_actionSumCheck_triggered(true);
        }
    }

    //轴标签
    ui->customPlot->xAxis->setLabel(Config::getXAxisName());
    ui->customPlot->yAxis->setLabel(Config::getYAxisName());
    //数值显示器
    ui->actionValueDisplay->setChecked(Config::getValueDisplayState());
    on_actionValueDisplay_triggered(Config::getValueDisplayState());
    //图像名字集
    ui->customPlot->plotControl->setNameSet(ui->customPlot, Config::getPlotterGraphNames(ui->customPlot->plotControl->getMaxValidGraphNumber()));
    //OpenGL
    ui->actionOpenGL->setChecked(Config::getOpengGLState());
    on_actionOpenGL_triggered(Config::getOpengGLState());
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    connect(this, SIGNAL(parseFileSignal()),this,SLOT(parseFileSlot()));

    //槽
    connect(&cycleSendTimer, SIGNAL(timeout()), this, SLOT(cycleSendTimerSlot()));
    connect(&secTimer, SIGNAL(timeout()), this, SLOT(secTimerSlot()));
    connect(&printToTextBrowserTimer, SIGNAL(timeout()), this, SLOT(printToTextBrowserTimerSlot()));
    connect(&plotterParseTimer, SIGNAL(timeout()), this, SLOT(plotterParseTimerSlot()));
    connect(&serial, SIGNAL(readyRead()), this, SLOT(readSerialPort()));
    connect(&serial, SIGNAL(bytesWritten(qint64)), this, SLOT(serialBytesWritten(qint64)));
    connect(&serial, SIGNAL(error(QSerialPort::SerialPortError)),  this, SLOT(handleSerialError(QSerialPort::SerialPortError)));

    connect(ui->textBrowser->verticalScrollBar(),SIGNAL(actionTriggered(int)),this,SLOT(verticalScrollBarActionTriggered(int)));

    //状态栏标签
    statusRemoteMsgLabel = new QLabel(this);
    statusSpeedLabel = new QLabel(this);
    statusStatisticLabel = new QLabel(this);
    statusTimer = new QLabel(this);
    statusTimer->setText("计时器:"+formatTime(0));
    statusRemoteMsgLabel->setOpenExternalLinks(true);//可打开外链
    ui->statusBar->addPermanentWidget(statusRemoteMsgLabel);
    ui->statusBar->addPermanentWidget(statusTimer);
    ui->statusBar->addPermanentWidget(statusStatisticLabel);//显示永久信息
    ui->statusBar->addPermanentWidget(statusSpeedLabel);

    //设置波特率框和发送间隔框的合法输入范围
    ui->baudrateList->setValidator(new QIntValidator(0,9999999,this));
    ui->sendInterval->setValidator(new QIntValidator(0,99999,this));
    ui->timeStampTimeOut->setValidator(new QIntValidator(0,99999,this));

    //加载高亮规则
    on_actionKeyWordHighlight_triggered(ui->actionKeyWordHighlight->isChecked());

    //初始化绘图控制器
    ui->customPlot->init(ui->statusBar);

    //http
    http = new HTTP(this);

    //读取配置（所有资源加->完成后、动作执行前读取）
    readConfig();

    //显示收发统计
    serial.resetCnt();
    statusStatisticLabel->setText(serial.getTxRxString());

    //搜寻可用串口，并尝试打开
    on_refreshCom_clicked();
    tryOpenSerial();

    //数值显示器初始化
    ui->valueDisplay->setColumnCount(2);
    ui->valueDisplay->setHorizontalHeaderItem(0,new QTableWidgetItem("名称"));
    ui->valueDisplay->setHorizontalHeaderItem(1,new QTableWidgetItem("值"));
    ui->valueDisplay->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->valueDisplay->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->valueDisplay->horizontalHeader()->setStretchLastSection(true);

    //加载样式表
    QFile file(":/style.css");
    file.open(QFile::ReadOnly);
    QString style = file.readAll();
    file.close();
    this->setStyleSheet(style);
    g_font = Config::getGUIFont();
    ui->textBrowser->setFont(g_font);
    ui->textEdit->setFont(g_font);
    ui->multiString->setFont(g_font);
    ui->customPlot->plotControl->setupFont(ui->customPlot, g_font);

    this->setWindowTitle("串口调试助手 V"+Config::getVersion());

    //启动定时器
    secTimer.setTimerType(Qt::PreciseTimer);
    secTimer.start(1000);
    printToTextBrowserTimer.start(20);
    plotterParseTimer.setTimerType(Qt::PreciseTimer);

    //计时器
    g_lastSecsSinceEpoch = QDateTime::currentSecsSinceEpoch();

    //显示界面
    this->show();
    //是否首次运行
    if(Config::getFirstRun()){
        //弹出帮助文件
        on_actionManual_triggered();
        //弹出声明
        on_actionAbout_triggered();
        QMessageBox::information(this, "提示", "欢迎使用本串口调试助手。\n\n请认真阅读帮助文件与相关声明。\n若您继续使用本软件则代表您接受并同意相关声明。\n若您不同意相关声明请自行关闭软件。");
    }
}

void MainWindow::printToTextBrowserTimerSlot()
{
    if(RefreshTextBrowser==false)
        return;

    //打印数据
    printToTextBrowser();

    if(RefreshTextBrowser)
        RefreshTextBrowser = false;
}


QString MainWindow::formatTime(int ms)
{
    int ss = 1000;
    int mi = ss * 60;
    int hh = mi * 60;
    int dd = hh * 24;

    long day = ms / dd;
    long hour = (ms - day * dd) / hh;
    long minute = (ms - day * dd - hour * hh) / mi;
    long second = (ms - day * dd - hour * hh - minute * mi) / ss;
    long milliSecond = ms - day * dd - hour * hh - minute * mi - second * ss;

    QString hou = QString::number(hour,10);
    QString min = QString::number(minute,10);
    QString sec = QString::number(second,10);
    QString msec = QString::number(milliSecond,10);

    //qDebug() << "minute:" << min << "second" << sec << "ms" << msec <<endl;

    return hou + ":" + min + ":" + sec ;
}

void MainWindow::secTimerSlot()
{
    static int64_t secCnt = 0;
    static int adIndex = 0;

    //传输速度统计与显示
    rxSpeedKB = static_cast<double>(statisticRxByteCnt) / 1024.0;
    statisticRxByteCnt = 0;
    txSpeedKB = static_cast<double>(statisticTxByteCnt) / 1024.0;
    statisticTxByteCnt = 0;
    statusSpeedLabel->setText(" Tx:" + QString::number(txSpeedKB, 'f', 2) + "KB/s " + "Rx:" + QString::number(rxSpeedKB, 'f', 2) + "KB/s");

    //显示远端下载的信息
    if(http->getMsgList().size()>0 && secCnt%10==0){
        statusRemoteMsgLabel->setText(http->getMsgList().at(adIndex++));
        if(adIndex == http->getMsgList().size())
            adIndex = 0;
    }

    if(ui->comSwitch->isChecked()){
        qint64 consumedTime = QDateTime::currentSecsSinceEpoch() - g_lastSecsSinceEpoch;
        statusTimer->setText("计时器:"+formatTime(consumedTime*1000));
    }else{
        g_lastSecsSinceEpoch = QDateTime::currentSecsSinceEpoch();
    }

    secCnt++;
    currentRunTime++;
}

void MainWindow::debugTimerSlot()
{
    #define BYTE0(dwTemp)   static_cast<char>((*reinterpret_cast<char *>(&dwTemp)))
    #define BYTE1(dwTemp)   static_cast<char>((*(reinterpret_cast<char *>(&dwTemp) + 1)))
    #define BYTE2(dwTemp)   static_cast<char>((*(reinterpret_cast<char *>(&dwTemp) + 2)))
    #define BYTE3(dwTemp)   static_cast<char>((*(reinterpret_cast<char *>(&dwTemp) + 3)))

    static double count;
    float num1, num2, num3, num4, num5, num6;
    num1 = static_cast<float>(qSin(count/0.3843));
    num2 = static_cast<float>(qCos(count/0.3843));
    num3 = static_cast<float>(qCos(count/0.6157)+qSin(count/0.3843));
    num4 = static_cast<float>(qCos(count/0.6157)-qSin(count/0.3843));
    num5 = static_cast<float>(qSin(count/0.3843)+qrand()/static_cast<double>(RAND_MAX)*1*qSin(count/0.6157));
    num6 = static_cast<float>(qCos(count/0.3843)+qrand()/static_cast<double>(RAND_MAX)*1*qCos(count/0.6157));

    if(ui->actionAscii->isChecked()){
        QString tmp;
        tmp = "{"+QString::number(static_cast<int>(count*10))+":" +
                  QString::number(static_cast<double>(num1),'f') + "," +
                  QString::number(static_cast<double>(num2),'f') + "," +
                  QString::number(static_cast<double>(num3),'f') + "," +
                  QString::number(static_cast<double>(num4),'f') + "," +
                  QString::number(static_cast<double>(num5),'f') + "," +
                  QString::number(static_cast<double>(num6),'f') + "}" + enter;
        if(serial.isOpen()){
            serial.write(tmp.toLocal8Bit());
        }
    }else if(ui->actionFloat->isChecked()){
        QByteArray tmp;
        tmp.append(BYTE0(num1));tmp.append(BYTE1(num1));tmp.append(BYTE2(num1));tmp.append(BYTE3(num1));
        tmp.append(BYTE0(num2));tmp.append(BYTE1(num2));tmp.append(BYTE2(num2));tmp.append(BYTE3(num2));
        tmp.append(BYTE0(num3));tmp.append(BYTE1(num3));tmp.append(BYTE2(num3));tmp.append(BYTE3(num3));
        tmp.append(BYTE0(num4));tmp.append(BYTE1(num4));tmp.append(BYTE2(num4));tmp.append(BYTE3(num4));
        tmp.append(BYTE0(num5));tmp.append(BYTE1(num5));tmp.append(BYTE2(num5));tmp.append(BYTE3(num5));
        tmp.append(BYTE0(num6));tmp.append(BYTE1(num6));tmp.append(BYTE2(num6));tmp.append(BYTE3(num6));
        tmp.append(static_cast<char>(0x00));tmp.append(static_cast<char>(0x00));tmp.append(static_cast<char>(0x80));tmp.append(static_cast<char>(0x7F));
        if(serial.isOpen()){
            serial.write(tmp);
        }
    }

    if(ui->actionPlotterSwitch->isChecked()||ui->actionValueDisplay->isChecked()){
        count = count + 0.1;
    }
}

MainWindow::~MainWindow()
{
    if(needSaveConfig){
        Config::setFirstRun(false);
        if(ui->actionUTF8->isChecked()){
            Config::setCodeRule(CodeRule_e::UTF8);
        }else if(ui->actionGBK->isChecked()){
            Config::setCodeRule(CodeRule_e::GBK);
        }
        if(ui->action_winLikeEnter->isChecked()){
            Config::setEnterStyle(EnterStyle_e::WinStyle);
        }else if(ui->action_unixLikeEnter->isChecked()){
            Config::setEnterStyle(EnterStyle_e::UnixStyle);
        }

        //global
        Config::setHexSendState(ui->hexSend->isChecked());
        Config::setHexShowState(ui->hexDisplay->isChecked());
        Config::setSendInterval(ui->sendInterval->text().toInt());
        Config::setTimeStampState(ui->timeStampCheckBox->isChecked());
        Config::setTimeStampTimeOut(ui->timeStampTimeOut->text().toInt());
        Config::setMultiStringState(ui->actionMultiString->isChecked());
        Config::setKeyWordHighlightState(ui->actionKeyWordHighlight->isChecked());
        Config::setTextSendArea(ui->textEdit->toPlainText());
        Config::setVersion();
        QStringList multi;
        for(int i = 0; i < ui->multiString->count(); i++){
            multi.append(ui->multiString->item(i)->text());
        }
        Config::setMultiString(multi);
        Config::setLastFileDialogPath(lastFileDialogPath);
        Config::setGUIFont(g_font);
        Config::setBackGroundColor(g_background_color);

        //serial 只保存成功打开过的
        Config::setPortName(serial.portName());
        Config::setBaudrate(serial.baudRate());
        Config::setDataBits(serial.dataBits());
        Config::setStopBits(serial.stopBits());
        Config::setParity(serial.parity());
        Config::setFlowControl(serial.flowControl());

        //plotter
        Config::setPlotterState(ui->actionPlotterSwitch->isChecked());
        if(ui->actionAscii->isChecked()){
            if(ui->actionSumCheck->isChecked())
                Config::setPlotterType(ProtocolType_e::Ascii_SumCheck);
            else
                Config::setPlotterType(ProtocolType_e::Ascii);
        }
        else if(ui->actionFloat->isChecked()){
            if(ui->actionSumCheck->isChecked())
                Config::setPlotterType(ProtocolType_e::Float_SumCheck);
            else
                Config::setPlotterType(ProtocolType_e::Float);
        }

        Config::setPlotterGraphNames(ui->customPlot->plotControl->getNameSetsFromPlot());
        Config::setXAxisName(ui->customPlot->xAxis->label());
        Config::setYAxisName(ui->customPlot->yAxis->label());
        Config::setValueDisplayState(ui->actionValueDisplay->isChecked());
        Config::setOpengGLState(ui->actionOpenGL->isChecked());

        //static
        Config::setLastRunTime(currentRunTime);
        Config::setTotalRunTime(currentRunTime);
        Config::setLastTxCnt(serial.getTotalTxCnt());//getTotalTxCnt是本次软件运行时的总发送量
        Config::setTotalTxCnt(serial.getTotalTxCnt());
        Config::setLastRxCnt(serial.getTotalRxCnt());
        Config::setTotalRxCnt(serial.getTotalRxCnt());
        Config::setTotalRunCnt(1);
    }else{
        Config::writeDefault();
    }
    delete highlighter;
    delete ui;
    delete http;
}

/*
 * Function:刷新串口按下。不知道为什么打开串口后再调用该函数就崩溃
*/
void MainWindow::on_refreshCom_clicked()
{   
    if(ui->refreshCom->isEnabled()==false){
        ui->statusBar->showMessage("刷新功能被禁用",1000);
        return;
    }

    //测试更新下拉列表
    mySerialPort *testSerial = new mySerialPort;
    QList<QString> tmp;

    tmp = testSerial->refreshSerialPort();
    //刷新串口状态，需要记录当前选择的条目用于刷新后恢复
    QString portName = ui->comList->currentText().mid(0,ui->comList->currentText().indexOf('('));
    ui->comList->clear();
    foreach(const QString &info, tmp)
    {
        ui->comList->addItem(info);
    }
    if(ui->comList->count() == 0)
        ui->comList->addItem("未找到可用串口!");

    //恢复刷新前的选择
    ui->comList->setCurrentIndex(0);
    for(int i = 0; i < ui->comList->count(); i++){
        if(ui->comList->itemText(i).startsWith(portName)){
            ui->comList->setCurrentIndex(i);
            break;
        }
    }

    delete testSerial;
}

/*
 * Function:在只有一个串口设备时且未被占用时尝试打开
*/
void MainWindow::tryOpenSerial()
{
    //只存在一个串口时且串口未被占用时自动打开
    if(ui->comList->count()==1 && ui->comList->currentText().indexOf("占用")==-1 && ui->comList->currentText()!="未找到可用串口!"){
        ui->refreshCom->setChecked(false);
        ui->comSwitch->setChecked(true);
        on_comSwitch_clicked(true);
    }else{
        //如果有多个串口，则尝试选择上次使用的端口号
        if(ui->comList->count()>1){
            QString name = Config::getPortName();
            for(int i = 0; i < ui->comList->count(); i++){
                if(ui->comList->itemText(i).startsWith(name)){
                    ui->comList->setCurrentIndex(i);
                    break;
                }
            }
        }
    }
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
            ui->comSwitch->setChecked(true);
            g_lastSecsSinceEpoch = QDateTime::currentSecsSinceEpoch();
            qint64 consumedTime = QDateTime::currentSecsSinceEpoch() - g_lastSecsSinceEpoch;
            statusTimer->setText("计时器:"+formatTime(consumedTime*1000));
        }
        else {
            ui->comSwitch->setText("打开串口");
            ui->comSwitch->setChecked(false);
            QString msg = "请检查下列情况后重新打开串口：\n\n"
                          "# USB线缆是否松动？\n"
                          "# 是否选择了正确的串口设备？\n"
                          "# 该串口是否被其他程序占用？\n"
                          "# 是否设置了过高的波特率？\n";
            QMessageBox::critical(this, "串口打开失败!", msg, QMessageBox::Ok);
        }
    }
    else
    {
        //清空文件缓冲
        SendFileBuff.clear();
        SendFileBuffIndex = 0;
        //关闭定时器
        if(cycleSendTimer.isActive()){
            cycleSendTimer.stop();
            ui->cycleSendCheck->setChecked(false);
        }

        serial.close();
        ui->comSwitch->setText("打开串口");
        ui->comSwitch->setChecked(false);
    }

    on_refreshCom_clicked();
}

/*
 * Function:从串口读取数据
*/
void MainWindow::readSerialPort()
{
    QByteArray tmpReadBuff;
    QByteArray floatParseBuff;//用于绘图协议解析的缓冲。其中float协议不处理中文

    //先获取时间，避免解析数据导致时间消耗的影响
    QString timeString;
    timeString = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    timeString = "\n["+timeString+"]Rx<- ";

    //解析文件模式
    if(parseFile){
        tmpReadBuff = parseFileBuff.at(parseFileBuffIndex++);
        RxBuff.append(tmpReadBuff);
    }
    else{
        if(serial.isOpen()){
            tmpReadBuff = serial.readAll(); //tmpReadBuff一定不为空。
            RxBuff.append(tmpReadBuff);
        }else
            return;
    }

    //空数据检查
    if(tmpReadBuff.isEmpty()){
        return;
    }

//    //'\0'检查
//    while(tmpReadBuff.indexOf('\0')!=-1){
//        tmpReadBuff.replace('\0',"\\0");
//    }

    //收到数据且时间戳超时则可以添加新的时间戳和换行
//    if(ui->timeStampCheckBox->isChecked() && timeStampTimer.isActive()==false){
//        timeStampTimer.setSingleShot(true);
//        timeStampTimer.start(ui->timeStampTimeOut->text().toInt());
//    }

    //速度统计，不能和下面的互换，否则不准确
    statisticRxByteCnt += tmpReadBuff.size();

    //读取数据并衔接到上次未处理完的数据后面
    tmpReadBuff = unshowedRxBuff + tmpReadBuff;
    unshowedRxBuff.clear();

    //'\r'若单独结尾则可能被误切断，放到下一批数据中
    if(tmpReadBuff.endsWith('\r')){
        unshowedRxBuff.append(tmpReadBuff.at(tmpReadBuff.size()-1));
        tmpReadBuff.remove(tmpReadBuff.size()-1,1);
        if(tmpReadBuff.size()==0)
            return;
    }

    //如果不是hex显示则要考虑中文处理
    if(ui->hexDisplay->isChecked()==false){
        //只需要保证上屏的最后一个字节的高位不是1即可
        if(tmpReadBuff.back() & 0x80){
            int reversePos = tmpReadBuff.size()-1;
            while(tmpReadBuff.at(reversePos)&0x80){
                reversePos--;
                if(reversePos<0)
                    break;
            }
            unshowedRxBuff = tmpReadBuff.mid(reversePos+1);
            tmpReadBuff = tmpReadBuff.mid(0,reversePos+1);
        }
        //如果unshowedRxBuff正好是相关编码长度的倍数，则可以上屏
        if((ui->actionGBK->isChecked() && unshowedRxBuff.size()%2==0) ||
           (ui->actionUTF8->isChecked() && unshowedRxBuff.size()%3==0)){
            tmpReadBuff.append(unshowedRxBuff);
            unshowedRxBuff.clear();
        }
    }

    //时间戳选项
    if(ui->timeStampCheckBox->isChecked() && timeStampTimer.isActive()==false){
        //hex解析
        hexBrowserBuff.append(timeString + toHexDisplay(tmpReadBuff).toLatin1());//换行符在前面判断没有数据时自动追加一次
        //asic解析，显示的数据一律不要\r。且进行\0显示的检查
        while(tmpReadBuff.indexOf("\r\n")!=-1){
            tmpReadBuff.replace("\r\n","\n");
        }
        while(tmpReadBuff.indexOf('\0')!=-1){
            tmpReadBuff.replace('\0',"\\0");
        }
        BrowserBuff.append(timeString + QString::fromLocal8Bit(tmpReadBuff));//换行符在前面判断没有数据时自动追加一次
        timeStampTimer.setSingleShot(true);
        timeStampTimer.start(ui->timeStampTimeOut->text().toInt());
    }else{
        //hex解析
        hexBrowserBuff.append(toHexDisplay(tmpReadBuff).toLatin1());
        //asic解析，显示的数据一律不要\r。且进行\0显示的检查
        while(tmpReadBuff.indexOf("\r\n")!=-1){
            tmpReadBuff.replace("\r\n","\n");
        }
        while(tmpReadBuff.indexOf('\0')!=-1){
            tmpReadBuff.replace('\0',"\\0");
        }
        BrowserBuff.append(QString::fromLocal8Bit(tmpReadBuff));
    }

    //更新收发统计
    statusStatisticLabel->setText(serial.getTxRxString());

    //允许数据刷新
    RefreshTextBrowser = true;
}

void MainWindow::parseFileSlot()
{
    readSerialPort();
    qApp->processEvents();
    ui->customPlot->replot();
//    qDebug()<<tt<<tt1<<parseFileBuffIndex/parseFileBuff.size();
    if(parseFileBuffIndex!=parseFileBuff.size()){
        ui->statusBar->showMessage("解析进度："+QString::number(static_cast<int>(100.0*(parseFileBuffIndex+1.0)/parseFileBuff.size()))+"% ",1000);
        emit parseFileSignal();
    }else{
        parseFile = false;
        parseFileBuffIndex = 0;
        parseFileBuff.clear();
        ui->sendButton->setEnabled(true);
        ui->multiString->setEnabled(true);
        ui->cycleSendCheck->setEnabled(true);
        ui->clearWindows->setText("清  空");
    }
}

static int PAGING_SIZE = 4068 * 2; //TextBrowser分页显示大小，这个值正好满屏显示
void MainWindow::printToTextBrowser()
{
    //估计当前窗口可显示多少字符
    int HH = static_cast<int>(ui->textBrowser->height()/19.2);
    int WW = static_cast<int>(ui->textBrowser->width()/9.38);
    PAGING_SIZE = static_cast<int>(HH*WW*1.25); //冗余
    //不超过最大值
    if(PAGING_SIZE > PAGEING_SIZE_MAX){
        PAGING_SIZE = PAGEING_SIZE_MAX;
    }
    //且满足gbk/utf8编码长度的倍数
    if(ui->actionGBK->isChecked()){
        while(PAGING_SIZE%2!=0)
            PAGING_SIZE--;
    }else if(ui->actionUTF8->isChecked()){
        while(PAGING_SIZE%3!=0)
            PAGING_SIZE--;
    }
//    qDebug()<<"printToTextBrowser"<<HH<<WW<<HH*WW<<PAGING_SIZE;

    //打印数据
    if(ui->hexDisplay->isChecked()){
        if(hexBrowserBuff.size()<PAGING_SIZE){
            ui->textBrowser->setText(hexBrowserBuff);
            hexBrowserBuffIndex = hexBrowserBuff.size();
        }else{
            ui->textBrowser->setText(hexBrowserBuff.mid(hexBrowserBuff.size()-PAGING_SIZE));
            hexBrowserBuffIndex = PAGING_SIZE;
        }
    }else{
        if(BrowserBuff.size()<PAGING_SIZE){
            ui->textBrowser->setText(BrowserBuff);
            BrowserBuffIndex = BrowserBuff.size();
        }else{
            ui->textBrowser->setText(BrowserBuff.mid(BrowserBuff.size()-PAGING_SIZE));
            BrowserBuffIndex = PAGING_SIZE;
        }
    }

    ui->textBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::serialBytesWritten(qint64 bytes)
{
    //发送速度统计
    statisticTxByteCnt += bytes;

    if(SendFileBuff.size()>0 && SendFileBuffIndex!=SendFileBuff.size()){
        ui->statusBar->showMessage("发送进度："+QString::number(static_cast<int>(100.0*(SendFileBuffIndex+1.0)/SendFileBuff.size()))+"%",1000);
        serial.write(SendFileBuff.at(SendFileBuffIndex++));
        if(SendFileBuffIndex == SendFileBuff.size()){
            SendFileBuffIndex = 0;
            SendFileBuff.clear();
        }
    }

    //更新收发统计
    statusStatisticLabel->setText(serial.getTxRxString());
}

void MainWindow::handleSerialError(QSerialPort::SerialPortError errCode)
{
    //故障检测
    if(errCode == QSerialPort::ResourceError){
        //记录故障的串口号
        QString portName = serial.portName();
        //关闭串口
        on_comSwitch_clicked(false);
        //强提醒也争取了时间，如果是短时间松动，则点击确定后可以恢复所选的端口
        QMessageBox::warning(this,"警告","检测到串口故障，已关闭串口。\n串口是否发生了松动？");
        //【还要】再刷新一次
        on_refreshCom_clicked();
        //尝试恢复所选端口号
        for(int i = 0; i < ui->comList->count(); i++){
            if(ui->comList->itemText(i).startsWith(portName)){
                ui->comList->setCurrentIndex(i);
            }
        }
    }
//    qDebug()<<"handleSerialError"<<errCode;
}

/*
 * Function:连续发送定时器槽，执行数据发送
*/
void MainWindow::cycleSendTimerSlot()
{
    on_sendButton_clicked();
}

/*
 * Function:发送数据
*/
void MainWindow::on_sendButton_clicked()
{
    static QByteArray tmp;//用static是担心write是传递指针，发送大量数据可能会由于未发送完成而被销毁？

    if(!serial.isOpen()){
        QMessageBox::information(this,"提示","串口未打开");
        return;
    }

    //回车风格转换，win风格补上'\r'，默认unix风格
    tmp = ui->textEdit->toPlainText().toLocal8Bit();
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

    //十六进制检查
    QByteArray sendArr; //真正发送出去的数据
    if(ui->hexSend->isChecked()){
        //以hex发送数据
        //HexStringToByteArray函数必须传入格式化后的字符串，如"02 31"
        bool ok;
        sendArr = HexStringToByteArray(tmp,ok); //hex转发送数据流
        if(ok){
            serial.write(sendArr);
        }else{
            ui->statusBar->showMessage("文本输入区数据转换失败，放弃此次发送！", 2000);
        }
    }else {
        sendArr = tmp;
        //utf8编码
        serial.write(sendArr);
    }

    //周期发送开启则立刻发送
    if(ui->cycleSendCheck->isChecked())
        serial.flush();

    //若添加了时间戳则把发送的数据也显示在接收区
    if(ui->timeStampCheckBox->isChecked()){
        QString timeString;
        timeString = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        timeString = "\n["+timeString+"]Tx-> ";

        hexBrowserBuff.append(timeString + toHexDisplay(sendArr));
        BrowserBuff.append(timeString + QString::fromLocal8Bit(sendArr));

        //打印数据
        printToTextBrowser();
    }

    //给多字符串控件添加条目
    if(ui->actionMultiString->isChecked()){
        bool hasItem=false;
        for(int i = 0; i < ui->multiString->count(); i++){
            if(ui->multiString->item(i)->text()==ui->textEdit->toPlainText())
                hasItem = true;
        }
        if(!hasItem)
            ui->multiString->addItem(ui->textEdit->toPlainText());
    }

    //更新收发统计
    statusStatisticLabel->setText(serial.getTxRxString());
}

void MainWindow::on_clearWindows_clicked()
{
    ui->clearWindows->setText("清  空");

    //定时器
    g_lastSecsSinceEpoch = QDateTime::currentSecsSinceEpoch();
    qint64 consumedTime = QDateTime::currentSecsSinceEpoch() - g_lastSecsSinceEpoch;
    statusTimer->setText("计时器:"+formatTime(consumedTime*1000));

    //串口
    serial.resetCnt();
    if(serial.isOpen())
        serial.flush();

    //接收区
    ui->textBrowser->clear();
    RxBuff.clear();
    hexBrowserBuff.clear();
    hexBrowserBuffIndex = 0;
    BrowserBuff.clear();
    BrowserBuffIndex = 0;
    unshowedRxBuff.clear();

    //清空文件缓冲
    SendFileBuff.clear();
    SendFileBuffIndex = 0;
    parseFileBuff.clear();
    parseFileBuffIndex = 0;

    //绘图器相关
    ui->customPlot->protocol->clearBuff();
    ui->customPlot->plotControl->clearPlotter(ui->customPlot, -1);
    while(ui->customPlot->graphCount()>1){
        ui->customPlot->removeGraph(ui->customPlot->graphCount()-1);
    }
    ui->customPlot->yAxis->setRange(0,5);
    ui->customPlot->xAxis->setRange(0, ui->customPlot->plotControl->getXAxisLength(), Qt::AlignRight);
    ui->customPlot->replot();
    plotterParsePosInRxBuff = 0;

    //数值显示器
    deleteValueDisplaySlot();

    //更新收发统计
    statusStatisticLabel->setText(serial.getTxRxString());


}

void MainWindow::on_cycleSendCheck_clicked(bool checked)
{
    if(ui->sendInterval->text().toInt() < 15 && checked){
        QMessageBox::warning(this,"警告","发送间隔较小可能不够准确");
    }

    if(!serial.isOpen()){
        QMessageBox::information(this,"提示","串口未打开");
        ui->cycleSendCheck->setChecked(false);
        return;
    }

    //启停定时器
    if(checked){
        ui->cycleSendCheck->setChecked(true);
        cycleSendTimer.setTimerType(Qt::PreciseTimer);
        cycleSendTimer.start(ui->sendInterval->text().toInt());
    }
    else {
        ui->cycleSendCheck->setChecked(false);
        cycleSendTimer.stop();
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
    arg1++;
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
    checked = !checked;
    printToTextBrowser();
}

/*
 * Action:激活使用win风格回车（\r\n）
 * Function:
*/
void MainWindow::on_action_winLikeEnter_triggered(bool checked)
{
    enter = "\r\n";
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
    enter = "\n";
    if(checked){
        ui->action_winLikeEnter->setChecked(false);
    }else {
        ui->action_unixLikeEnter->setChecked(false);
    }
}

/*
 * Action:激活使用UTF8编码
 * Function:
*/
void MainWindow::on_actionUTF8_triggered(bool checked)
{
    ui->actionUTF8->setChecked(true);
    if(checked){
        //设置中文编码
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);
        ui->actionGBK->setChecked(false);
    }
}

void MainWindow::on_actionGBK_triggered(bool checked)
{
    ui->actionGBK->setChecked(true);
    if(checked){
        //设置中文编码
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        QTextCodec::setCodecForLocale(codec);
        ui->actionUTF8->setChecked(false);
    }
}

/*
 * Action:保存数据动作触发
 * Function:
*/
void MainWindow::on_actionSaveOriginData_triggered()
{
    //如果追加时间戳则提示时间戳不会被保存
    if(ui->timeStampCheckBox->isChecked())
        QMessageBox::information(this,"提示","时间戳数据不会被保存！只保存接收到的原始数据。");

    //打开保存文件对话框
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "保存原始数据-选择文件路径",
                                                    lastFileDialogPath + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".dat",
                                                    "Dat File(*.dat);;All File(*.*)");
    //检查路径格式
    if(!savePath.endsWith(".dat")){
        if(!savePath.isEmpty())
            QMessageBox::information(this,"尚未支持的文件格式","请选择dat文件。");
        return;
    }

    //保存数据
    QFile file(savePath);
    //删除旧数据形式写文件
    if(file.open(QFile::WriteOnly|QFile::Truncate)){
        file.write(RxBuff);//不用DataStream写入非文本文件，它会额外添加4个字节作为文件头
        file.flush();
        file.close();

        QString str = enter + "Total saved "+QString::number(RxBuff.size())+" Bytes in "+savePath + enter;
        BrowserBuff.append(str);
        hexBrowserBuff.append(toHexDisplay(str.toLocal8Bit()));
        printToTextBrowser();
    }

    //记忆路径
    lastFileDialogPath = savePath;
    lastFileDialogPath = lastFileDialogPath.mid(0, lastFileDialogPath.lastIndexOf('/')+1);
}

/*
 * Action:读取数据动作触发
 * Function:
*/
void MainWindow::on_actionOpenOriginData_triggered()
{   
    static QString lastFileName;
    //打开文件对话框
    QString readPath = QFileDialog::getOpenFileName(this,
                                                    "读取原始数据-选择文件路径",
                                                    lastFileDialogPath + lastFileName,
                                                    "Dat File(*.dat);;All File(*.*)");
    //检查文件路径结尾
    if(!readPath.endsWith(".dat")){
        if(!readPath.isEmpty())
            QMessageBox::information(this,"尚未支持的文件格式","请选择dat文件。");
        return;
    }
    //记录上次路径
    lastFileDialogPath = readPath;
    lastFileDialogPath = lastFileDialogPath.mid(0, lastFileDialogPath.lastIndexOf('/')+1);
    //读取文件
    QFile file(readPath);
    //读文件
    if(file.open(QFile::ReadOnly)){
        //记录上一次文件名
        lastFileName = readPath;
        while(lastFileName.indexOf('/')!=-1){
            lastFileName = lastFileName.mid(lastFileName.indexOf('/')+1);
        }
        on_clearWindows_clicked();
        RxBuff.clear();
        RxBuff = file.readAll();
        file.close();

        //文件分包
        #define PACKSIZE 4096
        parseFileBuffIndex = 0;
        parseFileBuff.clear();
        while(RxBuff.size()>PACKSIZE){
            parseFileBuff.append(RxBuff.mid(0,PACKSIZE));
            RxBuff.remove(0,PACKSIZE);
        }
        parseFileBuff.append(RxBuff); //一定会有一个元素
        RxBuff.clear();
        if(parseFileBuff.size()<1){
            return;
        }

        //重置绘图器解析点，以触发解析
        if(ui->actionPlotterSwitch->isChecked())
            plotterParsePosInRxBuff = 0;

        ui->textBrowser->clear();
        ui->textBrowser->append("File size: "+QString::number(file.size())+" Byte");
        ui->textBrowser->append("Read containt:"+enter);
        BrowserBuff.clear();
        BrowserBuff.append(ui->textBrowser->document()->toPlainText());

        //关闭不必要的控件
        ui->sendButton->setEnabled(false);
        ui->multiString->setEnabled(false);
        cycleSendTimer.stop();
        ui->cycleSendCheck->setEnabled(false);
        ui->cycleSendCheck->setChecked(false);
        ui->clearWindows->setText("中  止");

        // 解析读取的数据
        parseFile = true;
        unshowedRxBuff.clear();
        emit parseFileSignal();
    }else{
        QMessageBox::information(this,"提示","文件打开失败。");
        lastFileName.clear();
    }
}

/*
 * Action:触发“关于”按钮
 * Function:弹出关于对话框
*/
void MainWindow::on_actionAbout_triggered()
{
    //创建关于我对话框资源
    About_Me_Dialog* p = new About_Me_Dialog(this);
    p->getVersionString(Config::getVersion());
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
    if(p->clickedOK()){
        if(!serial.moreSetting(p->getStopBits(),p->getParity(),p->getFlowControl(),p->getDataBits()))
            QMessageBox::information(this,"提示","串口设置失败，请关闭串口重试");
    }

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
 * Function:选择了新的端口号，重新打开串口
*/
void MainWindow::on_comList_textActivated(const QString &arg1)
{
    //关闭自动发送功能
    if(ui->cycleSendCheck->isChecked()){
        on_cycleSendCheck_clicked(false);
    }
    if(ui->actiondebug->isChecked()){
        debugTimer.stop();
        ui->actiondebug->setChecked(false);
    }

    QString unused = arg1;//屏蔽警告
    //重新打开串口
    if(serial.isOpen()){
        on_comSwitch_clicked(false);
        on_comSwitch_clicked(true);
        if(serial.isOpen())
            ui->statusBar->showMessage("已重新启动串口",1000);
        else
            ui->statusBar->showMessage("串口重启失败",1000);
    }
}

void MainWindow::on_actionSaveShowedData_triggered()
{
    //打开保存文件对话框
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "保存显示数据-选择文件路径",
                                                    lastFileDialogPath + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".txt",
                                                    "Text File(*.txt);;All File(*.*)");
    //检查路径
    if(!savePath.endsWith("txt")){
        if(!savePath.isEmpty())
            QMessageBox::information(this,"尚未支持的文件格式","请选择txt文本文件。");
        return;
    }

    //记忆路径
    lastFileDialogPath = savePath;
    lastFileDialogPath = lastFileDialogPath.mid(0, lastFileDialogPath.lastIndexOf('/')+1);

    //保存数据
    QFile file(savePath);
    QTextStream stream(&file);
    //删除旧数据形式写文件
    if(file.open(QFile::WriteOnly|QFile::Text|QFile::Truncate)){
        if(ui->hexDisplay->isChecked()){
            stream<<hexBrowserBuff;
        }else{
            stream<<BrowserBuff;
        }
        file.close();

        QString str = enter + "Total saved "+QString::number(file.size())+" Bytes in "+savePath + enter;
        BrowserBuff.append(str);
        hexBrowserBuff.append(toHexDisplay(str.toLocal8Bit()));
        printToTextBrowser();
    }
}

void MainWindow::on_actionUpdate_triggered()
{
    http->addTask(HTTP::GetVersion);
//    http->addTask(HTTP::GetVersion_MY_SERVER);
}

void MainWindow::on_sendInterval_textChanged(const QString &arg1)
{
    if(cycleSendTimer.isActive())
        cycleSendTimer.setInterval(arg1.toInt());
}

void MainWindow::on_actionSTM32_ISP_triggered()
{
    bool serialState = ui->comSwitch->isChecked();
    if(serialState)
        on_comSwitch_clicked(false);

    QFile file;
    file.copy(":/stm32isp.exe","stm32isp.exe");
    file.setFileName("stm32isp.exe");
    file.setPermissions(QFile::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner);

    STM32ISP_Dialog *p = new STM32ISP_Dialog(this);
    p->exec();
    delete p;

    file.remove();

    if(serialState)
        on_comSwitch_clicked(true);
}

/*
 * Event: 多字符串条目双击
*/
void MainWindow::on_multiString_itemDoubleClicked(QListWidgetItem *item)
{
    ui->textEdit->clear();
    ui->textEdit->setText(item->text());
    on_sendButton_clicked();
}

/*
 * Action:multiString开关
*/
void MainWindow::on_actionMultiString_triggered(bool checked)
{
    if(checked){
        ui->multiString->show();
        //设置颜色交错
        ui->multiString->setAlternatingRowColors(true);
        //设置高度
        QList<int> widthList;
        int width = ui->splitter->width();
        widthList << static_cast<int>(width*0.78) << static_cast<int>(width*0.22);
        ui->splitter->setSizes(widthList);
    }else {
        ui->multiString->hide();
    }
}

/*
 * Function:多字符串右键菜单
*/
void MainWindow::on_multiString_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem* curItem = ui->multiString->itemAt( pos );
    QAction *editSeed = nullptr;
    QAction *clearSeeds = nullptr;
    QAction *deleteSeed = nullptr;
    QMenu *popMenu = new QMenu( this );
    //添加右键菜单
    if( curItem != nullptr ){
        editSeed = new QAction("编辑当前条目", this);
        popMenu->addAction( editSeed );
        connect( editSeed, SIGNAL(triggered() ), this, SLOT( editSeedSlot()) );

        popMenu->addSeparator();

        deleteSeed = new QAction("删除当前条目", this);
        popMenu->addAction( deleteSeed );
        connect( deleteSeed, SIGNAL(triggered() ), this, SLOT( deleteSeedSlot()) );

        popMenu->addSeparator();
    }
    clearSeeds = new QAction("清空所有条目", this);
    popMenu->addAction( clearSeeds );
    connect( clearSeeds, SIGNAL(triggered() ), this, SLOT( clearSeedsSlot()) );
    popMenu->exec( QCursor::pos() );
    delete popMenu;
    delete clearSeeds;
    delete deleteSeed;
}

/*
 * Function:编辑multiString条目
*/
void MainWindow::editSeedSlot()
{
    QListWidgetItem * item = ui->multiString->currentItem();
    if( item == nullptr )
        return;

    int curIndex = ui->multiString->row(item);
    bool ok = false;
    QString newStr = QInputDialog::getText(this,"编辑条目","新的文本：", QLineEdit::Normal,
                                           ui->multiString->item(curIndex)->text(), &ok,Qt::WindowCloseButtonHint);
    if(ok == true)
        ui->multiString->item(curIndex)->setText(newStr);
}

/*
 * Function:删除multiString条目
*/
void MainWindow::deleteSeedSlot()
{
    QListWidgetItem * item = ui->multiString->currentItem();
    if( item == nullptr )
        return;

    int curIndex = ui->multiString->row(item);
    ui->multiString->takeItem(curIndex);
    delete item;
}

/*
 * Function:清除multiString条目
*/
void MainWindow::clearSeedsSlot()
{
    QListWidgetItem * item = ui->multiString->currentItem();
    if( item == nullptr )
        return;

    ui->multiString->clear();
}

/*
 * Function:绘图器开关
*/
void MainWindow::on_actionPlotterSwitch_triggered(bool checked)
{
    if(checked){
        ui->customPlot->show();
        //设置高度
        QList<int> heightList;
        int height = ui->splitter_3->height();
        heightList << static_cast<int>(height*0.8) << static_cast<int>(height*0.2);
        ui->splitter_3->setSizes(heightList);

        //没激活就打开（数值显示器也可能激活）
        if(!plotterParseTimer.isActive()){
            plotterParseTimer.start(PLOTTER_PARSE_PERIOD);
            plotterParsePosInRxBuff = RxBuff.size() - 1;
        }

        if(ui->actionAscii->isChecked()){
            if(ui->actionSumCheck->isChecked())
                ui->plotter->setTitle("数据可视化：ASCII协议(和校验)");
            else
                ui->plotter->setTitle("数据可视化：ASCII协议");
        }
        else if(ui->actionFloat->isChecked()){
            if(ui->actionSumCheck->isChecked())
                ui->plotter->setTitle("数据可视化：FLOAT协议(和校验)");
            else
                ui->plotter->setTitle("数据可视化：FLOAT协议");
        }
    }else{
        ui->customPlot->hide();

        //数值显示器也未勾选时才停止定时器
        if(!ui->actionValueDisplay->isChecked())
            plotterParseTimer.stop();

        ui->plotter->setTitle("数据可视化");
    }
}

void MainWindow::plotterParseTimerSlot()
{
    QElapsedTimer elapsedTimer;
    int32_t maxParseLengthLimit = 4096;
    int32_t parsedLength;
    QVector<double> oneRowData;
    elapsedTimer.start();

    if(plotterParsePosInRxBuff >= RxBuff.size() - 1){
        return;
    }

    if(!ui->actionPlotterSwitch->isChecked() &&
       !ui->actionValueDisplay->isChecked()){
        return;
    }
    //关定时器，防止数据量过大导致咬尾振荡
    plotterParseTimer.stop();
    //添加解析长度限制，防止数据量过大振荡
    parsedLength = ui->customPlot->protocol->parse(RxBuff, plotterParsePosInRxBuff, maxParseLengthLimit, g_enableSumCheck);
    plotterParsePosInRxBuff += parsedLength;

    //数据填充
    while(ui->customPlot->protocol->parsedBuffSize()>0){

        oneRowData = ui->customPlot->protocol->popOneRowData();
        //绘图显示器
        if(ui->actionPlotterSwitch->isChecked()){
            //关闭刷新，数据全部填充完后统一刷新
            if(false == ui->customPlot->plotControl->displayToPlotter(ui->customPlot, oneRowData, false, false))
                ui->statusBar->showMessage("出现一组异常绘图数据，已丢弃。", 1000);
        }

    }
    if(ui->actionAutoRefreshYAxis->isChecked())
        ui->customPlot->yAxis->rescale(true);
    //曲线刷新
    ui->customPlot->replot();   //<10ms

    //数值显示器
    if(ui->actionValueDisplay->isChecked()){
        //判断是否添加行
        if(ui->valueDisplay->rowCount() < oneRowData.size()){
            //设置行
            ui->valueDisplay->setRowCount(oneRowData.size());
            //设置列，固定的
            ui->valueDisplay->setColumnCount(2);
            ui->valueDisplay->setHorizontalHeaderItem(0,new QTableWidgetItem("名称"));
            ui->valueDisplay->setHorizontalHeaderItem(1,new QTableWidgetItem("值"));
            ui->valueDisplay->horizontalHeader()->setStretchLastSection(true);
            ui->valueDisplay->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
            ui->valueDisplay->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
        }
        //添加数据
        int min = oneRowData.size() < ui->customPlot->plotControl->getNameSetsFromPlot().size() ? oneRowData.size() : ui->customPlot->plotControl->getNameSetsFromPlot().size();
        for(int i=0; i < min; i++){
            //这里会重复new对象导致内存溢出吗
            ui->valueDisplay->setItem(i,0,new QTableWidgetItem(ui->customPlot->plotControl->getNameSetsFromPlot().at(i)));
            ui->valueDisplay->setItem(i,1,new QTableWidgetItem(QString::number(oneRowData.at(i),'f')));
            //不可编辑
            ui->valueDisplay->item(i,0)->setFlags(ui->valueDisplay->item(i,0)->flags() & (~Qt::ItemIsEditable));
            ui->valueDisplay->item(i,1)->setFlags(ui->valueDisplay->item(i,1)->flags() & (~Qt::ItemIsEditable));
        }
    }

    if(parsedLength == maxParseLengthLimit){
        QString temp;
        temp = temp + "警告：绘图器繁忙，待解析数据长度：" + QString::number(RxBuff.size() - plotterParsePosInRxBuff - 1) + "Byte";
        ui->statusBar->showMessage(temp, 2000);
    }

    int32_t elapsed_time = elapsedTimer.elapsed();
    double parseSpeed = parsedLength/(elapsed_time/1000.0)/1024.0;
    parseSpeed = (double)((int)(parseSpeed*100))/100.0;   //保留两位小数
    static int32_t dynamic_period = PLOTTER_PARSE_PERIOD;
    if(parseSpeed < rxSpeedKB){
        dynamic_period = dynamic_period - 5;
        if(dynamic_period <= 5)
            dynamic_period = 5;
    }else{
        dynamic_period = dynamic_period + 5;
        if(dynamic_period >= PLOTTER_PARSE_PERIOD)
            dynamic_period = PLOTTER_PARSE_PERIOD;
    }
    plotterParseTimer.start(dynamic_period);
//    qDebug()<<"parsedLength"<<parsedLength<<"dynamic_period"<<dynamic_period;
//    qDebug()<<"plotterParseTimerSlot elapsed_time:"<<elapsed_time;
}

void MainWindow::on_actionAscii_triggered(bool checked)
{
    checked = !!checked;
    ui->customPlot->protocol->clearBuff();
    ui->customPlot->protocol->setProtocolType(DataProtocol::Ascii);
    ui->actionAscii->setChecked(true);
    ui->actionFloat->setChecked(false);

    if(ui->actionPlotterSwitch->isChecked()){
        if(ui->actionSumCheck->isChecked())
            ui->plotter->setTitle("数据可视化：ASCII协议(和校验)");
        else
            ui->plotter->setTitle("数据可视化：ASCII协议");
    }
}

void MainWindow::on_actionFloat_triggered(bool checked)
{
    checked = !!checked;
    ui->customPlot->protocol->clearBuff();
    ui->customPlot->protocol->setProtocolType(DataProtocol::Float);
    ui->actionAscii->setChecked(false);
    ui->actionFloat->setChecked(true);

    if(ui->actionPlotterSwitch->isChecked()){
        if(ui->actionSumCheck->isChecked())
            ui->plotter->setTitle("数据可视化：FLOAT协议(和校验)");
        else
            ui->plotter->setTitle("数据可视化：FLOAT协议");
    }
}

void MainWindow::on_actiondebug_triggered(bool checked)
{
    if(checked){
        debugTimer.setTimerType(Qt::PreciseTimer);
        debugTimer.start(10);
        connect(&debugTimer, SIGNAL(timeout()), this, SLOT(debugTimerSlot()));
    }else{
        debugTimer.stop();
        disconnect(&debugTimer, SIGNAL(timeout()), this, SLOT(debugTimerSlot()));
    }
}

void MainWindow::verticalScrollBarActionTriggered(int action)
{
    QScrollBar* bar = ui->textBrowser->verticalScrollBar();
    if(action == QAbstractSlider::SliderSingleStepAdd ||
       action == QAbstractSlider::SliderSingleStepSub||
       action == QAbstractSlider::SliderPageStepAdd||
       action == QAbstractSlider::SliderPageStepSub||
       action == QAbstractSlider::SliderMove){
        int value = bar->value();
        int oldMax = bar->maximum();
        int newValue;
        bool res;

        //
        if(ui->hexDisplay->isChecked()){
            res = hexBrowserBuffIndex != hexBrowserBuff.size();
        }else{
            res = BrowserBuffIndex != BrowserBuff.size();
        }
        //翻到顶部了，加载更多内容
        if(value == 0 && res){

            //直接显示全部
            BrowserBuffIndex = BrowserBuff.size();
            hexBrowserBuffIndex = hexBrowserBuff.size();
            //显示内容指数型增加
//            if(BrowserBuffIndex*2 < BrowserBuff.size()){
//                BrowserBuffIndex = BrowserBuffIndex*2;
//            }
//            else{
//                BrowserBuffIndex = BrowserBuff.size();
//            }
//            if(hexBrowserBuffIndex*2 < hexBrowserBuff.size()){
//                hexBrowserBuffIndex = hexBrowserBuffIndex*2;
//            }
//            else{
//                hexBrowserBuffIndex = hexBrowserBuff.size();
//            }

            if(ui->hexDisplay->isChecked()){
                ui->textBrowser->setText(hexBrowserBuff.mid(hexBrowserBuff.size() - hexBrowserBuffIndex));
            }else{
                ui->textBrowser->setText(BrowserBuff.mid(BrowserBuff.size() - BrowserBuffIndex));
            }

            //保持bar位置不动
            newValue = bar->maximum()-oldMax;
            bar->setValue(newValue);
        }
    }
}

void MainWindow::on_actionLinePlot_triggered()
{
    ui->actionLinePlot->setChecked(true);
    ui->actionScatterLinePlot->setChecked(false);
    ui->actionScatterPlot->setChecked(false);
    ui->customPlot->plotControl->setupLineType(ui->customPlot, QCustomPlotControl::Line);
}

void MainWindow::on_actionScatterLinePlot_triggered()
{
    ui->actionLinePlot->setChecked(false);
    ui->actionScatterLinePlot->setChecked(true);
    ui->actionScatterPlot->setChecked(false);
    ui->customPlot->plotControl->setupLineType(ui->customPlot, QCustomPlotControl::ScatterLine);
}

void MainWindow::on_actionScatterPlot_triggered()
{
    ui->actionLinePlot->setChecked(false);
    ui->actionScatterLinePlot->setChecked(false);
    ui->actionScatterPlot->setChecked(true);
    ui->customPlot->plotControl->setupLineType(ui->customPlot, QCustomPlotControl::Scatter);
}

void MainWindow::on_actionResetDefaultConfig_triggered(bool checked)
{
    if(checked)
    {
        QMessageBox::Button button = QMessageBox::warning(this,"警告：确认恢复默认设置吗？","该操作会重置软件初始状态！",QMessageBox::Ok|QMessageBox::No);
        if(button == QMessageBox::No)
            return;
        needSaveConfig = false;
        QMessageBox::information(this, "提示", "重置成功。请重启程序。");
    }else{
        needSaveConfig = true;
    }
}

void MainWindow::on_actionManual_triggered()
{
    //创建关于我对话框资源
    About_Me_Dialog* p = new About_Me_Dialog(this);
    p->getVersionString(Config::getVersion());
    p->showManualDoc();
    p->resize(1024,768);
    //设置close后自动销毁
    p->setAttribute(Qt::WA_DeleteOnClose);
    //非阻塞式显示
    p->show();
}

void MainWindow::on_actionSavePlotData_triggered()
{
    //打开保存文件对话框
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "保存绘图数据-选择文件路径",
                                                    lastFileDialogPath + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".xlsx",
                                                    "XLSX File(*.xlsx);;CSV File(*.csv);;TXT File(*.txt);;All File(*.*)");
    //检查路径格式
    if(!savePath.endsWith(".xlsx") &&
       !savePath.endsWith(".csv") &&
       !savePath.endsWith(".txt")){
        if(!savePath.isEmpty())
            QMessageBox::information(this,"提示","尚未支持的文件格式。请选择xlsx或者csv或者txt格式文件。");
        return;
    }

    //记录路径
    lastFileDialogPath = savePath;
    lastFileDialogPath = lastFileDialogPath.mid(0, lastFileDialogPath.lastIndexOf('/')+1);

    bool ok = false;
    if(savePath.endsWith(".xlsx")){
        if(MyXlsx::write(ui->customPlot, savePath))
            ok = true;
    }else if(savePath.endsWith(".csv")){
        if(ui->customPlot->saveGraphAsTxt(savePath,','))
            ok = true;
    }else if(savePath.endsWith(".txt")){
        if(ui->customPlot->saveGraphAsTxt(savePath,' '))
            ok = true;
    }

    if(ok){
        QString str = enter + "Save successful in "+savePath + enter;
        BrowserBuff.append(str);
        hexBrowserBuff.append(toHexDisplay(str.toLocal8Bit()));
        printToTextBrowser();
    }else{
        QMessageBox::warning(this, "警告", "保存失败。文件是否被其他软件占用？");
    }

}

void MainWindow::on_actionSavePlotAsPicture_triggered()
{
    //打开保存文件对话框
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "曲线保存图片-选择文件路径",
                                                    lastFileDialogPath + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"),
                                                    "Bmp File(*.bmp);;Pdf File(*.pdf);;Jpeg File(*.jpg);;Png File(*.png);;All File(*.*)");
    //检查路径格式
    if(!savePath.endsWith(".jpg") &&
       !savePath.endsWith(".bmp") &&
       !savePath.endsWith(".png") &&
       !savePath.endsWith(".pdf")){
        if(!savePath.isEmpty())
            QMessageBox::information(this,"提示","尚未支持的文件格式。请选择jpg/bmp/png/pdf文件。");
        return;
    }
    //记录上次路径
    lastFileDialogPath = savePath;
    lastFileDialogPath = lastFileDialogPath.mid(0, lastFileDialogPath.lastIndexOf('/')+1);

    //保存
    bool ok = false;
    if(savePath.endsWith(".jpg")){
        if(ui->customPlot->saveJpg(savePath,0,0,1,100))
            ok = true;
    }
    if(savePath.endsWith(".bmp")){
        if(ui->customPlot->saveBmp(savePath))
            ok = true;
    }
    if(savePath.endsWith(".png")){
        if(ui->customPlot->savePng(savePath,0,0,1,100))
            ok = true;
    }
    if(savePath.endsWith(".pdf")){
        if(ui->customPlot->savePdf(savePath))
            ok = true;
    }

    if(ok){
        QString str = enter + "Save successful in "+savePath + enter;
        BrowserBuff.append(str);
        hexBrowserBuff.append(toHexDisplay(str.toLocal8Bit()));
        printToTextBrowser();
    }else{
        QMessageBox::warning(this, "警告", "保存失败。文件是否被其他软件占用？");
    }
}

void MainWindow::on_actionKeyWordHighlight_triggered(bool checked)
{
    if(checked){
        if(highlighter==nullptr)
            highlighter = new Highlighter(ui->textBrowser->document());
    }else{
        delete highlighter;
        highlighter = nullptr;
    }
}
/*
 * Funciont:显示使用统计
*/
void MainWindow::on_actionUsageStatistic_triggered()
{
    double currentTx = serial.getTxCnt();
    double currentRx = serial.getRxCnt();
    double currentRunTime_f = currentRunTime;
    double totalTx = Config::getTotalTxCnt().toUInt() + currentTx;
    double totalRx = Config::getTotalRxCnt().toUInt() + currentRx;
    double totalRunTime = Config::getTotalRunTime().toUInt() + currentRunTime_f;
    double totalTxRx_MB = totalTx/1024/1024 + totalRx/1024/1024;
    //单位
    QString totalTxUnit;
    QString totalRxUnit;
    QString currentTxUnit;
    QString currentRxUnit;
    //时间换算
    QString days;
    QString hou;
    QString min;
    QString sec;
    QString currentRunTimeStr, totalRunTimeStr;
    long day;
    long hour;
    long minute;
    long second;
    int nest = 0; //执行统计

    //单位换算
    nest = 0;
    while(totalTx>1024){
        totalTx = totalTx/1024;
        nest++;
    }
    switch(nest){
        case 0:totalTxUnit = " B";break;
        case 1:totalTxUnit = " KB";break;
        case 2:totalTxUnit = " MB";break;
        case 3:totalTxUnit = " GB";break;
        case 4:totalTxUnit = " TB";break;
    }
    //单位换算
    nest = 0;
    while(totalRx>1024){
        totalRx = totalRx/1024;
        nest++;
    }
    switch(nest){
        case 0:totalRxUnit = " B";break;
        case 1:totalRxUnit = " KB";break;
        case 2:totalRxUnit = " MB";break;
        case 3:totalRxUnit = " GB";break;
        case 4:totalRxUnit = " TB";break;
    }
    //单位换算
    nest = 0;
    while(currentTx>1024){
        currentTx = currentTx/1024;
        nest++;
    }
    switch(nest){
        case 0:currentTxUnit = " B";break;
        case 1:currentTxUnit = " KB";break;
        case 2:currentTxUnit = " MB";break;
        case 3:currentTxUnit = " GB";break;
        case 4:currentTxUnit = " TB";break;
    }
    //单位换算
    nest = 0;
    while(currentRx>1024){
        currentRx = currentRx/1024;
        nest++;
    }
    switch(nest){
        case 0:currentRxUnit = " B";break;
        case 1:currentRxUnit = " KB";break;
        case 2:currentRxUnit = " MB";break;
        case 3:currentRxUnit = " GB";break;
        case 4:currentRxUnit = " TB";break;
    }
    //时间常数
    int mi = 60;
    int hh = mi * 60;
    int dd = hh * 24;
    //时间换算
    day = static_cast<long>(currentRunTime_f / dd);
    hour = static_cast<long>((currentRunTime_f - day * dd) / hh);
    minute = static_cast<long>((currentRunTime_f - day * dd - hour * hh) / mi);
    second = static_cast<long>((currentRunTime_f - day * dd - hour * hh - minute * mi));

    days = QString::number(day,10);
    hou = QString::number(hour,10);
    min = QString::number(minute,10);
    sec = QString::number(second,10);
    currentRunTimeStr = days + " 天 " + hou + " 小时 " + min + " 分钟 " + sec + " 秒";
    //时间换算
    day = static_cast<long>(totalRunTime / dd);
    hour = static_cast<long>((totalRunTime - day * dd) / hh);
    minute = static_cast<long>((totalRunTime - day * dd - hour * hh) / mi);
    second = static_cast<long>((totalRunTime - day * dd - hour * hh - minute * mi));

    days = QString::number(day,10);
    hou = QString::number(hour,10);
    min = QString::number(minute,10);
    sec = QString::number(second,10);
    totalRunTimeStr = days + " 天 " + hou + " 小时 " + min + " 分钟 " + sec + " 秒";

    QString rankStr;
    if(totalTxRx_MB<100){
        rankStr = "恭喜您，获得了【青铜码农】的称号！请再接再厉！";
    }else if(totalTxRx_MB<200){
        rankStr = "恭喜您，获得了【白银码农】的称号！请再接再厉！";
    }else if(totalTxRx_MB<400){
        rankStr = "恭喜您，获得了【黄金码农】的称号！请再接再厉！";
    }else if(totalTxRx_MB<800){
        rankStr = "恭喜您，获得了【铂金码农】的称号！请再接再厉！";
    }else if(totalTxRx_MB<1600){
        rankStr = "恭喜您，获得了【星钻码农】的称号！请再接再厉！";
    }else if(totalTxRx_MB<3200){
        rankStr = "恭喜您，获得了【皇冠码农】的称号！请再接再厉！";
    }else if(totalTxRx_MB<6400){
        rankStr = "恭喜您，获得了【王牌码农】的称号！请再接再厉！";
    }else{
        rankStr = "荣誉只是浮云~";
    }

    //上屏显示
    //ui->textBrowser->clear(); //如果清屏的话要做提示，可能用户数据还未保存
    ui->textBrowser->append("软件版本："+Config::getVersion());
    ui->textBrowser->append("");
    ui->textBrowser->append("【设备信息】");
    ui->textBrowser->append("   MAC地址："+HTTP::getHostMacAddress());
    ui->textBrowser->append("");
    ui->textBrowser->append("【软件使用统计】");
    ui->textBrowser->append("   自本次启动软件以来，您：");
    ui->textBrowser->append("   - 共发送数据："+QString::number(currentTx,'f',2)+currentTxUnit);
    ui->textBrowser->append("   - 共接收数据："+QString::number(currentRx,'f',2)+currentRxUnit);
    ui->textBrowser->append("   - 共运行本软件："+currentRunTimeStr);
    ui->textBrowser->append("   自首次启动软件以来，您：");
    ui->textBrowser->append("   - 共发送数据："+QString::number(totalTx,'f',2)+totalTxUnit);
    ui->textBrowser->append("   - 共接收数据："+QString::number(totalRx,'f',2)+totalRxUnit);
    ui->textBrowser->append("   - 共运行本软件："+totalRunTimeStr);
    ui->textBrowser->append("   - 共启动本软件："+QString::number(Config::getTotalRunCnt().toInt()+1)+" 次");
    ui->textBrowser->append("");
    ui->textBrowser->append("   "+rankStr);
    ui->textBrowser->append("");
    ui->textBrowser->append("【隐私声明】");
    ui->textBrowser->append("  - 以上统计信息可能会被上传至服务器用于统计。");
    ui->textBrowser->append("  - 其他任何信息均不会被上传。");
    ui->textBrowser->append("  - 如您不同意本声明，可阻断本软件的网络请求或者您应该停止使用本软件。");
    ui->textBrowser->append("");
    ui->textBrowser->append("感谢您的使用");
    ui->textBrowser->append("");

    QString str = ui->textBrowser->document()->toPlainText();
    BrowserBuff.clear();
    BrowserBuff.append(str);
    hexBrowserBuff.clear();
    hexBrowserBuff.append(toHexDisplay(str.toLocal8Bit()));
}

void MainWindow::on_actionSendFile_triggered()
{
    static QString lastFileName;
    //打开文件对话框
    QString readPath = QFileDialog::getOpenFileName(this,
                                                    "打开文件",
                                                    lastFileDialogPath + lastFileName,
                                                    "All File(*.*)");
    //检查文件路径结尾
    if(readPath.isEmpty()){
        return;
    }

    //记录上次路径
    lastFileDialogPath = readPath;
    lastFileDialogPath = lastFileDialogPath.mid(0, lastFileDialogPath.lastIndexOf('/')+1);
    //读取文件
    QFile file(readPath);

    //读文件
    if(file.open(QFile::ReadOnly)){
        //记录上一次文件名
        lastFileName = readPath;
        while(lastFileName.indexOf('/')!=-1){
            lastFileName = lastFileName.mid(lastFileName.indexOf('/')+1);
        }

        TxBuff.clear();
        TxBuff = file.readAll();
        file.close();

        //文件分包
        #define PACKSIZE_SENDFILE 256
        SendFileBuffIndex = 0;
        SendFileBuff.clear();
        while(TxBuff.size()>PACKSIZE_SENDFILE){
            SendFileBuff.append(TxBuff.mid(0,PACKSIZE_SENDFILE));
            TxBuff.remove(0,PACKSIZE_SENDFILE);
        }
        SendFileBuff.append(TxBuff); //一定会有一个元素
        TxBuff.clear();
        if(SendFileBuff.size()<1){
            return;
        }

        if(serial.isOpen()){
            ui->textBrowser->clear();
            ui->textBrowser->append("File size: "+QString::number(file.size())+" Bytes");
            ui->textBrowser->append("One pack size: "+QString::number(PACKSIZE_SENDFILE)+" Bytes");
            ui->textBrowser->append("Total packs: "+QString::number(SendFileBuff.size())+" packs");
            ui->textBrowser->append("");
            QString str = ui->textBrowser->document()->toPlainText();
            BrowserBuff.clear();
            BrowserBuff.append(str);
            hexBrowserBuff.clear();
            hexBrowserBuff.append(toHexDisplay(str.toLocal8Bit()));
            serial.write(SendFileBuff.at(SendFileBuffIndex++));//后续缓冲的发送在串口发送完成的槽里
        }
        else
            QMessageBox::information(this,"提示","请先打开串口。");
    }else{
        QMessageBox::information(this,"提示","文件打开失败。");
        lastFileName.clear();
    }
}

void MainWindow::on_actionValueDisplay_triggered(bool checked)
{
    if(checked){
        ui->valueDisplay->show();
        //设置宽度
        QList<int> widthList;
        int width = ui->splitter_2->width();
        widthList << static_cast<int>(width*0.75) << static_cast<int>(width*0.25);
        ui->splitter_2->setSizes(widthList);

        //没激活就打开（绘图器也可能激活）
        if(!plotterParseTimer.isActive()){
            plotterParseTimer.start(PLOTTER_PARSE_PERIOD);
            plotterParsePosInRxBuff = RxBuff.size() - 1;
        }
    }else{
        ui->valueDisplay->hide();

        //绘图器也未勾选时才停止定时器
        if(!ui->actionPlotterSwitch->isChecked())
            plotterParseTimer.stop();
    }
}

void MainWindow::on_textBrowser_customContextMenuRequested(const QPoint &pos)
{
    QPoint noWarning = pos;
    noWarning.x();

    QAction *clearTextBrowser = nullptr;
    QAction *saveOriginData = nullptr;
    QAction *saveShowedData = nullptr;
    QAction *tips = nullptr;
    QMenu *popMenu = new QMenu( this );
    //添加右键菜单
    saveOriginData = new QAction("保存原始数据", this);
    saveShowedData = new QAction("保存显示数据", this);
    clearTextBrowser = new QAction("清空数据显示区", this);

    popMenu->addAction( saveOriginData );
    popMenu->addAction( saveShowedData );
    popMenu->addSeparator();
    popMenu->addAction( clearTextBrowser );
    popMenu->addSeparator();
    popMenu->addAction( tips );
    connect( saveOriginData, SIGNAL(triggered() ), this, SLOT( on_actionSaveOriginData_triggered()) );
    connect( saveShowedData, SIGNAL(triggered() ), this, SLOT( on_actionSaveShowedData_triggered()) );
    connect( clearTextBrowser, SIGNAL(triggered() ), this, SLOT( clearTextBrowserSlot()) );
    popMenu->exec( QCursor::pos() );
    delete popMenu;
    delete clearTextBrowser;
}

void MainWindow::clearTextBrowserSlot()
{
    ui->textBrowser->clear();
    RxBuff.clear();
    hexBrowserBuff.clear();
    hexBrowserBuffIndex = 0;
    BrowserBuff.clear();
    BrowserBuffIndex = 0;
    unshowedRxBuff.clear();
}

void MainWindow::on_valueDisplay_customContextMenuRequested(const QPoint &pos)
{
    //消除警告
    QPoint pp = pos;
    pp.isNull();

    QList<QTableWidgetItem*> selectedItems = ui->valueDisplay->selectedItems();
    QAction *deleteValueDisplayRow = nullptr;
    QAction *deleteValueDisplay = nullptr;
    QMenu *popMenu = new QMenu( this );
    //添加右键菜单
    if( selectedItems.size() ){
        deleteValueDisplayRow = new QAction("删除元素所在行", this);
        popMenu->addAction( deleteValueDisplayRow );
        connect( deleteValueDisplayRow, SIGNAL(triggered() ), this, SLOT( deleteValueDisplayRowSlot()) );

        popMenu->addSeparator();
    }
    deleteValueDisplay = new QAction("删除所有行", this);
    popMenu->addAction( deleteValueDisplay );
    connect( deleteValueDisplay, SIGNAL(triggered() ), this, SLOT( deleteValueDisplaySlot()) );
    popMenu->exec( QCursor::pos() );
    delete popMenu;
    delete deleteValueDisplayRow;
    delete deleteValueDisplay;
}

void MainWindow::deleteValueDisplayRowSlot()
{
    QList<QTableWidgetItem*> selectedItems = ui->valueDisplay->selectedItems();

    while(selectedItems.size()){
        ui->valueDisplay->removeRow(selectedItems.at(0)->row());
        selectedItems.pop_front();
    }
}

void MainWindow::deleteValueDisplaySlot()
{
    while(ui->valueDisplay->rowCount()>0){
        ui->valueDisplay->removeRow(0);
    }
}


void MainWindow::on_timeStampCheckBox_stateChanged(int arg1)
{
    if(arg1 != 0){
        ui->timeStampTimeOut->setEnabled(true);
        timeStampTimer.setTimerType(Qt::PreciseTimer);
        timeStampTimer.setSingleShot(true);
        timeStampTimer.start(ui->timeStampTimeOut->text().toInt());
    }else{
        ui->timeStampTimeOut->setEnabled(false);
        timeStampTimer.stop();
    }
}

void MainWindow::on_timeStampTimeOut_textChanged(const QString &arg1)
{
    timeStampTimer.setSingleShot(true);
    timeStampTimer.start(arg1.toInt());
}

void MainWindow::on_actionOpenGL_triggered(bool checked)
{
    if(checked){
        ui->customPlot->setOpenGl(true);
    }
    else{
        ui->customPlot->setOpenGl(false);
    }
    ui->customPlot->replot();
}

void MainWindow::on_actionFontSetting_triggered()
{
    bool ok;
    QFont font;
    font = QFontDialog::getFont(&ok, font, this, "选择字体");
    if(ok){
        g_font = font;
        ui->textBrowser->setFont(g_font);
        ui->textEdit->setFont(g_font);
        ui->multiString->setFont(g_font);
        ui->customPlot->plotControl->setupFont(ui->customPlot, g_font);
    }
}

void MainWindow::on_actionBackGroundColorSetting_triggered()
{
    QColor color;
    color = QColorDialog::getColor(Qt::white, this,
                                          "选择背景色",
                                          QColorDialog::ShowAlphaChannel);
    if(!color.isValid())
        return;

    int r,g,b;
    g_background_color = color;
    g_background_color.getRgb(&r,&g,&b);
    QString str = "background-color: rgb(RGBR,RGBG,RGBB);";
    str.replace("RGBR", QString::number(r));
    str.replace("RGBG", QString::number(g));
    str.replace("RGBB", QString::number(b));
    ui->textBrowser->setStyleSheet(str);
}

void MainWindow::on_actionSumCheck_triggered(bool checked)
{
    ui->actionSumCheck->setChecked(checked);
    g_enableSumCheck = checked;
    if(checked){
        if(ui->actionPlotterSwitch->isChecked()){
            if(ui->actionAscii->isChecked())
                ui->plotter->setTitle("数据可视化：ASCII协议(和校验)");
            else if(ui->actionFloat->isChecked())
                ui->plotter->setTitle("数据可视化：FLOAT协议(和校验)");
        }
    }else{
        if(ui->actionPlotterSwitch->isChecked()){
            if(ui->actionAscii->isChecked())
                ui->plotter->setTitle("数据可视化：ASCII协议");
            else if(ui->actionFloat->isChecked())
                ui->plotter->setTitle("数据可视化：FLOAT协议");
        }
    }
}
