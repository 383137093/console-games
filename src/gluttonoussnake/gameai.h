#ifndef GLUTTONOUSSNAKE_GAMEAI_H
#define GLUTTONOUSSNAKE_GAMEAI_H

#include <vector>
#include <utility>
#include "ashes/coord.h"
#include "ashes/direction4.h"
#include "gamecore.h"

class GameAI
{
public:
   
    GameAI();
    GameAI(const GameAI&) = delete;
    ~GameAI();
    GameAI& operator = (const GameAI&) = delete;

    ashes::Direction4 MakeStrategy(const GameCore& game);

private:
    
    void CopyGameData(const GameCore& game);
    std::pair<bool, ashes::Direction4> FindSafelyPathToFood();
    std::pair<bool, ashes::Direction4> FindPathToTail();

    GameMap      game_map_;
    Snake        snake_;
    ashes::Coord food_ = {0, 0};

    std::vector<int>               dists_;
    std::vector<ashes::Direction4> dir_path_;
    std::vector<ashes::Coord>      coord_path_;
};

#endif