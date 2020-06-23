#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget* parent):
    QDialog (parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}
Dialog::~Dialog(){
    delete ui;
}

void Dialog::on_btnQueen_clicked()
{
    choice = 3;accept();
}


void Dialog::on_btnRook_clicked()
{
    choice = 4;accept();
}

void Dialog::on_btnKnight_clicked()
{
    choice = 5;accept();
}

void Dialog::on_btnBishop_clicked()
{
    choice = 6;accept();
}
