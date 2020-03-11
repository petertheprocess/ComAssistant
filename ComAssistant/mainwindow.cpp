#include "mainwindow.h"
#include "ui_mainwindow.h"

//获取MAC地址
QString getHostMacAddress()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i = 0; i < nCnt; i ++)
    {
        // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            strMacAddr = nets[i].hardwareAddress();
            break;
        }
    }
    return strMacAddr;
}

/*
 * Function:上传使用统计
 * Web端代码：static.php
<?php
    $val = "";
    $fileName = $_GET["filename"];//从URL中获取文件名称,格式http://www.inhowe.com/test.php?filename=a.txt
    @$data = fopen($fileName,'a+');//添加不覆盖，首先会判断这个文件是否存在，如果不存在，则会创建该文件
    //应该以键值对的形式提交信息
    if($_POST){
        $val.='|POST|';
        foreach($_POST as $key =>$value){
            $val .= '|'.$key.":".$value;
        }
    }else{
        $val.='|GET|';
        foreach($_GET as $key =>$value){
                $val .= '|'.$key.":".$value;
        }
    }
    $val.= "\n";
    fwrite($data,$val);//写入文本中
    fclose($data);
?>
*/
bool MainWindow::postUsageStatic(void)
{
    //旧请求未完成时不执行。
    if(httpTimeout>0)
        return false;

    ui->statusBar->showMessage("正在提交使用统计...", 1000);
//    httpFunction = PostStatic;

    //准备上传数据
    QString sendData = "startTime=#STARTTIME#&lastRunTime=#LASTRUNTIME#&lastTxCnt=#LASTTXCNT#&lastRxCnt=#LASTRXCNT#";
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date_str =current_date_time.toString("yyyyMMddhhmmss");
    sendData.replace("#STARTTIME#",current_date_str);
    sendData.replace("#LASTRUNTIME#",Config::getLastRunTime());
    sendData.replace("#LASTTXCNT#",Config::getLastTxCnt());
    sendData.replace("#LASTRXCNT#",Config::getLastRxCnt());

    //以本机MAC地址作为上传的文件名
    QString tmp = "http://www.inhowe.com/ComAssistant/static.php?filename=#FILENAME#.txt";
    tmp.replace("#FILENAME#",getHostMacAddress());
    QUrl url(tmp);

    //request请求
    QNetworkRequest request;
//    request.setSslConfiguration(QSslConfiguration::defaultConfiguration()); //开启ssl，需要在exe下放openssl的lib包
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=utf-8"); //以表单键值对的形式提交
    request.setHeader(QNetworkRequest::ContentLengthHeader, sendData.size());
//    m_NetManger = new QNetworkAccessManager;//重复使用所以不重复定义
    m_Reply = m_NetManger->post(request, sendData.toUtf8());
    return true;
}

bool MainWindow::getRemoteVersion(void)
{
    //旧请求未完成时不执行。
    if(httpTimeout>0)
        return false;

    ui->statusBar->showMessage("正在检查更新，请稍候……", 2000);
//    httpFunction = GetVersion;
    //发起http请求远端的发布版本号
    QUrl url("https://api.github.com/repos/inhowe/ComAssistant/releases");
    QNetworkRequest request;
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setUrl(url);
    m_Reply = m_NetManger->get(request);
    return true;
}

bool MainWindow::downloadAdvertisement(void)
{
    //旧请求未完成时不执行。
    if(httpTimeout>0)
        return false;
//    httpFunction = DownloadADs;
    //发起http请求远端的发布版本号
    QUrl url("http://www.inhowe.com/ComAssistant/ad.txt");
    QNetworkRequest request;
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setUrl(url);
    m_Reply = m_NetManger->get(request);
    return true;
}
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

    //多字符串
    ui->actionMultiString->setChecked(Config::getMultiStringState());
    on_actionMultiString_triggered(Config::getMultiStringState());

    //加载高亮规则
    ui->actionKeyWordHighlight->setChecked(Config::getKeyWordHighlightState());
    on_actionKeyWordHighlight_triggered(Config::getKeyWordHighlightState());

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

    //绘图器
    ui->actionPlotterSwitch->setChecked(Config::getPlotterState());
    if(ui->actionPlotterSwitch->isChecked())
        ui->customPlot->show();
    else
        ui->customPlot->close();
    if(Config::getPlotterType()==ProtocolType_e::Ascii){
        ui->actionAscii->setChecked(true);
        ui->actionFloat->setChecked(false);
    }else{
        ui->actionAscii->setChecked(false);
        ui->actionFloat->setChecked(true);
    }
    //图像名字集
    plotControl.setNameSet(ui->customPlot, Config::getPlotterGraphNames(plotControl.getMaxValidGraphNumber()));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    //槽
    connect(&continuousWriteTimer, SIGNAL(timeout()), this, SLOT(continuousWriteSlot()));
    connect(&autoSubcontractTimer, SIGNAL(timeout()), this, SLOT(autoSubcontractTimerSlot()));
    connect(&secTimer, SIGNAL(timeout()), this, SLOT(secTimerSlot()));
    connect(&serial, SIGNAL(readyRead()), this, SLOT(readSerialPort()));
    connect(&serial, SIGNAL(bytesWritten(qint64)), this, SLOT(serialBytesWritten(qint64)));

    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(ui->customPlot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(ui->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));

    // setup policy and connect slot for context menu popup:
    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    // 坐标跟随
    connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showTracer(QMouseEvent*)));

    //http访问
    m_NetManger = new QNetworkAccessManager(this);
    connect(m_NetManger, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpFinishedSlot(QNetworkReply*)));

    //状态栏标签
    statusAdLabel = new QLabel(this);
    statusSpeedLabel = new QLabel(this);
    statusStatisticLabel = new QLabel(this);
    statusAdLabel->setOpenExternalLinks(true);//可打开外链
    ui->statusBar->addPermanentWidget(statusAdLabel);
    ui->statusBar->addPermanentWidget(statusStatisticLabel);//显示永久信息
    ui->statusBar->addPermanentWidget(statusSpeedLabel);

    //设置波特率框和发送间隔框的合法输入范围
    ui->baudrateList->setValidator(new QIntValidator(0,9999999,this));
    ui->sendInterval->setValidator(new QIntValidator(0,99999,this));

    //加载高亮规则
    on_actionKeyWordHighlight_triggered(ui->actionKeyWordHighlight->isChecked());

    //初始化协议栈
    protocol = new DataProtocol;
    protocol->setProtocolType(DataProtocol::Ascii);

    //初始化绘图器
    plotControl.setupPlotter(ui->customPlot);
    m_Tracer = new MyTracer(ui->customPlot, ui->customPlot->graph(), TracerType::DataTracer);

    //读取配置（所有资源加载完成后、动作执行前读取）
    readConfig();

    //显示收发统计
    serial.resetCnt();
    statusStatisticLabel->setText(serial.getTxRxString());

    //搜寻可用串口
    on_refreshCom_clicked();

    //初始化秒定时器
    secTimer.start(1000);

    //提交使用统计任务
