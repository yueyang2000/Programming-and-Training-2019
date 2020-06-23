#ifndef CHESS_H
#define CHESS_H
#include <QVector>
enum Type{BISHOP, KING, KNIGHT, PAWN, QUEEN, ROOK, NONE};
enum Color{BLACK, WHITE, NOCOLOR};
struct Piece
{
    Type type;
    Color color;
    bool moved = false;
    Piece(Type t = NONE,Color c = NOCOLOR):type(t),color(c){}
};
struct pos
{
    int x;
    int y;
    pos(int xx = 0,int yy = 0):x(xx),y(yy){}
    bool operator==(const pos& p){
        return p.x == x && p.y == y;
    }
};

struct packet
{
    int type;
    int x1;
    int y1;
    int x2;
    int y2;
};

class Chess
{
public:
    bool game_on = false;
    bool file = false;
    void load_file();
    void save_file();
    QString file_path;
    bool on_move = false;

    Chess(){clear();}
    Piece grid[8][8];
    void clear();
    void init();
    void set(int,int,Type,Color);
    void simple_move(int x1,int y1,int x2,int y2);
    void castling(int,int,int,int);
    void pawn_shift(int type,int x,int y);


    bool check(Color color);
    bool check_stalemate(Color color);

    Color m_color = WHITE;
    int pick_x;
    int pick_y;

    bool choose_from(int x,int y);
    QVector<pos> avaliable;
    void cal_avail(int x,int y);
    void check_castling(int,int);
    int choose_to(int x,int y);
    bool valid(int x,int y){return x>=0 && x<8 && y>=0 && y<8;}
    QVector<pos> attack_area;
    void generate_attack_area(Color color);//计算威胁color的地方



};
extern Chess c;


#endif // CHESS_H
