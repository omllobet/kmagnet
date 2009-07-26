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
#include <QDebug>

#include "kmagnetscene.h"
#include <QThread>

kmagnetScene::kmagnetScene(QObject * parent, int rows, int columns) :
        QGraphicsScene(parent),
        COLUMNS(columns),
        ROWS(rows),
        hasLost(false),
        hasWon(false),
        editorMode(false),
        movements(0),
        startPosition(QPoint(03,03)),
        sol(QVector<Moves::Move>())
{
    resizeScene((int)sceneRect().width() , (int)sceneRect().height());
    cache = new QPixmapCache();
    cache->insert("free", QPixmap(KStandardDirs::locate("appdata", "images/free.png")));
    cache->insert("notfree", QPixmap(KStandardDirs::locate("appdata", "images/notfree.png")));
    cache->insert("final", QPixmap(KStandardDirs::locate("appdata", "images/final.png")));
    m_ball=0;
}

void kmagnetScene::newGame()
{
    hasLost=false;
    hasWon=false;
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
            m_cells[i] = new kmagnetCell(0, this);
    }

    for (int row=0; row<ROWS; ++row) {
        for (int col=0; col<COLUMNS; ++col)
        {
            //qDebug()<<"s "<< row*COLUMNS+col;
            m_cells[row*COLUMNS+col]->setPos(QPointF((col)*Global::itemSize, (row)*Global::itemSize));
        }
    }

    if (!m_ball)
    {
        QRadialGradient radialGradient(7, 7, 7, 4, 4);
        radialGradient.setColorAt(0.0, Qt::white);
        radialGradient.setColorAt(0.35, Qt::lightGray);
        radialGradient.setColorAt(0.75, Qt::gray);
        radialGradient.setColorAt(1.0, Qt::darkGray);
        m_ball= addEllipse(0,0,14,14, QPen(Qt::NoPen),radialGradient);
        m_ball->setPos(startPosition);
        m_ball->setZValue(5.0);
        this->update();
    }
    else
    {
        m_ball->setPos(startPosition);
        dynamic_cast<kmagnetCell*>(itemAt(startPosition))->setIsFinal(false);
        dynamic_cast<kmagnetCell*>(itemAt(startPosition))->setIsFree(true);
    }
    dynamic_cast<kmagnet*>(parent())->calculateMinimiumSize();
}

void kmagnetScene::process(Moves::Move mov)
{
    //qDebug() << "proxima posicio" << this->getNextPosition(mov);
    if (hasLost || hasWon) return;
    //animateMovement(mov);
    if (mov==Moves::UP)
    {
        movement(0, -Global::itemSize);
    }
    else if (mov==Moves::DOWN)
    {
        movement(0, Global::itemSize);
    }
    else if (mov==Moves::LEFT)
    {
        movement(-Global::itemSize, 0);
    }
    else if (mov==Moves::RIGHT)
    {
        movement(Global::itemSize, 0);
    }

    //advance movements
    movements++;
    if (movements==1000) hasLost=true;
    emit advanceMovements(movements);
    //check if has won or lost
    if (hasWon)
    {
        emit itsover(true);
    }
    else if (hasLost)
    {
        emit itsover(false);
    }
}

void kmagnetScene::movement(int x, int y)
{
    bool found=false;
    while (!found)
    {
        QList<QGraphicsItem *> ci= this->collidingItems (m_ball);
        found=false;
        int i=0;
        while (i < ci.size() && !found)
        {
            kmagnetCell* cell =dynamic_cast<kmagnetCell*>( ci.at(i));
            if (!cell->getIsFree())
            {
                found=true;
                m_ball->setPos(m_ball->pos().x()-x,m_ball->pos().y()-y);
            }
            else if ( cell->getIsFinal())
            {
                found=true;
                hasWon=true;
            }
            i++;
        }
        if (!found && m_ball->pos().y()+y >=0 && m_ball->pos().x()+x >=0
                && m_ball->pos().y()+y <=ROWS*Global::itemSize && m_ball->pos().x()+x <=COLUMNS*Global::itemSize)
        {
            m_ball->setPos(m_ball->pos().x()+x,m_ball->pos().y()+y);
        }
        else if (!found)
        {
            found=true;
            hasLost=true;
        }
    }
    currentPosition=m_ball->pos().toPoint();
}

