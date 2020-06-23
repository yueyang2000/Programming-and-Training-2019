#include "graph.h"
#include <QPainter>


Graph::Graph(QWidget *parent):
    QWidget (parent)
{
    setMouseTracking(true);
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
}

void Graph::setup(Setting setting)
{
    r = setting.r;c = setting.c;
    ox = setting.ox;oy = setting.oy;
    inputs = setting.inputs;
    ready = true;
    wash_mode = setting.wash_mode;
    if(wash_mode){
        qDebug()<<"graph get wash mode";
    }
}

void Graph::play_music()
{
    bool move = false;
    bool split_stretch = false;
    bool split_finish = false;
    bool merge_finish = false;
    for(int lp = 0;lp<cur_phase->size();lp++){
        Node node = (*cur_phase)[lp];
        if(node.state == SPLITING){
            split_stretch = true;
        }
        switch(node.just_finish){
        case Move:{
            move=true;break;
        }
        case Split:{
            split_finish = true;break;
        }
        case Merge:{
            merge_finish = true;break;
        }
        default:break;
        }
    }
    if(merge_finish){
        player->setMedia(QUrl("qrc:/resource/sound/merge_finish.wav"));
    }
    else if(split_stretch){
        player->setMedia(QUrl("qrc:/resource/sound/split_stretch.wav"));
    }
    else if(split_finish){
        player->setMedia(QUrl("qrc:/resource/sound/split_finish.wav"));
    }
    else if(move){
        player->setMedia(QUrl("qrc:/resource/sound/move.wav"));
    }
    if(merge_finish ||split_finish||split_stretch||move)
        player->play();
}

void Graph::draw_con(contamination* con)
{
    m_con = con;
    con_mode = true;
    repaint();
    con_mode = false;
}

