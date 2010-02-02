/*************************************************************************************
 *  Copyright (C) 2010 by Oscar Martinez <omllobet@gmail.com>                        *
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

#include "kmagnetgenerator.h"

#include <QtGlobal>
#include <QDebug>

//This should be fast
// Hola Victor!!! XD
//maybe should change kmagnetcell to inherit from simplecell
//but the redraw only occurs at the end...així que...tampoc es problem
//TODO Don't drink and code
//TODO rethink?? cleanup?? simplify??
//this one first places start and end an then generates a path between them
//I should try to place blocks randomly and then generate a path? DFS?
//mmm maybe just generate a random number between 8 and 20
//dependending on the level, then make a path more or less random
//and then try to make other paths to go nowhere...
//I also could make the algorithm find a lot of solutions and then
//choose one...but the cost wolud me much higher
//maybe I should just leave this more ot less as is
//and add a function to try to add more paths to
//make a fully map and not just a path from the beginning to the end
//maybe I could add a minimium of three steps or something
kmagnetGenerator::kmagnetGenerator(QObject* parent):QObject(parent),
    forbiddenPos(QVector<int>()),
    movements(QVector<int>())
{
    m_scene=dynamic_cast<kmagnet*>(parent)->get_scene();
}

void kmagnetGenerator::generate()
{
    forbiddenPos.clear();
    movements.clear();
    lastmovement=-1;
    size=0;
    m_scene->newGame();
    int maxpos=m_scene->getNumCells();
    columns=m_scene->getColumns();
    int initialpos=qrand() % maxpos;
    finalpos=qrand() % maxpos;
    //check if its a valid final pos
    while (finalpos==initialpos || finalpos+1==initialpos
            || finalpos-1==initialpos || finalpos-columns==initialpos
            || finalpos+columns==initialpos)
    {//make a better function?? check row and column??
        finalpos=qrand() % maxpos;
    }
    qDebug() << "initialpos: " << initialpos;
    qDebug() << "finalpos: " << finalpos;
    m_scene->setCurrentPosition(initialpos);
    m_scene->setStartPosition(initialpos);
    m_scene->setVisited(initialpos,true);
    forbiddenPos.append(initialpos);
    movements.append(initialpos);
    m_scene->getCell(finalpos)->setIsFinal(true);
    numcrides=0;
    generaterec();
    //TODO makeMorePaths
    //reset
    m_scene->setCurrentPosition(initialpos);
    m_scene->setStartPosition(initialpos);
    m_scene->setBallPos(initialpos);
    m_scene->setAllNotVisited();
}

void kmagnetGenerator::generaterec()
{
    if (numcrides>=1500) { qDebug() << "Aiooooooosssssssss"; return; }
    numcrides++;
    if (movements.isEmpty()) { qDebug() << "impossible is nothing"; return; }
    if (m_scene->getNextPosition(Moves::UP)==finalpos ||
            m_scene->getNextPosition(Moves::DOWN)==finalpos ||
            m_scene->getNextPosition(Moves::LEFT)==finalpos ||
            m_scene->getNextPosition(Moves::RIGHT)==finalpos
       )
    {//path completed
        qDebug() << "FINITTTTTOOOOOOOOOOOO";
        m_scene->resizeScene( (int)m_scene->sceneRect().width(),
                              (int)m_scene->sceneRect().height() );//redraw
    }
    else
    {
        qDebug() << "trying to move to a nonvisited";
        //try to move to a non visited place
        int nextmove=qrand() % 4;
        if (trymove((Moves::Move)(nextmove))) { generaterec();return;}
        if (trymove((Moves::Move)((int)(nextmove+1)%4))) { generaterec(); return; }
        if (trymove((Moves::Move)((int)(nextmove+2)%4))) { generaterec(); return; }
        if (trymove((Moves::Move)((int)(nextmove+3)%4))) { generaterec(); return; }
     
     //if its not possible, place a new block
        qDebug() << "trying to place a block";
        uint current=m_scene->getCurrentCell();
        if (newBlock(current)) { generaterec(); return;}
     
     //if its not possible, try to go back
        qDebug() << "try to continue!! " << " mida= " << movements.size();
        movements.pop_back();
        while (!movements.isEmpty())
        {
            int n=movements.last();
            kmagnetCell* c= m_scene->getCell(n);
            if (c->getIsFree()){
                m_scene->setCurrentPosition(n);
                generaterec();
                return;
            }
            movements.pop_back();
        }
        qDebug() << "no hay mas llamadas!!!!";
      //if its not possible try to place a new ending cell
        if (tryplacefinal(lastmove)) return;
        if (tryplacefinal((Moves::Move)((int)(lastmove+1)%4))) return;
        if (tryplacefinal((Moves::Move)((int)(lastmove+2)%4))) return;
        if (tryplacefinal((Moves::Move)((int)(lastmove+3)%4))) return;
        qDebug() << "GAME OVER !!!!";
    }
}

bool kmagnetGenerator::tryplacefinal(Moves::Move m)
{
    qDebug() << "final size= " << size;
    int cur =lastmovement;
    int size= m_scene->getNumCells();
    kmagnetCell* cell;
    if (m==Moves::UP){
        if (cur-columns>=0){
            cell=m_scene->getCell(cur-columns);
            if (cell->getIsFree()){ cell->setIsFinal(true);return true;}
        }
    }
    else if (m==Moves::DOWN){
        if (cur+columns< size){
            cell=m_scene->getCell(cur+columns);
            if (cell->getIsFree()){ cell->setIsFinal(true);return true;}
        }
    }
    else if (m==Moves::RIGHT){
        if (cur+1< size){
            cell=m_scene->getCell(cur+1);
            if (cell->getIsFree()){ cell->setIsFinal(true);return true;}
        }
    }
    else if (m==Moves::LEFT){
        if (cur-1>=0){
            cell=m_scene->getCell(cur-1);
            if (cell->getIsFree()){ cell->setIsFinal(true);return true;}
        }
    }
    return false;
}

bool kmagnetGenerator::trymove(Moves::Move m)
{
    int nextPos= m_scene->getNextPosition(m);
    int curCell=m_scene->getCurrentCell();
    //qDebug() << "next position= " << nextPos << "while moving: " << m << "from pos :" << curCell;
    if ( nextPos != curCell && !m_scene->getCell(nextPos)->getVisited())
    {
        // qDebug() << "forbiding positions!!";
        int i=0;
        if (m==Moves::UP){
            for (i=curCell;i>= nextPos;i=i-columns) {forbiddenPos.append(i);}
        } 
        else if (m==Moves::DOWN){
            for (i=curCell;i<= nextPos;i=i+columns) {forbiddenPos.append(i);}
        } 
        else if (m==Moves::LEFT){
            for (i=curCell;i>= nextPos;i--) {forbiddenPos.append(i);}
        }
        else if (m==Moves::RIGHT){
            for (i=curCell;i<= nextPos;i++) {forbiddenPos.append(i);}
        }
        //DEBUG
        /*QString nums=QString("");
        QString temp;
        for (i=0; i< forbiddenPos.size(); i++)
        {
            nums.append( temp.setNum(forbiddenPos.at(i)) + " " );
        }
        qDebug() << "forbidden positions trymove: " << nums;
        qDebug() << "setting next position to" << nextPos;*/
        //FIDEBUG
        //save best movement
        if (movements.size()>size || lastmovement==-1) {
            lastmovement= movements.last();
            size=movements.size(); qDebug() << "mov: " << lastmovement << " size: " << size;
        }
        lastmove=m;
        
        m_scene->setCurrentPosition(nextPos);
        m_scene->setVisited(nextPos, true);
        movements.append(nextPos);
        return true;
    }
    return false;
}

