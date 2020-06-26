#ifndef HTTP_H
#define HTTP_H

//网络类
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QNetworkInterface>
//Json类
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <QMainWindow>
#include <QtDebug>
#include <QTimer>
#include <QDesktopServices>
#include <QMessageBox>

#include "config.h"

#include <QObject>

namespace Ui {
class MainWindow;
}

class HTTP: public QObject
{
    Q_OBJECT

public:
    typedef enum{
        Idle,
        GetVersion, //获取版本号
        BackStageGetVersion,//后台检查更新
        BackStageGetVersion_MyServer, //从私人服务器获取版本号
        DownloadFile, //下载文件，暂未使用
        PostStatic, //上传统计
        DownloadMSGs, //信息发布系统
    }HttpFunction_e;
    HTTP(QWidget *parentWidget);

    static QString getHostMacAddress();
    bool postUsageStatistic(void);
    bool getRemoteVersion(void);
    bool getRemoteVersion_my_server(void);
    bool downloadMessages(void);
    void addTask(HttpFunction_e name);
    QStringList getMsgList();

private:
    void parseReleaseInfo(QString &inputStr, QString &remoteVersion, QString &remoteNote, QString &publishedTime);

    QWidget *parent;
    QTimer secTimer;
    //http访问
    QNetworkAccessManager *m_NetManger;
    QNetworkReply* m_Reply;
    QVector<HttpFunction_e> httpTaskVector;
    int httpTimeout = 0;
    QStringList msgList;//远端下载的信息列表

private slots:
    void httpTimeoutHandle();
    void httpFinishedSlot(QNetworkReply *);
};

#endif // HTTP_H
