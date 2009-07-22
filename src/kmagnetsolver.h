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

#include <QObject>

#include <vector>

#include "common.h"
#include "kmagnetscene.h"
#include "kmagnet.h"

using namespace std;

class kmagnetSolver: public QObject
{

    Q_OBJECT

public:

    kmagnetSolver(kmagnetScene* scene);
    void solve(vector<Moves::Move> &lm, nextMove sg, int numrec);
    vector<Moves::Move> getSolution() {
        return solution;
    };
    void findSolution();

signals:
    void finished();

private:

    void trymove(Moves::Move m, vector<Moves::Move> &l, int n);

    kmagnetScene* m_scene;
    vector<Moves::Move> solution;

};

#endif // KMAGNETSOLVER_H
