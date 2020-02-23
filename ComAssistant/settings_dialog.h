#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class settings_dialog;
}

class settings_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit settings_dialog(QWidget *parent = nullptr);
    ~settings_dialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::settings_dialog *ui;
};

#endif // SETTINGS_DIALOG_H
