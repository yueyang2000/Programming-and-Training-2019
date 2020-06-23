#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "setting.h"
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    QString path;
    int c,r;
    QVector<QPair<int,int> > inputs;
    int ox;
    int oy;
    bool wash_mode;
    ~Dialog();

private slots:
    void on_FilePath_textChanged(const QString &arg1);


    void on_toolButton_clicked();

    void on_columns_valueChanged(int arg1);

    void on_rows_valueChanged(int arg1);


    void on_btnAdd_clicked();

    void on_btnDelete_clicked();

    void on_outputX_valueChanged(int arg1);

    void on_outputY_valueChanged(int arg1);

    void on_btnReset_clicked();


    void on_btnOK_clicked();

    void on_btnCancel_clicked();

    void on_btnWash_clicked(bool checked);

private:
    Ui::Dialog *ui;
    void update_display();
signals:
    void get_setting(Setting setting);

};

#endif // DIALOG_H
