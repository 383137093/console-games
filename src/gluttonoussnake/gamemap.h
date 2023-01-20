#ifndef GLUTTONOUSSNAKE_GAMEMAP_H
#define GLUTTONOUSSNAKE_GAMEMAP_H

#include <vector>
#include "ashes/coord.h"

class GameMap
{
public:
    
    enum class GridType { Space, Snake, Food, Wall };

    GameMap();
    explicit GameMap(const ashes::Coord& size);
    ~GameMap();
    
    const ashes::Coord& Size() const;
    int NumGrids() const;
    int NumSpaces() const;
    void Resize(const ashes::Coord& size);

    bool ValidateCoord(const ashes::Coord& coord) const;
    int CoordToIndex(const ashes::Coord& coord) const;
    ashes::Coord IndexToCoord(int index) const;

    ashes::Coord RandSpaceCoord() const;
    GridType GetGridType(const ashes::Coord& coord) const;
    void SetGridType(const ashes::Coord& coord, GridType type);
    void Clear();

private:

    ashes::Coord          size_ = {0, 0};
    std::vector<GridType> grids_;
    int                   num_spaces_ = 0;
};

#endif