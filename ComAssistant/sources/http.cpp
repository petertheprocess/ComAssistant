#include "http.h"

HTTP::HTTP(QWidget *parentWidget)
{
    parent = parentWidget;

    //http访问
    m_NetManger = new QNetworkAccessManager(this);
    connect(m_NetManger, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpFinishedSlot(QNetworkReply*)));

    //提交使用统计任务
    httpTaskVector.push_back(PostStatic);
    httpTaskVector.push_back(DownloadMSGs);
    httpTaskVector.push_back(BackStageGetVersion);

    connect(&secTimer, SIGNAL(timeout()), this, SLOT(httpTimeoutHandle()));
    secTimer.start(1000);
}

//获取MAC地址
QString HTTP::getHostMacAddress()
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
bool HTTP::postUsageStatistic(void)
{
    //旧请求未完成时不执行。
    if(httpTimeout>0)
        return false;

//    ui->statusBar->showMessage("正在提交使用统计...", 1000);

    //准备上传数据
    //为了不改动服务器端程序，不要打乱已有的顺序
    QString sendData = "Version=#VERSION#&StartTime=#STARTTIME#&RunTime=#RUNTIME#&TxCnt=#TXCNT#&RxCnt=#RXCNT#";
    sendData.replace("#VERSION#",Config::getVersion());
    QString current_date_str = Config::getStartTime();
    sendData.replace("#STARTTIME#",current_date_str);
    sendData.replace("#RUNTIME#",Config::getLastRunTime());
    sendData.replace("#TXCNT#",Config::getLastTxCnt());
    sendData.replace("#RXCNT#",Config::getLastRxCnt());

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
    m_Reply = m_NetManger->post(request, sendData.toLocal8Bit());
    return true;
}

bool HTTP::getRemoteVersion(void)
{
    //旧请求未完成时不执行。
    if(httpTimeout>0)
        return false;

//    ui->statusBar->showMessage("正在检查更新，请稍候……", 1000);

    //发起http请求远端的发布版本号
    QUrl url("https://api.github.com/repos/inhowe/ComAssistant/releases");
    QNetworkRequest request;
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setUrl(url);
    m_Reply = m_NetManger->get(request);
    return true;
}

bool HTTP::getRemoteVersion_my_server(void)
{
    //旧请求未完成时不执行。
    if(httpTimeout>0)
        return false;

    //发起http请求远端的发布版本号
    QUrl url("http://www.inhowe.com/ComAssistant/Request/releases.txt");
    QNetworkRequest request;
    request.setUrl(url);
    m_Reply = m_NetManger->get(request);
    return true;
}

bool HTTP::downloadMessages(void)
{
    //旧请求未完成时不执行。
    if(httpTimeout>0)
        return false;

    //下载远端信息
    QUrl url("http://www.inhowe.com/ComAssistant/Request/msg.txt");
    QNetworkRequest request;
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setUrl(url);
    m_Reply = m_NetManger->get(request);
    return true;
}

void HTTP::addTask(HttpFunction_e name)
{
    httpTaskVector.append(name);
}

QStringList HTTP::getMsgList()
{
    return msgList;
}

void HTTP::httpTimeoutHandle()
{
    //处理http任务请求队列
    if(httpTaskVector.size()>0 && httpTimeout==0){
        switch(httpTaskVector.at(0)){
        case GetVersion:
            getRemoteVersion();break;
        case BackStageGetVersion:
            getRemoteVersion();break;
        case DownloadFile:
            httpTaskVector.pop_front();break;
        case PostStatic:
            postUsageStatistic();break;
        case DownloadMSGs:
            downloadMessages();break;
        case GetVersion_MY_SERVER:
            getRemoteVersion_my_server();break;
        default:
            httpTaskVector.pop_front();break;
        }
        httpTimeout = 5;
    }

    //http超时放弃
    if(httpTimeout){
        httpTimeout--;
        if(httpTimeout==0){
            m_Reply->abort();
            m_Reply->deleteLater();
            if(httpTaskVector.size()>0){
                qDebug()<<"http request timed out."<<httpTaskVector.at(0);
                httpTaskVector.pop_front();
            }
//            ui->statusBar->showMessage("Http请求超时。", 1000);
        }
    }

}

