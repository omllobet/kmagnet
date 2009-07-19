/*
*/

#ifndef KMAGNETSOLVER_H
#define KMAGNETSOLVER_H

#include <vector>

#include "common.h"
#include "kmagnetscene.h"

using namespace std;

class kmagnetScene;
class kmagnetSolver
{
public:

      kmagnetSolver();
      kmagnetSolver(kmagnetScene* scene);
      vector<Moves::Move> getSolution(){return solution;};
      void solve(vector<Moves::Move> lm, nextMove sg, int numrec);    

private:
    void trymove(Moves::Move m, vector<Moves::Move> &l, int n);
    
    kmagnetScene* m_scene;
    vector<Moves::Move> solution;
    
};

#endif // KMAGNETSOLVER_H
