
#include "core.h"
#include <algorithm>
#include <QMessageBox>
#include <string.h>
#include <QDebug>
int Core::find_index(int x, int y)
{
    int i = -1;
    for(int lp = 0;lp<p.size();lp++){
        if(p[lp].state == DISABLED) continue;
        if(p[lp].x == x && p[lp].y == y){
            i = lp;
        }
    }
    if(i == -1){
        qDebug()<<"Move target"<<x<<y<<"not found!";
    }
    return i;
}


bool Core::near_input(int x, int y)
{
    for(int lp = 0;lp<inputs.size();lp++){
        if(inputs[lp].first == x && inputs[lp].second == y){
            return true;
        }
    }
    qDebug()<<"no input!";
    return false;
}

void Core::static_check()
{
    for(int t = 0;t<=max_t;t++){
        for(int i = 0;i < phases[t].size() -1; i++){
            if(phases[t][i].state == DISABLED) continue;
            for(int j = i+1; j < phases[t].size();j ++){
                if(phases[t][j].state == DISABLED) continue;
                int dx = phases[t][i].x - phases[t][j].x;
                int dy = phases[t][i].y - phases[t][j].y;
                if(dx*dx + dy*dy <= 2){
                    //是个错误
                    if(max_t > t){//错误靠前
                        max_t = t;
                        err = STATIC_LIMIT;
                        err_pos[0] = phases[t][i].x;err_pos[1] = phases[t][i].y;
                        err_pos[2] = phases[t][j].x;err_pos[3] = phases[t][j].y;
                    }
                }
            }
        }
    }
}

void Core::dynamic_check()
{
    //qDebug()<<"dynamic check";
    for(int t = 0;t<=max_t-1;t++){
        for(int i = 0;i < phases[t].size() -1; i++){
            if(phases[t][i].state == DISABLED) continue;
            for(int j = i+1; j < phases[t].size();j ++){
                if(phases[t+1][j].state == DISABLED) continue;
                int dx = phases[t][i].x - phases[t+1][j].x;
                int dy = phases[t][i].y - phases[t+1][j].y;
                if(dx*dx + dy*dy <= 2){
                    //是个错误
                    if(max_t > t+1 || err == NO_ERROR){//错误靠前
                        max_t = t+1;
                        err = DYNAMIC_LIMIT;
                        err_pos[0] = phases[t][i].x; err_pos[1] = phases[t][i].y;
                        err_pos[2] = phases[t+1][j].x; err_pos[3] = phases[t+1][j].y;
                    }
                }
            }
        }
    }
}

void Core::setup(Setting setting)
{

    if(!read_file(setting.path)){
        QMessageBox m; m.setText("File not valid.");m.exec();
    }
    else{
        max_t = operations[operations.size()-1].time +1;//最大时间定位最后操作时间+1
        phases.clear();
        ready = true;
        inputs = setting.inputs;
        r = setting.r;
        c = setting.c;
        ox = setting.ox; oy = setting.oy;
        wash_mode = setting.wash_mode;
        if(wash_mode){
            wash = new Wash(r,c);
            qDebug()<<"core get wash mode";
        }
        else{
            process_operation();
            static_check();
            dynamic_check();
        }
    }
}

bool Core::read_file(QString path)
{
    operations.clear();

    QFile file(path);
    if(file.open(QIODevice::ReadOnly)){
        while(true){
            QString  t = file.readLine();
            if(t.size() == 0) break;

            t.remove('\n');
            t.remove('\r');
            t.remove(';');

            t.replace(' ',',');
            //qDebug()<<t;
            QStringList args = t.split(',');

            if(args[0] == "Input"){
                Operate o(Input);
                o.time = args[1].toInt();
                o.val.push_back(args[2].toInt());
                o.val.push_back(args[3].toInt());
                operations.push_back(o);
            }
            else if(args[0] == "Move"){
                Operate o(Move);
                o.time = args[1].toInt();
                o.val.push_back(args[2].toInt());
                o.val.push_back(args[3].toInt());
                o.val.push_back(args[4].toInt());
                o.val.push_back(args[5].toInt());
                operations.push_back(o);
            }
            else if(args[0] == "Merge"){
                Operate o(Merge);
                o.time = args[1].toInt();
                o.val.push_back(args[2].toInt());
                o.val.push_back(args[3].toInt());
                o.val.push_back(args[4].toInt());
                o.val.push_back(args[5].toInt());
                operations.push_back(o);
            }
            else if(args[0] == "Mix"){
                Operate o(Mix);
                o.time = args[1].toInt();
                for(int i = 2;i<args.size();i++){
                    o.val.push_back(args[i].toInt());
                }
                operations.push_back(o);
            }
            else if(args[0] == "Split"){
                Operate o(Split);
                o.time = args[1].toInt();
                o.val.push_back(args[2].toInt());
                o.val.push_back(args[3].toInt());
                o.val.push_back(args[4].toInt());
                o.val.push_back(args[5].toInt());
                o.val.push_back(args[6].toInt());
                o.val.push_back(args[7].toInt());
                operations.push_back(o);
            }
            else if(args[0] == "Output"){
                Operate o(Output);
                o.time = args[1].toInt();
                o.val.push_back(args[2].toInt());
                o.val.push_back(args[3].toInt());
                operations.push_back(o);
            }
            else{
                qDebug()<<"file not valid";
                return false;
            }
        }
        std::sort(operations.begin(),operations.end(),cmp);//按时间排序

        qDebug()<<"read succeed";
        return true;
    }
    else{
        qDebug()<<"open file failed";
        return false;
    }
}

