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
//#include <QThread>
#include "kmagnet.h"

kmagnetScene::kmagnetScene ( QObject * parent, int rows, int columns ) :
        QGraphicsScene ( parent ),
        COLUMNS ( columns ),
        ROWS ( rows ),
        hasLost ( false ),
        hasWon ( false ),
        editorMode ( false ),
        movements ( 0 ),
        startPosition ( 0 ),
        sol ( QVector<Moves::Move>() )
{
    cache = new QPixmapCache();
    cache->insert ( "free", QPixmap ( KStandardDirs::locate ( "appdata", "images/free.png" ) ) );
    cache->insert ( "notfree", QPixmap ( KStandardDirs::locate ( "appdata", "images/notfree.png" ) ) );
    cache->insert ( "final", QPixmap ( KStandardDirs::locate ( "appdata", "images/final.png" ) ) );
    m_ball=0;
}

void kmagnetScene::setBoardPosition()
{
    Global::itemSize = qMin ( this->height() /ROWS, this->width() /COLUMNS );
    qreal itemsize= Global::itemSize;
    qreal Xcorrection= ( this->width()-COLUMNS*itemsize ) /2;
    qreal Ycorrection= ( this->height()-ROWS*itemsize ) /2;
    for ( uint row=0; row<ROWS; ++row )
    {
        for ( uint col=0; col<COLUMNS; ++col )
        {
            m_cells[row*COLUMNS+col]->setPos ( QPointF ( Xcorrection+ ( col ) *itemsize, Ycorrection+ ( row ) *itemsize ) );
        }
    }
    if ( m_ball )
    {
        qreal size=itemsize-0.10*itemsize;
        m_ball->setRect ( 0,0, size ,size );
        setBallPos ( currentPosition );
    }
}

void kmagnetScene::newGame()
{
    hasLost=false;
    hasWon=false;
    movements=0;
    int oldSize = m_cells.size();
    int newSize = ROWS*COLUMNS;
    if ( oldSize > newSize )
    {
        for ( int i=newSize; i<oldSize; ++i )
        {
            this->removeItem ( m_cells[i] );
            delete m_cells[i];
        }
    }
    m_cells.resize ( newSize );
    for ( int i=0; i<newSize; ++i )
    {
        // reset old, create new
        if ( i<oldSize )
            m_cells[i]->reset();
        else
            m_cells[i] = new kmagnetCell ( 0, this );
    }

    setBoardPosition();

    if ( !m_ball )
    {
        QRadialGradient radialGradient ( 7, 7, 7, 4, 4 );
        radialGradient.setColorAt ( 0.0, Qt::white );
        radialGradient.setColorAt ( 0.35, Qt::lightGray );
        radialGradient.setColorAt ( 0.75, Qt::gray );
        radialGradient.setColorAt ( 1.0, Qt::darkGray );
        qreal size=Global::itemSize-0.10*Global::itemSize;
        m_ball= addEllipse ( 0,0, size, size, QPen ( Qt::NoPen ),radialGradient );
        m_ball->setZValue ( 5.0 );
    }
    setBallPos ( startPosition );
    this->update ( sceneRect() );
}

void kmagnetScene::process ( Moves::Move mov )
{
    if ( hasLost || hasWon ) return;
    //animateMovement(mov);
    movement ( mov );
    movements++;
    if ( movements==1000 ) hasLost=true;
    emit advanceMovements ( movements );
    //check if has won or lost
    if ( hasWon )
    {
        emit itsover ( true );
    }
    else if ( hasLost )
    {
        emit itsover ( false );
    }
}

void kmagnetScene::movement ( Moves::Move mov )
{
    nextMove nm= isPossibleMove ( mov );
    if ( !nm.getIsPossible() )
    {
        //hasLost=true;
        return;
    };
    currentPosition=nm.getPosition();
    setBallPos ( currentPosition );
}

kmagnetScene::~kmagnetScene()
{
    delete cache;
    for ( int i=0;i<m_timers.size();i++ ) //buff que lleig...mirar quan ja no fan falta i esborrar-los llavors
        delete m_timers[i];
    for ( int i=0;i<m_animations.size();i++ )
        delete m_animations[i];
}

