#include "gamemap.h"
#include <cassert>
#include <algorithm>
#include "ashes/random.h"

GameMap::GameMap()
{
}

GameMap::GameMap(const ashes::Coord& size)
{
    Resize(size);
}

GameMap::~GameMap()
{
}

const ashes::Coord& GameMap::Size() const
{
    return size_;
}

int GameMap::NumGrids() const
{
    return static_cast<int>(grids_.size());
}

int GameMap::NumSpaces() const
{
    return num_spaces_;
}

void GameMap::Resize(const ashes::Coord& size)
{
    int num_grids = size.X * size.Y;
    size_ = size;
    grids_.assign(num_grids, GridType::Space);
    num_spaces_ = num_grids;
}

bool GameMap::ValidateCoord(const ashes::Coord& coord) const
{
    return (0 <= coord.X && coord.X < size_.X)
        && (0 <= coord.Y && coord.Y < size_.Y);
}

int GameMap::CoordToIndex(const ashes::Coord& coord) const
{
    return coord.X + coord.Y * size_.X;
}

ashes::Coord GameMap::IndexToCoord(int index) const
{
    return {static_cast<SHORT>(index % size_.X), 
            static_cast<SHORT>(index / size_.X)};
}

ashes::Coord GameMap::RandSpaceCoord() const
{
    int num_remain_spaces = num_spaces_;
    int index = -1;

    while (++index < NumGrids() && num_remain_spaces > 0)
    {
        if (grids_[index] == GridType::Space)
        {
            if (ashes::random::Probability(1.0f / num_remain_spaces))
                break;
            --num_remain_spaces;
        }
    }

    return IndexToCoord(index);
}

GameMap::GridType GameMap::GetGridType(const ashes::Coord& coord) const
{
    return ValidateCoord(coord) ? grids_[CoordToIndex(coord)] : GridType::Wall;
}

void GameMap::SetGridType(const ashes::Coord& coord, GridType type)
{
    assert(ValidateCoord(coord));
    assert(type != GridType::Wall);

    int index = CoordToIndex(coord);
    num_spaces_ -= (grids_[index] == GridType::Space);
    num_spaces_ += (type == GridType::Space);

    grids_[index] = type;
}

void GameMap::Clear()
{
    std::fill(grids_.begin(), grids_.end(), GridType::Space);
    num_spaces_ = size_.X * size_.Y;
}
