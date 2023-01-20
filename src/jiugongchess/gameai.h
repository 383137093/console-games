#ifndef JIUGONGCHESS_GAMEAI_H
#define JIUGONGCHESS_GAMEAI_H

#include <vector>
#include <cstdint>
#include <unordered_map>
#include "chessboard.h"

class GameAI
{
public:

    GameAI(const GameAI&) = delete;
    explicit GameAI(Chessboard::PlayerSide role);
    ~GameAI();
    GameAI& operator = (const GameAI&) = delete;

    static void Initialize();
    void SetIQ(int min, int max);
    Chessboard::PieceStep HandleGameTurn(const Chessboard& chessboard) const;

private:

    typedef std::uint32_t U32GameSituation;
    typedef std::uint32_t U32Strategy;
    typedef std::unordered_map<U32GameSituation, std::vector<U32Strategy>> Database;

    static Database LoadDatabase();
    static bool ValidateDatabase(const Database& database);
    static const Database& GetDatabse();

    Chessboard::PlayerSide role_;
    int iq_min_ = 0;
    int iq_max_ = 0;
};

#endif