void Graph::paintEvent(QPaintEvent *ev)
{
    if(!ready) return;
    //背景
    QPainter p(this);
    p.setBrush(Qt::white);
    p.drawRect(0,0,width(),height());

    QFont f= p.font();
    f.setPixelSize(20);
    p.setFont(f);
    //draw table
    p.setBrush(Qt::transparent);
    for(int y = 0;y < r;y++){
        for(int x = 0;x < c;x++){
            QRect rect(100+x*50,100+y*50,50,50);
            if(disable[x][y] && wash_mode){
                p.setBrush(Qt::gray);
                p.drawRect(rect);
                p.setBrush(Qt::NoBrush);
            }
            else{
                p.drawRect(rect);
            }
            if(con_mode){
                p.drawText(rect,Qt::AlignCenter,QString("%1").arg((*m_con)[x][y]));
            }
        }
    }

    //draw input and output
    p.setBrush(Qt::yellow);
    for(int lp =0;lp<inputs.size();lp++){
        int x = inputs[lp].first;
        int y = inputs[lp].second;
        if(x == 1){//左
            QRect rect(0,50+y*50,100,50);
            p.drawRect(rect);
            p.drawText(rect,Qt::AlignCenter,tr("Input"));
        }
        else if(x == c){//右
            QRect rect(100+x*50,50+y*50,100,50);
            p.drawRect(rect);
            p.drawText(rect,Qt::AlignCenter,tr("Input"));
        }
        else if(y == 1){//上
            QRect rect(50+x*50,0,50,100);
            p.drawRect(rect);
            p.drawText(rect,Qt::AlignCenter,tr("Input"));
        }
        else{//下
            QRect rect(50+x*50,100+y*50,50,100);
            p.drawRect(rect);
            p.drawText(rect,Qt::AlignCenter,tr("Input"));
        }
    }
    p.setBrush(Qt::blue);
    if(ox == 1){//左
        QRect rect(0,50+oy*50,100,50);
        p.drawRect(rect);
        p.drawText(rect,Qt::AlignCenter,tr("Output"));
    }
    else if(ox == c){//右
        QRect rect(100+ox*50,50+oy*50,100,50);
        p.drawRect(rect);
        p.drawText(rect,Qt::AlignCenter,tr("Output"));
    }
    else if(oy == 1){//上
        QRect rect(50+ox*50,0,50,100);
        p.drawRect(rect);
        p.drawText(rect,Qt::AlignCenter,tr("Output"));
    }
    else{//下
        QRect rect(50+ox*50,100+oy*50,50,100);
        p.drawRect(rect);
        p.drawText(rect,Qt::AlignCenter,tr("Output"));
    }
    if(wash_mode){
        //draw wash input and waste
        p.setBrush(QColor(170,192,229));
        QRect rect(0,50+r*50,100,50);
        p.drawRect(rect);
        p.drawText(rect,Qt::AlignCenter,tr("Wash Input"));

        p.setBrush(QColor(159,155,104));
        QRect rect1(100+c*50,100,100,50);
        p.drawRect(rect1);
        p.drawText(rect1,Qt::AlignCenter,tr("Waste"));
    }
    if(con_mode) return;
    Node node;
    if(cur_phase == nullptr) return;
    for(int i = 0;i< cur_phase->size();i++)
    {
        node = (*cur_phase)[i];
        QColor color = get_color_by_id(node.id);
        QPen pen;
        pen.setColor(color);
        pen.setWidth(3);
        p.setPen(pen);
        //draw trace
        p.setBrush(Qt::NoBrush);
        //qDebug()<<"trace size:"<<node.trace.size();
        for(int lp = 0;lp<node.trace.size();lp++){
            int x = node.trace[lp].first;
            int y = node.trace[lp].second;
            p.drawEllipse(60+x*50,60+y*50,30,30);
        }
        //draw node
        p.setBrush(color);
        p.setPen(Qt::NoPen);
        if(node.state == NORMAL){
            p.drawEllipse(60+node.x*50,60+node.y*50,30,30);
        }
        else if(node.state == MERGING | node.state == SPLITING){
            if(node.horizontal){//横着的
                p.drawEllipse(10+node.x*50,60+50*node.y,130,30);
            }
            else{
                p.drawEllipse(60+node.x*50,10+50*node.y,30,130);
            }
        }
        else if(node.state == MERGED){
            p.drawEllipse(50+node.x*50,50+node.y*50,50,50);
        }
        else if(node.state == WASH){
            p.setBrush(QColor(170,192,229));
            p.drawEllipse(60+node.x*50,60+node.y*50,30,30);
        }
        else{}//DISABLED

    }
}
void Graph::mouseMoveEvent(QMouseEvent *ev)
{
    QPoint point = ev->pos();
    int x = (point.x()-100)/50;
    int y = (point.y()-100)/50;
    if(cur_phase != nullptr){
        if(0<=x && x<c && 0<=y && y<r){
            int i = -1;
            for(int lp = 0;lp<cur_phase->size();lp++){
                if((*cur_phase)[lp].x == x+1 && (*cur_phase)[lp].y == y+1){
                    i = lp;
                }
            }
            if(i == -1 ||(*cur_phase)[i].state == DISABLED){
                QString text;
                text = QString("electrode (%1,%2)").arg(x+1).arg(y+1);
                emit mouseMoved(text);
            }
            else{
                QString text;
                text = QString("microfluid (%1,%2)").arg(x+1).arg(y+1);
                emit mouseMoved(text);
            }
        }
    }
}

void Graph::mousePressEvent(QMouseEvent *ev){
    QPoint point = ev->pos();
    int x = (point.x()-100)/50;
    int y = (point.y()-100)/50;
    if(0<=x && x<c && 0<=y && y<r){
        disable[x][y]= 1- disable[x][y];
        emit disable_changed(x+1,y+1);
        repaint();
    }
}

QColor Graph::get_color_by_id(int i)
{
    static QVector<QColor> palette= {
        QColor(255, 165 ,0),
        QColor(123,104,238),
        QColor(	30 ,144 ,255),
        QColor(	60,179,113),
        QColor(	160, 32, 240),
        QColor(205,92, 92),
        QColor(173,255, 47),
        QColor(	255, 20 ,147),
        QColor(	0 ,178 ,238)
    };
    return palette[abs(i)%9];//这里出错！！！！
}

