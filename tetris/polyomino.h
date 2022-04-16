#ifndef TETRIS_POLYOMINO_H
#define TETRIS_POLYOMINO_H

#include <bitset>
#include "ashes/rect.h"
#include "ashes/coord.h"

class Polyomino
{
	friend class PolyominoShapePool;

public:

	Polyomino();
	~Polyomino();
	
	const ashes::Coord& TopLeft() const;
	ashes::Rect Bounding() const;
	bool TestSquare(const ashes::Coord& coord) const;

	void SetTopLeft(const ashes::Coord& coord);
	void Offset(const ashes::Coord& offset);
	void RotateCW();
	void RotateCCW();

private:

	struct Shape
	{
		static const int kMaxEdge = 4;

		static bool ValidateCoord(const ashes::Coord& coord);
		static int CoordToIndex(const ashes::Coord& coord);
		static ashes::Coord IndexToCoord(int index);

		std::bitset<kMaxEdge * kMaxEdge> bitmap;
		ashes::Rect                      bounding;
	};

	const Shape* shape_ = nullptr;
	ashes::Coord topleft_ = {0, 0};
};

#endif