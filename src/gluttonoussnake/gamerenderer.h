#ifndef GLUTTONOUSSNAKE_GAMERENDERER_H
#define GLUTTONOUSSNAKE_GAMERENDERER_H

#include <memory>
#include "ashes/coord.h"
#include "gamecore.h"

class GameRenderer
{
public:
    
    typedef GameCore::SnakeMoveResult SnakeMoveResult;
    typedef GameCore::SnakeMoveEvent  SnakeMoveEvent;

    static std::unique_ptr<GameRenderer> MakeLineStyleRenderer();
    static std::unique_ptr<GameRenderer> MakeSquareStyleRenderer();

    GameRenderer();
    GameRenderer(const GameRenderer&) = delete;
    virtual ~GameRenderer();
    GameRenderer& operator = (const GameRenderer&) = delete;

    void SetupConsole(const GameCore& game);
    void DrawGuide(const GameCore& game);
    void DrawGaming(const GameCore& game);
    void DrawResult(const GameCore& game);

    virtual ashes::Coord TransformMapSize(const ashes::Coord& size) const = 0;
    virtual ashes::Coord TransformMapCoord(const ashes::Coord& coord) const = 0;

    virtual void DrawSnake(const Snake& snake) = 0;
    virtual void DrawSnakeMove(const SnakeMoveEvent& move) = 0;
    virtual void DrawFood(const ashes::Coord& food) = 0;

protected:

    WORD text_color_ = 0x0F;
    WORD space_color_ = 0x00;
    WORD snake_color_ = 0x00;
    WORD food_color_ = 0x00;
};

#endif