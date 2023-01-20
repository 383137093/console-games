#include "gamecore.h"

using namespace std::chrono_literals;

const ashes::Coord GameCore::kGameMapSizeMin = {20, 12};

const ashes::Coord GameCore::kGameMapSizeMax = {32, 24};

const ashes::Coord GameCore::kGameMapSizeDef = {26, 18};

const std::vector<ashes::time::Msecs> GameCore::kSnakeMoveCooldowns = {
    505ms, 325ms, 185ms, 85ms, 25ms, 0ms};

GameCore::GameCore()
{
    game_map_.Resize(kGameMapSizeDef);
}

GameCore::~GameCore()
{
}

GameCore::GameState GameCore::GetGameState() const
{
    return game_state_;
}

const GameMap& GameCore::GetGameMap() const
{
    return game_map_;
}

const Snake& GameCore::GetSnake() const
{
    return snake_;
}

const ashes::Coord& GameCore::GetFood() const
{
    return food_;
}

bool GameCore::ExpandGameMap()
{
    if (game_state_ == GameState::Ready)
    {
        ashes::Coord new_size = game_map_.Size() + ashes::Coord(2, 2);

        if (new_size <= kGameMapSizeMax)
        {
            game_map_.Resize(new_size);
            return true;
        }
    }
    return false;
}

bool GameCore::ShrinkGameMap()
{
    if (game_state_ == GameState::Ready)
    {
        ashes::Coord new_size = game_map_.Size() - ashes::Coord(2, 2);

        if (new_size >= kGameMapSizeMin)
        {
            game_map_.Resize(new_size);
            return true;
        }
    }
    return false;
}

bool GameCore::AccelerateSnake()
{
    if (game_state_ == GameState::Ready ||
        game_state_ == GameState::UnderWay ||
        game_state_ == GameState::Pause)
    {
        if (snake_speed_level_ < static_cast<int>(kSnakeMoveCooldowns.size()) - 1)
        {
            snake_speed_level_ += 1;
            return true;
        }
    }
    return false;
}

bool GameCore::DecelerateSnake()
{
    if (game_state_ == GameState::Ready ||
        game_state_ == GameState::UnderWay ||
        game_state_ == GameState::Pause)
    {
        if (snake_speed_level_ > 0)
        {
            snake_speed_level_ -= 1;
            return true;
        }
    }
    return false;
}

bool GameCore::SetSnakeForwardDirection(ashes::Direction4 dir)
{
    return snake_.SetForwardDirection(dir);
}

void GameCore::ReadyGame()
{
    if (game_state_ != GameState::Ready)
    {
        game_state_ = GameState::Ready;
        game_map_.Clear();
        snake_.Reset({{0, 0}, ashes::Direction4::Forward});
        food_ = {0, 0};
        snake_speed_level_ = 0;
        snake_move_timing_ = ashes::time::TimePoint();
    }
}

void GameCore::StartGame()
{
    if (game_state_ == GameState::Ready)
    {
        game_state_ = GameState::UnderWay;
        PlaceSnakeOnCenterCoord();
        PlaceFoodOnRandomCoord();
    }
}

void GameCore::PauseGame(bool pause)
{
    if (pause && game_state_ == GameState::UnderWay)
    {
        game_state_ = GameState::Pause;
    }
    else if (!pause && game_state_ == GameState::Pause)
    {
        game_state_ = GameState::UnderWay;
    }
}

void GameCore::TickGame()
{
    if (game_state_ == GameState::UnderWay)
    {
        ashes::time::TimePoint now = ashes::time::Now();

        if (now >= snake_move_timing_)
        {
            ForwardMoveSnake();
            snake_move_timing_ = now + kSnakeMoveCooldowns[snake_speed_level_];
        }
    }
}

void GameCore::PlaceSnakeOnCenterCoord()
{
    ashes::Coord coord = {game_map_.Size().X / 2, game_map_.Size().Y / 2};
    snake_.Reset({coord, ashes::direction4::Rand()});
    game_map_.SetGridType(coord, GameMap::GridType::Snake);
}

void GameCore::PlaceFoodOnRandomCoord()
{
    food_ = game_map_.RandSpaceCoord();
    game_map_.SetGridType(food_, GameMap::GridType::Food);
}

void GameCore::ForwardMoveSnake()
{
    SnakeMoveEvent event = {snake_, SnakeMoveResult::Die, snake_.Tail()};
    GameMap::GridType grid = game_map_.GetGridType(snake_.GetForwardCoord());

    if (grid == GameMap::GridType::Space)
    {
        snake_.MoveForward();
        game_map_.SetGridType(event.tailprint.pos, GameMap::GridType::Space);
        game_map_.SetGridType(snake_.Head().pos, GameMap::GridType::Snake);
        event.result = SnakeMoveResult::Move;
    }
    else if (grid == GameMap::GridType::Food)
    {
        if (game_map_.NumSpaces() > 1)
        {
            snake_.GrowForward();
            game_map_.SetGridType(snake_.Head().pos, GameMap::GridType::Snake);
            PlaceFoodOnRandomCoord();
            event.result = SnakeMoveResult::Grow;
        }
        else
        {
            snake_.GrowForward();
            game_map_.SetGridType(snake_.Head().pos, GameMap::GridType::Snake);
            game_state_ = GameState::Win;
            event.result = SnakeMoveResult::Win;
        }
    }
    else
    {
        game_state_ = GameState::Over;
        event.result = SnakeMoveResult::Die;
    }

    OnSnakeMoved(event);
}