void kmagnetScene::animateMovement ( Moves::Move mov )
{
    nextMove nm= isPossibleMove ( mov );
    //QPoint end=getNextPosition(m);
    if ( !nm.getIsPossible() )
    {
        //hasLost=true;
        return;
    };
    kmagnetCell* finalCell=m_cells[nm.getPosition() ];
    QPoint end= finalCell->pos().toPoint();
    QPoint start=m_cells[currentPosition]->pos().toPoint();
    //if (start==end) {hasLost=true; return;}//better use isPossibleMove?
    //if ( finalCell->getIsFinal() ) hasWon=true;
    QPoint dif=end-start;
    int time;
    if ( dif.x() !=0 )
        time=dif.x();
    else
        time=dif.y();
    QTimeLine *timer = new QTimeLine ( 175+abs ( time ) );
    m_timers.append ( timer );
    connect ( timer, SIGNAL ( finished() ),this, SLOT ( finishWait() ) );
    timer->setFrameRange ( 0, 100 );
    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation();
    m_animations.append ( animation );
    animation->setItem ( m_ball );
    animation->setTimeLine ( timer );
    animation->setPosAt ( 1.0,end );
    timer->start();
    currentPosition=nm.getPosition();
}

void kmagnetScene::replay ( QVector<Moves::Move> lm )
{
    sol.clear();
    sol=lm;
    animateMovement ( lm.at ( 0 ) );
    sol.pop_front();
}

void kmagnetScene::finishWait()
{
    if ( sol.size() !=0 )
    {
        animateMovement ( sol.at ( 0 ) );
        sol.pop_front();//sol.erase(sol.begin());
    }
    else
    {
	kmagnet* mainKmagnet = dynamic_cast<kmagnet*> ( parent() );
        mainKmagnet->action ( "move_solve" )->setEnabled ( true );
        mainKmagnet->action ( "game_restart" )->setEnabled ( true );
    }
}

void kmagnetScene::resizeScene ( int width, int height )
{
    setSceneRect ( 0, 0, width, height );
    setBoardPosition();
}

void kmagnetScene::setFinalPosition ( uint num)
{
    if ( num>=COLUMNS*ROWS)
        return;
    m_cells[num]->setIsFinal(true);
}

void kmagnetScene::setNotFreePosition ( uint num )
{
    if ( num>=COLUMNS*ROWS)
        return;
    m_cells[num]->setIsFree(false);
}

void kmagnetScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{//FIXME I can do it better
    dynamic_cast<kmagnet*> ( parent() )->setFocus();
    QGraphicsItem* item=0;
    item= ( this->itemAt ( mouseEvent->scenePos() ) );
    if ( item==0 ) return;
    if ( item->zValue() ==5.0 || !item ) return;//if its the ball skip//FIXME
    uint cell=posToCell(item->pos().toPoint());
    if ( mouseEvent->button() == Qt::LeftButton )
    {
        if ( editorMode )
        {
            //kmagnetCell *currentCell= dynamic_cast<kmagnetCell*> ( item );
            kmagnetCell *currentCell= m_cells[cell];
            if ( mouseEvent->modifiers() ==Qt::ControlModifier )
            {
                //QPoint p =QPoint ( currentCell->x() +0.05*Global::itemSize,currentCell->y() +0.05*Global::itemSize );
                //setBallPos ( posToCell ( p ) );
                //startPosition=posToCell ( p );
                setBallPos(cell);
                startPosition=cell;
                currentCell->reset();//reset cell
                restart();
            }
            else
                currentCell->setIsFree ( false );
            return;
        }
        if ( !editorMode )
        {
            //better just check row and column
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
            if ( x1norm==x2norm )
            {
                if ( y2>y1 )
                    process ( Moves::DOWN );
                else
                    process ( Moves::UP );
            }
            else if ( y1norm==y2norm )
            {
                if ( x2>x1 )
                    process ( Moves::RIGHT );
                else
                    process ( Moves::LEFT );
            }
        }
    }
    else if ( mouseEvent->button() == Qt::RightButton )
    {
        if ( editorMode )
        {
            if ( mouseEvent->modifiers() ==Qt::ControlModifier )
            {
                dynamic_cast<kmagnetCell*> ( item )->reset();
            }
            else
                dynamic_cast<kmagnetCell*> ( item )->setIsFinal ( true );
        }
    }
    else if ( mouseEvent->button() == Qt::MidButton )
    {
        if ( editorMode ) dynamic_cast<kmagnetCell*> ( item )->reset();
    }
}

