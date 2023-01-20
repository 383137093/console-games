#include <fstream>
#include "gameapp.h"
#include "gamesolver.h"

void RunGameSolver()
{
    GameSolver solver;
    solver.BruteForceSolve();
    std::ofstream ofs("res\\gameaidatabase.dat");
    solver.Print(ofs);
}

int main()
{
    GameApp ui;
    ui.Run();
    return 0;
}
