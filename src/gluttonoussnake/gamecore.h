#ifndef GLUTTONOUSSNAKE_GAMECORE_H
#define GLUTTONOUSSNAKE_GAMECORE_H

#include <vector>
#include "ashes/time.h"
#include "ashes/coord.h"
#include "ashes/direction4.h"
#include "ashes/eventdispatcher.h"
#include "snake.h"
#include "gamemap.h"

class GameCore
{
public:
    
    enum class GameState { Ready, UnderWay, Pause, Over, Win };
    enum class SnakeMoveResult { Move, Grow, Die, Win };

    struct SnakeMoveEvent
    {
        const Snake&    snake;      // the moved snake.
        SnakeMoveResult result;     // move result.
        Snake::BodyNode tailprint;  // snake tail before move.
    };

    GameCore();
    GameCore(const GameCore&) = delete;
    ~GameCore();
    GameCore& operator = (const GameCore&) = delete;

    GameState GetGameState() const;
    const GameMap& GetGameMap() const;
    const Snake& GetSnake() const;
    const ashes::Coord& GetFood() const;

    bool ExpandGameMap();
    bool ShrinkGameMap();
    bool AccelerateSnake();
    bool DecelerateSnake();
    bool SetSnakeForwardDirection(ashes::Direction4 dir);

    void ReadyGame();
    void StartGame();
    void PauseGame(bool pause);
    void TickGame();

    ashes::EventDispatcher<void(SnakeMoveEvent)> OnSnakeMoved;

private:

    static const ashes::Coord kGameMapSizeMin;
    static const ashes::Coord kGameMapSizeMax;
    static const ashes::Coord kGameMapSizeDef;
    static const std::vector<ashes::time::Msecs> kSnakeMoveCooldowns;

    void PlaceSnakeOnCenterCoord();
    void PlaceFoodOnRandomCoord();
    void ForwardMoveSnake();

    GameState              game_state_ = GameState::Ready;
    GameMap                game_map_;
    Snake                  snake_;
    ashes::Coord           food_ = {0, 0};
    int                    snake_speed_level_ = 0;
    ashes::time::TimePoint snake_move_timing_;
};

#endif