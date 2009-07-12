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
#include <QPixmapCache>

#include "common.h"
#include "kmagnetcell.h"

class kmagnetCell;
class kmagnetScene : public QGraphicsScene
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    kmagnetScene(QObject * parent = 0, int rows = 25, int columns = 20 );

    QPixmapCache* getCache() {
        return cache;
    };
    /**
     * Default Destructor
     */
    virtual ~kmagnetScene();
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void keyReleaseEvent ( QKeyEvent * keyEvent); 
    void setFinalPosition(QPoint pos);
    void setNotFreePosition(QPoint pos);
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
    void setBallPos(QPoint p) {
        m_ball->setPos(p);
        currentPosition=p;
    };
    void setStartPosition(QPoint p){
	startPosition=p;
    };
    QPoint getStartPosition(){
	return startPosition;
    };
    bool getEditorMode(){
	return editorMode;
    };
    void setSize(int r, int c){
	ROWS=r;
	COLUMNS=c;
    };

public slots:
    void resizeScene(int width, int height);
    void process(int mov);

signals:
    void advanceMovements(int m);
    void itsover(bool haswon);

private:

    int COLUMNS;
    int ROWS;
    bool haslost;
    bool haswon;
    bool editorMode;
    int movements;
    QPoint startPosition;
    QPoint currentPosition;
    QPixmapCache* cache;
    QGraphicsEllipseItem* m_ball;
    QVector<kmagnetCell*> m_cells;
    void movement(int x, int y);
};

#endif // KMAGNETSCENE_H
