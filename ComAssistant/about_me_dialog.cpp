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
