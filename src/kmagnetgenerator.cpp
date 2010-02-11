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
#include "settings.h"

#include <QtGlobal>//for qrand
#include <QDebug>

kmagnetGenerator::kmagnetGenerator(QObject* parent):QObject(parent),
        forbiddenPos(QVector<int>()),
        movements(QVector<int>())
{
    m_scene=dynamic_cast<kmagnet*>(parent)->get_scene();
}

void kmagnetGenerator::generate()
{
    forbiddenPos.clear();
    numcells=m_scene->getNumCells();
    movements.clear();
    makingmorepaths=false;
    lastmovement=-1;
    size=0;
    m_scene->newGame();
    columns=m_scene->getColumns();
    uint initialpos=qrand() % numcells;
    finalpos=qrand() % numcells;
    checkValidFinalPos(initialpos);
    m_scene->setCurrentPosition(initialpos);
    m_scene->setStartPosition(initialpos);
    m_scene->setVisited(initialpos,true);
    forbiddenPos.append(initialpos);
    forbiddenPos.append(finalpos);
    movements.append(initialpos);
    m_scene->getCell(finalpos)->setIsFinal(true);
    generaterec();
    m_scene->setCurrentPosition(initialpos);
    if (Settings::extrapaths())
        makeMorePaths();
    //reset
    m_scene->setCurrentPosition(initialpos);
    m_scene->setStartPosition(initialpos);
    m_scene->setBallPos(initialpos);
    m_scene->setAllNotVisited();
}

void kmagnetGenerator::checkValidFinalPos(uint initialpos)
{
    while (finalpos==initialpos || finalpos+1==initialpos
            || finalpos-1==initialpos || finalpos-columns==initialpos
            || finalpos+columns==initialpos || finalpos% columns== initialpos %columns
            || finalpos / columns  == initialpos / columns
            )
    {
        finalpos=qrand() % numcells;
    }
}

void kmagnetGenerator::generaterec()
{
    if (checkFinalPosition(finalpos))
    {
        m_scene->resizeScene( (int)m_scene->sceneRect().width(),
                              (int)m_scene->sceneRect().height() );//redraw
    }
    else
    {
        //try to move to a non visited place
        int nextmove=qrand() % 4;
        if (trymove((Moves::Move)(nextmove))) {
            generaterec();
            return;
        }
        if (trymove((Moves::Move)((int)(nextmove+1)%4))) {
            generaterec();
            return;
        }
        if (trymove((Moves::Move)((int)(nextmove+2)%4))) {
            generaterec();
            return;
        }
        if (trymove((Moves::Move)((int)(nextmove+3)%4))) {
            generaterec();
            return;
        }
        //if its not possible, place a new block
        uint current=m_scene->getCurrentCell();
        nextmove=qrand() % 4;
        if (newBlock(current, (Moves::Move)(nextmove))) {
            generaterec();
            return;
        }
        if (newBlock(current, (Moves::Move)((int)(nextmove+1)%4))) {
            generaterec();
            return;
        }
        if (newBlock(current, (Moves::Move)((int)(nextmove+2)%4))) {
            generaterec();
            return;
        }
        if (newBlock(current, (Moves::Move)((int)(nextmove+3)%4))) {
            generaterec();
            return;
        }
        //if its not possible, try to go back
        movements.pop_back();
        while (!movements.isEmpty())
        {
            int n=movements.last();
            kmagnetCell* c= m_scene->getCell(n);
            if (c->getIsFree() && !c->getIsFinal()) {
                m_scene->setCurrentPosition(n);
                if (checkFinalPosition(finalpos))
                    return;
            }
            movements.pop_back();
        }
        //if its not possible try to place a new ending cell
        if (tryplacefinal(lastmove)) return;
        if (tryplacefinal((Moves::Move)((int)(lastmove+1)%4))) return;
        if (tryplacefinal((Moves::Move)((int)(lastmove+2)%4))) return;
        if (tryplacefinal((Moves::Move)((int)(lastmove+3)%4))) return;
        //qDebug() << "GAME OVER !!!!";
    }
}

/*checks if the next movement could end the game*/
bool kmagnetGenerator::checkFinalPosition(uint finalpos)
{
    nextMove nmUp=m_scene->isPossibleMove(Moves::UP);
    nextMove nmDo=m_scene->isPossibleMove(Moves::DOWN);
    nextMove nmL=m_scene->isPossibleMove(Moves::LEFT);
    nextMove nmR=m_scene->isPossibleMove(Moves::RIGHT);
    int i=0;//No posar com uint!!!!
    uint curCell=m_scene->getCurrentCell();
    bool ret= false;
    if (nmUp.getIsPossible() && nmUp.getPosition()==finalpos)
    {
        for (i=curCell;i>= (int)nmUp.getPosition();i=i-columns) {
            forbiddenPos.append(i);
        }
        ret=true;
    }
    else if (nmDo.getIsPossible() && nmDo.getPosition()==finalpos)
    {
        for (i=curCell;(uint)i<= nmDo.getPosition();i=i+columns) {
            forbiddenPos.append(i);
        }
        ret=true;
    }
    else if (nmL.getIsPossible() && nmL.getPosition()==finalpos)
    {
        for (i=curCell;i>= (int)nmL.getPosition();i--) {
            forbiddenPos.append(i);
        }
        ret=true;
    }
    else if (nmR.getIsPossible() && nmR.getPosition()==finalpos)
    {
        for (i=curCell;(uint)i<= nmR.getPosition();i++) {
            forbiddenPos.append(i);
        }
        ret=true;
    }
    return ret;
}

