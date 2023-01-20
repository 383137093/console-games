#ifndef JIUGONGCHESS_GAMESOLVER_H
#define JIUGONGCHESS_GAMESOLVER_H

#include <map>
#include <iosfwd>
#include <vector>
#include <cstdint>
#include "chessboard.h"

//==============================================================================
// GameSituation
//==============================================================================

class GameSituation
{
public:

    enum class Type
    {
        Win,     // current player will win if he makes no mistakes.
        Draw,    // both sides player will draw if both them make no mistakes.
        Fail,    // current player will fail if his enemy makes no mistakes.
        Failed,  // current player is already fail at time.
    };

    struct Link
    {
        Chessboard::PieceStep step;            // if make the piece move,
        GameSituation*        next = nullptr;  // will get the situation.
    };

    struct Strategy
    {
        Chessboard::PieceStep step;       // the better the piece move is,
        int                   score = 0;  // the bigger the score get.
    };

public:

    // Special functions: uncopyable but moveable.
    GameSituation(const GameSituation&) = delete;
    GameSituation(GameSituation&&) = default;
    GameSituation(const Chessboard& chessboard, Chessboard::PlayerSide player);
    ~GameSituation();
    GameSituation& operator = (const GameSituation&) = delete;
    GameSituation& operator = (GameSituation&&) = default;

    // Basic infos.
    const Chessboard& GetChessboard() const;
    Chessboard::PlayerSide GetPlayer() const;
    Type GetType() const;

    // Situation link.
    std::vector<Strategy> GetSortedStrategys() const;
    void AddLink(const Chessboard::PieceStep& step, GameSituation* next);
    bool Update();

private:

    const Chessboard             chessboard_;
    const Chessboard::PlayerSide player_;

    Type type_ = Type::Draw;            // situation type after last update.
    int  end_steps_min_ = INT_MAX - 1;  // minimum number of win/fail steps.
    int  end_steps_max_ = INT_MIN + 1;  // maximum number of win/fail steps.

    std::vector<Link> win_links_;   // let we go to a win situation.
    std::vector<Link> draw_links_;  // let we go to a draw situation.
    std::vector<Link> fail_links_;  // let we go to a fail situation.
};


//==============================================================================
// GameSolver
// 1.brute-force search out all situations, build a situation graph.
//   at this time, only <failed> situation is solved, all others are temporarily 
//   regarded as <draw>.
// 2.update unsolved situations by their next situations, a <draw> situation
//   could be update to <win> or <fail>. if no situation can be update, then 
//   all situations is solved. 
//==============================================================================

class GameSolver
{
public:

    // Compress data to integer.
    static std::uint16_t CompressStrategy(const GameSituation::Strategy& strategy);
    static std::uint32_t CompressChessboard(const Chessboard& chessboard);
    static std::uint32_t CompressSituation(
        const Chessboard& chessboard, Chessboard::PlayerSide player);

    // DeCompress data from integer.
    static GameSituation::Strategy DeCompressStrategy(std::uint32_t bits);
    static Chessboard DeCompressChessboard(std::uint32_t bits);
    static std::pair<Chessboard, Chessboard::PlayerSide> DeCompressSituation(
        std::uint32_t bits);

    // Special functions.
    GameSolver();
    GameSolver(const GameSolver&) = delete;
    ~GameSolver();
    GameSolver& operator = (const GameSolver&) = delete;

    // Solve & Print
    void Print(std::ostream& os) const;
    void BruteForceSolve();

private:

    std::pair<GameSituation*, bool> FindOrAddSituation(
        const Chessboard& chessboard, Chessboard::PlayerSide player);

    void BruteForceSearch();
    void BruteForceUpdate();

    std::map<std::uint32_t, GameSituation> situations_;
};

#endif