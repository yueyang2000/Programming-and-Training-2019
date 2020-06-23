#ifndef CORE_H
#define CORE_H
#include <QWidget>
#include <QFile>
#include <QDebug>
#include  "setting.h"
#include "wash.h"
#include "error.h"
enum State{NORMAL,MERGING,MERGED,SPLITING,DISABLED,WASH};//液滴状态
enum Type{NOT,Input, Move, Merge, Mix, Split, Output};//指令种类
struct Node
{
    int id;
    State state;
    int x,y;
    bool horizontal;//MERGING

    Type just_finish = NOT;//刚刚完成的操作
    QVector<QPair<int,int> > trace{};

    Node(int xx = -1,int yy = -1,State s = NORMAL):x(xx),y(yy),state(s){}
    bool operator==(const Node& n){return x == n.x && y == n.y;}
};

typedef QVector<Node> phase;//每时刻的状态
typedef QPair<int,int> position;


struct Operate
{
    Type type;
    int time;
    QVector<int> val;
    Operate(Type t):type(t){}
    Operate(){}
};
inline bool cmp(Operate &op1,Operate& op2){return op1.time<op2.time; }


class Core: public QObject
{
    Q_OBJECT

    //QVector<phase> phases;
    bool ready = false;

    QVector<Operate> operations;
    phase p;
    QVector<phase> phases;
    contamination con;
    QVector<contamination> contaminations;

    int phase_num = -1;
    int node_num = 0;
    int new_id = 0;

    int max_t = -1;
    int r,c;
    QVector<QPair<int,int> > inputs;
    int ox,oy;//ouput坐标

    void static_check();
    void dynamic_check();
    int find_index(int x,int y);
    bool near_input(int x,int y);


    Wash* wash;
    bool wash_mode = false;

    int cur_operation = 0;
public:
    explicit Core(QWidget *parent = nullptr){}
    void setup(Setting setting);
    bool read_file(QString path);
    int max_time(){return max_t;}
    void process_operation();
    phase* get_phase(int t){return &phases[t];}
    contamination* get_con(int t){return &contaminations[t];}
    void run(int t);
    Error err = NO_ERROR;
    int err_pos[4];

    phase wash_phase;
    int disable[15][15]{{}};
    int wash_time = 0;
    phase* get_wash_phase(QVector<QPair<int,int> >* V);
    bool trace_to_clean();
    void wash_trace(int x,int y);
    void run_wash(int t);
    int find_index_wash(int x,int y);
    bool check_contamination();
    void reset_wash();
signals:
    void report_error(Error e);
private slots:
    void disable_update(int x,int y);
};

#endif // CORE_H
