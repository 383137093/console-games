#ifndef ASHES_RECT_H
#define ASHES_RECT_H

#include <algorithm>
#include <windows.h>
#include "coord.h"

namespace ashes {

struct Rect : SMALL_RECT
{
    // Construction
    Rect() = default;
    Rect(const SMALL_RECT& rect);
    Rect(const Coord& topleft, const Coord& size);
    Rect(SHORT left, SHORT top, SHORT right, SHORT bottom);

    // Modification
    Rect& OffsetHori(SHORT offset);
    Rect& OffsetVert(SHORT offset);
    Rect& Offset(const Coord& offset);
    Rect& ExtendHori(SHORT size);
    Rect& ExtendVert(SHORT size);
    Rect& Extend(const Coord& size);

    // Attributes
    Coord TopLeft() const;
    Coord BottomRight() const;
    SHORT Width() const;
    SHORT Height() const;
    Coord Size() const;
    bool IsEmpty() const;

    // Geometric relationship
    friend bool operator == (const Rect& lhs, const Rect& rhs);
    friend bool operator != (const Rect& lhs, const Rect& rhs);
    bool Contain(const Coord& point) const;
    bool Contain(const Rect& rect) const;
    bool IntersectsWith(const Rect& rect) const;

    // Geometric operation
    static Rect Union(const Rect& a, const Rect& b);
    static Rect Intersect(const Rect& a, const Rect& b);
};

inline Rect::Rect(const SMALL_RECT& rect)
    : SMALL_RECT(rect)
{
}

inline Rect::Rect(const Coord& point, const Coord& size)
    : SMALL_RECT({point.X, point.Y, point.X + size.X - 1, point.Y + size.Y - 1})
{
}

inline Rect::Rect(SHORT left, SHORT top, SHORT right, SHORT bottom)
    : SMALL_RECT({left, top, right, bottom})
{
}

inline Rect& Rect::OffsetHori(SHORT offset)
{
    Left  += offset;
    Right += offset;
    return *this;
}

inline Rect& Rect::OffsetVert(SHORT offset)
{
    Top    += offset;
    Bottom += offset;
    return *this;
}

inline Rect& Rect::Offset(const Coord& offset)
{
    OffsetHori(offset.X);    
    OffsetVert(offset.Y);
    return *this;
}

inline Rect& Rect::ExtendHori(SHORT size)
{
    Left  -= size;
    Right += size;
    return *this;
}

inline Rect& Rect::ExtendVert(SHORT size)
{
    Top    -= size;
    Bottom += size;
    return *this;
}

inline Rect& Rect::Extend(const Coord& size)
{
    ExtendHori(size.X);
    ExtendVert(size.Y);
    return *this;
}

inline Coord Rect::TopLeft() const
{
    return Coord(Left, Top);
}

inline Coord Rect::BottomRight() const
{
    return Coord(Right, Bottom);
}

inline SHORT Rect::Width() const
{
    return Right - Left + 1;
}

inline SHORT Rect::Height() const
{
    return Bottom - Top + 1;
}

inline Coord Rect::Size() const
{
    return Coord(Width(), Height());
}

inline bool Rect::IsEmpty() const
{
    return Width() <= 0 || Height() <= 0;
}

inline bool operator == (const Rect& lhs, const Rect& rhs)
{
    return lhs.Left  == rhs.Left  && lhs.Top    == rhs.Top
        && lhs.Right == rhs.Right && lhs.Bottom == rhs.Bottom;
}

inline bool operator != (const Rect& lhs, const Rect& rhs)
{
    return !(lhs == rhs);
}

inline bool Rect::Contain(const Coord& point) const
{
    return Left <= point.X && point.X <= Right
        && Top  <= point.Y && point.Y <= Bottom;
}

inline bool Rect::Contain(const Rect& rect) const
{
    return Left <= rect.Left && rect.Right  <= Right
        && Top  <= rect.Top  && rect.Bottom <= Bottom;
}

inline bool Rect::IntersectsWith(const Rect& rect) const
{
    return Left <= rect.Right  && Right  >= rect.Left
        && Top  <= rect.Bottom && Bottom >= rect.Top;
}

inline Rect Rect::Union(const Rect& a, const Rect& b)
{
    return {(std::min)(a.Left,  b.Left),  (std::min)(a.Top,    b.Top),
            (std::max)(a.Right, b.Right), (std::max)(a.Bottom, b.Bottom)};
}

inline Rect Rect::Intersect(const Rect& a, const Rect& b)
{
    return {(std::max)(a.Left,  b.Left),  (std::max)(a.Top,    b.Top),
            (std::min)(a.Right, b.Right), (std::min)(a.Bottom, b.Bottom)};
}

}

#endif