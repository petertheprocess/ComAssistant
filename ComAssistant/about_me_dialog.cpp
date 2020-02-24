#include "about_me_dialog.h"
#include "ui_about_me_dialog.h"

About_Me_Dialog::About_Me_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About_Me_Dialog)
{
    ui->setupUi(this);
}

About_Me_Dialog::~About_Me_Dialog()
{
    delete ui;
}

void About_Me_Dialog::on_okButton_clicked()
{
    this->close();
}

void About_Me_Dialog::on_label_2_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void About_Me_Dialog::on_label_3_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void About_Me_Dialog::on_label_7_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}
