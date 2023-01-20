#ifndef ASHES_JIUGONGCHESS_JIUGONGCHESS_H
#define ASHES_JIUGONGCHESS_JIUGONGCHESS_H

#include <stack>
#include "ashes/eventdispatcher.h"
#include "chessboard.h"

class JiugongChess
{
public:

    //==========================================================================
    // Constants & Types
    //==========================================================================

    static const int kNumMaxRounds = 999;

    enum class GameState { UnderWay, Draw, SenteWin, GoteWin };

    typedef std::stack<Chessboard::PieceStep> GameRecord;

    //==========================================================================
    // Play game
    //==========================================================================

    JiugongChess();
    JiugongChess(const JiugongChess&) = delete;
    ~JiugongChess();
    JiugongChess& operator = (const JiugongChess&) = delete;

    void NewGame();
    bool MovePiece(const Chessboard::PieceStep& step);
    bool BackMove(Chessboard::PlayerSide player);

    //==========================================================================
    // Game info
    //==========================================================================

    GameState GetGameState() const;
    const Chessboard& GetChessboard() const;
    const GameRecord& GetGameRecord() const;

    int GetNumSteps() const;
    int GetNumRounds() const;
    Chessboard::PlayerSide GetCurrentPlayer() const;

    ashes::EventDispatcher<void()> OnNewTurn;

private:

    //==========================================================================
    // Privates
    //==========================================================================

    void UpdateGameState();

    GameState  game_state_ = GameState::UnderWay;
    Chessboard chessboard_;
    GameRecord game_record_;
};

#endif