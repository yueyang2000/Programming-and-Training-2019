#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "board.h"
#include "server.h"
#include "client.h"
#include <QMainWindow>
#include <QTimer>
#include <QLabel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void connect_established();
    void game_end();

    void readHello(packet p);
    void readMove(packet p);
    void readEnd(packet p);

    void turn();
    void wait();


private slots:
    void on_actionServer_triggered();

    void parse_message();

    void on_actionClient_triggered();

    void sendData(packet);



    void on_actionInit_triggered();

    void on_actionLoad_triggered();

    void on_actionSave_triggered();

    void timeLoose();
    void handle_win();
    void handle_stalemate();
    void give_up_loose();


    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    Board* board;
    Server* server;
    Client* client;
    QTcpSocket* socket;
    QLabel* labelConnect;
    bool isServer;

    int time;
    QTimer* timer;
};

#endif // MAINWINDOW_H
