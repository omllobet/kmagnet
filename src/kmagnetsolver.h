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

#ifndef KMAGNETSOLVER_H
#define KMAGNETSOLVER_H

//#include <QObject>
#include <QThread>
#include <QVector>
//#include <vector>

#include "common.h"
typedef QVector<Moves::Move> QVectorMoves;

#include "kmagnetscene.h"

class kmagnetScene;
class kmagnetSolver: public QThread //I should read Qts docs about threads
{

   Q_OBJECT
   
public:

    kmagnetSolver(QObject* parent=0);
    void findSolution(kmagnetScene *scene);
protected:    
    virtual void run();

signals:
    void sendSolution(QVectorMoves sol);

private:

    void trymove(Moves::Move m, QVector<Moves::Move> &l, int n);
    void solve(QVector<Moves::Move> &lm, nextMove sg, int numrec);
    
    kmagnetScene* m_scene;
    QVector<Moves::Move> solution;

};

#endif // KMAGNETSOLVER_H