//    postUsageStatic();
//    downloadAdvertisement();
    httpTaskVector.push_back(PostStatic);
    httpTaskVector.push_back(DownloadADs);
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

    //处理http任务请求队列
    if(httpTaskVector.size()>0 && httpTimeout==0){
        switch(httpTaskVector.at(0)){
        case GetVersion:
            getRemoteVersion();break;
        case DownloadFile:
            httpTaskVector.pop_front();break;
        case PostStatic:
            postUsageStatic();break;
        case DownloadADs:
            downloadAdvertisement();break;
        default:
            httpTaskVector.pop_front();break;
        }
        httpTimeout = 5;
    }
    //显示广告
    if(adList.size()>0 && secCnt%10==0){
        statusAdLabel->setText(adList.at(adIndex++));
        if(adIndex==adList.size())
            adIndex = 0;
    }

    //http超时放弃
    if(httpTimeout){
        httpTimeout--;
        if(httpTimeout==0){
            m_Reply->abort();
            if(httpTaskVector.size()>0){
                qDebug()<<"http timed out."<<httpTaskVector.at(0);
                httpTaskVector.pop_front();
            }
            ui->statusBar->showMessage("Http request timed out.", 2000);
        }
    }

    secCnt++;
    currentRunTime++;
}

void MainWindow::debugTimerSlot()
{
    QString tmp;
    static double count;
    double num1, num2, num3;
    num1 = qCos(count)+qSin(count/0.4364)*2.5;
    num2 = qSin(count)+qrand()/static_cast<double>(RAND_MAX)*1*qSin(count/0.3843);
    num3 = qCos(count)*1.5-qSin(count/0.4364)*0.5;

    if(ui->actionAscii->isChecked()){
        tmp = "{:" + QString::number(num1,'f') + "," + QString::number(num2,'f') + "," + QString::number(num3,'f') + "}\r\n";
//        tmp = "{:" + QString::number(num1) + "," + QString::number(num2) + "," + QString::number(num3) + "}\r\n";//这样可以生成一些错误数据
    }

    if(serial.isOpen()){
        serial.write(tmp.toUtf8());
    }
    count = count + 0.1;
}