/* puts a new block in the board*/
bool kmagnetGenerator::newBlock(int currentpos)
{
    QVector<int> u,d,l,r;
    QVector<int> all;
    int numcells=m_scene->getNumCells();
    int toadd=currentpos-2*columns;
    int pos=currentpos-columns;
    //generate possible places where to add a block
    if (pos>=0 && !m_scene->getCell(pos)->getIsFree()) toadd=-1;//put in the while condition better
    while (toadd>=0 && m_scene->getCell(toadd)->getIsFree())
    {
        if (!forbiddenPos.contains(toadd)){
            u.append(toadd);
            all.append(toadd);
        }
        toadd=toadd-columns;
    }
    toadd=currentpos+2*columns;
    pos=currentpos+columns;
    if (pos <numcells && !m_scene->getCell(pos)->getIsFree()) toadd=numcells+1;
    while (toadd<numcells && m_scene->getCell(toadd)->getIsFree())
    {
        if (!forbiddenPos.contains(toadd)){
            d.append(toadd);
            all.append(toadd);
        }
        toadd=toadd+columns;
    }
    toadd=currentpos+2;
    int endr=( currentpos/columns ) *columns+columns;
    pos=currentpos+1;
    if (pos < numcells && !m_scene->getCell(pos)->getIsFree()) toadd=endr;
    while (toadd<endr && m_scene->getCell(toadd)->getIsFree())
    {
        if (!forbiddenPos.contains(toadd)){
            r.append(toadd);
            all.append(toadd);
        }
        toadd++;
    }
    toadd=currentpos-2;
    int endl=static_cast<int> ( ( currentpos/columns ) *columns );
    pos=currentpos-1;
    if (pos >=0 && !m_scene->getCell(pos)->getIsFree()) toadd=endl-1;
    while (toadd>=endl && m_scene->getCell(toadd)->getIsFree())
    {
        if (!forbiddenPos.contains(toadd)){
            l.append(toadd);
            all.append(toadd);
        }
        toadd--;
    }
    //choose one place where to add the block
    if (all.isEmpty()) return false;
    int num=qrand() % all.size();
    num=all.at(num);
    qDebug() << "setting nonfreecell: " << num;
    m_scene->getCell(num)->setIsFree(false);
    //ban the cells in the direction where it came from
    forbidcells(currentpos, num, u, d, l, r);
    return true;
}

