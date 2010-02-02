/*************************************************************************************
 *  Copyright (C) 2009 by Oscar Martinez <omllobet@gmail.com>                        *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 3                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#ifndef KMAGNETSCENE_H
#define KMAGNETSCENE_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QVector>
#include <QKeyEvent>
#include <QGraphicsItemAnimation>
#include <QTimeLine>
#include <QSignalMapper>

#include "common.h"
#include "kmagnetcell.h"
#include "kmagnet.h"

#include <vector>

class kmagnet;
class kmagnetCell;
class kmagnetScene : public QGraphicsScene
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    explicit kmagnetScene(QObject * parent = 0, int rows = 25, int columns = 20 );

    /**
     * Default Destructor
     */
    virtual ~kmagnetScene();
    void setFinalPosition(uint num);
    void setNotFreePosition(uint num);
    void restart();
    void newGame();
    int getMovements() {
        return movements;
    };
    void setMovements(int p) {
        movements=p;
    };
    void toogleEditorMode(bool b) {
        Q_UNUSED(b);
        editorMode=!editorMode;
    };
    QPointF getBallPos() {
        return m_ball->pos();
    };
    void setStartPosition(uint p) {
        startPosition=p;
    };
    uint getStartPosition();
    void setCurrentPosition(uint p) {
        currentPosition=p;
    };
    QPoint getCurrentPosition();
    uint getCurrentCell();
    bool getEditorMode() {
        return editorMode;
    };
    void setSize(int r, int c);
    void setHasLost(bool haslostGame) {
        hasLost=haslostGame;
    };
    bool gameIsLost() {
        return hasLost;
    };
    bool gameIsWon() {
        return hasWon;
    };
    void setBallPos(uint cellNumber);
    int getNextPosition(Moves::Move m);
    nextMove isPossibleMove(Moves::Move m);
    void setVisited(uint p,bool b);
    void animateMovement(Moves::Move m);
    void replay(QVector<Moves::Move> lm);
    kmagnetCell* getCell(uint n);
    uint getNumCells() {
        return m_cells.size();
    };
    uint getColumns(){
        return COLUMNS;
    };
    uint getRows(){
        return ROWS;
    };
    void setAllNotVisited();

public slots:
    void resizeScene(int width, int height);
    void process(Moves::Move mov);
    void finishWait(int number);

signals:
    void advanceMovements(int m);
    void itsover(bool haswon);

protected:
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );

private:

    void setBoardPosition();
    uint posToCell(QPoint p);
    virtual void drawBackground( QPainter*, const QRectF& );

    uint COLUMNS;
    uint ROWS;
    bool hasLost;
    bool hasWon;
    bool editorMode;
    uint movements;
    uint startPosition;
    uint currentPosition;
    QGraphicsEllipseItem* m_ball;
    QVector<kmagnetCell*> m_cells;
    void movement(Moves::Move mov);
    QVector<Moves::Move> sol;
    QSignalMapper *signalMapper;
};

#endif // KMAGNETSCENE_H