MainWindow::~MainWindow()
{
    if(needSaveConfig){
        if(ui->actionUTF8->isChecked()){
            Config::setCodeRule(CodeRule_e::UTF8);
        }
        if(ui->action_winLikeEnter->isChecked()){
            Config::setEnterStyle(EnterStyle_e::WinStyle);
        }else if(ui->action_unixLikeEnter->isChecked()){
            Config::setEnterStyle(EnterStyle_e::UnixStyle);
        }
        //general
        Config::setHexSendState(ui->hexSend->isChecked());
        Config::setHexShowState(ui->hexDisplay->isChecked());
        Config::setSendInterval(ui->sendInterval->text().toInt());
        Config::setTimeStampState(ui->timeStampDisplayCheckBox->isChecked());
        Config::setMultiStringState(ui->actionMultiString->isChecked());
        Config::setKeyWordHighlightState(ui->actionKeyWordHighlight->isChecked());
        Config::setVersion();
        //serial
        Config::setBaudrate(serial.baudRate());
        Config::setDataBits(serial.dataBits());
        Config::setStopBits(serial.stopBits());
        Config::setParity(serial.parity());
        Config::setFlowControl(serial.flowControl());
        //plotter
        Config::setPlotterState(ui->actionPlotterSwitch->isChecked());
        if(ui->actionAscii->isChecked())
            Config::setPlotterType(ProtocolType_e::Ascii);
        else
            Config::setPlotterType(ProtocolType_e::Float);
        Config::setPlotterGraphNames(plotControl.getNameSet());
        //static
        Config::setLastRunTime(currentRunTime);
        Config::setTotalRunTime(currentRunTime);
        Config::setLastTxCnt(serial.getTotalTxCnt());//getTotalTxCnt是本次软件运行时的总发送量
        Config::setTotalTxCnt(serial.getTotalTxCnt());
        Config::setLastRxCnt(serial.getTotalRxCnt());
        Config::setTotalRxCnt(serial.getTotalRxCnt());
        Config::setTotalRunCnt(1);
    }

    delete protocol;
    delete highlighter;
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

    //只存在一个串口时自动打开
    if(ui->comList->count()==1 && ui->comList->currentText()!="未找到可用串口!"){
        ui->refreshCom->setChecked(false);
        ui->comSwitch->setChecked(true);
        on_comSwitch_clicked(true);
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
            ui->refreshCom->setEnabled(false);
        }
        else {
            ui->comSwitch->setText("打开串口");
            ui->comSwitch->setChecked(false);
            ui->refreshCom->setEnabled(true);
            QString msg = "请检查下列情况后重新打开串口：\n\n# USB线缆是否松动？\n# 是否选择了正确的串口设备？\n# 该串口是否被其他程序占用？\n# 是否设置了过高的波特率？\n";
            QMessageBox::critical(this, "串口打开失败!", msg, QMessageBox::Ok);
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
        ui->comSwitch->setChecked(false);
        ui->refreshCom->setEnabled(true);
    }
}

/*
 * Function:从串口读取数据
*/
void MainWindow::readSerialPort()
{
    QByteArray tmpReadBuff;

    if(paraseFromRxBuff)
        tmpReadBuff = RxBuff;
    else{
        tmpReadBuff = serial.readAll();
        RxBuff.append(tmpReadBuff);
    }
    //读取数据并衔接到上次未处理完的数据后面
    tmpReadBuff = unshowedRxBuff + tmpReadBuff;
    unshowedRxBuff.clear();

    //速度统计
    statisticRxByteCnt += tmpReadBuff.size();

    if(ui->actionPlotterSwitch->isChecked()){
        //绘图器解析
        protocol->parase(tmpReadBuff);
        while(protocol->parasedBuffSize()>0){
            plotControl.displayToPlotter(ui->customPlot, protocol->popOneRowData());
        }
    }

    //'\r'若单独结尾则可能被误切断，放到下一批数据中
    if(tmpReadBuff.endsWith('\r')){
        unshowedRxBuff.append(tmpReadBuff.at(tmpReadBuff.size()-1));
        tmpReadBuff.remove(tmpReadBuff.size()-1,1);
        if(tmpReadBuff.size()==0)
            return;
    }

    //是否进制转换，不转换则考虑中文处理
    if(ui->hexDisplay->isChecked()){
        tmpReadBuff = toHexDisplay(tmpReadBuff).toUtf8();
    }else if(ui->actionUTF8->isChecked()){
        //UTF8中文字符处理
        //最后一个字符不是ascii字符才处理，否则直接上屏       
        if(tmpReadBuff.back() & 0x80){
            //中文一定有连续3个字符高位为1
            int continuesCnt = 0;
            int lastUTFpos = -1;
            for(int i = 0; i < tmpReadBuff.size(); i++){
                if(tmpReadBuff.at(i)&0x80){
                    continuesCnt++;
                }else {
                    continuesCnt=0;
                }
                if(continuesCnt == 3){
                    continuesCnt = 0;
                    lastUTFpos = i;
                }
            }
            unshowedRxBuff = tmpReadBuff.mid(lastUTFpos+1);
            tmpReadBuff = tmpReadBuff.mid(0,lastUTFpos+1);
        }
//        qDebug()<<unshowedRxBuff<<tmpReadBuff;
    }

    //打印数据
    if(!tmpReadBuff.isEmpty()){
        //移动光标
        ui->textBrowser->moveCursor(QTextCursor::End);
        //追加数据
        if(ui->timeStampDisplayCheckBox->isChecked()){
            //需要添加时间戳
            QString timeString;
            if(!autoSubcontractTimer.isActive()){
                timeString = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
                timeString = "["+timeString+"]Rx<- ";
            }
            ui->textBrowser->insertPlainText(timeString + tmpReadBuff);
            //分包定时器
            autoSubcontractTimer.start(10);
            autoSubcontractTimer.setSingleShot(true);
        }else{
            //不需要时间戳
            ui->textBrowser->insertPlainText(tmpReadBuff);
        }

        //更新收发统计
        statusStatisticLabel->setText(serial.getTxRxString());
    }
}

void MainWindow::serialBytesWritten(qint64 bytes)
{
    //发送速度统计
    statisticTxByteCnt += bytes;
}

/*
 * Function:连续发送定时器槽，执行数据发送
*/
void MainWindow::continuousWriteSlot()
{
    on_sendButton_clicked();
}

