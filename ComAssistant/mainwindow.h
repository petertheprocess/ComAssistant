#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QtCore/QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QRegExp>

#include "myserialport.h"
#include "baseconversion.h"
#include "config.h"

#include "about_me_dialog.h"
#include "settings_dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_refreshCom_clicked();

    void on_clearRecvAreaButton_clicked();

    void on_clearSendAreaButton_clicked();

    void on_comSwitch_clicked(bool checked);

    void on_sendButton_clicked();

    void on_clearStatistic_clicked();

    void readSerialPort();

    void continuousWriteSlot();

    void on_TimerSendCheck_clicked(bool checked);

    void on_textEdit_textChanged();

    void on_hexSend_stateChanged(int arg1);

    void on_hexDisplay_clicked(bool checked);

    void on_action_winLikeEnter_triggered(bool checked);

    void on_action_unixLikeEnter_triggered(bool checked);

    void on_actionUTF8_triggered(bool checked);

    void on_actionSaveData_triggered();

    void on_actionReadData_triggered();

    void on_actionAbout_triggered();

    void on_actionCOM_Config_triggered();

    void on_baudrateList_currentTextChanged(const QString &arg1);

    void on_comList_currentTextChanged(const QString &arg1);

    void on_actionSaveShowedData_triggered();

private:
    void readConfig();
    Ui::MainWindow *ui;
    mySerialPort serial;
    QByteArray RxBuff, TxBuff;
    QTimer continuousWriteTimer;
};

#endif // MAINWINDOW_H
