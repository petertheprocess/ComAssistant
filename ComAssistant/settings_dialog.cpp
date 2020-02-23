#include "settings_dialog.h"
#include "ui_settings_dialog.h"

settings_dialog::settings_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings_dialog)
{
    ui->setupUi(this);
}

settings_dialog::~settings_dialog()
{
    delete ui;
}

void settings_dialog::on_buttonBox_accepted()
{
    QMessageBox::information(this,"提示","未完成的功能");
    this->close();
}

void settings_dialog::on_buttonBox_rejected()
{
    this->close();
}
