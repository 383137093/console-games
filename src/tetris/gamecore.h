#ifndef TETRIS_GAMECORE_H
#define TETRIS_GAMECORE_H

#include <vector>
#include "ashes/time.h"
#include "ashes/eventdispatcher.h"
#include "polyomino.h"
#include "playingfield.h"

class GameCore
{

public:

	GameCore();
	GameCore(const GameCore&) = delete;
	~GameCore();
	GameCore& operator = (const GameCore&) = delete;

	bool IsGameOVer() const;
	const PlayingField& GetPlayingField() const;
	const Polyomino& GetPolyomino() const;
	const Polyomino& GetNextPolyomino() const;
	int GetDifficultyLevel() const;
	int GetScore() const;

	void ShiftPolyominoL();
	void ShiftPolyominoR();
	void RotatePolyomino();
	void DescendPolyomino(int max_height);

	void StartGame();
	void TickGame();

	ashes::EventDispatcher<void(const Polyomino&)> OnPolyominoTransformed;
	ashes::EventDispatcher<void(int)>              OnPolyominoPlaced;

private:
	
	static const std::vector<ashes::time::Msecs> kDescendCooldowns;
	static const std::vector<int>                kScoreCoefficients;

	static Polyomino SpawnNewPolyomino();
	void TryTransformCurrentPolyomino(const Polyomino& polyomino);
	void PlaceCurrentPolyomino();

private:

	bool                   game_over_ = true;
	PlayingField           field_;
	Polyomino              polyomino_;
	Polyomino              next_polyomino_;
	int                    descend_speed_level_ = 0;
	ashes::time::TimePoint descend_timing_;
	int                    score_ = 0;
};

#endif
