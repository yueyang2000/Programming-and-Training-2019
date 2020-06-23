#ifndef SERVER_H_
#define SERVER_H_
#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
namespace Ui {
class Server;
}

class Server : public QDialog
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server();
    QTcpServer* server;
    QTcpSocket* socket;
    QString ip;
private slots:

    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    Ui::Server *ui;

};


#endif
