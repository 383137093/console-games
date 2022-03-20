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
	void SetIQ(float min, float max);
	Chessboard::PieceStep HandleGameTurn(const Chessboard& chessboard) const;

private:

	typedef std::uint32_t U32GameSituation;
	typedef std::uint32_t U32PieceStep;
	typedef std::unordered_map<U32GameSituation, std::vector<U32PieceStep>> Database;

	static Database LoadDatabase();
	static bool ValidateDatabase(const Database& database);
	static const Database& GetDatabse();

	Chessboard::PlayerSide role_;
	float iq_min_ = 0.0f;
	float iq_max_ = 0.0f;
};

#endif
