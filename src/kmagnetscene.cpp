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
#include "kmagnet.h"
#include "kmagnetrenderer.h"
#include <settings.h>

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
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int )),this, SLOT(finishWait(int )));
    setItemIndexMethod( NoIndex );
    m_ball=NULL;
}

void kmagnetScene::setBoardPosition()
{
    Global::itemSize = qMin ( this->height() /ROWS, this->width() /COLUMNS );
    qreal itemsize= Global::itemSize;
    kmagnetRenderer::self()->setCellSize(itemsize);//update cell size for the renderer
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
    update();
}

void kmagnetScene::newGame()
{
    //part of the code similar to kmines
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
            m_cells[i] = new kmagnetCell (0 , this );
    }

    currentPosition=21;
    startPosition=21;
    setBoardPosition();

    if ( !m_ball )
    {
        QRadialGradient radialGradient ( 7, 7, 7, 4, 4 );
        radialGradient.setColorAt ( 0.0, Qt::white );
        radialGradient.setColorAt ( 0.35, Qt::lightGray );
        radialGradient.setColorAt ( 0.75, Qt::gray );
        radialGradient.setColorAt ( 1.0, Qt::darkGray );
        qreal size=Global::itemSize-0.10*Global::itemSize;
        m_ball= addEllipse ( 0,0, size, size,  QPen(Qt::gray, 0.1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin), radialGradient);
        m_ball->setZValue ( 5.0 );
    }
    setBallPos ( startPosition );
    this->update ();
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
        return;
    };
    int pos=nm.getPosition();
    setCurrentPosition(pos);
    if (m_cells[currentPosition]->getIsFinal()) hasWon=true;
    setBallPos ( pos );
}

kmagnetScene::~kmagnetScene()
{
    for ( int i=0;i<m_cells.size();i++ )//cells have no parent//but are associated with this scene
        delete m_cells[i];
}

void kmagnetScene::animateMovement ( Moves::Move mov )
{
    nextMove nm= isPossibleMove ( mov );
    if ( !nm.getIsPossible() )
    {
        return;
    };
    unsigned int pos=nm.getPosition();
    QTimeLine *timer = new QTimeLine ( Settings::animationTime(), this );//better use heap?
    signalMapper->setMapping(timer,currentPosition+pos*1000);
    connect ( timer, SIGNAL ( finished() ),signalMapper, SLOT ( map() ) );
    timer->setFrameRange ( 0, 150 );
    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation(this);
    animation->setItem ( m_ball );
    animation->setTimeLine ( timer );
    animation->setPosAt ( 1.0,m_cells[pos]->pos() );
    timer->start();
    setCurrentPosition(nm.getPosition());
}

void kmagnetScene::replay ( QVector<Moves::Move> lm )
{
    sol.clear();
    sol=lm;
    animateMovement ( lm.at ( 0 ) );
    sol.pop_front();
}

