#include "server.h"
#include "ui_server.h"
#include <QDebug>
#include <QNetworkInterface>
Server::Server(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    for (auto &address : QNetworkInterface::allAddresses())
        if(address.toIPv4Address())
        {
            ui->comboBox->addItem(address.toString());
        }
    setWindowTitle(tr("create new game"));

    server = new QTcpServer;
    connect(server,&QTcpServer::newConnection,[this]{
        //if(socket!=nullptr) delete socket;
        this->socket = this->server->nextPendingConnection();
        this->server->close();
        if(this->socket!=nullptr) {
            qDebug()<<"server connected!";
            accept();
        }
    });
    ui->btnCancel->setEnabled(false);
}

Server::~Server()
{
    delete ui;
}


void Server::on_btnOK_clicked()
{
    setWindowTitle("waiting for connection...");
    //qDebug()<<ui->comboBox->currentText();
    server->listen(QHostAddress(ui->comboBox->currentText()),8888);
    ui->btnCancel->setEnabled(true);
    ui->btnOK->setEnabled(false);
    ui->btnClose->setEnabled(false);
}

void Server::on_btnCancel_clicked()
{
    server->close();
    setWindowTitle(tr("create new game"));
    ui->btnOK->setEnabled(true);
    ui->btnClose->setEnabled(true);
    ui->btnCancel->setEnabled(false);
}