/*tries to place a final cell in the direction pointed by the move
for now just in the immediately precedent cell*/
bool kmagnetGenerator::tryplacefinal(Moves::Move m)
{
    int cur =lastmovement;
    int size= m_scene->getNumCells();
    kmagnetCell* cell;
    if (m==Moves::UP) {
        if (cur+columns<size) {
            cell=m_scene->getCell(cur+columns);
            if (cell->getIsFree()) {
                cell->setIsFinal(true);
                forbiddenPos.append(cur+columns);
                return true;
            }
        }
    }
    else if (m==Moves::DOWN) {
        if (cur-columns>= 0) {
            cell=m_scene->getCell(cur-columns);
            if (cell->getIsFree()) {
                cell->setIsFinal(true);
                forbiddenPos.append(cur-columns);
                return true;
            }
        }
    }
    else if (m==Moves::LEFT) {
        if (cur+1< size) {
            cell=m_scene->getCell(cur+1);
            if (cell->getIsFree()) {
                cell->setIsFinal(true);
                forbiddenPos.append(cur+1);
                return true;
            }
        }
    }
    else if (m==Moves::RIGHT) {
        if (cur-1>=0) {
            cell=m_scene->getCell(cur-1);
            if (cell->getIsFree()) {
                cell->setIsFinal(true);
                forbiddenPos.append(cur-1);
                return true;
            }
        }
    }
    return false;
}

/*tries to move to the direction pointed by Move::m
returns true if can move, false if couldnt*/
bool kmagnetGenerator::trymove(Moves::Move m)
{
    //avoid returning from the same path
    if (m==lastmove ||
            (lastmove==Moves::UP && m==Moves::DOWN) ||
            (m==Moves::UP && lastmove==Moves::DOWN) ||
            (lastmove==Moves::LEFT && m==Moves::RIGHT) ||
            (m==Moves::LEFT && lastmove==Moves::RIGHT)
       )
        return false;
    nextMove nm=m_scene->isPossibleMove(m);
    int nextPos= nm.getPosition();
    int curCell=m_scene->getCurrentCell();
    if ( nm.getIsPossible()  && nextPos != curCell && !m_scene->getCell(nextPos)->getVisited())
    {
        int i=0;
        if (m==Moves::UP) {
            for (i=curCell;i>= nextPos;i=i-columns) {
                forbiddenPos.append(i);
            }
        }
        else if (m==Moves::DOWN) {
            for (i=curCell;i<= nextPos;i=i+columns) {
                forbiddenPos.append(i);
            }
        }
        else if (m==Moves::LEFT) {
            for (i=curCell;i>= nextPos;i--) {
                forbiddenPos.append(i);
            }
        }
        else if (m==Moves::RIGHT) {
            for (i=curCell;i<= nextPos;i++) {
                forbiddenPos.append(i);
            }
        }
        //save last movement
        if (movements.size()>size || lastmovement==-1) {
            lastmovement= movements.last();
            size=movements.size();
        }
        lastmove=m;
        m_scene->setCurrentPosition(nextPos);
        m_scene->setVisited(nextPos, true);
        movements.append(nextPos);
        forbiddenPos.append(nextPos);
        return true;
    }
    return false;//couldn't move
}

/* puts a new block in the board*/
bool kmagnetGenerator::newBlock(int currentpos, Moves::Move m)
{
    QVector<int> u,d,l,r;
    int toadd=0;
    int pos=0;
    if (m==Moves::UP)
    {
        toadd=currentpos-2*columns;
        pos=currentpos-columns;
        if (pos>=0 && !m_scene->getCell(pos)->getIsFree()) toadd=-1;//ugly
        while (toadd>=0 && m_scene->getCell(toadd)->getIsFree())
        {
            if (!forbiddenPos.contains(toadd)) {
                u.append(toadd);
            }
            toadd=toadd-columns;
        }
        return placeBlock(u, m);
    }
    else if (m==Moves::DOWN)
    {
        toadd=currentpos+2*columns;
        pos=currentpos+columns;
        if (pos <numcells && !m_scene->getCell(pos)->getIsFree()) toadd=numcells;
        while (toadd<numcells && m_scene->getCell(toadd)->getIsFree())
        {
            if (!forbiddenPos.contains(toadd)) {
                d.append(toadd);
            }
            toadd=toadd+columns;
        }
        return placeBlock(d, m);
    }
    else if (m==Moves::LEFT)
    {
        toadd=currentpos+2;
        int endr=( currentpos/columns ) *columns+columns;
        pos=currentpos+1;
        if (pos < numcells && !m_scene->getCell(pos)->getIsFree()) toadd=endr;
        while (toadd<endr && m_scene->getCell(toadd)->getIsFree())
        {
            if (!forbiddenPos.contains(toadd)) {
                r.append(toadd);
            }
            toadd++;
        }
        return placeBlock(r, m);
    }
    else if (Moves::RIGHT)
    {
        toadd=currentpos-2;
        int endl=static_cast<int> ( ( currentpos/columns ) *columns );
        pos=currentpos-1;
        if (pos >=0 && !m_scene->getCell(pos)->getIsFree()) toadd=endl-1;
        while (toadd>=endl && m_scene->getCell(toadd)->getIsFree())
        {
            if (!forbiddenPos.contains(toadd)) {
                l.append(toadd);
            }
            toadd--;
        }
        return placeBlock(l, m);
    }
    return true;
}

