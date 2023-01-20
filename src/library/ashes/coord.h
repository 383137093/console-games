#ifndef ASHES_COORD_H
#define ASHES_COORD_H

#include <cmath>
#include <windows.h>
#include "direction4.h"

namespace ashes {
    
struct Coord : COORD
{
    // Construction
    Coord() = default;
    Coord(const COORD& coord);
    Coord(SHORT x, SHORT y);

    // Modification
    Coord& operator += (const Coord& rhs);
    Coord& operator -= (const Coord& rhs);

    // Geometric relationship
    friend bool operator == (const Coord& lhs, const Coord& rhs);
    friend bool operator != (const Coord& lhs, const Coord& rhs);
    friend bool operator > (const Coord& lhs, const Coord& rhs);
    friend bool operator < (const Coord& lhs, const Coord& rhs);
    friend bool operator >= (const Coord& lhs, const Coord& rhs);
    friend bool operator <= (const Coord& lhs, const Coord& rhs);

    // Geometric operation
    friend Coord operator + (const Coord& lhs, const Coord& rhs);
    friend Coord operator - (const Coord& lhs, const Coord& rhs);
    friend Coord operator * (const Coord& lhs, int rhs);
    friend Coord operator / (const Coord& lhs, int rhs);
    static SHORT ManhattanDistance(const Coord& a, const Coord& b);
    Coord Adjacency(Direction4 dir) const;
};

inline Coord::Coord(const COORD& coord)
    : COORD(coord)
{
}

inline Coord::Coord(SHORT x, SHORT y) 
    : COORD({x, y})
{
}

inline Coord& Coord::operator += (const Coord& rhs)
{
    X += rhs.X;
    Y += rhs.Y;
    return *this;
}

inline Coord& Coord::operator -= (const Coord& rhs)
{
    X -= rhs.X;
    Y -= rhs.Y;
    return *this;
}

inline bool operator == (const Coord& lhs, const Coord& rhs)
{
    return lhs.X == rhs.X && lhs.Y == rhs.Y;
}

inline bool operator != (const Coord& lhs, const Coord& rhs)
{
    return !(lhs == rhs);
}

inline bool operator > (const Coord& lhs, const Coord& rhs)
{
    return lhs.X > rhs.X && lhs.Y > rhs.Y;
}

inline bool operator < (const Coord& lhs, const Coord& rhs)
{
    return lhs.X < rhs.X && lhs.Y < rhs.Y;
}

inline bool operator >= (const Coord& lhs, const Coord& rhs)
{
    return lhs.X >= rhs.X && lhs.Y >= rhs.Y;
}

inline bool operator <= (const Coord& lhs, const Coord& rhs)
{
    return lhs.X <= rhs.X && lhs.Y <= rhs.Y;
}

inline Coord operator + (const Coord& lhs, const Coord& rhs)
{
    return Coord(lhs) += rhs;
}

inline Coord operator - (const Coord& lhs, const Coord& rhs)
{
    return Coord(lhs) -= rhs;
}

inline Coord operator * (const Coord& lhs, int rhs)
{
    return {static_cast<SHORT>(lhs.X * rhs), static_cast<SHORT>(lhs.Y * rhs)};
}

inline Coord operator / (const Coord& lhs, int rhs)
{
    return {static_cast<SHORT>(lhs.X / rhs), static_cast<SHORT>(lhs.Y / rhs)};
}

inline SHORT Coord::ManhattanDistance(const Coord& a, const Coord& b)
{
    return static_cast<SHORT>(std::abs(a.X - b.X) + std::abs(a.Y - b.Y));
}

inline Coord Coord::Adjacency(Direction4 dir) const
{
    static const Coord kOffset[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
    return *this + kOffset[direction4::ToInt(dir)];
}

}

#endif