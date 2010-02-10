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


#ifndef KMAGNETGENERATOR_H
#define KMAGNETGENERATOR_H

#include <QObject>

#include "kmagnetscene.h"

class kmagnetScene;
class kmagnetGenerator: public QObject
{
    
    Q_OBJECT

public:
    kmagnetGenerator(QObject* parent=0);
    void generate();
    
private:

    void generaterec();
    bool trymove(Moves::Move m);
    bool newBlock(int currentpos, Moves::Move m);
    bool tryplacefinal(Moves::Move m);
    void forbidcells(int num, QVector<int> &positions, Moves::Move m);
    void makeMorePaths();
    bool placeBlock(QVector<int> &positions, Moves::Move m);
    bool checkFinalPosition(uint finalpos);
    void makeAction(int move, Moves::Move m);
    void checkValidFinalPos(uint initialpos);
    
    kmagnetScene* m_scene;
    uint finalpos;
    int columns;
    QVector<int> forbiddenPos;//posicions on esta prohibit posar un bloc
    QVector<int> movements;//moviments fets fins ara
    int numcells;
    bool makingmorepaths;
    int lastmovement;//cell in the last movement
    int size;//size of the movements made
    Moves::Move lastmove;//direction of the last movement
};

#endif // KMAGNETGENERATOR_H
