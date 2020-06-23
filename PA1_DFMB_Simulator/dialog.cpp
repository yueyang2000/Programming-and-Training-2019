#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->inputDisplay->setStyleSheet("background-color:white");
    setWindowTitle("Init parameters");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_FilePath_textChanged(const QString &arg1)
{
    path = ui->FilePath->text();
}


void Dialog::on_toolButton_clicked()
{
    path = QFileDialog::getOpenFileName(this,tr("open file"),"/Users/yueyang/yiqunyang/大一暑假/程序设计/qt/Week1/Input","*.txt");
    ui->FilePath->setText(path);
}

void Dialog::on_columns_valueChanged(int arg1)
{
    c = arg1;
}

void Dialog::on_rows_valueChanged(int arg1)
{
    r = arg1;
}


void Dialog::on_btnAdd_clicked()
{
    int x = ui->inputX->value();
    int y  =ui->inputY->value();
    if(x<=0 || x>c || y<=0 || y>r){
        QMessageBox m;m.setText(tr("out of range!"));
        m.exec();
        ui->inputX->setValue(0);
        ui->inputY->setValue(0);
        return;
    }
    else if(x!=1 && y!=1 && x!=c && y!=r){
        QMessageBox m; m.setText(tr("input must on the boundary!"));
        m.exec();
        ui->inputX->setValue(0);
        ui->inputY->setValue(0);
        return;
    }
    if(wash_mode && ((x==1 && y==r)||(x==c && y==1))){
        QMessageBox m; m.setText(tr("input conflict with wash input and waste!"));
        m.exec();ui->inputX->setValue(0);ui->inputY->setValue(0);return;
    }
    int i = -1;
    for(int lp = 0;lp<inputs.size();lp++){
        if(inputs[lp].first == x && inputs[lp].second == y){
            i = lp;
            break;
        }
    }
    if(i == -1){
        inputs.push_back(QPair<int,int>(x,y));
    }
    update_display();
}

void Dialog::update_display()
{
    QString t;
    for(int lp = 0;lp<inputs.size();lp++){
        t += QString("(%1,%2) ").arg(inputs[lp].first).arg(inputs[lp].second);
    }
    //qDebug()<<t;
    ui->inputDisplay->setText(t);
}

void Dialog::on_btnDelete_clicked()
{
    int x = ui->inputX->value();
    int y = ui->inputY->value();
    int i = -1;
    for(int lp = 0;lp<inputs.size();lp++){
        if(inputs[lp].first == x && inputs[lp].second == y){
            i = lp;
            break;
        }
    }
    if(i != -1){
        inputs.remove(i);
    }
    update_display();
}

void Dialog::on_outputX_valueChanged(int arg1)
{
    ox = arg1;
}

void Dialog::on_outputY_valueChanged(int arg1)
{
    oy = arg1;
}

void Dialog::on_btnReset_clicked()
{
    r = c = ox = oy = 0;
    ui->inputX->setValue(0);
    ui->inputY->setValue(0);
    ui->outputX->setValue(0);
    ui->outputY->setValue(0);
    inputs.clear();
    update_display();
    path = "";
    ui->FilePath->setText(path);
}


void Dialog::on_btnOK_clicked()
{
    if(path == ""){
        QMessageBox m; m.setText(tr("Please select a file"));m.exec();return;
    }
    else if(r <=3 && c <= 3){
        QMessageBox m; m.setText(tr("Size of the grid is too small"));m.exec();return;
    }
    else if(inputs.size() == 0){
        QMessageBox m; m.setText(tr("Please choose input coordinates"));m.exec();return;
    }
    else if(ox<=0 || ox>c || oy<=0 || oy>r){
        QMessageBox m;m.setText(tr("output out of range!"));
        m.exec();
        ui->outputX->setValue(0);
        ui->outputY->setValue(0);
        return;
    }
    else if(ox!=1 && oy!=1 && ox!=c && oy!=r){
        QMessageBox m; m.setText(tr("output must on the boundary!"));
        m.exec();
        ui->outputX->setValue(0);
        ui->outputY->setValue(0);
        return;
    }
    else if(wash_mode && ((ox==1 && oy==r)||(ox==c && oy==1))){
        QMessageBox m; m.setText(tr("output conflict with wash input and waste!"));
        m.exec();
        ui->outputX->setValue(0);
        ui->outputY->setValue(0);
        return;
    }
    else{
        int i = -1;
        for(int lp = 0;lp<inputs.size();lp++){
            if(inputs[lp].first == ox && inputs[lp].second ==  oy){
                i = lp;
                break;
            }
        }
        if(i != -1){
            QMessageBox m; m.setText(tr("input and output cannot be at the same place!"));
            m.exec();return;
        }
    }
    //没有错误
    emit get_setting(Setting(path,c,r,inputs,ox,oy,wash_mode));
    close();
}

void Dialog::on_btnCancel_clicked()
{
    close();
}

void Dialog::on_btnWash_clicked(bool checked)
{
    wash_mode = checked;
}