void Core::run(int t)
{
    //qDebug()<<"run"<<t;
    p.clear();
    new_id = 0;


    for(int index = 0;index<operations.size();index++){
        int op_time = operations[index].time;
        if(op_time >= t) break;//只执行t之前的操作

        switch(operations[index].type){
        case Input:{
            Node n;n.x = operations[index].val[0];n.y = operations[index].val[1];
            //检查
            if(!near_input(n.x,n.y)){
                err = NO_INPUT;
                err_pos[0] = n.x; err_pos[0] = n.y;
                max_t = t;
            }
            //qDebug()<<"input"<<n.x<<n.y;
            n.id = new_id; new_id++;
            if(n.trace.count(QPair<int,int> (n.x,n.y)) == 0)
                n.trace.push_back(QPair<int,int> (n.x,n.y));
            p.push_back(n);
            break;
        }
        case Move:{

            int i = find_index(operations[index].val[0],operations[index].val[1]);

            p[i].x = operations[index].val[2];p[i].y = operations[index].val[3];
            if(op_time + 1 == t){
                p[i].just_finish = Move;
            }
            if(p[i].trace.count(QPair<int,int> (p[i].x,p[i].y)) == 0)
                 p[i].trace.push_back(QPair<int,int> (p[i].x,p[i].y));

            //qDebug()<<"move"<<p[i].x<<p[i].y;
            break;
        }
        case Merge:{
            int x1 = operations[index].val[0];int y1 = operations[index].val[1];
            int x2 = operations[index].val[2];int y2 = operations[index].val[3];
            //qDebug()<<"Merge"<<x1<<y1<<x2<<y2;
            int i1 = find_index(x1,y1); int i2 = find_index(x2,y2);
            Node n;//新节点
            n.id = new_id; new_id++;//分配新id
            n.x = (x1+x2)/2; n.y = (y1+y2)/2;
            n.trace.push_back(QPair<int,int>(n.x,n.y));

            if(t == op_time+1){//一个长椭圆
                n.state = MERGING;
                n.horizontal = (y1 == y2);
            }
            else{
                n.state = MERGED;
                if(op_time + 2 == t){
                    n.just_finish = Merge;
                }
            }
            p.push_back(n);
            p[i1].state = DISABLED;
            p[i2].state = DISABLED;
            break;
        }
        case Split:{
            int x1 = operations[index].val[0];int y1 = operations[index].val[1];
            int x2 = operations[index].val[2];int y2 = operations[index].val[3];
            int x3 = operations[index].val[4];int y3 = operations[index].val[5];
            int i = find_index(x1,y1);
            //if(p[i].state!=MERGED){emit report_error(FALSE_SPLIT); return nullptr;}
            if(t == op_time + 1){//一个长椭圆
                p[i].state = SPLITING;
                p[i].horizontal = (y2 == y3);
            }
            else{
                Node n1(x2,y2);n1.id = new_id;new_id++;
                Node n2(x3,y3);n2.id = new_id;new_id++;
                if(t == op_time+2){
                    n1.just_finish = Split; n2.just_finish = Split;
                }
                n1.trace.push_back(QPair<int,int>(n1.x,n1.y));
                n2.trace.push_back(QPair<int,int>(n2.x,n2.y));
                p[i].state = DISABLED;
                p.push_back(n1);p.push_back(n2);//新加入两个节点
            }
            break;
        }
        case Mix:{
            int i = find_index(operations[index].val[0],operations[index].val[1]);//获得最初点的位置
            int step = operations[index].val.size()/2 - 1;//命令的总步数
            if(op_time + step >t){//如果执行不完
                step = t - op_time;
            }
            if(op_time + step >=t){
                p[i].just_finish = Move;
            }
            for(int lp = 1;lp <=step;lp++){
                if(p[i].trace.count(QPair<int,int>
                                       (operations[index].val[lp*2],
                                        operations[index].val[lp*2+1])) == 0){
                 p[i].trace.push_back(QPair<int,int>
                                     (operations[index].val[lp*2],operations[index].val[lp*2+1]));
            }

            }
            p[i].x = operations[index].val[step*2];
            p[i].y = operations[index].val[step*2+1];//更新位置
            break;
        }
        case Output:{
            //qDebug()<<"output";
            int x = operations[index].val[0];int y = operations[index].val[1];
            if(x!=ox || y!=oy) {
                err = NO_OUTPUT;
                err_pos[0] = x; err_pos[1] = y;
                max_t = t;
            }
            int i = find_index(x,y);//待输出节点
            p[i].state = DISABLED;
        }
        }
    }

    //污染情况

    con.clear();
    for(int i = 0;i<c;i++){
        QVector<int> temp;
        for(int j = 0;j<r;j++){
            temp.push_back(0);
        }
        con.push_back(temp);
    }
    for(int i = 0;i<p.size();i++){
        for(int j = 0;j < p[i].trace.size();j++){
            //qDebug()<<i<<j;
            con[p[i].trace[j].first-1][p[i].trace[j].second-1] += 1;
        }
    }
}


void Core::process_operation()
{
    for(int t = 0; t<= max_t;t++){
        run(t);
        phases.push_back(p);
        contaminations.push_back(con);
        //qDebug()<<"phase"<<t<<"size"<<p.size();
    }
}