/*
 * Function:自动分包定时器槽
*/
void MainWindow::autoSubcontractTimerSlot()
{
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

            //若添加了时间戳则把发送的数据也显示在接收区
            if(ui->timeStampDisplayCheckBox->isChecked()){
                QString timeString;
                timeString = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
                timeString = "["+timeString+"]Tx-> ";
                //如果hex发送则把显示在接收区的发送数据转为hex模式
                if(ui->hexDisplay->isChecked())
                    tmp = toHexDisplay(tmp).toUtf8();
                ui->textBrowser->moveCursor(QTextCursor::End);
                ui->textBrowser->insertPlainText("\r\n" + timeString + tmp + "\r\n");
            }
        }
        else {
            //以hex发送数据
            //HexStringToByteArray函数必须传入格式化后的字符串，如"02 31"
            QByteArray tmp;
            bool ok;
            tmp = HexStringToByteArray(ui->textEdit->toPlainText(),ok);
            if(ok){
                serial.write(tmp);
            }
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
    }else {
        QMessageBox::information(this,"提示","串口未打开");
    }
}

void MainWindow::on_clearWindows_clicked()
{
    //串口
    serial.resetCnt();
    if(serial.isOpen())
        serial.flush();

    //接收区
    ui->textBrowser->clear();
    RxBuff.clear();

    //发送区
//    ui->textEdit->clear();

    //对象缓存
    unshowedRxBuff.clear();
    protocol->clearBuff();
    plotControl.clearPlotter(ui->customPlot, -1);
    ui->customPlot->replot();

    //更新收发统计
    statusStatisticLabel->setText(serial.getTxRxString());
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
        ui->TimerSendCheck->setChecked(true);
        continuousWriteTimer.start(ui->sendInterval->text().toInt());
    }
    else {
        ui->TimerSendCheck->setChecked(false);
        continuousWriteTimer.stop();
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
void MainWindow::on_actionSaveOriginData_triggered()
{
    //如果追加时间戳则提示时间戳不会被保存
    if(ui->timeStampDisplayCheckBox->isChecked())
        QMessageBox::information(this,"提示","时间戳数据不会被保存！只保存接收到的原始数据。");

    //打开保存文件对话框
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "保存数据-选择文件路径",
                                                    lastPath + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".dat",
                                                    "Dat File(*.dat);;All File(*.*)");
    //检查路径格式
    if(!savePath.endsWith(".dat")){
        if(!savePath.isEmpty())
            QMessageBox::information(this,"尚未支持的文件格式","请选择dat文件。");
        return;
    }
    //记忆路径
    lastPath = savePath;
    lastPath = lastPath.mid(0, lastPath.lastIndexOf('/')+1);

    //保存数据
    QFile file(savePath);
    //删除旧数据形式写文件
    if(file.open(QFile::WriteOnly|QFile::Truncate)){
        file.write(RxBuff);//不用DataStream写入非文本文件，它会额外添加4个字节作为文件头
        file.flush();
        file.close();
        ui->textBrowser->append("Total saved "+QString::number(RxBuff.size())+" Bytes in "+savePath);
    }
}

/*
 * Action:读取数据动作触发
 * Function:
*/
void MainWindow::on_actionReadOriginData_triggered()
{   
    static QString lastFileName;
    //打开文件对话框
    QString readPath = QFileDialog::getOpenFileName(this,
                                                    "读取数据-选择文件路径",
                                                    lastPath + lastFileName,
                                                    "Dat File(*.dat);;All File(*.*)");
    //检查文件路径结尾
    if(!readPath.endsWith(".dat")){
        if(!readPath.isEmpty())
            QMessageBox::information(this,"尚未支持的文件格式","请选择dat文件。");
        return;
    }
    //记录上次路径
    lastPath = readPath;
    lastPath = lastPath.mid(0, lastPath.lastIndexOf('/')+1);
    //读取文件
    QFile file(readPath);
    //读文件
    if(file.open(QFile::ReadOnly)){
        //记录上一次文件名
        lastFileName = readPath;
        while(lastFileName.indexOf('/')!=-1){
            lastFileName = lastFileName.mid(lastFileName.indexOf('/')+1);
        }

        RxBuff.clear();
        RxBuff = file.readAll();
        file.close();
        ui->textBrowser->clear();
        ui->textBrowser->append("File size:"+QString::number(file.size())+" Byte");
        ui->textBrowser->append("Read size:"+QString::number(RxBuff.size())+" Byte");
        ui->textBrowser->append("Read containt:\n");
        qDebug()<<"Read:"<<RxBuff;
        // 解析读取的数据
        paraseFromRxBuff = true;
        unshowedRxBuff.clear();
        readSerialPort();
        paraseFromRxBuff = false;
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
 * Function:端口号文本变化，重新打开串口
*/
void MainWindow::on_comList_currentTextChanged(const QString &arg1)
{
    //关闭自动发送功能
    if(ui->TimerSendCheck->isChecked()){
        on_TimerSendCheck_clicked(false);
    }
    if(ui->actiondebug->isChecked()){
        debugTimer.stop();
        ui->actiondebug->setChecked(false);
    }
    ui->statusBar->showMessage("重新启动串口",2000);
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
                                                    lastPath + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".txt",
                                                    "Text File(*.txt);;All File(*.*)");
    //检查路径
    if(!savePath.endsWith("txt")){
        if(!savePath.isEmpty())
            QMessageBox::information(this,"尚未支持的文件格式","请选择txt文本文件。");
        return;
    }
    //记忆路径
    lastPath = savePath;
    lastPath = lastPath.mid(0, lastPath.lastIndexOf('/')+1);
    //保存数据
    QFile file(savePath);
    QTextStream stream(&file);
    //删除旧数据形式写文件
    if(file.open(QFile::WriteOnly|QFile::Text|QFile::Truncate)){
        stream<<ui->textBrowser->toPlainText();
        file.close();
    }
}

void MainWindow::on_actionUpdate_triggered()
{
    httpTaskVector.push_back(GetVersion);
}

void MainWindow::on_sendInterval_textChanged(const QString &arg1)
{
    if(continuousWriteTimer.isActive())
        continuousWriteTimer.setInterval(arg1.toInt());
}

void MainWindow::on_actionSTM32_ISP_triggered()
{
    on_comSwitch_clicked(false);
    STM32ISP_Dialog *p = new STM32ISP_Dialog(this);
    p->exec();
    delete p;
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
        ui->multiString->setAlternatingRowColors(true);
    }else {
        ui->multiString->close();
    }
}

