#ifndef TETRIS_PLAYINGFIELD_H
#define TETRIS_PLAYINGFIELD_H

#include <array>
#include <bitset>
#include "ashes/coord.h"
#include "polyomino.h"

class PlayingField
{
public:
	
	static const SHORT kWidth = 10;
	static const SHORT kHeight = 20;
	static const ashes::Coord kSize;

	PlayingField();
	~PlayingField();

	static bool ValidateCoord(const ashes::Coord& coord);
	bool TestSquare(const ashes::Coord& coord) const;
	bool CanPlacePolyomino(const Polyomino& polyomino, bool allow_overflow) const;
	bool CanLandPolyomino(const Polyomino& polyomino) const;

	void PlacePolyomino(const Polyomino& polyomino);
	int RemoveCompletedLine();
	void Clear();

private:
	
	std::array<std::bitset<kWidth>, kHeight> squares_;
};

#endif