void kmagnetGenerator::forbidcells(int currentpos, int num, QVector<int> &u, QVector<int> &d, QVector<int> &l, QVector<int> &r )
{
    //forbid cells//does not check for dupes...yet?
    int i=0;
    int vecnum;
    bool finished=false;
    if (u.contains(num))
    {
        forbiddenPos.append(currentpos-columns);
        for (i=0;i < u.size() && !finished ;i++)
        {
            vecnum=u.at(i);
            if (vecnum>=num) {
                forbiddenPos.append(vecnum);
                if (vecnum<=num)finished=true;
            }
        }
    }
    else if (d.contains(num))
    {
        forbiddenPos.append(currentpos+columns);
        for (i=d.size()-1; i>=0 && !finished ;i--)
        {
            vecnum=d.at(i);
            if (vecnum<=num) {
                forbiddenPos.append(vecnum);
                if (vecnum>=num)finished=true;
            }
        }
    }
    else if (l.contains(num))
    {
        forbiddenPos.append(currentpos-1);
        for (i=0; i< l.size() && !finished;i++)
        {
            vecnum=l.at(i);
            if (vecnum>=num) {
                forbiddenPos.append(vecnum);
                if (vecnum<=num)finished=true;
            }
        }
    }
    else if (r.contains(num))
    {
        forbiddenPos.append(currentpos+1);
        for (i=r.size()-1; i>=0 && !finished ;i--)
        {
            vecnum=r.at(i);
            if (vecnum<=num) {
                forbiddenPos.append(vecnum);
                if (vecnum>=num)finished=true;
            }
        }
    }
    //DEBUG
    /*QString nums=QString("");
    QString temp;
    for (i=0; i< forbiddenPos.size(); i++)
    {
        nums.append( temp.setNum(forbiddenPos.at(i)) + " " );
    }
    qDebug() << "forbidden positions: " << nums;*/
    //FIDEBUG
}