/*
 * Function:多字符串右键菜单
*/
void MainWindow::on_multiString_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem* curItem = ui->multiString->itemAt( pos );
    QAction *clearSeeds = nullptr;
    QAction *deleteSeed = nullptr;
    QMenu *popMenu = new QMenu( this );
    //添加右键菜单
    if( curItem != nullptr ){
        deleteSeed = new QAction(tr("删除"), this);
        popMenu->addAction( deleteSeed );
        connect( deleteSeed, SIGNAL(triggered() ), this, SLOT( deleteSeedSlot()) );
    }
    clearSeeds = new QAction(tr("清除"), this);

    popMenu->addAction( clearSeeds );
    connect( clearSeeds, SIGNAL(triggered() ), this, SLOT( clearSeedsSlot()) );
    popMenu->exec( QCursor::pos() );
    delete popMenu;
    delete clearSeeds;
    delete deleteSeed;
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
    }else{
        ui->customPlot->close();
    }
}

void MainWindow::on_actionAscii_triggered(bool checked)
{
    ui->actionFloat->setChecked(!checked);
}

void MainWindow::on_actionFloat_triggered(bool checked)
{
    ui->actionAscii->setChecked(!checked);
}

void MainWindow::on_actiondebug_triggered(bool checked)
{
    if(checked){
        debugTimer.start(50);
        connect(&debugTimer, SIGNAL(timeout()), this, SLOT(debugTimerSlot()));
    }else{
        debugTimer.stop();
        disconnect(&debugTimer, SIGNAL(timeout()), this, SLOT(debugTimerSlot()));
    }
}

/*plotter交互*/

void MainWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      ui->customPlot->replot();
    }
  }
}

void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      ui->customPlot->replot();
    }
  }
}

void MainWindow::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->customPlot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->customPlot->graph(i);
    QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}

void MainWindow::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
    QCPRange range = ui->customPlot->axisRect()->axis(QCPAxis::atBottom, 0)->range();
    plotControl.adjustXRange(ui->customPlot, range);
  }
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
  }
  else{
    //只调X轴
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal);
    QCPRange range = ui->customPlot->axisRect()->axis(QCPAxis::atBottom, 0)->range();
    plotControl.adjustXRange(ui->customPlot, range);
  }
}

void MainWindow::removeSelectedGraph()
{
  if (ui->customPlot->selectedGraphs().size() > 0)
  {
    ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
    ui->customPlot->replot();
  }
}

void MainWindow::removeAllGraphs()
{
    //使用给对象赋值空数据的方法清空，而不是删除对象。
    plotControl.clearPlotter(ui->customPlot,-1);
//  ui->customPlot->clearGraphs();
//  ui->customPlot->replot();
}

void MainWindow::hideSelectedGraph()
{
    if (ui->customPlot->selectedGraphs().size() > 0)
    {
        //获取图像编号
        int index = 0;
        for(;index < ui->customPlot->graphCount(); index++){
            if(ui->customPlot->graph(index)->name() == ui->customPlot->selectedGraphs().first()->name()){
                break;
            }
        }
        //可见性控制
        if(ui->customPlot->selectedGraphs().first()->visible()){
            ui->customPlot->selectedGraphs().first()->setVisible(false);
            ui->customPlot->legend->item(index)->setTextColor(Qt::gray);
        }
        else{
            ui->customPlot->selectedGraphs().first()->setVisible(true);
            ui->customPlot->legend->item(index)->setTextColor(Qt::black);
        }
      ui->customPlot->replot();
    }
}

void MainWindow::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  if (ui->customPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
  {
    menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignTop|Qt::AlignLeft));
    menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignTop|Qt::AlignHCenter));
    menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignTop|Qt::AlignRight));
    menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignBottom|Qt::AlignRight));
    menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData(static_cast<int>(Qt::AlignBottom|Qt::AlignLeft));
  } else  // general context menu on graphs requested
  {
    if (ui->customPlot->graphCount() > 0)
      menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
  }
  if (ui->customPlot->selectedGraphs().size() > 0){
    menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
    if(ui->customPlot->selectedGraphs().first()->visible()){
        menu->addAction("Hide selected graph", this, SLOT(hideSelectedGraph()));
    }else{
        menu->addAction("Show selected graph", this, SLOT(hideSelectedGraph()));
    }
  }

  menu->popup(ui->customPlot->mapToGlobal(pos));
}

void MainWindow::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, static_cast<Qt::Alignment>(dataInt));
      ui->customPlot->replot();
    }
  }
}

void MainWindow::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
  double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
  qDebug()<<message;
}