int32_t version_to_number(QString str)
{
    QStringList list = str.split(".");
    if(list.size()!=3)
        return -1;

    int32_t number = 0;
    number = list.at(0).toInt()*65536 + list.at(1).toInt()*256 + list.at(2).toInt();
    return number;
}

void HTTP::httpFinishedSlot(QNetworkReply *)
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

        if(state == GetVersion || state == BackStageGetVersion || state == GetVersion_MY_SERVER ){
            QString remoteVersion;
            QString remoteNote;

            if(state != GetVersion_MY_SERVER){
                //提取版本号
                remoteVersion = string.mid(string.indexOf("tag_name"));
                remoteVersion = remoteVersion.mid(remoteVersion.indexOf(":")+2, remoteVersion.indexOf(",") - remoteVersion.indexOf(":")-3);
                remoteNote = string.mid(string.indexOf("body"));
                remoteNote = remoteNote.mid(remoteNote.indexOf(":\"")+2, remoteNote.indexOf("}") - remoteNote.indexOf(":\"")-3);
                while(remoteNote.indexOf("\\r\\n")!=-1){
                    remoteNote = remoteNote.replace("\\r\\n","\n");
                }
            }else{
                remoteVersion = string;
            }
            QString localVersion;
            localVersion = Config::getVersion();
//            qDebug()<<remoteVersion<<remoteNote;

            //版本号比较
            if(version_to_number(remoteVersion) > version_to_number(localVersion)){
                QMessageBox::Button button;
                if(state == GetVersion){
                    button = QMessageBox::information(nullptr,"提示","当前版本号："+ localVersion +
                                                        "\n远端版本号："+remoteVersion+
                                                        "\n更新内容："+
                                                        "\n"+remoteNote, QMessageBox::Ok|QMessageBox::No);
                    if(button == QMessageBox::Ok)
                        QDesktopServices::openUrl(QUrl("https://github.com/inhowe/ComAssistant/releases"));
                }else{
                     parent->setWindowTitle("串口调试助手 发现新版本：V"+remoteVersion);
                }
            }else{
                if(state == GetVersion){
                    QMessageBox::information(nullptr,"提示","当前版本号："+ localVersion +
                                                                        "\n远端版本号："+remoteVersion+
                                                                        "\n已经是最新版本。");
                }
            }
        }else if(state == PostStatic){
            if(!string.isEmpty())
                qDebug()<<"PostStatic:"<<string;
        }else if(state == DownloadMSGs){
            //把下载的远端信息添加进变量
            msgList = string.split('\n',QString::SkipEmptyParts);
        }else{
            qDebug()<<string;
        }
    }
    else
    {
        //只有当我的服务器也获取不到版本号时才弹失败提示
        if(state == BackStageGetVersion){
            httpTaskVector.push_back(GetVersion_MY_SERVER);
        }else if(state == GetVersion){
            httpTaskVector.push_back(GetVersion_MY_SERVER);
        }else if(state == GetVersion_MY_SERVER){
            QMessageBox::Button button;
            button = QMessageBox::information(nullptr,"提示","当前版本号："+Config::getVersion()+
                                          "\n检查更新失败。"+
                                          "\n请访问：https://github.com/inhowe/ComAssistant/releases",  QMessageBox::Ok|QMessageBox::No);
            if(button == QMessageBox::Ok)
                QDesktopServices::openUrl(QUrl("https://github.com/inhowe/ComAssistant/releases"));
        }else if(state == PostStatic){

        }
        qDebug()<< m_Reply->errorString();
        m_Reply->abort();
    }

    m_Reply->deleteLater();
}
