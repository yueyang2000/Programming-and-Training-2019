#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
namespace Ui {
class Dialog;
}

class Dialog: public QDialog
{
    Q_OBJECT
public:
    explicit Dialog(QWidget* parent = nullptr);
    ~Dialog();
    int choice;
private slots:
    void on_btnQueen_clicked();

    void on_btnRook_clicked();

    void on_btnKnight_clicked();

    void on_btnBishop_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