void kmagnetScene::restart()
{
    setBallPos ( startPosition );
    hasWon=false;
    hasLost=false;
    movements=0;
}

int kmagnetScene::getNextPosition ( Moves::Move m )
{
    uint now= currentPosition;
    switch ( m ) //change switch to if elseif?
    {
    case ( Moves::UP ) :
    {
        for ( int i=now; i>=0; i=i-COLUMNS )
        {
            kmagnetCell* currentCell= m_cells.at ( i );
            if ( !currentCell->getIsFree() )
                return i+COLUMNS;
            else if ( currentCell->getIsFinal() )
                return i;
        }
        break;
    }
    case ( Moves::DOWN ) :
    {
        for ( uint i=now; i< ROWS*COLUMNS; i=i+COLUMNS )
        {
            kmagnetCell* currentCell= m_cells.at ( i );
            if ( !currentCell->getIsFree() )
                return i-COLUMNS;
            else if ( currentCell->getIsFinal() )
                return i;
        }
        break;
    }
    case ( Moves::LEFT ) :
    {
        for ( int i =now; i>= static_cast<int> ( ( now/COLUMNS ) *COLUMNS ) ; i=i-1 )
        {
            kmagnetCell* currentCell= m_cells.at ( i );
            if ( !currentCell->getIsFree() )
                return i+1;
            else if ( currentCell->getIsFinal() )
                return i;
        }
        break;
    }
    case ( Moves::RIGHT ) :
    {
        for ( uint i=now; i< ( now/COLUMNS ) *COLUMNS+COLUMNS; i=i+1 )
        {
            kmagnetCell* currentCell= m_cells.at ( i );
            if ( !currentCell->getIsFree() )
                return i-1;
            else if ( currentCell->getIsFinal() )
                return i;
        }
        break;
    }
    default:
        qDebug() << "invalid move";
        break;
    }
    qDebug() << "lost!!!!!!!!!!";
    hasLost=true;
    return  now;
}

nextMove kmagnetScene::isPossibleMove ( Moves::Move m )
{
    uint next=getNextPosition ( m );
    if ( currentPosition==next || m_cells[next]->getVisited() )
    {
        return nextMove ( false,next );
    }
    return nextMove ( true,next );
}

void kmagnetScene::setVisited ( uint p,bool b )
{
    m_cells[p]->setVisited ( b );
}

uint kmagnetScene::posToCell ( QPoint pos )
{
    qreal itemsize= Global::itemSize;
    qreal Xcorrection= ( this->width()-COLUMNS*itemsize ) /2;
    qreal Ycorrection= ( this->height()-ROWS*itemsize ) /2;//>>1
    int row = static_cast<int> ( ( pos.y()-Ycorrection ) /itemsize );
    int col = static_cast<int> ( ( pos.x()-Xcorrection ) /itemsize );
    return row*COLUMNS+col;
}

void kmagnetScene::setBallPos ( uint cellNumber )
{
    qreal correction=0.05*Global::itemSize;
    QPointF pos=m_cells[cellNumber]->pos();
    m_ball->setPos ( QPointF ( pos.x() +correction, pos.y() +correction ) );
    currentPosition=cellNumber;
}

uint kmagnetScene::getStartPosition()
{
    return startPosition;
}

QPoint kmagnetScene::getCurrentPosition()
{
    return ( m_cells.at ( currentPosition ) )->pos().toPoint();
}

uint kmagnetScene::getCurrentCell()
{
    return currentPosition;
}

kmagnetCell* kmagnetScene::getCell ( uint n )
{
    //if (n<ROWS*COLUMNS)//not needed for now
    return m_cells[n];

}