bool kmagnetGenerator::placeBlock(QVector<int> &positions, Moves::Move m)
{
    if (positions.isEmpty()) return false;
    int num=qrand() % positions.size();
    num=positions.at(num);
    m_scene->getCell(num)->setIsFree(false);
    forbidcells(num,positions, m);
    if (makingmorepaths) {
        int posorig=m_scene->getCurrentCell();
        if (trymove(m) && checkFinalPosition(finalpos))
        {
            m_scene->getCell(num)->setIsFree(true);
        }
        m_scene->setCurrentPosition(posorig);
    }
    return true;
}

/*forbids cells so no block is put there to avoid blocking the a path*/
void kmagnetGenerator::forbidcells(int num, QVector<int> &positions, Moves::Move m)
{
    int i=0;
    int vecnum;
    int banpos=0;
    int currentpos=m_scene->getCurrentCell();
    bool finished=false;
    if (m==Moves::UP)
    {
        banpos=currentpos-columns;
        if (banpos>=0)
            forbiddenPos.append(banpos);
        for (i=0;i < positions.size() && !finished ;i++)
        {
            vecnum=positions.at(i);
            if (vecnum>=num) {
                forbiddenPos.append(vecnum);
                if (vecnum==num)finished=true;
            }
        }
    }
    else if (m==Moves::DOWN)
    {
        finished=false;
        banpos= currentpos+columns;
        if (banpos<=numcells) forbiddenPos.append(banpos);
        for (i=positions.size()-1; i>=0 && !finished ;i--)
        {
            vecnum=positions.at(i);
            if (vecnum<=num) {
                forbiddenPos.append(vecnum);
                if (vecnum==num)finished=true;
            }
        }
    }
    else if (m==Moves::LEFT)
    {
        finished=false;
        if ((currentpos % columns)!=0)
            forbiddenPos.append(currentpos-1);
        for (i=0; i < positions.size() && !finished;i++)
        {
            vecnum=positions.at(i);
            if (vecnum>=num) {
                forbiddenPos.append(vecnum);
                if (vecnum==num)finished=true;
            }
        }
    }
    else if (m==Moves::RIGHT)
    {
        finished=false;
        if ((currentpos % columns)!=9)
            forbiddenPos.append(currentpos+1);
        for (i=positions.size()-1; i>=0 && !finished ;i--)
        {
            vecnum=positions.at(i);
            if (vecnum<=num) {
                forbiddenPos.append(vecnum);
                if (vecnum==num)finished=true;
            }
        }
    }
}

void kmagnetGenerator::makeMorePaths()
{
    int i=0;
    makingmorepaths=true;
    while ( i < movements.size())
    {
        int move=movements.at(i);
        if (m_scene->getCell(move)->getIsFree() && !m_scene->getCell(move)->getIsFinal())
        {
            m_scene->setCurrentPosition(move);
        }
        else
        {   
            i++;
            continue;
        }
        int moveDirection=qrand() % 4;
        Moves::Move m = (Moves::Move)moveDirection;
        makeAction(move,m);
        m=(Moves::Move)((int)(moveDirection+1) %4);
        makeAction(move,m);
        m=(Moves::Move)((int)(moveDirection+2) %4);
        makeAction(move,m);
        m=(Moves::Move)((int)(moveDirection+3) %4);
        makeAction(move,m);
        i++;
    }
}

/*if its possible to makes a move, adds the move to the movements vector
if not, tries to add a block*/
void kmagnetGenerator::makeAction(int move, Moves::Move m)
{
    nextMove nm= m_scene->isPossibleMove(m);
    int nextpos=nm.getPosition();
    if (nm.getIsPossible())
    {
        if (!movements.contains(nextpos))
            movements.append(nextpos);
    }
    else//new block
    {
        bool b= newBlock(move,m);
        nm= m_scene->isPossibleMove(m);
        nextpos=nm.getPosition();
        if (b && nm.getIsPossible() && !movements.contains(nextpos)) {
            movements.append(nextpos);
        }
    }
}
