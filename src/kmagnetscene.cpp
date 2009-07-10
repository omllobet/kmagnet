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

#include <KStandardDirs>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "kmagnetscene.h"

kmagnetScene::kmagnetScene(QObject * parent, int rows, int columns) :
        QGraphicsScene(parent),
        COLUMNS(columns),
        ROWS(rows),
        haslost(false),
        haswon(false),
        editorMode(false),
        movements(0),
        startposition(QPoint(03,03))

{
    resizeScene((int)sceneRect().width(), (int)sceneRect().height());
    cache = new QPixmapCache();
    cache->insert("free", QPixmap(KStandardDirs::locate("appdata", "images/free.png")));
    cache->insert("notfree", QPixmap(KStandardDirs::locate("appdata", "images/notfree.png")));
    cache->insert("final", QPixmap(KStandardDirs::locate("appdata", "images/final.png")));
}

void kmagnetScene::newGame()
{
    haslost=false;
    haswon=false;
    movements=0;
    int oldSize = m_cells.size();
    int newSize = ROWS*COLUMNS;
    if (oldSize > newSize)
    {
        for ( int i=newSize; i<oldSize; ++i )
        {
            // is this the best way to remove an item?
            this->removeItem(m_cells[i]);
            delete m_cells[i];
        }
    }
    m_cells.resize(newSize);
    for (int i=0; i<newSize; ++i)
    {
        // reset old, create new
        if (i<oldSize)
            m_cells[i]->reset();
        else
            m_cells[i] = new kmagnetcell(0, this);
    }

    for (int row=0; row<ROWS; ++row) {
        for (int col=0; col<COLUMNS; ++col)
        {
            m_cells.at(row*COLUMNS+col)->setPos((col)*20, (row)*20);
        }
    }

    if (!elli)
    {
	QRadialGradient radialGradient(7, 7, 7, 4, 4);
	radialGradient.setColorAt(0.0, Qt::white);
        radialGradient.setColorAt(0.35, Qt::lightGray);
        radialGradient.setColorAt(0.75, Qt::gray);
        radialGradient.setColorAt(1.0, Qt::darkGray);
        elli= addEllipse(0,0,14,14, QPen(Qt::NoPen),radialGradient);
        elli->setPos(startposition);
        elli->setZValue(1000);
        this->update();
    }
    else
    {
        elli->setPos(startposition);
        dynamic_cast<kmagnetcell*>(itemAt(startposition))->setisfinal(false);
        dynamic_cast<kmagnetcell*>(itemAt(startposition))->setisfree(true);
    }
    //this->update();
}

void kmagnetScene::process(int mov)
{
    if (haslost) return;
    if (mov==Moves::UP)
    {
        movement(0, -20);
    }
    else if (mov==Moves::DOWN)
    {
        movement(0, 20);
    }
    else if (mov==Moves::LEFT)
    {
        movement(-20, 0);
    }
    else if (mov==Moves::RIGHT)
    {
        movement(20, 0);
    }

    //advance movements
    movements++;
    if (movements==1000) haslost=true;
    emit advanceMovements(movements);
    //check if has won
    if (haswon)
    {
        emit itsover(true);
    }
    else if (haslost)
    {
        emit itsover(false);
    }
}

void kmagnetScene::movement(int x, int y)
{
    bool trobat=false;
    while (!trobat)
    {
        QList<QGraphicsItem *> ci= this->collidingItems (elli);
        int i=0;
        trobat=false;
        while (i < ci.size() && !trobat)
        {
            if (!dynamic_cast<kmagnetcell*>( ci.at(i))->getisfree())
            {
                trobat=true;
                elli->setPos(elli->pos().x()-x,elli->pos().y()-y);
            }
            else if ( dynamic_cast<kmagnetcell*>( ci.at(i))->getisfinal())
            {
                trobat=true;
                haswon=true;
            }
            i++;
        }
        if (!trobat && elli->pos().y()+y >=0 && elli->pos().x()+x >=0
                && elli->pos().y()+y <=ROWS*20.0 && elli->pos().x()+x <=COLUMNS*20.0)
        {
            elli->setPos(elli->pos().x()+x,elli->pos().y()+y);
        }
        else if (!trobat)
        {
            trobat=true;
            haslost=true;
        }
    }
}
kmagnetScene::~kmagnetScene()
{
}

void kmagnetScene::resizeScene(int width, int height)
{
    setSceneRect(0, 0, width, height);
}

void kmagnetScene::setfinalposition(QPoint p)
{
    int x=p.x();
    int y =p.y();
    //qDebug("x=%d, y=%d, size=%d",x, y, this->items().size());
    if (x<0||y<0||x>=COLUMNS*20||y>=ROWS*20)
        return;
    QGraphicsItem* item= this->itemAt(QPoint(x,y));
    if (item!=0) dynamic_cast<kmagnetcell*>(item)->setisfinal(true);
}

void kmagnetScene::setnotfreeposition(QPoint p)
{
    int x=p.x();
    int y =p.y();
    if (x<0||y<0||x>=COLUMNS*20||y>=ROWS*20)
        return;
    QGraphicsItem* item= this->itemAt(QPointF(x,y));
    if (item!=0) dynamic_cast<kmagnetcell*>(item)->setisfree(false);
}
void kmagnetScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsItem* item=(this->itemAt(mouseEvent->scenePos()));
    if (mouseEvent->button() == Qt::LeftButton && item!=0)
    {
        if (editorMode) {
            dynamic_cast<kmagnetcell*>(item)->setisfree(false);
	    return;
        }
        if ( !editorMode && item!=elli) {
            QPointF p1= elli->pos();
            int x1 = p1.x();
            int y1= p1.y();
            int x1norm =x1 - x1%20;
            int y1norm=y1 - y1%20;
            //qDebug("x1 =%d, x1norm elipse=%d, y1=%d, y1norm elipse=%d",x1, x1norm, y1, y1norm);
            QPointF p2= mouseEvent->scenePos();
            int x2= p2.x();
            int y2= p2.y();
            int x2norm=x2 - x2%20;
            int y2norm=y2 - y2%20;
            //qDebug("x2 =%d, x2norm mouse=%d, y2=%d, y2norm mouse=%d",x2, x2norm, y2, y2norm);
            if (x1norm==x2norm) {
                if (y2>y1)
                    process(Moves::DOWN);
                else
                    process(Moves::UP);
            }
            else if (y1norm==y2norm) {
                if (x2>x1)
                    process(Moves::RIGHT);
                else
                    process(Moves::LEFT);
            }
        }
    }
    else if (mouseEvent->button() == Qt::RightButton)
    {
        if (item!=0 && editorMode) dynamic_cast<kmagnetcell*>(item)->setisfinal(true);
    }
    else if (mouseEvent->button() == Qt::MidButton)
    {
        if (item!=0 && editorMode) dynamic_cast<kmagnetcell*>(item)->reset();
    }
}

void kmagnetScene::restart()
{
    setElliPos(startposition);
    haswon=false;
    haslost=false;
    movements=0;
}

void kmagnetScene::keyReleaseEvent ( QKeyEvent * keyEvent)
{
    switch ( keyEvent->key() )
    {
    case Qt::Key_Down:
        process(Moves::DOWN);
        break;
    case Qt::Key_Up:
        process(Moves::UP);
        break;
    case Qt::Key_Left:
        process(Moves::LEFT);
        break;
    case Qt::Key_Right:
        process(Moves::RIGHT);
        break;
    }
}
