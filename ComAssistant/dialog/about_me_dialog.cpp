#include "about_me_dialog.h"
#include "ui_about_me_dialog.h"

About_Me_Dialog::About_Me_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About_Me_Dialog)
{
    ui->setupUi(this);
    //可见性设置
    ui->label_github->setVisible(false);
    ui->githubUrl->setVisible(false);
}

About_Me_Dialog::~About_Me_Dialog()
{
    delete ui;
}

void About_Me_Dialog::getVersionString(QString str)
{
    if(str.isEmpty())
        return;
    ui->version->setText(str);
}

void About_Me_Dialog::showManualDoc(void)
{
    QFile file(":/manual.html");
    QString html;
    if(file.exists()){
        if(file.open(QFile::ReadOnly)){
            html = file.readAll();
            file.close();

            ui->textBrowser->clear();
            ui->textBrowser->append(html);

            //滚动到最前面，不知道为什么设置滚动条没效果
            QTextCursor cursor = ui->textBrowser->textCursor();
            cursor.setPosition(0);
            ui->textBrowser->setTextCursor(cursor);
        }else{
            QMessageBox::information(this, "提示", "帮助文件被占用。");
        }
    }else{
        QMessageBox::information(this, "提示", "帮助文件丢失。");
    }
}

void About_Me_Dialog::on_okButton_clicked()
{
    this->close();
}

void About_Me_Dialog::on_githubUrl_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void About_Me_Dialog::on_email_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}
