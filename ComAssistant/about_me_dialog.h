#ifndef ABOUT_ME_DIALOG_H
#define ABOUT_ME_DIALOG_H

#include <QDialog>

namespace Ui {
class About_Me_Dialog;
}

class About_Me_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit About_Me_Dialog(QWidget *parent = nullptr);
    ~About_Me_Dialog();

private slots:
    void on_okButton_clicked();

private:
    Ui::About_Me_Dialog *ui;
};

#endif // ABOUT_ME_DIALOG_H
