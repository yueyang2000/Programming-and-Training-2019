#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(800,600);
    initAction();
    mouse_pos = new QLabel(this);
    ui->statusBar->addWidget(mouse_pos);
    setWindowTitle(QString("DFMB Simulator"));
    initConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initAction()
{
    QFont font;
    font.setPointSize(20);

    actInit = new QAction(this);
    actInit->setFont(font);
    actInit->setText(tr("Init"));
    ui->mainToolBar->addAction(actInit);
    connect(actInit,SIGNAL(triggered()),this,SLOT(init()));

    actFirst = new QAction(this);
    actFirst->setFont(font);
    actFirst->setText(tr("First"));
    ui->mainToolBar->addAction(actFirst);
    connect(actFirst,SIGNAL(triggered()),this,SLOT(first()));

    actPrev = new QAction(this);
    actPrev->setFont(font);
    actPrev->setText(tr("Prev"));
    //actPrev->setIcon(QIcon(":/resource/icons/left.png"));
    actPrev->setShortcut(tr("Left"));
    ui->mainToolBar->addAction(actPrev);
    connect(actPrev,SIGNAL(triggered()),this,SLOT(prev()));

    actNext = new QAction(this);
    actNext->setFont(font);
    actNext->setText(tr("Next"));
    actNext->setShortcut(tr("Right"));
    //actNext->setIcon(QIcon(":/resource/icons/right.png"));
    ui->mainToolBar->addAction(actNext);
    connect(actNext,SIGNAL(triggered()),this,SLOT(next()));


    actPlay = new QAction(this);
    actPlay->setFont(font);
    actPlay->setText(tr("Play"));
    ui->mainToolBar->addAction(actPlay);
    connect(actPlay,SIGNAL(triggered()),this,SLOT(play()));

    actQuit = new QAction(this);
    actQuit->setFont(font);
    actQuit->setText(tr("Quit"));
    ui->mainToolBar->addAction(actQuit);
    connect(actQuit,SIGNAL(triggered()),this,SLOT(close()));

    actCon = new QAction(this);
    actCon->setFont(font);
    actCon->setText(tr("Contamination"));
    ui->mainToolBar->addAction(actCon);
    connect(actCon,SIGNAL(triggered()),this,SLOT(con()));

    ui->mainToolBar->addSeparator();
    lcd = new QLCDNumber(this);
    ui->mainToolBar->addWidget(lcd);
    ui->mainToolBar->setIconSize(QSize(50,50));

}

void MainWindow::init()
{

    //qDebug()<<"init";
    dialog = new Dialog;//弹出一个设置dialog

    dialog->show();

    connect(dialog,SIGNAL(get_setting(Setting)),
            this,SLOT(receive_setting(Setting)));


    //ui->centralWidget->update();
    //core.read_file(QString("/Users/yueyang/yiqunyang/大一暑假/程序设计/qt/Week1/Input/testcase3.txt"));
    //first();
}

void MainWindow::first()
{
    if(!wash_mode){
        time = 0;
        update_graph();
    }
    else{
        core.reset_wash();
        wash_direction.clear();
        lcd->display(core.wash_time);
        ui->centralWidget->update_phase(nullptr);
    }

}

void MainWindow::play()
{
    while(true){
        //0.5s换一次
        QEventLoop eventloop;
        QTimer::singleShot(1000, &eventloop, SLOT(quit()));
        eventloop.exec();
        time++;
        if(time >core.max_time()){time--;break;}
        update_graph();
        ui->centralWidget->play_music();
    }
}

void MainWindow::con()
{
    ui->centralWidget->draw_con(core.get_con(time));
}

void MainWindow::receive_setting(Setting setting)
{
    qDebug()<<"receive setting!";
    if(setting.r >= 6){
        resize(800,800);
    }
    ui->centralWidget->setup(setting);
    core.setup(setting);
    wash_mode = setting.wash_mode;
    if(wash_mode){
        handle_wash_mode();
        return;
    }
    actCon->setEnabled(true);
    actPrev->setEnabled(true);
    actPlay->setEnabled(true);
    ready = true;
    stop = false;
    time = 0;
    update_graph();
}

void MainWindow::disable_update()
{//需要重新计算路径
    wash_direction.clear();
}

void MainWindow::receive_error(Error e)
{
    handle_err(e,nullptr);
}

void MainWindow::initConnect()
{
    //跟踪鼠标
    connect(ui->centralWidget,SIGNAL(mouseMoved(QString)),
            this,SLOT(mouse_moved(QString)));
    //报错
    connect(&core,SIGNAL(report_error(Error)),this,SLOT(receive_error(Error)));

    //约束更新
    connect(ui->centralWidget,SIGNAL(disable_changed(int,int)),&core,SLOT(disable_update(int,int)));
    connect(ui->centralWidget,SIGNAL(disable_changed(int,int)),this,SLOT(disable_update()));
}

void MainWindow::update_graph()
{
    ui->centralWidget->update_phase(core.get_phase(time));
    lcd->display(time);
    if(time == core.max_time() && core.err != NO_ERROR){//最后一步出错
        handle_err(core.err,core.err_pos);
    }
}

void MainWindow::handle_err(Error err, int *err_pos)
{
    switch (err) {
    case NO_INPUT:{
        QMessageBox m;
        m.setText(QString("no input near location (%1,%2)").arg(err_pos[0]).arg(err_pos[1]));
        m.exec();break;
    }
    case NO_OUTPUT:{
        QMessageBox m;
        m.setText(QString("no output near location (%1,%2)").arg(err_pos[0]).arg(err_pos[1]));
        m.exec();break;
    }
    case STATIC_LIMIT:{
        QMessageBox m;
        m.setText(QString("static limit violation at location (%1,%2) and (%3,%4)")
                  .arg(err_pos[0]).arg(err_pos[1]).arg(err_pos[2]).arg(err_pos[3]));
        m.exec();break;
    }
    case DYNAMIC_LIMIT:{
        QMessageBox m;
        m.setText(QString("dynamic limit violation between time %1 and %2, location (%3,%4) and (%5,%6)")
                  .arg(core.max_time()-1).arg(core.max_time())
                  .arg(err_pos[0]).arg(err_pos[1]).arg(err_pos[2]).arg(err_pos[3]));
        m.exec();break;
    }
    case NO_WAY_TO_GO:{
        QMessageBox m;
        m.setText(QString("washing fluid cannot get to the output!, please release some blocks"));
        m.exec();break;
    }
    }
}

void MainWindow::handle_wash_mode()
{
    qDebug()<<"on handle wash mode";
    //清洗模式
    actCon->setEnabled(false);
    actPrev->setEnabled(false);
    actPlay->setEnabled(false);
    ui->centralWidget->update_phase(nullptr);
    qDebug()<<"wash mode init success";
}


void MainWindow::mouse_moved(QString text)
{
    mouse_pos->setText(text);
}

void MainWindow::prev()
{
    qDebug()<<"prev";
    if(time >0) time--;
    update_graph();
}

void MainWindow::next()
{
    if(!wash_mode){
        if(time >= core.max_time()) return;//大于最大时间
        time++; update_graph();
        ui->centralWidget->play_music();
    }
    else{
        ui->centralWidget->update_phase(core.get_wash_phase(&wash_direction));
        lcd->display(core.wash_time);
        if(!core.check_contamination()){
            QMessageBox m;
            m.setText(QString("cross contamination at (%1,%2)").arg(core.err_pos[0]).arg(core.err_pos[1]));
            m.exec();
        }
    }

}