void MainWindow::on_actionLinePlot_triggered()
{
    ui->actionLinePlot->setChecked(true);
    ui->actionScatterLinePlot->setChecked(false);
    ui->actionScatterPlot->setChecked(false);
    plotControl.setupLineType(ui->customPlot, QCustomPlotControl::Line);
}

void MainWindow::on_actionScatterLinePlot_triggered()
{
    ui->actionLinePlot->setChecked(false);
    ui->actionScatterLinePlot->setChecked(true);
    ui->actionScatterPlot->setChecked(false);
    plotControl.setupLineType(ui->customPlot, QCustomPlotControl::ScatterLine);
}

void MainWindow::on_actionScatterPlot_triggered()
{
    ui->actionLinePlot->setChecked(false);
    ui->actionScatterLinePlot->setChecked(false);
    ui->actionScatterPlot->setChecked(true);
    plotControl.setupLineType(ui->customPlot, QCustomPlotControl::Scatter);
}

void MainWindow::showTracer(QMouseEvent *event)
{
    if(ui->customPlot->selectedGraphs().size() <= 0){
        m_Tracer->setVisible(false);
        ui->customPlot->replot();
        return;
    }
    m_Tracer->setVisible(true);

    //获取x轴
    double x = ui->customPlot->xAxis->pixelToCoord(event->pos().x());

    //寻找Y轴
    double y = 0;
    QSharedPointer<QCPGraphDataContainer> tmpContainer;
    tmpContainer = ui->customPlot->selectedGraphs().first()->data();
    //使用二分法快速查找所在点数据！！！敲黑板，下边这段是重点
    int low = 0, high = tmpContainer->size();
    while(high > low)
    {
        int middle = (low + high) / 2;
        if(x < tmpContainer->constBegin()->mainKey() ||
           x > (tmpContainer->constEnd()-1)->mainKey())
            break;

        if(x == (tmpContainer->constBegin() + middle)->mainKey())
        {
            y = (tmpContainer->constBegin() + middle)->mainValue();
            break;
        }
        if(x > (tmpContainer->constBegin() + middle)->mainKey())
        {
            low = middle;
        }
        else if(x < (tmpContainer->constBegin() + middle)->mainKey())
        {
            high = middle;
        }
        if(high - low <= 1)
        {   //差值计算所在位置数据
            y = (tmpContainer->constBegin()+low)->mainValue() + ( (x - (tmpContainer->constBegin() + low)->mainKey()) *
                ((tmpContainer->constBegin()+high)->mainValue() - (tmpContainer->constBegin()+low)->mainValue()) ) /
                ((tmpContainer->constBegin()+high)->mainKey() - (tmpContainer->constBegin()+low)->mainKey());
            break;
        }
    }

    //范围约束
    QCPRange xRange = ui->customPlot->axisRect()->axis(QCPAxis::atBottom, 0)->range();
    QCPRange yRange = ui->customPlot->axisRect()->axis(QCPAxis::atLeft, 0)->range();
    if(x > xRange.upper)
        x = xRange.upper;
    if(x < xRange.lower)
        x = xRange.lower;
    if(y > yRange.upper)
        y = yRange.upper;
    if(y < yRange.lower)
        y = yRange.lower;

    //更新Tracer
    QString text = "X:" + QString::number(x, 'f', 2) + " Y:" + QString::number(y, 'f', 2);
    m_Tracer->updatePosition(x, y);
    m_Tracer->setText(text);

    ui->customPlot->replot();
}

void MainWindow::on_actionResetDefaultConfig_triggered(bool checked)
{
    if(checked)
    {
        QMessageBox::Button button = QMessageBox::warning(this,"警告：确认恢复默认设置吗？","该操作会重置软件初始状态，统计信息也会被删除！",QMessageBox::Ok|QMessageBox::No);
        if(button == QMessageBox::No)
            return;

        QFile file(SAVE_PATH);
        if(file.exists()){
            if(file.remove()){
                needSaveConfig = false;
                QMessageBox::information(this, "提示", "重置成功。请重启程序。");
            }else{
                QMessageBox::information(this, "提示", "操作失败。请自行删除程序目录下的" + QString(SAVE_PATH) + "文件。");
            }
            return;
        }
        needSaveConfig = false;
        QMessageBox::information(this, "提示", "重置成功。请重启程序。");
    }else{
        needSaveConfig = true;
    }
}

void MainWindow::on_actionManual_triggered()
{
    QFile file("manual.html");
    QString html;
    if(file.exists()){
        if(file.open(QFile::ReadOnly)){
            html = file.readAll();
            file.close();
            ui->textBrowser->clear();
            ui->textBrowser->append(html);
        }else{
            QMessageBox::information(this, "提示", "帮助文件被占用。");
        }
    }else{
        QMessageBox::information(this, "提示", "帮助文件丢失。");
    }

}

