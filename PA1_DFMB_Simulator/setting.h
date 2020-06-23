#ifndef SETTING_H
#define SETTING_H
#include <QWidget>

struct Setting{
    QString path;
    int c,r;
    QVector<QPair<int,int> > inputs;
    int ox,oy;//output
    bool wash_mode;
    Setting(){}
    Setting(QString p,int cc,int rr,QVector<QPair<int,int> > i,int oxx,int oyy,bool wash):
        path(p),c(cc),r(rr),inputs(i),ox(oxx),oy(oyy),wash_mode(wash){}
};

#endif // SETTING_H
