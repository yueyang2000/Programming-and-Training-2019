#include "chess.h"
#include <QDebug>
#include <QFile>
#include <string>

Chess c;
void Chess::load_file()
{
    clear();
    QFile data(file_path);
    if(!data.open(QIODevice::ReadOnly)){
        qDebug()<<"open failed";
    }
    QTextStream stream(&data);
    QString line;
    m_color = NOCOLOR;
    Color cur_color;
    while(true){
        line = stream.readLine();
        if(line.size() == 0) break;
        line.remove('\n');line.remove('\r');
        //用m_color暂存先手方
        if(line == "white"){
            cur_color = WHITE;
            if(m_color == NOCOLOR){
                m_color = WHITE;
            }
        }
        else if(line == "black"){
            cur_color = BLACK;
            if(m_color == NOCOLOR){
                m_color = BLACK;
            }
        }
        else{
            QStringList list = line.split(' ');
            Type type;
            QString name = list.at(0);
            if(name == "king") type = KING;
            else if(name == "queen") type = QUEEN;
            else if(name == "rook") type = ROOK;
            else if(name == "bishop") type = BISHOP;
            else if(name == "knight") type = KNIGHT;
            else type = PAWN;
             int num = list.at(1).toInt();
            for(int i = 0;i<num;i++){
                std::string loc = list[i+2].toStdString();
                int x = loc[0] - 'a';
                int y = 7 - loc[1] + '1';
                set(x,y,type,cur_color);
            }
        }
    }
    data.close();
}

