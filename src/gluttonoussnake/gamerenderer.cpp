#include "gamerenderer.h"
#include <vector>
#include <tchar.h>
#include "ashes/coord.h"
#include "ashes/graph.h"
#include "ashes/console.h"
#include "ashes/direction4.h"

//==============================================================================
// GameRenderer - LineStyle
//==============================================================================

class GameRendererLineStyle : public GameRenderer
{
public:

    GameRendererLineStyle();
    
    ashes::Coord TransformMapSize(const ashes::Coord& size) const override;
    ashes::Coord TransformMapCoord(const ashes::Coord& coord) const override;

    void DrawSnake(const Snake& snake) override;
    void DrawSnakeMove(const SnakeMoveEvent& move) override;
    void DrawFood(const ashes::Coord& food) override;

    void DrawSnakeNode(const Snake::BodyNode& node, const Snake::BodyNode* next);
    void DrawSnakeTailprint(const Snake::BodyNode& tailprint);
};

GameRendererLineStyle::GameRendererLineStyle()
{
    snake_color_ = 0x0E;
    food_color_  = 0x0B;
}

ashes::Coord GameRendererLineStyle::TransformMapSize(
    const ashes::Coord& size) const
{
    return {static_cast<SHORT>(size.X * 2 + 4),
            static_cast<SHORT>(size.Y + 2)};
}

ashes::Coord GameRendererLineStyle::TransformMapCoord(
    const ashes::Coord& coord) const
{
    return {static_cast<SHORT>(coord.X * 2 + 2),
            static_cast<SHORT>(coord.Y + 1)};
}

void GameRendererLineStyle::DrawSnake(const Snake& snake)
{
    for (int i = 0; i < snake.Length() - 1; ++i)
    {
        DrawSnakeNode(snake.Node(i), &snake.Node(i + 1));
    }
    DrawSnakeNode(snake.Tail(), nullptr);
}

void GameRendererLineStyle::DrawSnakeMove(const SnakeMoveEvent& move)
{
    DrawSnakeNode(move.snake.Head(), nullptr);

    if (move.snake.Length() > 1)
    {
        DrawSnakeNode(move.snake.Node(1), (move.snake.Length() > 2 ?
            &move.snake.Node(2) : &move.tailprint));
    }

    if (move.result == SnakeMoveResult::Move ||
        move.result == SnakeMoveResult::Win)
    {
        DrawSnakeTailprint(move.tailprint);
    }
}

void GameRendererLineStyle::DrawFood(const ashes::Coord& food)
{
    static const ashes::String kText = TEXT("●");
    ashes::Coord coord = TransformMapCoord(food);
    ashes::GConsole().DrawString(kText, coord, food_color_);
}

void GameRendererLineStyle::DrawSnakeNode(
    const Snake::BodyNode& node,
    const Snake::BodyNode* next)
{
    static const ashes::String kNodeTexts[4][4] = {
        {TEXT("━"), TEXT("┗"), TEXT("？"), TEXT("┏")},
        {TEXT("┓"), TEXT("┃"), TEXT("┏"), TEXT("？")},
        {TEXT("？"), TEXT("┛"), TEXT("━"), TEXT("┓")},
        {TEXT("┛"), TEXT("？"), TEXT("┗"), TEXT("┃")},};

    int dir1 = ashes::direction4::ToInt(node.dir);
    int dir2 = (next != nullptr ? ashes::direction4::ToInt(next->dir) : dir1);

    const ashes::String& text = kNodeTexts[dir2][dir1];
    ashes::Coord coord = TransformMapCoord(node.pos);
        
    ashes::GConsole().DrawString(text, coord, snake_color_);
}

void GameRendererLineStyle::DrawSnakeTailprint(const Snake::BodyNode& tailprint)
{
    static const ashes::String kText = TEXT("　");
    ashes::Coord coord = TransformMapCoord(tailprint.pos);
    ashes::GConsole().DrawString(kText, coord, space_color_);
}

//==============================================================================
// GameRenderer - Square
//==============================================================================

class GameRendererSquareStyle : public GameRenderer
{
public:

    GameRendererSquareStyle();
    
    ashes::Coord TransformMapSize(const ashes::Coord& size) const override;
    ashes::Coord TransformMapCoord(const ashes::Coord& coord) const override;

    void DrawSnake(const Snake& snake) override;
    void DrawSnakeMove(const SnakeMoveEvent& move) override;
    void DrawFood(const ashes::Coord& food) override;

    void DrawGrid(const ashes::Coord& coord, WORD color);
    void DrawGridJoint(const ashes::Coord& a, const ashes::Coord& b, WORD color);
};

GameRendererSquareStyle::GameRendererSquareStyle()
{
    snake_color_ = 0xE0;
    food_color_  = 0xB0;
}

ashes::Coord GameRendererSquareStyle::TransformMapSize(
    const ashes::Coord& size) const
{
    return {static_cast<SHORT>(size.X * 4 + 2),
            static_cast<SHORT>(size.Y * 2 + 1)};
}