/*
 * Function:保存图像为文本格式，可以选择不同的分隔符，csv文件可以用,
*/
bool MainWindow::saveGraphAsTxt(const QString& filePath, char separate)
{
    //列表头和值
    double value;
    QString txtBuff;
    QSharedPointer<QCPGraphDataContainer> tmpContainer;

    //构造表头
    for(int j = 0; j < ui->customPlot->graphCount(); j++){
        txtBuff += ui->customPlot->graph(j)->name() + separate;
    }
    txtBuff += "\n";

    //构造数据行
    int dataLen = ui->customPlot->graph(0)->data()->size();
    for(int i = 0; i < dataLen; i++){
        for(int j = 0; j < ui->customPlot->graphCount(); j++){
            tmpContainer = ui->customPlot->graph(j)->data();
            value = (tmpContainer->constBegin()+i)->mainValue();
            txtBuff += QString::number(value,'f') + separate;
        }
        txtBuff += "\n";
    }

    QFile file(filePath);
    if(!file.open(QFile::WriteOnly|QFile::Text))
        return false;
    file.write(txtBuff.toUtf8());
    file.flush();
    file.close();
    return true;
}

void MainWindow::on_actionSavePlotData_triggered()
{
    //打开保存文件对话框
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "保存数据-选择文件路径",
                                                    lastPath + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")+".xlsx",
                                                    "XLSX File(*.xlsx);;CSV File(*.csv);;TXT File(*.txt);;All File(*.*)");
    //检查路径格式
    if(!savePath.endsWith(".xlsx") && !savePath.endsWith(".csv")&& !savePath.endsWith(".txt")){
        if(!savePath.isEmpty())
            QMessageBox::information(this,"提示","尚未支持的文件格式。请选择xlsx或者csv或者txt格式文件。");
        return;
    }
    if(savePath.endsWith(".xlsx")){
        if(!MyXlsx::write(ui->customPlot, savePath))
            QMessageBox::warning(this, "警告", "保存失败。文件是否被其他软件占用？");
    }else if(savePath.endsWith(".csv")){
        if(!saveGraphAsTxt(savePath,','))
            QMessageBox::warning(this, "警告", "保存失败。文件是否被其他软件占用？");
    }else if(savePath.endsWith(".txt")){
        if(!saveGraphAsTxt(savePath,' '))
            QMessageBox::warning(this, "警告", "保存失败。文件是否被其他软件占用？");
    }
    lastPath = savePath;
    lastPath = lastPath.mid(0, lastPath.lastIndexOf('/')+1);
}