void Chess::save_file()
{


    QVector<QString> black(6);
    QVector<QString> white(6);

    for(int i = 0;i<8;i++){
        for(int j = 0;j<8;j++){
            if(grid[i][j].type == NONE) continue;
            char c1 = i+'a';
            char c2 = 7-j+'1';
            QString loc = QString(c1) + QString(c2);
            //qDebug()<<loc;
            if(grid[i][j].color == WHITE){
                switch (grid[i][j].type) {
                case KING:{
                    if(white[0].size()==0){
                        white[0]=loc;
                    }
                    else{
                        white[0] = white[0]+" "+loc;
                    }
                    break;
                }
                case QUEEN:{
                    if(white[1].size()==0){
                        white[1]=loc;
                    }
                    else{
                        white[1] = white[1]+" "+loc;
                    }
                    break;
                }
                case ROOK:{
                    if(white[2].size()==0){
                        white[2]=loc;
                    }
                    else{
                        white[2] = white[2]+" "+loc;
                    }
                    break;
                }
                case BISHOP:{
                    if(white[3].size()==0){
                        white[3]=loc;
                    }
                    else{
                        white[3] = white[3]+" "+loc;
                    }
                    break;
                }
                case KNIGHT:{
                    if(white[4].size()==0){
                        white[4]=loc;
                    }
                    else{
                        white[4] = white[4]+" "+loc;
                    }
                    break;
                }
                case PAWN:{
                    if(white[5].size()==0){
                        white[5]=loc;
                    }
                    else{
                        white[5] = white[5]+" "+loc;
                    }
                    break;
                }
                }
            }
            else{//BLACK
                switch (grid[i][j].type) {
                case KING:{
                    if(black[0].size()==0){
                        black[0]=loc;
                    }
                    else{
                        black[0] = black[0]+" "+loc;
                    }
                    break;
                }
                case QUEEN:{
                    if(black[1].size()==0){
                        black[1]=loc;
                    }
                    else{
                        black[1] = black[1]+" "+loc;
                    }
                    break;
                }
                case ROOK:{
                    if(black[2].size()==0){
                        black[2]=loc;
                    }
                    else{
                        black[2] = black[2]+" "+loc;
                    }
                    break;
                }
                case BISHOP:{
                    if(black[3].size()==0){
                        black[3]=loc;
                    }
                    else{
                        black[3] = black[3]+" "+loc;
                    }
                    break;
                }
                case KNIGHT:{
                    if(black[4].size()==0){
                        black[4]=loc;
                    }
                    else{
                        black[4] = black[4]+" "+loc;
                    }
                    break;
                }
                case PAWN:{
                    if(black[5].size()==0){
                        black[5]=loc;
                    }
                    else{
                        black[5] = black[5]+" "+loc;
                    }
                    break;
                }
                }
            }
        }
    }

    QFile data(file_path);
    data.open(QIODevice::WriteOnly);
    QTextStream st(&data);
    Color color_first;
    if(game_on && !on_move){//m_color没记载先手的情况
        if(m_color == WHITE) color_first = BLACK;
        else color_first = WHITE;
    }
    else{
        color_first = m_color;
    }
    if(color_first == WHITE){
        st<<"white\n";
        if(white[0].size()) st<<"king "<<(white[0].size()+1)/3<<' '<<white[0]<<'\n';
        if(white[1].size()) st<<"queen "<<(white[1].size()+1)/3<<' '<<white[1]<<'\n';
        if(white[2].size()) st<<"rook "<<(white[2].size()+1)/3<<' '<<white[2]<<'\n';
        if(white[3].size()) st<<"bishop "<<(white[3].size()+1)/3<<' '<<white[3]<<'\n';
        if(white[4].size()) st<<"knight "<<(white[4].size()+1)/3<<' '<<white[4]<<'\n';
        if(white[5].size()) st<<"pawn "<<(white[5].size()+1)/3<<' '<<white[5]<<'\n';
        st<<"black\n";
        if(black[0].size()) st<<"king "<<(black[0].size()+1)/3<<' '<<black[0]<<'\n';
        if(black[1].size()) st<<"queen "<<(black[1].size()+1)/3<<' '<<black[1]<<'\n';
        if(black[2].size()) st<<"rook "<<(black[2].size()+1)/3<<' '<<black[2]<<'\n';
        if(black[3].size()) st<<"bishop "<<(black[3].size()+1)/3<<' '<<black[3]<<'\n';
        if(black[4].size()) st<<"knight "<<(black[4].size()+1)/3<<' '<<black[4]<<'\n';
        if(black[5].size()) st<<"pawn "<<(black[5].size()+1)/3<<' '<<black[5]<<'\n';
    }
    else{
        st<<"black\n";
        if(black[0].size()) st<<"king "<<(black[0].size()+1)/3<<' '<<black[0]<<'\n';
        if(black[1].size()) st<<"queen "<<(black[1].size()+1)/3<<' '<<black[1]<<'\n';
        if(black[2].size()) st<<"rook "<<(black[2].size()+1)/3<<' '<<black[2]<<'\n';
        if(black[3].size()) st<<"bishop "<<(black[3].size()+1)/3<<' '<<black[3]<<'\n';
        if(black[4].size()) st<<"knight "<<(black[4].size()+1)/3<<' '<<black[4]<<'\n';
        if(black[5].size()) st<<"pawn "<<(black[5].size()+1)/3<<' '<<black[5]<<'\n';
        st<<"white\n";
        if(white[0].size()) st<<"king "<<(white[0].size()+1)/3<<' '<<white[0]<<'\n';
        if(white[1].size()) st<<"queen "<<(white[1].size()+1)/3<<' '<<white[1]<<'\n';
        if(white[2].size()) st<<"rook "<<(white[2].size()+1)/3<<' '<<white[2]<<'\n';
        if(white[3].size()) st<<"bishop "<<(white[3].size()+1)/3<<' '<<white[3]<<'\n';
        if(white[4].size()) st<<"knight "<<(white[4].size()+1)/3<<' '<<white[4]<<'\n';
        if(white[5].size()) st<<"pawn "<<(white[5].size()+1)/3<<' '<<white[5]<<'\n';
    }
    data.close();
}

void Chess::clear()
{
    Piece tmp;
    for(int i =0;i < 8;i++){
        for(int j = 0;j < 8;j++){
            grid[i][j] = tmp;
        }
    }
}
void Chess::set(int x,int y,Type t,Color c){
    grid[x][y] = Piece(t,c);
}

void Chess::simple_move(int x1, int y1, int x2, int y2)
{
    //qDebug()<<"move"<<x1<<y1<<"to"<<x2<<y2;
    grid[x2][y2]=grid[x1][y1];
    grid[x2][y2].moved=true;
    grid[x1][y1] = Piece();
}

void Chess::castling(int x1, int y1, int x2, int y2)
{
    if(x1<x2){//短易位
        grid[x2][y2] = grid[x1][y1];
        grid[x2-1][y2] = grid[7][y2];
        grid[x1][y1] = Piece();
        grid[7][y2] = Piece();
    }
    else{//长易位
        grid[x2][y2] = grid[x1][y1];
        grid[x2+1][y2] = grid[0][y2];
        grid[x1][y1] = Piece();
        grid[0][y2] = Piece();
    }
}

