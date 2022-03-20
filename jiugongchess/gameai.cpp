#include "gameai.h"

#include <string>
#include <cassert>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "ashes/math.h"
#include "ashes/winapi.h"
#include "gamesolver.h"
#include "res/resource.h"

GameAI::GameAI(Chessboard::PlayerSide role)
	: role_(role)
{
}

GameAI::~GameAI()
{
}

void GameAI::Initialize()
{
	GetDatabse();
}

void GameAI::SetIQ(float min, float max)
{
	iq_min_ = min;
	iq_max_ = max;
}

Chessboard::PieceStep GameAI::HandleGameTurn(const Chessboard& chessboard) const
{
	U32GameSituation key = GameSolver::CompressSituation(chessboard, role_);
	assert(GetDatabse().count(key) == 1);
	const std::vector<U32PieceStep>& steps = GetDatabse().find(key)->second;
	assert(!steps.empty());

	int index_max = static_cast<int>(steps.size()) - 1;
	float iq = ashes::math::Lerp(iq_min_, iq_max_, ashes::random::Uniform());
	int index = ashes::math::Lerp(index_max, 0, iq);
	
	index = ashes::math::Clamp(index, 0, index_max);
	Chessboard::PieceStep step = GameSolver::DeCompressPieceStep(steps[index]);
	assert(chessboard.CanMovePiece(step));

	return step;
}

GameAI::Database GameAI::LoadDatabase()
{
	const char* name = MAKEINTRESOURCEA(IDR_DAT_GAMEAI_DATABASE);
	const std::string resource = ashes::win::LoadModuleResource(name, "dat");
	assert(!resource.empty());
	Database database;

	for (std::stringstream stream(resource); stream.good();)
	{
		std::istream_iterator<int> iter(stream);
		
		int chessboard = *iter++;
		int num_steps = *iter++;
		assert(database.count(chessboard) == 0);
		assert(num_steps > 0);

		auto& steps = database[chessboard];
		steps.resize(num_steps);
		std::copy_n(iter, num_steps, steps.begin());
	}

	assert(ValidateDatabase(database));
	return database;
}

bool GameAI::ValidateDatabase(const Database& database)
{
	for (const auto& row : database)
	{
		const auto situation = GameSolver::DeCompressSituation(row.first);
		for (U32PieceStep u32_step : row.second)
		{
			Chessboard::PieceStep step = GameSolver::DeCompressPieceStep(u32_step);
			if (!situation.first.CanMovePiece(situation.second, step))
				return false;
		}
	}
	return true;
}

const GameAI::Database& GameAI::GetDatabse()
{
	static const Database kDatabase = LoadDatabase();
	return kDatabase;
}
