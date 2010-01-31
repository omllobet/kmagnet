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
#include <QStack>
 
kmagnetGenerator::kmagnetGenerator(QObject* parent):QObject(parent)
{
    m_scene=dynamic_cast<kmagnet*>(parent)->get_scene();
}

void kmagnetGenerator::generate()
{
    m_scene->newGame();
    int maxpos=m_scene->getNumCells();
    int numcols=m_scene->getColumns();
    int initialpos=qrand() % maxpos;
    int finalpos=qrand() % maxpos;
    while(finalpos==initialpos || finalpos+1==initialpos 
        || finalpos-1==initialpos || finalpos-numcols==initialpos
        || finalpos+numcols==initialpos)
    {
        finalpos=qrand() % maxpos;
    }
    //QStack<int>  stack;
    kmagnetCell* currentCell= m_scene->getCell(initialpos);
    m_scene->setCurrentPosition(initialpos);
    m_scene->setStartPosition(initialpos);
    m_scene->setBallPos(initialpos);
    
    int currentpos=initialpos;
    QVector<int> u,d,l,r;
    
    //while (currentpos!=finalpos)
    if (m_scene->getNextPosition(Moves::UP)==currentpos)
    {//no nf cell
     //add all positions except contiguous
     int toadd=currentpos-2*numcols;
     while (toadd>=0)
     {
        
     }
    }
        
}

/*create a CellStack (LIFO) to hold a list of cell locations 
set TotalCells = number of cells in grid 
choose a cell at random and call it CurrentCell 
set VisitedCells = 1 
  
while VisitedCells < TotalCells 

      find all neighbors of CurrentCell with all walls intact  
      if one or more found 
            choose one at random 
            knock down the wall between it and CurrentCell 
            push CurrentCell location on the CellStack 
            make the new cell CurrentCell 
            add 1 to VisitedCells else 
            pop the most recent cell entry off the CellStack 
            make it CurrentCell endIf 

endWhile*/  

void kmagnetGenerator::generaterec()
{

}

