#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "core.h"
#include <QLabel>
#include <QLCDNumber>
#include "dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    phase* cur_phase;

private:
    Ui::MainWindow *ui;
    Core core;//核心计算
    QAction* actPrev;
    QAction* actNext;
    QAction* actFirst;
    QAction* actInit;
    QAction* actPlay;
    QAction* actQuit;
    QAction* actCon;
    QLabel* mouse_pos;
    QLCDNumber* lcd;
    Dialog* init_dialog;
    void initAction();
    void initConnect();
    int time = 0;
    void update_graph();
    bool stop = false;
    bool ready = false;
    Dialog* dialog;
    void handle_err(Error err,int* err_pos);

    bool wash_mode = false;
    void handle_wash_mode();//清洗模式单独处理
    QVector<position> wash_direction;
private slots:
    void mouse_moved(QString);
    void prev();
    void next();
    void init();
    void first();
    void play();
    void con();
    void receive_setting(Setting setting);
    void disable_update();
    void receive_error(Error e);
};

#endif // MAINWINDOW_H
