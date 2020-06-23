
#include "core.h"
#include <list>
using namespace std;

QVector<QPair<int,int> > Wash::get_path(int g[15][15], int sxx, int syy, int txx, int tyy)
{

    //A-Star最短路算法
    //输入起点、终点和障碍，输出最短路径
    sx = sxx;sy = syy; tx = txx;ty = tyy;
    for(int i = 0;i<15;i++){
        for(int j = 0;j<15;j++){
            grid[i][j] = g[i][j];
        }
    }
    qDebug()<<"calculate"<<sx<<sy<<tx<<ty;

    list<Block> open;
    list<Block> close;
    Block current(sx,sy,0,tx,ty);
    current.px = 0;
    current.py = 0;
    open.push_back(current);

    while(!open.empty()){//open非空时
        int minF = 99999;
        for(auto it = open.begin();it!=open.end();it++){
            if(minF > it->F){
                minF = it->F;
                current = *it;//current 为最小F的节点
            }
        }
        //qDebug()<<"current "<<current.x<<current.y;
        open.remove(current);//将current从open移除
        close.push_back(current);//将current加入close
        for(int k = 0;k<4;k++){//四个方向
            int x = current.x + dir[k][0];
            int y = current.y + dir[k][1];
            //qDebug()<<"try "<<x<<y;
            if(!valid(x,y)) continue;
            Block n(x,y,current.G+1,tx,ty);
            auto iter1 = find(close.begin(),close.end(),n);
            if(iter1==close.end()){//不在close列表中
                //qDebug()<<"not in close";
                auto iter2 = find(open.begin(),open.end(),n);
                if(iter2 == open.end()){//不在open列表中
                    //qDebug()<<"not in open";
                    n.px = current.x;
                    n.py = current.y;
                    open.push_back(n);
                }
                else{//在open 列表中
                    //qDebug()<<"in open";
                    if(iter2->F > n.F){//估计值更小，更新F,G,parent
                        iter2->F = n.F;
                        iter2->G = n.G;
                        iter2->px=  current.x;
                        iter2->py= current.y;
                        //qDebug()<<"update";
                    }
                }
            }
            //else{qDebug()<<"in close";}
        }
    }
    QVector<QPair<int,int> >ans;
    //输出结果
    if(find(close.begin(),close.end(),Block(tx,ty)) == close.end()){
        qDebug()<<"no path!";
    }
    else{
        int cx = tx; int cy = ty;
        qDebug()<<"have path!";
        while(!(cx == sx && cy == sy)){
            ans.prepend(QPair<int,int>(cx,cy));
            auto iter = find(close.begin(),close.end(),Block(cx,cy));
            cx = iter->px;cy = iter->py;
        }
    }

    return ans;

}