void Chess::check_castling(int x,int y)
{
    QVector<pos> copy = avaliable;
    avaliable.clear();
    if(m_color == WHITE){
        if(x == 4 && y == 7){
            generate_attack_area(WHITE);//敌方攻击范围
            if(grid[7][7].type == ROOK && grid[6][7].type == NONE && grid[5][7].type == NONE
                    && attack_area.indexOf(pos(4,7))==-1 && attack_area.indexOf(pos(5,7))==-1
                    && attack_area.indexOf(pos(6,7))==-1){
                //王的路径不在攻击范围
                //短易位
                copy.push_back(pos(6,7));
            }
            if(grid[0][7].type == ROOK && grid[1][7].type == NONE && grid[2][7].type == NONE
                    && grid[3][7].type == NONE && attack_area.indexOf(pos(2,7))==-1
                    && attack_area.indexOf(pos(3,7))==-1 && attack_area.indexOf(pos(4,7))==-1){
                //长易位
                copy.push_back(pos(2,7));
            }
        }
    }
    else{
        if(x==4 && y==0){
            generate_attack_area(BLACK);
            if(grid[7][0].type == ROOK && grid[6][0].type == NONE && grid[5][0].type ==NONE
                    && attack_area.indexOf(pos(6,0))==-1 && attack_area.indexOf(pos(5,0))==-1
                    && attack_area.indexOf(pos(4,0))==-1){
                //短易位
                copy.push_back(pos(6,0));
            }
            if(grid[0][0].type == ROOK && grid[1][0].type == NONE && grid[2][0].type == NONE
                    && grid[3][0].type == NONE && attack_area.indexOf(pos(2,0))==-1
                    && attack_area.indexOf(pos(3,0))==-1 && attack_area.indexOf(pos(4,0))==-1){
                //长易位
                copy.push_back(pos(2,0));
            }
        }
    }
    avaliable.clear();
    avaliable = copy;
}

void Chess::pawn_shift(int type, int x, int y)
{
    switch (type) {
    case 3:{
        grid[x][y].type = QUEEN;break;
    }
    case 4:{
        grid[x][y].type = ROOK;break;
    }
    case 5:{
        grid[x][y].type = KNIGHT;break;
    }
    case 6:{
        grid[x][y].type = BISHOP;break;
    }
    default:{
        qDebug()<<"error on pawn shift!";
    }
    }
}

