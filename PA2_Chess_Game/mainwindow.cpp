#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    board = ui->centralWidget;
    board->update();
    server = new Server(this);
    client = new Client(this);
    timer = new QTimer(this);
    timer->setInterval(1000);
    labelConnect = new QLabel(this);
    ui->statusBar->addWidget(labelConnect);

    connect(timer,&QTimer::timeout,[this]{
        time--;
        ui->lcd->display(time);
        if(time == 0){
            c.on_move = false;
            timer->stop();
            timeLoose();
        }
    });
    connect(board,&Board::move_piece,this,&MainWindow::sendData);
    connect(board,&Board::win,this,&MainWindow::handle_win);
    connect(board,&Board::stalemate,this,&MainWindow::handle_stalemate);
    //连上以后才能用
    ui->actionInit->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionLoad->setEnabled(false);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connect_established()
{
    QString txt = QString("connect to %1").arg(socket->peerAddress().toString());
    if(isServer) txt += " Server";
    else txt+=" Client";
    labelConnect->setText(txt);
    ui->label->setText("wait for game to begin...");
    if(isServer){
        ui->actionInit->setEnabled(true);
        ui->actionLoad->setEnabled(true);
    }
    ui->actionSave->setEnabled(true);
    ui->actionServer->setEnabled(false);
    ui->actionClient->setEnabled(false);
    c.game_on = false;
    c.on_move = false;
}

void MainWindow::game_end()
{
    qDebug()<<"game end";
    //一局游戏结束
    c.game_on = false;
    c.on_move = false;
    board->picking=false;
    update();
    timer->stop();
    if(isServer){
        ui->actionInit->setEnabled(true);
        ui->actionLoad->setEnabled(true);
    }
}

void MainWindow::readHello(packet p)
{
    int file = p.x1;
    c.game_on = true;
    if(isServer){
        //是服务器，收到client的消息立刻开始
        //默认服务器为先手
        //上来就检查
        if(!c.check(c.m_color)){//王没了
            game_end();
            ui->label->setText("You Loose...");
            QMessageBox::information(this,"game over","You Loose...");
        }
        else if(c.check_stalemate(c.m_color) || c.check_stalemate(Color(1-c.m_color))){//逼和
            game_end();
            ui->label->setText("stalemate");
            QMessageBox::information(this,"Oops!","You reach a stalemate...");
        }
        else{
            turn();
        }
    }
    else{//是client,client为后手
        if(file){
            c.file_path = QFileDialog::getOpenFileName(this,tr("Open File"),"/Users/yueyang/yiqunyang/大一暑假/程序设计/qt/week2-1/大作业材料","*.txt");
            c.load_file();
            board->update();
            c.m_color = (Color)(1-c.m_color);
        }
        else{
            c.init();
            board->update();
            c.m_color = BLACK;
        }
        sendData(p);//再传回去，表示ack
        if(!c.check(c.m_color)){//王没了
            game_end();
            ui->label->setText("You Loose...");
            QMessageBox::information(this,"game over","You Loose...");
        }
        else if(c.check_stalemate(c.m_color) || c.check_stalemate(Color(1-c.m_color))){//逼和
            game_end();
            ui->label->setText("stalemate");
            QMessageBox::information(this,"Oops!","You reach a stalemate...");
        }
        else{
            wait();
        }
    }

    if(c.m_color == WHITE){
        ui->labelSide->setPixmap(QPixmap::fromImage(QImage(":/resource/icons/white_king.png").scaled(75,75)));
    }
    else{
        ui->labelSide->setPixmap(QPixmap::fromImage(QImage(":/resource/icons/black_king.png").scaled(75,75)));
    }
    qDebug()<<"read hello";
}

void MainWindow::readMove(packet p)
{
    qDebug()<<"read move";
    if(p.type == 1){
        c.simple_move(p.x1,p.y1,p.x2,p.y2);
    }
    else if(p.type == 2){//王车易位
        c.castling(p.x1,p.y1,p.x2,p.y2);
    }
    else{//兵升变
        c.simple_move(p.x1,p.y1,p.x2,p.y2);
        c.pawn_shift(p.type,p.x2,p.y2);
    }
    update();
    if(!c.check(c.m_color)){//王没了
        game_end();
        ui->label->setText("You Loose...");
        QMessageBox::information(this,"game over","You Loose...");
    }
    else if(c.check_stalemate(c.m_color)){//看自己有没有地方走，逼和
        game_end();
        ui->label->setText("stalemate");
        QMessageBox::information(this,"Oops!","You reach a stalemate...");
    }
    else{
        turn();
    }

}