void kmagnetScene::finishWait(int number)
{
    if ( sol.size() !=0 )
    {
        animateMovement ( sol.at ( 0 ) );
        unsigned int startNumber = number%1000;
        unsigned int endNumber = number/1000;
        QPointF startPos = m_cells[startNumber]->pos();
        QPointF endPos = m_cells[endNumber]->pos();
        QRectF rectange = QRectF();
        if (endNumber > startNumber) {//FIXME qt does not recommend using this constructor
            rectange = QRectF(m_cells[startNumber]->pos(), QPointF( endPos.x()+Global::itemSize+2,endPos.y()+Global::itemSize+2));
        } else {
            rectange = QRectF(m_cells[endNumber]->pos(), QPointF( startPos.x()+Global::itemSize+2,startPos.y()+Global::itemSize+2));
        }
        this->update(rectange);
        sol.pop_front();
    }
    else
    {
        kmagnet* mainKmagnet = dynamic_cast<kmagnet*> ( parent() );
        mainKmagnet->action ( "move_solve" )->setEnabled ( true );
        mainKmagnet->action ( "game_restart" )->setEnabled ( true );
        mainKmagnet->action ( "generate" )->setEnabled ( true );
        this->update();
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
{
    QGraphicsItem* item=NULL;
    item= ( this->itemAt ( mouseEvent->scenePos() ) );
    if ( !item ) return;
    uint cell=posToCell(item->pos().toPoint());
    kmagnetCell *currentCell= m_cells[cell];
    if ( mouseEvent->button() == Qt::LeftButton )
    {
        if ( editorMode )
        {
            if ( mouseEvent->modifiers() ==Qt::ControlModifier )
            {
                setBallPos(cell);
                startPosition=cell;
                currentCell->reset();//reset cell
                restart();
            }
            else
                currentCell->setIsFree ( false );
            return;
        }
        else if ( !editorMode )
        {
            unsigned int p1= currentPosition;
            unsigned int p2= cell;
            if (p2>p1) //dreta o abaix
            {
                if (p2/ COLUMNS==p1/ COLUMNS)
                    process ( Moves::RIGHT );
                else if (p2% COLUMNS==p1% COLUMNS)
                    process ( Moves::DOWN );
            }
            else //amunt o esquerra
            {
                if (p2/ COLUMNS==p1/ COLUMNS)
                    process ( Moves::LEFT );
                else if (p2% COLUMNS==p1% COLUMNS)
                    process ( Moves::UP );
            }
        }
    }
    else if ( mouseEvent->button() == Qt::RightButton )
    {
        if ( editorMode )
        {
            if ( mouseEvent->modifiers() ==Qt::ControlModifier )
            {
                currentCell->reset();
            }
            else
                currentCell->setIsFinal ( true );
        }
    }
    else if ( mouseEvent->button() == Qt::MidButton )
    {
        if ( editorMode ) currentCell->reset();
    }
    dynamic_cast<kmagnet*> ( parent() )->setFocus();
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
    if (m==Moves::UP){
        for ( int i=now; i>=0; i=i-COLUMNS )
        {
            kmagnetCell* currentCell= m_cells.at ( i );
            if ( !currentCell->getIsFree() )
                return i+COLUMNS;
            else if ( currentCell->getIsFinal() )
                return i;
        }
    }
    else if ( m==Moves::DOWN ){
        for ( uint i=now; i< ROWS*COLUMNS; i=i+COLUMNS )
        {
            kmagnetCell* currentCell= m_cells.at ( i );
            if ( !currentCell->getIsFree() )
                return i-COLUMNS;
            else if ( currentCell->getIsFinal() )
                return i;
        }
    }
    else if ( m==Moves::LEFT ){
        for ( int i =now; i>= static_cast<int> ( ( now/COLUMNS ) *COLUMNS ) ; i=i-1 )
        {
            kmagnetCell* currentCell= m_cells.at ( i );
            if ( !currentCell->getIsFree() )
                return i+1;
            else if ( currentCell->getIsFinal() )
                return i;
        }
    }
    else if ( m==Moves::RIGHT ){
        for ( uint i=now; i< ( now/COLUMNS ) *COLUMNS+COLUMNS; i=i+1 )
        {
            kmagnetCell* currentCell= m_cells.at ( i );
            if ( !currentCell->getIsFree() )
                return i-1;
            else if ( currentCell->getIsFinal() )
                return i;
        }
    }
    //qDebug() << "lost!";
    hasLost=true;
    return  now;
}

nextMove kmagnetScene::isPossibleMove ( Moves::Move m )
{
    int next=getNextPosition ( m );
    Q_ASSERT(next >=0 && next < m_cells.size());
    if ( (int)currentPosition==next || m_cells.at(next)->getVisited() )
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
    setCurrentPosition(cellNumber);
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
    Q_ASSERT(n<ROWS*COLUMNS && n>=0);
    return m_cells[n];
}

void kmagnetScene::drawBackground( QPainter* p, const QRectF& )
{
    p->drawPixmap( 0, 0, kmagnetRenderer::self()->backgroundPixmap(sceneRect().size().toSize()) );
}

void kmagnetScene::setSize ( int r, int c )
{
    this->ROWS=r;
    this->COLUMNS=c;
}

void kmagnetScene::setAllNotVisited()
{
    int i=0;
    while (i< m_cells.size())
    {
        m_cells[i]->setVisited(false);
        i++;
    }
}

void kmagnetScene::printPuzzle()
{
    QString text;
    for (uint i=0; i< COLUMNS; i++)
    {
        text.append("-");
    }
    qDebug() << text;
    text.clear();
    for (uint i=0; i< ROWS; i++)
    {
        for (uint j=0;j< COLUMNS; j++)
        {
            int num=j+i*COLUMNS;
            kmagnetCell *cell =m_cells.at(num);
            if (num==(int)currentPosition)
            {
                text.append("O");
            }
            else if (!cell->getIsFree())
            {
                text.append("X");
            }
            else if (cell->getIsFinal())
            {
                text.append("F");
            }
            else
            {
                text.append(" ");
            }
        }
        qDebug() << text;
        text.clear();
    }
    text.clear();
    for (uint i=0; i< COLUMNS; i++)
    {
        text.append("-");
    }
    qDebug() << text;
}