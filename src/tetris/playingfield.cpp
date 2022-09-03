#include "playingfield.h"
#include <cassert>
#include <algorithm>
#include "ashes/rect.h"

const ashes::Coord PlayingField::kSize = {kWidth, kHeight};

PlayingField::PlayingField()
{
}

PlayingField::~PlayingField()
{
}

bool PlayingField::ValidateCoord(const ashes::Coord& coord)
{
	return (0 <= coord.X && coord.X < kWidth)
		&& (0 <= coord.Y && coord.Y < kHeight);
}

bool PlayingField::TestSquare(const ashes::Coord& coord) const
{
	assert(ValidateCoord(coord));
	return squares_[coord.Y].test(coord.X);
}

bool PlayingField::CanPlacePolyomino(
	const Polyomino& polyomino,
	bool allow_overflow) const
{
	ashes::Rect bounding = polyomino.Bounding();
	bounding.Top = allow_overflow ? (std::max)(bounding.Top, SHORT(0)) : bounding.Top;

	if (!ashes::Rect({0, 0}, kSize).Contain(bounding))
		return false;

	for (SHORT y = bounding.Top; y <= bounding.Bottom; ++y)
	{
		for (SHORT x = bounding.Left; x <= bounding.Right; ++x)
		{
			if (polyomino.TestSquare({x, y}))
			{
				if (!ValidateCoord({x, y}))
					return false;

				if (TestSquare({x, y}))
					return false;
			}
		}
	}

	return true;
}

bool PlayingField::CanLandPolyomino(const Polyomino& polyomino) const
{
	const ashes::Rect bounding = polyomino.Bounding();

	for (SHORT y = bounding.Top; y <= bounding.Bottom; ++y)
	{
		for (SHORT x = bounding.Left; x <= bounding.Right; ++x)
		{
			if (polyomino.TestSquare({x, y}))
			{
				if (ValidateCoord({x, y}) && y + 1 == kHeight)
					return true;

				if (ValidateCoord({x, y + 1}) && TestSquare({x, y + 1}))
					return true;
			}
		}
	}

	return false;
}

void PlayingField::PlacePolyomino(const Polyomino& polyomino)
{
	const ashes::Rect bounding = polyomino.Bounding();

	for (SHORT y = bounding.Top; y <= bounding.Bottom; ++y)
	{
		for (SHORT x = bounding.Left; x <= bounding.Right; ++x)
		{
			if (polyomino.TestSquare({x, y}) && ValidateCoord({x, y}))
			{
				assert(!squares_[y].test(x));
				squares_[y].set(x);
			}
		}
	}
}

int PlayingField::RemoveCompletedLine()
{
	static const auto kCompletedLine = std::bitset<kWidth>().flip();

	auto iter = std::remove(squares_.rbegin(), squares_.rend(), kCompletedLine);
	std::fill(iter, squares_.rend(), 0);
	int num_removed = static_cast<int>(squares_.rend() - iter);
	
	return num_removed;
}

void PlayingField::Clear()
{
	std::fill(squares_.begin(), squares_.end(), 0);
}