void MainWindow::readEnd(packet p)
{
    switch (p.type) {
    case 10:{//下输(废弃)
        game_end();
        ui->label->setText("You Loose...");
        QMessageBox::information(this,"game over","You Loose...");
        break;
    }
    case 11:{//对方超时
        game_end();
        ui->label->setText(tr("You Win."));
        QMessageBox::information(this,tr("congratulations"),tr("your opponent runs out of time"));
        break;
    }
    case 12:{//对方认输
        game_end();
        ui->label->setText("You Win!");
        QMessageBox::information(this,"congratulations!","your opponent admits defeat");
        break;
    }
    default:{
        qDebug()<<"undefined end";
    }
    }

}

void MainWindow::turn()
{
    if(!c.game_on) return;
    c.on_move = true;
    ui->label->setText("Your Turn!");
    time = 30;
    ui->lcd->display(time);
    timer->start();
}

void MainWindow::wait(){
    if(!c.game_on) return;
    timer->stop();
    c.on_move = false;
    time = 0;
    ui->lcd->display(time);
    ui->label->setText("Wait for the other player");
}

void MainWindow::timeLoose()
{
    game_end();
    ui->label->setText("Timeout!");
    sendData(packet{11,0,0,0,0});//发送己方超时的消息
    QMessageBox::information(this,tr("you loose!"),tr("time runs out"));
}

void MainWindow::handle_win()
{
    game_end();
    ui->label->setText(tr("You Win!"));
    //sendData(packet{10,0,0,0,0});//发送对方输的消息
    QMessageBox::information(this,"congratulations!","You Win!");
}

void MainWindow::handle_stalemate()
{
    //逼和
    game_end();
    ui->label->setText("stalemate");

//    QEventLoop e;
//    QTimer::singleShot(100,&e,SLOT(quit()));e.exec();
    QMessageBox::information(this,"Oops!","You reach a stalemate...");
}

void MainWindow::give_up_loose()
{
    game_end();
    ui->label->setText("You Loose...");
    sendData(packet{12,0,0,0,0});//发送自己认输的消息

    QMessageBox::information(this,"game over","You admit defeat");
}

void MainWindow::on_actionServer_triggered()
{
    if(server->exec() == QDialog::Accepted){
        isServer = true;
        socket = server->socket;
        connect(socket,&QTcpSocket::readyRead,this,&MainWindow::parse_message);
        connect(socket,&QTcpSocket::disconnected,[this]{
            QMessageBox::information(this,"",tr("disconnected"));
            socket->abort();
            close();
        });
        connect_established();
    }
}

void MainWindow::parse_message()
{
    qDebug()<<"receive message";
    auto data = socket->readAll();
    QDataStream st(&data, QIODevice::ReadOnly);
    int type,x1,y1,x2,y2;
    st>>type>>x1>>y1>>x2>>y2;
    packet p{type,x1,y1,x2,y2};
    if(p.type == 0) readHello(p);
    else if(p.type >0 && p.type <10) readMove(p);
    else if(p.type >= 10) readEnd(p);
}


void MainWindow::on_actionClient_triggered()
{
    if(client->exec() == QDialog::Accepted){
        isServer = false;
        socket = client->socket;
        if(socket == nullptr){
            qDebug()<<"client no connection!"; return;
        }
        connect(socket,&QTcpSocket::readyRead,this,&MainWindow::parse_message);
        connect(socket,&QTcpSocket::disconnected,[this]{
            QMessageBox::information(this,"",tr("disconnected"));
            socket->abort();
            close();
        });
        connect_established();
    }
}

void MainWindow::sendData(packet p)
{
     QByteArray data;
     QDataStream st(&data, QIODevice::WriteOnly);
     st<<p.type<<p.x1<<p.y1<<p.x2<<p.y2;
     socket->write(data);
     socket->waitForBytesWritten();
     qDebug()<<"send data"<<p.type<<p.x1<<p.y1<<p.x2<<p.y2;
     if(p.type >=1){
         wait();
     }
}

void MainWindow::on_actionInit_triggered()
{
    if(c.game_on){
        QMessageBox::warning(this,tr("error"),tr("you are in a game now!"));
        return;
    }
    c.init();
    c.m_color = WHITE;
    board->update();
    c.file = false;
    sendData(packet{0,0,0,0,0});
}

void MainWindow::on_actionLoad_triggered()
{
    if(c.game_on){
        QMessageBox::warning(this,tr("error"),tr("you are in a game now!"));
    }
    c.file_path = QFileDialog::getOpenFileName(this,tr("Open File"),"/Users/yueyang/yiqunyang/大一暑假/程序设计/qt/week2-1/大作业材料","*.txt");
    c.load_file();
    c.file = true;
    board->update();
    sendData(packet{0,1,0,0,0});
}

void MainWindow::on_actionSave_triggered()
{
    c.file_path = QFileDialog::getSaveFileName(this,tr("save file"),"/Users/yueyang/yiqunyang/大一暑假/程序设计/qt/week2-1/大作业材料","*.txt");
    c.save_file();
}

void MainWindow::on_pushButton_clicked()
{
    if(!c.game_on) return;
    give_up_loose();
}
