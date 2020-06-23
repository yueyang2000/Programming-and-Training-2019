#ifndef GRAPH_H
#define GRAPH_H
#include <QWidget>
#include <QMouseEvent>
#include "core.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
class Graph: public QWidget
{
    Q_OBJECT
public:
    explicit Graph(QWidget* parent = nullptr);
    void update_phase(phase* p){cur_phase = p;update();}
    void setup(Setting setting);
    void play_music();
    void draw_con(contamination* con);
private:
    void paintEvent(QPaintEvent* ev);
    void mousePressEvent(QMouseEvent* ev);
    void mouseMoveEvent(QMouseEvent* ev);

    int r,c,x,y;
    bool ready = false;
    phase* cur_phase = nullptr;//目前状态
    QColor get_color_by_id(int i);
    QVector<QPair<int,int> > inputs;
    int ox,oy;
    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    bool con_mode = false;
    contamination* m_con;

    bool wash_mode = false;
    int disable[15][15]{{}};
signals:
    void mouseMoved(QString);
    void disable_changed(int x,int y);
};

#endif // GRAPH_H
