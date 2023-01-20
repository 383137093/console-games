#ifndef ASHES_DIRECTION4_H
#define ASHES_DIRECTION4_H

#include <cstdint>
#include "random.h"

namespace ashes {

enum class Direction4 : std::uint8_t
{
    Leftward = 0,
    Forward = 1,
    Rightward = 2,
    Backward = 3,
};

namespace direction4 {

static const Direction4 kFullSet[] = {
    Direction4::Leftward,
    Direction4::Forward,
    Direction4::Rightward,
    Direction4::Backward};

inline Direction4 Rand()
{
    return static_cast<Direction4>(random::IntRange(0, 3));
}

inline int ToInt(Direction4 dir)
{
    return static_cast<int>(dir);
}

inline bool IsHori(Direction4 dir)
{
    return (ToInt(dir) & 1) == 0;
}

inline bool IsVert(Direction4 dir)
{
    return (ToInt(dir) & 1) == 1;
}

inline Direction4 NegativeOf(Direction4 dir)
{
    return static_cast<Direction4>((ToInt(dir) + 2) % 4);
}

inline Direction4 LeftSideOf(Direction4 dir)
{
    return static_cast<Direction4>((ToInt(dir) + 3) % 4);
}

inline Direction4 RightSideOf(Direction4 dir)
{
    return static_cast<Direction4>((ToInt(dir) + 1) % 4);
}

}}

#endif