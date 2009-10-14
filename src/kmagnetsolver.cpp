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

#include <string>

#include "kmagnetsolver.h"
#include <settings.h>

using namespace std;

kmagnetSolver::kmagnetSolver(QObject* parent):QObject(parent)
{
    m_scene=dynamic_cast<kmagnet*>(parent)->get_scene();
}

void kmagnetSolver::findSolution()
{
    std::vector<Moves::Move> lm;
    nextMove nm = nextMove(false, m_scene->getCurrentCell());
    int calls=0;
    solution.clear();
    solve(lm,nm,calls);
    emit finished();
    qDebug("-- .- .-. - .. -. ..");
}

void kmagnetSolver::solve(vector<Moves::Move> &lm, nextMove sg, int numrec)
{
    if (numrec>=Settings::maxCalls()) return;
    if (sg.getIsPossible() && (m_scene->getCell(sg.getPosition())->getIsFinal()))
    {//is solution
        if (solution.size()==0 || solution.size()> lm.size())
        {
            solution.clear();
            solution=lm;
        }
        return;
    }
    else
    {
        if (lm.empty())
        {
            trymove(Moves::UP,lm, numrec);
            trymove(Moves::DOWN,lm, numrec);
            trymove(Moves::LEFT,lm, numrec);
            trymove(Moves::RIGHT,lm, numrec);
        }
        else
        {
            Moves::Move m=lm.at(lm.size()-1);
            if (m==Moves::UP || m==Moves::DOWN)
            {
                trymove(Moves::LEFT,lm, numrec);
                trymove(Moves::RIGHT,lm, numrec);
            }
            else if (m==Moves::LEFT || m==Moves::RIGHT)
            {
                trymove(Moves::UP,lm, numrec);
                trymove(Moves::DOWN,lm, numrec);
            }
        }
    }
}

void kmagnetSolver::trymove(Moves::Move m, vector<Moves::Move> &l, int n)
{
    nextMove nm = m_scene->isPossibleMove(m);
    if (nm.getIsPossible())
    {
        l.push_back(m);
        uint p= m_scene->getCurrentCell();
        m_scene->setVisited(p,true);
        m_scene->setCurrentPosition(nm.getPosition());
        solve(l,nm, n+1);
        m_scene->setCurrentPosition(p);
        m_scene->setVisited(p,false);
        l.pop_back();
    }
}