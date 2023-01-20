#include "jiugongchess.h"

JiugongChess::JiugongChess()
{
}

JiugongChess::~JiugongChess()
{
}

void JiugongChess::NewGame()
{
    if (!game_record_.empty())
    {
        game_state_ = GameState::UnderWay;
        chessboard_.Reset();
        game_record_ = GameRecord();
        OnNewTurn();
    }
}

bool JiugongChess::MovePiece(const Chessboard::PieceStep& step)
{
    if (game_state_ != GameState::UnderWay)
        return false;

    if (!chessboard_.CanMovePiece(GetCurrentPlayer(), step))
        return false;

    chessboard_.MovePiece(step);
    game_record_.push(step);
    UpdateGameState();
    OnNewTurn();

    return true;
}

bool JiugongChess::BackMove(Chessboard::PlayerSide player)
{
    if (game_state_ != GameState::UnderWay)
        return false;

    int num_back_steps = (player == GetCurrentPlayer() ? 2 : 1);
    if (GetNumSteps() < num_back_steps)
        return false;

    while (num_back_steps--)
    {
        const Chessboard::PieceStep& step = game_record_.top();
        chessboard_.MovePiece({step.second, step.first});
        game_record_.pop();
    }

    OnNewTurn();
    return true;
}

JiugongChess::GameState JiugongChess::GetGameState() const
{
    return game_state_;
}

const Chessboard& JiugongChess::GetChessboard() const
{
    return chessboard_;
}

const JiugongChess::GameRecord& JiugongChess::GetGameRecord() const
{
    return game_record_;
}

int JiugongChess::GetNumSteps() const
{
    return static_cast<int>(game_record_.size());
}

int JiugongChess::GetNumRounds() const
{
    return GetNumSteps() / 2 + 1;
}

Chessboard::PlayerSide JiugongChess::GetCurrentPlayer() const
{
    return GetNumSteps() % 2 == 0 
        ? Chessboard::PlayerSide::Sente
        : Chessboard::PlayerSide::Gote;
}

void JiugongChess::UpdateGameState()
{
    Chessboard::PlayerSide player = Chessboard::EnemyOfPlayer(GetCurrentPlayer());

    if (chessboard_.IsPlayerWin(player))
    {
        game_state_ = (player == Chessboard::PlayerSide::Sente
            ? GameState::SenteWin : GameState::GoteWin);
    }
    else
    {
        game_state_ = (GetNumRounds() >= kNumMaxRounds
            ? GameState::Draw : GameState::UnderWay);
    }
}