void MainWindow::on_actionSavePlotAsPicture_triggered()
{
    //打开保存文件对话框
    QString savePath = QFileDialog::getSaveFileName(this,
                                                    "保存图片-选择文件路径",
                                                    lastPath + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"),
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
    lastPath = savePath;
    lastPath = lastPath.mid(0, lastPath.lastIndexOf('/')+1);
    //保存
    if(savePath.endsWith(".jpg")){
        if(!ui->customPlot->saveJpg(savePath,0,0,1,100))
            QMessageBox::warning(this, "警告", "保存失败。文件是否被其他软件占用？");
        return;
    }
    if(savePath.endsWith(".bmp")){
        if(!ui->customPlot->saveBmp(savePath))
            QMessageBox::warning(this, "警告", "保存失败。文件是否被其他软件占用？");
        return;
    }
    if(savePath.endsWith(".png")){
        if(!ui->customPlot->savePng(savePath,0,0,1,100))
            QMessageBox::warning(this, "警告", "保存失败。文件是否被其他软件占用？");
        return;
    }
    if(savePath.endsWith(".pdf")){
        if(!ui->customPlot->savePdf(savePath))
            QMessageBox::warning(this, "警告", "保存失败。文件是否被其他软件占用？");
        return;
    }
}

void MainWindow::httpFinishedSlot(QNetworkReply *)
{
    //超时定时器清0
    httpTimeout = 0;
    HttpFunction_e state;
    if(httpTaskVector.size()>0){
        state = httpTaskVector.at(0); //提取，防止超时pop
        httpTaskVector.pop_front();
    }
    else{
        state = Idle;
    }

    m_Reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    m_Reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (m_Reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = m_Reply->readAll();
        QString string = QString::fromUtf8(bytes);

        if(state == GetVersion){
            QString remoteVersion;
            QString remoteNote;
            remoteVersion = string.mid(string.indexOf("tag_name"));
            remoteVersion = remoteVersion.mid(remoteVersion.indexOf(":")+2, remoteVersion.indexOf(",") - remoteVersion.indexOf(":")-3);
            remoteNote = string.mid(string.indexOf("body"));
            remoteNote = remoteNote.mid(remoteNote.indexOf(":\"")+2, remoteNote.indexOf("}") - remoteNote.indexOf(":\"")-3);
            while(remoteNote.indexOf("\\r\\n")!=-1){
                remoteNote = remoteNote.replace("\\r\\n","\n");
            }
//            qDebug()<<remoteVersion<<remoteNote;
            QMessageBox::Button button;
            button = QMessageBox::information(this,"提示","当前版本号："+Config::getVersion()+
                                                "\n远端版本号："+remoteVersion+
                                                "\n更新内容："+
                                                "\n"+remoteNote, QMessageBox::Ok|QMessageBox::No);
            if(button == QMessageBox::Ok)
                QDesktopServices::openUrl(QUrl("https://github.com/inhowe/ComAssistant/releases"));
        }else if(state == PostStatic){
            if(!string.isEmpty())
                qDebug()<<"PostStatic:"<<string;
        }else if(state == DownloadADs){
            //把下载的广告添加进变量
            adList = string.split('\n',QString::SkipEmptyParts);
        }else{
            qDebug()<<string;
        }
    }
    else
    {
        if(state == GetVersion){
            QMessageBox::information(this,"提示","当前版本号："+Config::getVersion()+
                                          "\n检查更新失败。"+
                                          "\n请访问：https://github.com/inhowe/ComAssistant/releases");
        }else if(state == PostStatic){

        }
        qDebug()<< m_Reply->errorString();
        m_Reply->abort();
    }

    m_Reply->deleteLater();
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
void MainWindow::on_actionUsageStatic_triggered()
{
    double currentTx = serial.getTxCnt();
    double currentRx = serial.getRxCnt();
    double currentRunTime_f = currentRunTime;
    double totalTx = Config::getTotalTxCnt().toUInt() + currentTx;
    double totalRx = Config::getTotalRxCnt().toUInt() + currentRx;
    double totalRunTime = Config::getTotalRunTime().toUInt() + currentRunTime_f;
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
        case 0:totalTxUnit = "B";break;
        case 1:totalTxUnit = "KB";break;
        case 2:totalTxUnit = "MB";break;
        case 3:totalTxUnit = "GB";break;
        case 4:totalTxUnit = "TB";break;
    }
    //单位换算
    nest = 0;
    while(totalRx>1024){
        totalRx = totalRx/1024;
        nest++;
    }
    switch(nest){
        case 0:totalRxUnit = "B";break;
        case 1:totalRxUnit = "KB";break;
        case 2:totalRxUnit = "MB";break;
        case 3:totalRxUnit = "GB";break;
        case 4:totalRxUnit = "TB";break;
    }
    //单位换算
    nest = 0;
    while(currentTx>1024){
        currentTx = currentTx/1024;
        nest++;
    }
    switch(nest){
        case 0:currentTxUnit = "B";break;
        case 1:currentTxUnit = "KB";break;
        case 2:currentTxUnit = "MB";break;
        case 3:currentTxUnit = "GB";break;
        case 4:currentTxUnit = "TB";break;
    }
    //单位换算
    nest = 0;
    while(currentRx>1024){
        currentRx = currentRx/1024;
        nest++;
    }
    switch(nest){
        case 0:currentRxUnit = "B";break;
        case 1:currentRxUnit = "KB";break;
        case 2:currentRxUnit = "MB";break;
        case 3:currentRxUnit = "GB";break;
        case 4:currentRxUnit = "TB";break;
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
    currentRunTimeStr = days + "天 " + hou + "小时 " + min + "分钟 " + sec + "秒";
    //时间换算
    day = static_cast<long>(totalRunTime / dd);
    hour = static_cast<long>((totalRunTime - day * dd) / hh);
    minute = static_cast<long>((totalRunTime - day * dd - hour * hh) / mi);
    second = static_cast<long>((totalRunTime - day * dd - hour * hh - minute * mi));

    days = QString::number(day,10);
    hou = QString::number(hour,10);
    min = QString::number(minute,10);
    sec = QString::number(second,10);
    totalRunTimeStr = days + "天 " + hou + "小时 " + min + "分钟 " + sec + "秒";
    //上屏显示
    ui->textBrowser->clear();
    ui->textBrowser->append("<h2>设备信息：</h2>");
    ui->textBrowser->append("MAC地址："+getHostMacAddress());
    ui->textBrowser->append("<h2>软件使用统计</h2>");
    ui->textBrowser->append("<h3>自本次启动软件以来，您：</h3>");
    ui->textBrowser->append("共发送数据："+QString::number(currentTx,'f',2)+currentTxUnit);
    ui->textBrowser->append("共接收数据："+QString::number(currentRx,'f',2)+currentRxUnit);
    ui->textBrowser->append("共运行本软件："+currentRunTimeStr);
    ui->textBrowser->append("<h3>自首次启动软件以来，您：</h3>");
    ui->textBrowser->append("共发送数据："+QString::number(totalTx,'f',2)+totalTxUnit);
    ui->textBrowser->append("共接收数据："+QString::number(totalRx,'f',2)+totalRxUnit);
    ui->textBrowser->append("共运行本软件："+totalRunTimeStr);
    ui->textBrowser->append("共启动本软件："+QString::number(Config::getTotalRunCnt().toInt()+1)+"次");
    ui->textBrowser->append("<h2>隐私声明</h2>");
    ui->textBrowser->append("以上统计信息可能会被上传至服务器用于统计。");
    ui->textBrowser->append("其他任何信息均不会被上传。");
    ui->textBrowser->append("如您不同意本声明，可通过系统防火墙阻断本软件的网络请求或者您应该停止使用本软件。");
    ui->textBrowser->append("<h2>感谢您的使用</h2>");
    ui->textBrowser->append("<p>");
}

void MainWindow::on_actionSendFile_triggered()
{
    static QString lastFileName;
    //打开文件对话框
    QString readPath = QFileDialog::getOpenFileName(this,
                                                    "打开文件",
                                                    lastPath + lastFileName,
                                                    "All File(*.*)");
    //检查文件路径结尾
    if(readPath.isEmpty()){
        return;
    }
    //记录上次路径
    lastPath = readPath;
    lastPath = lastPath.mid(0, lastPath.lastIndexOf('/')+1);
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
        if(serial.isOpen()){
            serial.write(TxBuff);
        }
        else
            QMessageBox::information(this,"提示","请先打开串口。");
    }else{
        QMessageBox::information(this,"提示","文件打开失败。");
        lastFileName.clear();
    }
}