bool Chess::choose_from(int x, int y)
{
    //选中一个棋子，返回true
    if(grid[x][y].type == NONE) return false;
    pick_x = x;pick_y = y;
    cal_avail(x,y);
    if(grid[x][y].type == KING) check_castling(x,y);
    return true;
}
void Chess::cal_avail(int from_x,int from_y)
{
    avaliable.clear();
    Color color = grid[from_x][from_y].color;
    switch (grid[from_x][from_y].type) {
    case BISHOP:{
        int dir[4][2]={{1,1},{-1,-1},{1,-1},{-1,1}};
        for(int i=0;i<4;i++){
            int dx = dir[i][0];int dy = dir[i][1];
            int xx =from_x+dx;int yy=from_y+dy;
            while(valid(xx,yy) && grid[xx][yy].type==NONE){
                avaliable.push_back(pos(xx,yy));
                xx+=dx;yy+=dy;
            }
            if(valid(xx,yy) && grid[xx][yy].color!=color){//吃子
                avaliable.push_back(pos(xx,yy));
            }
        }
        break;
    }
    case ROOK:{
        int dir[4][2]={{1,0},{-1,0},{0,1},{0,-1}};
        for(int i=0;i<4;i++){
            int dx = dir[i][0];int dy = dir[i][1];
            int xx =from_x+dx;int yy=from_y+dy;
            while(valid(xx,yy) && grid[xx][yy].type==NONE){
                avaliable.push_back(pos(xx,yy));
                xx+=dx;yy+=dy;
            }
            if(valid(xx,yy)&& grid[xx][yy].color!=color){//吃子
                avaliable.push_back(pos(xx,yy));
            }
        }
        break;
    }
    case QUEEN:{
        int dir[8][2]={{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,-1},{1,-1},{-1,1}};
        for(int i=0;i<8;i++){
            int dx = dir[i][0];int dy = dir[i][1];
            int xx =from_x+dx;int yy=from_y+dy;
            while(valid(xx,yy) && grid[xx][yy].type==NONE){
                avaliable.push_back(pos(xx,yy));
                xx+=dx;yy+=dy;
            }
            if(valid(xx,yy)&& grid[xx][yy].color!=color){//吃子
                avaliable.push_back(pos(xx,yy));
            }
        }
        break;
    }
    case KING:{
        int dir[8][2]={{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,-1},{1,-1},{-1,1}};
        for(int i=0;i<8;i++){
            int dx = dir[i][0];int dy = dir[i][1];
            int xx =from_x+dx;int yy=from_y+dy;
            if(valid(xx,yy)&& grid[xx][yy].color!=color){
                avaliable.push_back(pos(xx,yy));
                //qDebug()<<"king add"<<xx<<yy;
            }
        }
        break;
    }
    case KNIGHT:{
        int dir[8][2]={{2,1},{1,2},{-2,1},{-1,2},{2,-1},{1,-2},{-2,-1},{-1,-2}};
        for(int i=0;i<8;i++){
            int dx = dir[i][0];int dy = dir[i][1];
            int xx =from_x+dx;int yy=from_y+dy;
            if(valid(xx,yy) && grid[xx][yy].color!=color){
                avaliable.push_back(pos(xx,yy));
            }
        }
        break;
    }
    case PAWN:{
        if(grid[from_x][from_y].color == BLACK){//黑在上
            //斜线吃
            if(valid(from_x+1,from_y+1) && grid[from_x+1][from_y+1].type!=NONE
                    && grid[from_x+1][from_y+1].color!=color){
                avaliable.push_back(pos(from_x+1,from_y+1));
            }
            if(valid(from_x-1,from_y+1) && grid[from_x-1][from_y+1].type!=NONE
                    && grid[from_x-1][from_y+1].color!=color){
                avaliable.push_back(pos(from_x-1,from_y+1));
            }
            //向前一步
            if(valid(from_x,from_y+1) && grid[from_x][from_y+1].type==NONE){
                avaliable.push_back(pos(from_x,from_y+1));
            }
            //向前两步
            if(valid(from_x,from_y+2) && grid[from_x][from_y+2].type==NONE
                    && grid[from_x][from_y].moved == false
                    && grid[from_x][from_y+1].type == NONE){
                avaliable.push_back(pos(from_x,from_y+2));
            }
        }
        else{//白在下
            //斜线吃
            if(valid(from_x+1,from_y-1) && grid[from_x+1][from_y-1].type!=NONE
                    && grid[from_x+1][from_y-1].color!=color){
                avaliable.push_back(pos(from_x+1,from_y-1));
            }
            if(valid(from_x-1,from_y-1) && grid[from_x-1][from_y-1].type!=NONE
                    && grid[from_x-1][from_y-1].color!=color){
                avaliable.push_back(pos(from_x-1,from_y-1));
            }
            //向前一步
            if(valid(from_x,from_y-1) && grid[from_x][from_y-1].type==NONE){
                avaliable.push_back(pos(from_x,from_y-1));
            }
            //向前两步
            if(valid(from_x,from_y-2) && grid[from_x][from_y-2].type==NONE
                    && grid[from_x][from_y].moved == false
                    && grid[from_x][from_y-1].type == NONE){
                avaliable.push_back(pos(from_x,from_y-2));
            }
        }
        break;
    }
    default:{
        qDebug()<<"cal avail at nothing!"<<from_x<<from_y;
    }
    }
}

int Chess::choose_to(int x, int y)
{

    if(avaliable.indexOf(pos(x,y)) != -1){
        //可走
        if(grid[pick_x][pick_y].type==KING && abs(pick_x-x)==2){//王车易位
            castling(pick_x,pick_y,x,y);
            avaliable.clear();
            return 2;
        }
        else if(grid[pick_x][pick_y].type == PAWN && (y==0||y==7)){
            simple_move(pick_x,pick_y,x,y);
            avaliable.clear();
            return 3;
        }
        else{
            simple_move(pick_x,pick_y,x,y);
            avaliable.clear();
            return 1;
        }
    }
    else{
        return 0;
    }
}