ashes::Coord GameRendererSquareStyle::TransformMapCoord(
    const ashes::Coord& coord) const
{
    return {static_cast<SHORT>(coord.X * 4 + 2),
            static_cast<SHORT>(coord.Y * 2 + 1)};
}

void GameRendererSquareStyle::DrawSnake(const Snake& snake)
{
    for (int i = 0; i < snake.Length() - 1; ++i)
    {
        DrawGrid(snake.Node(i).pos, snake_color_);
        DrawGridJoint(snake.Node(i).pos, snake.Node(i + 1).pos, snake_color_);
    }
    DrawGrid(snake.Tail().pos, snake_color_);
}

void GameRendererSquareStyle::DrawSnakeMove(const SnakeMoveEvent& move)
{
    if (move.snake.Length() > 1)
    {
        DrawGridJoint(move.snake.Head().pos, move.snake.Node(1).pos, snake_color_);
    }

    DrawGrid(move.snake.Head().pos, snake_color_);

    if (move.result == SnakeMoveResult::Move ||
        move.result == SnakeMoveResult::Win)
    {
        DrawGridJoint(move.tailprint.pos, move.snake.Tail().pos, space_color_);
        DrawGrid(move.tailprint.pos, space_color_);
    }
}

void GameRendererSquareStyle::DrawFood(const ashes::Coord& food)
{
    DrawGrid(food, food_color_);
}

void GameRendererSquareStyle::DrawGrid(const ashes::Coord& coord, WORD color)
{
    DrawGridJoint(coord, coord, color);
}

void GameRendererSquareStyle::DrawGridJoint(
    const ashes::Coord& a,
    const ashes::Coord& b,
    WORD color)
{
    static const ashes::String kText = TEXT("　");
    ashes::Coord coord = (TransformMapCoord(a) + TransformMapCoord(b)) / 2;
    ashes::GConsole().DrawString(kText, coord, color);
}

//==============================================================================
// GameRenderer - Interface
//==============================================================================

std::unique_ptr<GameRenderer> GameRenderer::MakeLineStyleRenderer()
{
    return std::make_unique<GameRendererLineStyle>();
}

std::unique_ptr<GameRenderer> GameRenderer::MakeSquareStyleRenderer()
{
    return std::make_unique<GameRendererSquareStyle>();
}

GameRenderer::GameRenderer()
{
}

GameRenderer::~GameRenderer()
{
}

void GameRenderer::SetupConsole(const GameCore& game)
{
    ashes::Console& console = ashes::GConsole();
    ashes::graph::Border border = ashes::graph::BorderThickDoubleLine();
    border.color = text_color_;
    console.SetDefaultBorder(border);
    console.DrawBlank(console.Region(), space_color_);
    console.Resize(TransformMapSize(game.GetGameMap().Size()));
    console.DrawBlank(console.Region(), space_color_);
    console.Center();
}

void GameRenderer::DrawGuide(const GameCore&)
{
    static const std::vector<ashes::String> kTexts = {
        TEXT("          Gluttonous Snake          "),
        TEXT("                                    "),
        TEXT(" ←↑↓→   control direction       "),
        TEXT(" SPACE      start game / pause game "),
        TEXT(" TAB        switch painting style   "),
        TEXT(" CTRL       toggle AI controller    "),
        TEXT(" W / S      expand map / shrink map "),
        TEXT(" A / D      accelerate / decelerate "),
        TEXT("                                    "),};

    ashes::Console& console = ashes::GConsole();
    console.DrawBlank(console.Region(), space_color_);
    console.DrawRectInnerBorder(console.Region());
    console.DrawFramedStrings(kTexts, console.Region(), text_color_);
}

void GameRenderer::DrawGaming(const GameCore& game)
{
    ashes::Console& console = ashes::GConsole();
    console.DrawBlank(console.Region(), space_color_);
    console.DrawRectInnerBorder(console.Region());
    DrawSnake(game.GetSnake());
    DrawFood(game.GetFood());
}

void GameRenderer::DrawResult(const GameCore& game)
{
    static std::vector<ashes::String> texts = {
        TEXT("         Game Over            "),
        TEXT("                              "),
        TEXT(" SCORE   999 | 999 : 99 × 99 "),
        TEXT(" SPACE   start new game       "),
        TEXT(" ENTER   back to game guide   "),
        TEXT(" H       toggle message show  "),
        TEXT("                              "),};

    const int score = game.GetSnake().Length();
    const ashes::Coord& map_size = game.GetGameMap().Size();

    ashes::String& text2 = texts[2];
    text2.clear();
    text2.append(TEXT(" SCORE   "));
    text2.append(ashes::graph::ToHalfWidthDigit(score, 3));
    text2.append(TEXT(" | "));
    text2.append(ashes::graph::ToHalfWidthDigit(map_size.X * map_size.Y, 3));
    text2.append(TEXT(" : "));
    text2.append(ashes::graph::ToHalfWidthDigit(map_size.X, 2));
    text2.append(TEXT(" × "));
    text2.append(ashes::graph::ToHalfWidthDigit(map_size.Y, 2));
    text2.append(TEXT(" "));

    ashes::Console& console = ashes::GConsole();
    console.DrawFramedStrings(texts, console.Region(), text_color_);
}