kmagnetScene::~kmagnetScene()
{
  delete cache;
  for (int i=0;i<m_timers.size();i++)
    delete m_timers[i];
  for (int i=0;i<m_animations.size();i++)
    delete m_animations[i];
}

void kmagnetScene::animateMovement(Moves::Move m)
{
    nextMove nm= isPossibleMove(m);
    //QPoint end=getNextPosition(m);
    if (!nm.getIsPossible()) {
        hasLost=true;
        return;
    };
    QPoint end= nm.getPosition();
    QPoint start=currentPosition;
    //if (start==end) {hasLost=true; return;}//better use isPossibleMove?
    //qDebug() << it << "-----" << m_ball << "-----" << &it << "----------" << &m_ball;
    if (dynamic_cast<kmagnetCell*>(itemAt(end))->getIsFinal()) hasWon=true;
    QPoint dif=end-start;
    int time;
    if (dif.x()!=0)
        time=dif.x();
    else
        time=dif.y();
    QTimeLine *timer = new QTimeLine(250+abs(time));
    m_timers.append(timer);
    connect(timer, SIGNAL(finished()),this, SLOT(finishWait()));
    timer->setFrameRange(0, 100);
    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation();
    m_animations.append(animation);
    animation->setItem(m_ball);
    animation->setTimeLine(timer);
    animation->setPosAt(1.0,end);
    timer->start();
    currentPosition=end;
}

void kmagnetScene::replay(QVector<Moves::Move> lm)
{
    sol.clear();
    sol=lm;
    animateMovement(lm.at(0));
    sol.pop_front();
}

void kmagnetScene::finishWait()
{
    if (sol.size()!=0)
    {
        animateMovement(sol.at(0));
        sol.pop_front();//sol.erase(sol.begin());
    }
    else
    {
        dynamic_cast<kmagnet*>(parent())->action("solve")->setEnabled(true);
	dynamic_cast<kmagnet*>(parent())->action("game_restart")->setEnabled(true);
    }
}

void kmagnetScene::resizeScene(int width, int height)
{
    setSceneRect(0, 0, width, height);
}

void kmagnetScene::setFinalPosition(QPoint p)
{
    int x=p.x();
    int y =p.y();
    //qDebug("x=%d, y=%d, size=%d",x, y, this->items().size());
    if (x<0||y<0||x>=COLUMNS*Global::itemSize||y>=ROWS*Global::itemSize)
        return;
    QGraphicsItem* item= this->itemAt(QPoint(x,y));
    if (item!=0) dynamic_cast<kmagnetCell*>(item)->setIsFinal(true);
}

void kmagnetScene::setNotFreePosition(QPoint pos)
{
    int x=pos.x();
    int y=pos.y();
    if (x<0||y<0||x>=COLUMNS*Global::itemSize||y>=ROWS*Global::itemSize)
        return;
    QGraphicsItem* item= this->itemAt(QPointF(x,y));
    if (item!=0) dynamic_cast<kmagnetCell*>(item)->setIsFree(false);
}

void kmagnetScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent)
{
    dynamic_cast<kmagnet*>(parent())->setFocus();
    QGraphicsItem* item=(this->itemAt(mouseEvent->scenePos()));
    if (item->zValue()==5.0) return;//if its the ball skip//FIXME
    if (mouseEvent->button() == Qt::LeftButton && item)
    {
        if (editorMode) 
	{
	    kmagnetCell *currentCell= dynamic_cast<kmagnetCell*>(item);
	    if (mouseEvent->modifiers()==Qt::ControlModifier)
		{
		  QPoint p =QPoint(currentCell->x()+3,currentCell->y()+3);//this 3 has to be the same number as the startpositionbydefault mod itemSize which at time has to be itemsize-ballsize/2
		  setBallPos(p);
		  startPosition=p;
		  restart();
		}
	    else
		currentCell->setIsFree(false);
            return;
        }
        if ( !editorMode && item!=m_ball) {
            QPointF p1= m_ball->pos();
            int x1 = p1.x();
            int y1= p1.y();
            int x1norm =x1 - x1%Global::itemSize;
            int y1norm=y1 - y1%Global::itemSize;
            //qDebug("x1 =%d, x1norm elipse=%d, y1=%d, y1norm elipse=%d",x1, x1norm, y1, y1norm);
            QPointF p2= mouseEvent->scenePos();
            int x2= p2.x();
            int y2= p2.y();
            int x2norm=x2 - x2%Global::itemSize;
            int y2norm=y2 - y2%Global::itemSize;
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
        if (item && editorMode) dynamic_cast<kmagnetCell*>(item)->setIsFinal(true);
    }
    else if (mouseEvent->button() == Qt::MidButton)
    {
        if (item && editorMode) dynamic_cast<kmagnetCell*>(item)->reset();
    }
}

void kmagnetScene::restart()
{
    setBallPos(startPosition);
    hasWon=false;
    hasLost=false;
    movements=0;
}

QPoint kmagnetScene::getNextPosition(Moves::Move m)
{
    //int x = m_ball->scenePos().x();
    //int y = m_ball->scenePos().y();
    int x = currentPosition.x();
    int y = currentPosition.y();
    switch (m)
    {
    case (Moves::UP):
    {
        for (int i=y-Global::itemSize; i>=0;i=i-Global::itemSize)
        {
            //	kmagnetCell* currentCell= m_cells.at((x*COLUMNS)/((COLUMNS-1)*Global::itemSize+3)*COLUMNS+ (i*ROWS)/((ROWS-1)*Global::itemSize+3));
            kmagnetCell* currentCell= dynamic_cast<kmagnetCell*>(itemAt(x,i));
            if (!currentCell->getIsFree())
                return QPoint(x,i+Global::itemSize);
            else if (currentCell->getIsFinal())
                return QPoint(x,i);
        }
        return QPoint(x,y);
    }
    case (Moves::DOWN):
    {
        for (int i=y+Global::itemSize; i<ROWS*Global::itemSize;i=i+Global::itemSize)
        {
            kmagnetCell* currentCell= dynamic_cast<kmagnetCell*>(itemAt(x,i));
            if (!currentCell->getIsFree())
                return QPoint(x,i-Global::itemSize);
            else if (currentCell->getIsFinal())
                return QPoint(x,i);
        }
        return QPoint(x,y);
    }
    case (Moves::LEFT):
    {
        for (int i=x-Global::itemSize; i>=0;i=i-Global::itemSize)
        {
            kmagnetCell* currentCell= dynamic_cast<kmagnetCell*>(itemAt(i,y));
            if (!currentCell->getIsFree())
                return QPoint(i+Global::itemSize,y);
            else if (currentCell->getIsFinal())
                return QPoint(i,y);
        }
        return QPoint(x,y);
    }
    case (Moves::RIGHT):
    {
        for (int i=x+Global::itemSize; i<COLUMNS*Global::itemSize;i=i+Global::itemSize)
        {
            kmagnetCell* currentCell= dynamic_cast<kmagnetCell*>(itemAt(i,y));
            if (!currentCell->getIsFree())
                return QPoint(i-Global::itemSize,y);
            else if (currentCell->getIsFinal())
                return QPoint(i,y);
        }
        return QPoint(x,y);
    }
    default:
        qDebug() << "invalid move";
        break;
    }
    return  QPoint(x,y);
}

nextMove kmagnetScene::isPossibleMove( Moves::Move m)
{
    QPoint p = getNextPosition(m);
    int x=p.x();
    //qDebug()<< "x "<< x;
    int y=p.y();
    if ((x==currentPosition.x() && y==currentPosition.y()) || dynamic_cast<kmagnetCell*>(itemAt(x,y))->getVisited())
    {
        return nextMove(false,p);
    }
    return nextMove(true,p);
}

void kmagnetScene::setVisited(QPoint p,bool b)
{
    dynamic_cast< kmagnetCell* >(itemAt(p))->setVisited(b);
}

void kmagnetScene::setBallPos(QPoint p) 
{
    dynamic_cast<kmagnetCell *>(itemAt(p))->reset();
    m_ball->setPos(p);
    currentPosition=p;
}
