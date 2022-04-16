#include "gamecore.h"
#include <algorithm>
#include "ashes/rect.h"

using namespace std::chrono_literals;

const std::vector<ashes::time::Msecs> GameCore::kDescendCooldowns 
	= {500ms, 450ms, 400ms, 350ms, 300ms, 250ms, 200ms, 150ms};

const std::vector<int> GameCore::kScoreCoefficients = {0, 100, 200, 400, 800};

GameCore::GameCore()
{
}

GameCore::~GameCore()
{
}

bool GameCore::IsGameOVer() const
{
	return game_over_;
}

const PlayingField& GameCore::GetPlayingField() const
{
	return field_;
}

const Polyomino& GameCore::GetPolyomino() const
{
	return polyomino_;
}

const Polyomino& GameCore::GetNextPolyomino() const
{
	return next_polyomino_;
}

int GameCore::GetDifficultyLevel() const
{
	return (std::min)(score_ / 1500 + 1, static_cast<int>(kDescendCooldowns.size()));
}

int GameCore::GetScore() const
{
	return score_;
}

void GameCore::ShiftPolyominoL()
{
	if (!game_over_)
	{
		Polyomino new_polyomino = polyomino_;
		new_polyomino.Offset({-1, 0});
		TryTransformCurrentPolyomino(new_polyomino);
	}
}

void GameCore::ShiftPolyominoR()
{
	if (!game_over_)
	{
		Polyomino new_polyomino = polyomino_;
		new_polyomino.Offset({1, 0});
		TryTransformCurrentPolyomino(new_polyomino);
	}
}

void GameCore::RotatePolyomino()
{
	if (!game_over_)
	{
		Polyomino new_polyomino = polyomino_;
		new_polyomino.RotateCW();
		TryTransformCurrentPolyomino(new_polyomino);
	}
}

void GameCore::DescendPolyomino(int max_height)
{
	if (!game_over_ && max_height > 0)
	{
		while (max_height--)
		{
			if (field_.CanLandPolyomino(polyomino_))
			{
				if (field_.CanPlacePolyomino(polyomino_, false))
				{
					PlaceCurrentPolyomino();
					break;
				}
				else
				{
					game_over_ = true;
					OnPolyominoPlaced(-1);
					break;
				}
			}
			else
			{
				Polyomino new_polyomino = polyomino_;
				new_polyomino.Offset({0, 1});
				TryTransformCurrentPolyomino(new_polyomino);
			}
		}
	}
}

void GameCore::StartGame()
{
	game_over_ = false;
	field_.Clear();
	polyomino_ = SpawnNewPolyomino();
	next_polyomino_ = SpawnNewPolyomino();
	descend_speed_level_ = 0;
	descend_timing_ = {};
	score_ = 0;
}

void GameCore::TickGame()
{
	if (!game_over_)
	{
		ashes::time::TimePoint now = ashes::time::Now();

		if (now >= descend_timing_)
		{
			DescendPolyomino(1);
			descend_timing_ = now + kDescendCooldowns[descend_speed_level_];
		}
	}
}

Polyomino GameCore::SpawnNewPolyomino()
{
	Polyomino polyomino;
	ashes::Rect bounding = polyomino.Bounding();
	SHORT x = (PlayingField::kWidth - bounding.Width()) / 2;
	SHORT y = -bounding.Height();
	polyomino.SetTopLeft({x, y});
	return polyomino;
}

void GameCore::TryTransformCurrentPolyomino(const Polyomino& polyomino)
{
	assert(!game_over_);

	if (field_.CanPlacePolyomino(polyomino, true))
	{
		Polyomino original_polyomino = polyomino_;
		polyomino_ = polyomino;
		OnPolyominoTransformed(original_polyomino);
	}
}

void GameCore::PlaceCurrentPolyomino()
{
	assert(!game_over_);
	assert(field_.CanPlacePolyomino(polyomino_, false));

	field_.PlacePolyomino(polyomino_);
	int num_completed_lines = field_.RemoveCompletedLine();
	polyomino_ = next_polyomino_;
	next_polyomino_ = SpawnNewPolyomino();
	score_ += kScoreCoefficients[num_completed_lines];

	OnPolyominoPlaced(num_completed_lines);
}