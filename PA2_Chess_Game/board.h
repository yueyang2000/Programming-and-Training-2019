#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include "chess.h"
#include "dialog.h"
#include <QMouseEvent>
class Board: public QWidget
{
    Q_OBJECT
public:
    Board(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* ev);
    void mousePressEvent(QMouseEvent* ev);
    QVector<QImage> pics;
    bool picking = false;//已经拿起一个棋子
    int pick_x,pick_y;
    int pawn_shift();
    Dialog* dia;
signals:
    void move_piece(packet);
    void win();
    void stalemate();
};

#endif // BOARD_H
