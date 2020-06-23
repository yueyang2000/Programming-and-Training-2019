#ifndef CLIENT_H
#define CLIENT_H
#include <QDialog>
#include <QTcpSocket>
#include <QtNetwork>
namespace Ui {
class Client;
}

class Client : public QDialog
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();
    QTcpSocket* socket;
    QString ip;
    bool connecting = false;
private slots:
    void on_btnOK_clicked();


    void on_btnCancel_clicked();

private:
    Ui::Client *ui;

};
#endif // CLIENT_H
