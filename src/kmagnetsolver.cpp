/*
*/

#include <string>
#include <QDebug>

#include "kmagnetsolver.h"

using namespace std;

kmagnetSolver::kmagnetSolver(kmagnetScene* scene)
{
    this->m_scene=scene;
}

void kmagnetSolver::solve(vector<Moves::Move> lm, nextMove sg, int numrec)
{
    if (numrec>=25) return;
    if (sg.getIsPossible() && dynamic_cast<kmagnetCell*>(m_scene->itemAt(sg.getPosition()))->getIsFinal())
    {//is solution
        if (solution.size()==0 || solution.size()> lm.size())
        {
            solution.clear();
            solution=lm;
        }
        //qDebug() << "solution:";
        //System.out.println("Solution:");
        /*for(unsigned int i=0; i< lm.size(); i++)
        {
        //	System.out.println(lm.get(i));
          QString str=QString();
          switch (lm.at(i))
          {
            case 0:
              str= "UP";break;
            case 1:
              str= "DOWN";break;
            case 2:
              str= "LEFT";break;
            case 3:
              str= "RIGHT";break;
            default:
              str="meeeec";break;
          }
          qDebug() << str;
        }*/
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
        //QPoint p= m_scene->getBallPos().toPoint();
        QPoint p= m_scene->getCurrentPosition();
        //m_scene->setBallPos(nm.getPosition());
	m_scene->setCurrentPosition(nm.getPosition());
        solve(l,nm, n+1);
        //m_scene->setBallPos(p);
	m_scene->setCurrentPosition(p);
        m_scene->setVisited(p,false);
        l.pop_back();
    }
}
