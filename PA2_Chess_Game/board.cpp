#include "board.h"
#include <QPainter>


Board::Board(QWidget *parent):QWidget (parent)
{
    pics.push_back(QImage(":/resource/icons/black_bishop.png"));
    pics.push_back(QImage(":/resource/icons/black_king.png"));
    pics.push_back(QImage(":/resource/icons/black_knight.png"));
    pics.push_back(QImage(":/resource/icons/black_pawn.png"));
    pics.push_back(QImage(":/resource/icons/black_queen.png"));
    pics.push_back(QImage(":/resource/icons/black_rook.png"));
    pics.push_back(QImage(":/resource/icons/white_bishop.png"));
    pics.push_back(QImage(":/resource/icons/white_king.png"));
    pics.push_back(QImage(":/resource/icons/white_knight.png"));
    pics.push_back(QImage(":/resource/icons/white_pawn.png"));
    pics.push_back(QImage(":/resource/icons/white_queen.png"));
    pics.push_back(QImage(":/resource/icons/white_rook.png"));
    for(int i =0;i<12;i++){
        pics[i].scaled(75,75,Qt::IgnoreAspectRatio);
    }
    dia = new Dialog(this);
}

void Board::paintEvent(QPaintEvent *ev)
{
    QPainter p(this);
    p.translate(50,50);
    p.setPen(Qt::NoPen);
    for(int i = 0;i<8;i++){
        for(int j =0;j<8;j++){
            if((i+j)%2 == 0){
                p.setBrush(Qt::white);
            }
            else{
                p.setBrush(Qt::gray);
            }
            p.drawRect(i*75,j*75,75,75);
        }
    }

    p.setBrush(Qt::NoBrush);
    QPen pen;
    pen.setWidth(3);
    //画出可走区域

    if(picking){
        pen.setColor(Qt::red);
        p.setPen(pen);
        p.drawRect(pick_x*75 + 5,pick_y*75 +5,65,65);//选中的棋子
        pen.setColor(Qt::blue);
        p.setPen(pen);
        for(int lp = 0;lp<c.avaliable.size();lp++){
            int x = c.avaliable[lp].x;
            int y = c.avaliable[lp].y;
            p.drawRect(x*75 +5,y*75 + 5,65,65);

        }
    }

    for(int i = 0;i<8;i++){
        for(int j = 0;j<8;j++){
            int index = (int) c.grid[i][j].type;
            if(index == 6) continue;
            if(c.grid[i][j].color == WHITE){
                index = index+6;
            }
            QPixmap pix;
            pix = QPixmap::fromImage(pics[index]);
            p.drawPixmap(i*75,j*75,75,75,pix);
        }
    }

    pen.setColor(Qt::black);
    p.setPen(pen);
    for(int i = 0;i<8;i++){
        for(int j = 0;j<8;j++){
            p.drawRect(i*75,j*75,75,75);
        }
    }
}

void Board::mousePressEvent(QMouseEvent *ev)
{
    if(!c.game_on || !c.on_move) return;//尚未开始游戏，不能点击
    if(ev->pos().x()<=50 || ev->pos().y()<=50) return;
    int x = (ev->pos().x()-50)/75;
    int y = (ev->pos().y()-50)/75;
    if(x>=8 ||y>=8) return;
    if(!picking && c.grid[x][y].color!=c.m_color){
        //直接选择了空或对方棋子
        return;
    }
    if(x == pick_x && y==pick_y && picking) {
        //取消选择
        c.avaliable.clear();
        picking = false;update();
        return;
    }
    if(!picking){
        picking = true;
        if(c.choose_from(x,y)){
            pick_x = x;pick_y=y;
            picking = true;
            update();
        }
        else{
            picking = false;
        }
    }
    else{
        int type = c.choose_to(x,y);
        if(type == 1 || type == 2){//普通走子或王车易位
            update();
            picking = false;
            emit move_piece(packet{type,pick_x,pick_y,x,y});
        }
        else if(type == 3){//兵升变
            type = pawn_shift();
            c.pawn_shift(type,x,y);
            update();
            picking = false;
            emit move_piece(packet{type,pick_x,pick_y,x,y});
        }
        if(type!=0){
            if(!c.check((Color)(1-c.m_color))){
                emit win();
            }
            else if(c.check_stalemate(Color(1-c.m_color))){//看对方有没有地方走，逼和
                emit stalemate();
            }
        }
    }
}

int Board::pawn_shift()
{
//QUEEN 3 ROOK 4 KNIGHT 5 BISHOP 6
    if(dia->exec() == QDialog::Accepted){
        return dia->choice;
    }
}