phase *Core::get_wash_phase(QVector<QPair<int,int> >* V)
{
    //qDebug()<<*V;
    if(V->size() == 0){//没有wash液滴的行动
        qDebug()<<"no wash move";
        //判断wash液滴是否还在
        int i;
        for(i = 0;i<wash_phase.size();i++){
            if(wash_phase[i].state == WASH){
                break;
            }
        }
        if(i!=wash_phase.size()){//有wash液滴，i是其index
            qDebug()<<"have wash";
            if(wash_phase[i].x == r && wash_phase[i].y == 1){
                //到达waste，回收
                qDebug()<<"output wash";
                wash_phase.remove(i);
                return &wash_phase;
            }
            else{
                qDebug()<<"recalculate route";
                //未到达，重新规划路径
                //生成目标图,//将不可走区域置为1，trace区域置为2
                int grid[15][15] = {{}};
                for(int lp1 = 0;lp1<15;lp1++){
                    for(int lp2 = 0; lp2<15;lp2++){
                        grid[lp1][lp2] = disable[lp1][lp2];
                    }
                }

                for(int lp = 0;lp<wash_phase.size();lp++){
                    if(wash_phase[lp].state == WASH) continue;
                    int x = wash_phase[lp].x;
                    int y = wash_phase[lp].y;
                    if(wash_phase[lp].state!=DISABLED){
                        grid[x-1][y-1]=grid[x][y-1]=grid[x+1][y-1]=1;
                        grid[x-1][y]=grid[x][y]=grid[x+1][y]=1;
                        grid[x-1][y+1]=grid[x][y+1]=grid[x+1][y+1]=1;
                    }
                    for(int lpp = 0;lpp<wash_phase[lp].trace.size();lpp++){
                        //加入所有trace
                        int trX = wash_phase[lp].trace[lpp].first;
                        int trY = wash_phase[lp].trace[lpp].second;
                        if(grid[trX][trY]!=1){
                            grid[trX][trY]=2;
                        }
                    }
                }
                QVector<position> result;
                QVector<position> back;
                bool has_result = false;
                for(int cc=1;cc<=c;cc++){
                    if(has_result) break;
                    for(int rr=1;rr<=r;rr++){
                        if(has_result) break;
                        if(grid[cc][rr] == 2){
                            //从当前位置走到清洗位置
                            result = wash->get_path(grid,wash_phase[i].x,wash_phase[i].y,cc,rr);
                            back = wash->get_path(grid,cc,rr,r,1);
                            if(result.size()!=0 && (back.size()!=0 || (cc==r && rr==1))){
                                //能走到也能出去,加入队列
                                has_result = true;
                                while(!result.empty()){
                                    V->prepend(result.front());
                                    result.pop_front();
                                }
                                while(!back.empty()){
                                    V->prepend(back.front());
                                    back.pop_front();
                                }
                            }
                        }
                    }
                }
                if(!has_result){
                    //没有可清洗的trace,准备去往output
                    back = wash->get_path(grid,wash_phase[i].x,wash_phase[i].y,r,1);
                    if(back.size()==0){
                        qDebug()<<"no way to go!";
                        emit report_error(NO_WAY_TO_GO);
                        return &wash_phase;
                    }
                    else{
                        qDebug()<<"go home";
                        while(!back.empty()){
                            V->prepend(back.front());
                            back.pop_front();
                        }
                    }
                }
                else{
                    qDebug()<<"have next target";
                }
                //走一步，返回
                wash_phase[i].x = V->back().first;
                wash_phase[i].y = V->back().second;
                wash_trace(wash_phase[i].x,wash_phase[i].y);
                return &wash_phase;
            }
        }
        else{//没有wash液滴，可能会再派出液滴
            //生成目标图
            qDebug()<<"no wash";
            int grid[15][15] = {{}};
            for(int lp1 = 0;lp1<15;lp1++){
                for(int lp2 = 0; lp2<15;lp2++){
                    grid[lp1][lp2] = disable[lp1][lp2];
                }
            }
            for(int lp = 0;lp<wash_phase.size();lp++){
                if(wash_phase[lp].state == WASH) continue;

                int x = wash_phase[lp].x;
                int y = wash_phase[lp].y;
                if(wash_phase[lp].state!=DISABLED){
                    grid[x-1][y-1]=grid[x][y-1]=grid[x+1][y-1]=1;
                    grid[x-1][y]=grid[x][y]=grid[x+1][y]=1;
                    grid[x-1][y+1]=grid[x][y+1]=grid[x+1][y+1]=1;
                }
                for(int lpp = 0;lpp<wash_phase[lp].trace.size();lpp++){
                    //加入所有trace
                    int trX = wash_phase[lp].trace[lpp].first;
                    int trY = wash_phase[lp].trace[lpp].second;
                    if(grid[trX][trY]!=1){
                        grid[trX][trY]=2;
                    }
                }
            }
            QVector<position> result;
            QVector<position> back;
            bool has_result = false;
            if((grid[1][c]!=1) && (grid[r][1]!=1)){//出入口没被堵住
            for(int cc=1;cc<=c;cc++){
                if(has_result) break;
                for(int rr=1;rr<=r;rr++){
                    if(has_result) break;
                    if(grid[cc][rr] == 2){
                        //从清洗入口走到清洗位置
                        //qDebug()<<"cc="<<cc<<"rr="<<rr;
                        result = wash->get_path(grid,1,c,cc,rr);
                        back = wash->get_path(grid,cc,rr,r,1);
                        if(result.size()!=0 && (back.size()!=0 || (cc==r && rr==1))){
                            //能走到也能出去,添加新液滴，加入路径队列
                            Node n(1,c,WASH);
                            wash_phase.push_back(n);
                            wash_trace(1,c);
                            has_result = true;
                            while(!result.empty()){
                                V->prepend(result.front());
                                result.pop_front();
                            }
                            while(!back.empty()){
                                V->prepend(back.front());
                                back.pop_front();
                            }
                            qDebug()<<"send another wash";
                            return &wash_phase;
                        }
                    }
                }
            }}
            if(!has_result){
                //没有结果,不派出液滴，走下一步
                qDebug()<<"go with operation";
                wash_time++;
                run_wash(wash_time);
                return &wash_phase;
            }
        }
    }
    else{//有wash液滴的操作，走一步
        qDebug()<<"have wash move";
        int i;
        for(i = 0;i<wash_phase.size();i++){
            if(wash_phase[i].state == WASH){
                break;
            }
        }
        if(i == wash_phase.size()){
            qDebug()<<"wash no found!";
        }
        wash_phase[i].x = V->back().first;
        wash_phase[i].y = V->back().second;
        V->pop_back();
        wash_trace(wash_phase[i].x,wash_phase[i].y);
    }
    return &wash_phase;
}
void Core::run_wash(int t){
    qDebug()<<"run wash"<<t;
    for(int index = 0;index<operations.size();index++){
        int op_time = operations[index].time;
        if(op_time >=t) break;
        switch (operations[index].type){
        case Input:{
            if(op_time +1 == t){
                Node n;n.x = operations[index].val[0];n.y = operations[index].val[1];
                qDebug()<<"input"<<n.x<<n.y;
                n.id = new_id; new_id++;
                n.trace.push_back(QPair<int,int> (n.x,n.y));
                wash_phase.push_back(n);
            }
            break;
        }
        case Move:{
            if(op_time +1 <t) break;

            int i = find_index_wash(operations[index].val[0],operations[index].val[1]);

            wash_phase[i].x = operations[index].val[2];wash_phase[i].y = operations[index].val[3];

            if(wash_phase[i].trace.count(QPair<int,int> (wash_phase[i].x,wash_phase[i].y)) == 0)
                 wash_phase[i].trace.push_back(QPair<int,int> (wash_phase[i].x,wash_phase[i].y));

            qDebug()<<"move"<<wash_phase[i].x<<wash_phase[i].y;
            break;
        }
        case Merge:{
            int x1 = operations[index].val[0];int y1 = operations[index].val[1];
            int x2 = operations[index].val[2];int y2 = operations[index].val[3];
            if(t == op_time +2){//长椭圆即将合并成功
                qDebug()<<"merge finish"<<(x1+x2)/2<<(y1+y2)/2;
                int i = find_index_wash((x1+x2)/2,(y1+y2)/2);
                wash_phase[i].state = MERGED;

            }
            else if(t == op_time +1){//两圆合一
                int i1 = find_index_wash(x1,y1); int i2 = find_index_wash(x2,y2);
                Node n;//新节点
                n.id = new_id; new_id++;//分配新id
                n.x = (x1+x2)/2; n.y = (y1+y2)/2;
                n.state = MERGING;
                n.horizontal = (y1 == y2);
                wash_phase.push_back(n);
                qDebug()<<"merging"<<n.x<<n.y;
                wash_phase[i1].state = DISABLED;
                wash_phase[i2].state = DISABLED;
            }
            break;
        }
        case Split:{
            int x1 = operations[index].val[0];int y1 = operations[index].val[1];
            int x2 = operations[index].val[2];int y2 = operations[index].val[3];
            int x3 = operations[index].val[4];int y3 = operations[index].val[5];
            if(t == op_time+1){//椭圆拉长
                int i = find_index_wash(x1,y1);
                wash_phase[i].state = SPLITING;
                wash_phase[i].horizontal = (y2 == y3);
            }
            else if(t == op_time+2){//分裂
                int i = find_index_wash(x1,y1);
                Node n1(x2,y2);n1.id = new_id;new_id++;
                Node n2(x3,y3);n2.id = new_id;new_id++;
                n1.trace.push_back(QPair<int,int>(n1.x,n1.y));
                n2.trace.push_back(QPair<int,int>(n2.x,n2.y));
                wash_phase[i].state = DISABLED;
                wash_phase.push_back(n1);wash_phase.push_back(n2);//新加入两个节点
            }
            break;
        }
        case Mix:{
            qDebug()<<"Mix";
            int step = operations[index].val.size()/2 - 1;//命令的总步数
            if(step + op_time >=t){//没执行完
                step = t - op_time;
                int i = find_index_wash(operations[index].val[(step-1)*2],operations[index].val[step* 2-1]);
                wash_phase[i].x = operations[index].val[step*2];
                wash_phase[i].y = operations[index].val[step*2+1];
                if(wash_phase[i].trace.count(position(wash_phase[i].x,wash_phase[i].y)) == 0){
                    wash_phase[i].trace.push_back(position(wash_phase[i].x,wash_phase[i].y));
                }
            }
            break;
        }
        case Output:{
            if(op_time +1 == t){
                int x = operations[index].val[0];int y = operations[index].val[1];
                qDebug()<<"output"<<x<<y;
                int i = find_index_wash(x,y);//待输出节点
                wash_phase[i].state = DISABLED;
            }
            break;
        }
        default:break;
        }
    }
    qDebug()<<"run wash"<<t<<"succeed";
}
int Core::find_index_wash(int x, int y){
    int i = -1;
    for(int lp = 0;lp<wash_phase.size();lp++){
        if(wash_phase[lp].state == DISABLED) continue;
        if(wash_phase[lp].x == x && wash_phase[lp].y == y){
            i = lp;
        }
    }
    if(i == -1){
        qDebug()<<"Move target"<<x<<y<<"not found!";
    }
    return i;
}

bool Core::check_contamination()
{
    con.clear();
    for(int i = 0;i<c;i++){
        QVector<int> temp;
        for(int j = 0;j<r;j++){
            temp.push_back(0);
        }
        con.push_back(temp);
    }
    for(int i = 0;i<wash_phase.size();i++){
        for(int j = 0;j < wash_phase[i].trace.size();j++){
            //qDebug()<<i<<j;
            con[wash_phase[i].trace[j].first-1][wash_phase[i].trace[j].second-1] += 1;
        }
    }
    for(int i = 0;i<c;i++){
        for(int j = 0;j<c;j++){
            if(con[i][j]>=2){
                err_pos[0] = i;err_pos[1] = j;
                return false;
            }
        }
    }
    return true;
}

void Core::reset_wash()
{
    wash_time = 0;
    wash_phase.clear();
    new_id = 0;
}

void Core::disable_update(int x, int y)
{
    disable[x][y] = 1-disable[x][y];
}

void Core::wash_trace(int x, int y)
{//清理该位置的所有trace
    for(int i = 0;i<wash_phase.size();i++){
        wash_phase[i].trace.removeAll(QPair<int,int> (x,y));
    }
}


