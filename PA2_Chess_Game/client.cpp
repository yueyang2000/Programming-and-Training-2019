#include "client.h"
#include "ui_client.h"
#include <QMessageBox>
Client::Client(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    setWindowTitle("connect to server");
}

Client::~Client()
{
    delete ui;
}

void Client::on_btnOK_clicked()
{
    if(connecting) return;
    QString ip = ui->lineEdit->text();
    if(QHostAddress(ip).toString()!=ip){
        QMessageBox::warning(this,"Error","invalid IP format");
        return;
    }
    else{
        setWindowTitle("connecting...");
        //qDebug()<<"OK here";
        //if(socket!=nullptr) delete socket;

        socket = new QTcpSocket;
        connect(socket, &QTcpSocket::connected,[this]{
            qDebug()<<"client connected!";
            this->setWindowTitle("connected!");
//            QEventLoop e;
//            QTimer::singleShot(1000,&e,SLOT(quit()));
//            e.exec();
            accept();
        });
        socket->connectToHost(QHostAddress(ip),8888);
        connecting = true;
    }
}

void Client::on_btnCancel_clicked()
{
    setWindowTitle("connect to server");
    if(connecting){
        connecting  =false;
        delete socket;
    }
    reject();
}