void Chess::generate_attack_area(Color color)
{
    attack_area.clear();
    for(int i = 0;i<8;i++){
        for(int j=0;j<8;j++){
            if(grid[i][j].type!=NONE && grid[i][j].color!=color){
                //敌方单位
                if(grid[i][j].type == KING && grid[i][j].color!=color){
                    //敌方王单独考虑，避免循环调用
                    avaliable.clear();
                    int dir[8][2]={{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,-1},{1,-1},{-1,1}};
                    for(int i=0;i<8;i++){
                        int dx = dir[i][0];int dy = dir[i][1];
                        int xx =i+dx;int yy=j+dy;
                        if(valid(xx,yy)&& grid[xx][yy].color!=m_color){
                            avaliable.push_back(pos(xx,yy));
                        }
                    }
                }
                else{
                    cal_avail(i,j);
                }
                for(int lp=0;lp<avaliable.size();lp++){
                    if(attack_area.indexOf(avaliable[lp])==-1){
                        attack_area.push_back(avaliable[lp]);
                    }
                }
            }
        }
    }

//    QString t;
//    for(pos lp:attack_area){
//        t+='('+QString::number(lp.x)+','+QString::number(lp.y)+')';
//    }
//    qDebug()<<t;
}

bool Chess::check(Color color)
{
    for(int i = 0;i<8;i++){
        for(int j=0;j<8;j++){
            if(grid[i][j].type==KING && grid[i][j].color==color){
                return true;
            }
        }
    }
    return false;
}

bool Chess::check_stalemate(Color color)
{
    generate_attack_area(color);
    int king_x =-1;int king_y=-1;
    for(int i = 0;i<8;i++){
        if(king_x>=0) break;
        for(int j=0;j<8;j++){
            if(grid[i][j].type==KING && grid[i][j].color==color){
                king_x = i;king_y = j;
                break;
            }
        }
    }

    if(attack_area.indexOf(pos(king_x,king_y))!=-1){
        //将军状态，无逼和
        qDebug()<<"check";
        return false;
    }

    //考虑所有己方棋子的所有可能走情况，如果都会送吃王就逼和
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(grid[i][j].color!=color) continue;
            if(grid[i][j].type==KING){
                qDebug()<<i<<j<<"is king!";
            }


            cal_avail(i,j);
            qDebug()<<"test"<<i<<j;
            QVector<pos> copy = avaliable;//copy中存了所有可能到达的位置
            for(int k=0;k<copy.size();k++){
                int xx=copy[k].x;int yy=copy[k].y;
                qDebug()<<"to"<<xx<<yy;
                Piece tmp = grid[xx][yy];
                grid[xx][yy]=grid[i][j];
                grid[i][j]=Piece();//假装挪过去
                generate_attack_area(color);
                if(grid[xx][yy].type==KING){
                    if(attack_area.indexOf(pos(xx,yy))==-1){//此处可走
                        grid[king_x][king_y]=grid[xx][yy];
                        grid[xx][yy]=tmp;//放回去
                        qDebug()<<"can move king"<<king_x<<king_y<<"to"<<xx<<yy;
                        return false;
                    }
                    else{
                        grid[king_x][king_y]=grid[xx][yy];
                        grid[xx][yy]=tmp;//放回去
                        continue;
                    }
                }
                else{
                    if(attack_area.indexOf(pos(king_x,king_y))==-1){
                        grid[i][j]=grid[xx][yy];
                        grid[xx][yy]=tmp;
                        qDebug()<<"can move"<<i<<j<<"to"<<xx<<yy;
                        return false;
                    }
                    else{
                        grid[i][j]=grid[xx][yy];
                        grid[xx][yy]=tmp;
                        continue;
                    }
                }
            }
        }
    }
    return true;
}
void Chess::init()
{
    clear();

    set(0,0,ROOK,BLACK);set(1,0,KNIGHT,BLACK);set(2,0,BISHOP,BLACK);
    set(3,0,QUEEN,BLACK);set(4,0,KING,BLACK);set(5,0,BISHOP,BLACK);
    set(6,0,KNIGHT,BLACK);set(7,0,ROOK,BLACK);
    for(int i = 0;i<8;i++){
        set(i,1,PAWN,BLACK);
    }
    set(0,7,ROOK,WHITE);set(1,7,KNIGHT,WHITE);set(2,7,BISHOP,WHITE);
    set(3,7,QUEEN,WHITE);set(4,7,KING,WHITE);set(5,7,BISHOP,WHITE);
    set(6,7,KNIGHT,WHITE);set(7,7,ROOK,WHITE);
    for(int i = 0;i<8;i++){
        set(i,6,PAWN,WHITE);
    }
}
