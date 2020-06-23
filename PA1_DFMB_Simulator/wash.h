#ifndef WASH_H
#define WASH_H
#include <QWidget>
typedef QVector<QVector<int> > contamination;//污染次数
struct Block
{
public:
    int x,y;
    int px,py;
    int G,H,F;
    Block(int xx,int yy,int g = 0,int tx = 0,int ty = 0):x(xx),y(yy),G(g){
        H=abs(tx-x) +abs(ty-y);
        F = G + H;
    }
    bool operator==(const Block& n) const{return x == n.x && y == n.y;}
};

class Wash
{
    int r;
    int c;
    int dir[4][2]={{1,0},{-1,0},{0,1},{0,-1}};
    int sx,sy,tx,ty;
    int grid[15][15];

public:
    Wash(int rr,int cc):r(rr),c(cc){}
    QVector<QPair<int,int> > get_path(int g[15][15] ,int sxx,int syy,int txx,int tyy);
    bool valid(int x,int y){
        return (x>0 && x<=c && y>0 && y<=r && grid[x][y]!=1);
    }
    bool arrive(int x,int y){
        return x==tx && y == ty;
    }
};

#endif // WASH_H
