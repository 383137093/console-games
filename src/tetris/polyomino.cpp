#include "polyomino.h"
#include "polyominoshapepool.h"

bool Polyomino::Shape::ValidateCoord(const ashes::Coord& coord)
{
    return (0 <= coord.X && coord.X < kMaxEdge)
        && (0 <= coord.Y && coord.Y < kMaxEdge);
}

int Polyomino::Shape::CoordToIndex(const ashes::Coord& coord)
{
    return coord.Y * kMaxEdge + coord.X;
}

ashes::Coord Polyomino::Shape::IndexToCoord(int index)
{
    return {static_cast<SHORT>(index % kMaxEdge),
            static_cast<SHORT>(index / kMaxEdge)};
}

Polyomino::Polyomino()
{
    shape_ = &PolyominoShapePool::Instance().RandShape();
}

Polyomino::~Polyomino()
{
}

const ashes::Coord& Polyomino::TopLeft() const
{
    return topleft_;
}

ashes::Rect Polyomino::Bounding() const
{
    ashes::Rect bounding = shape_->bounding;
    bounding.Offset(topleft_);
    return bounding;
}

bool Polyomino::TestSquare(const ashes::Coord& coord) const
{
    ashes::Coord local_coord = coord - topleft_;
    if (!Shape::ValidateCoord(local_coord))
        return false;

    std::size_t index = Shape::CoordToIndex(local_coord);
    if (!shape_->bitmap.test(index))
        return false;

    return true;
}

void Polyomino::SetTopLeft(const ashes::Coord& coord)
{
    topleft_ = coord;
}

void Polyomino::Offset(const ashes::Coord& offset)
{
    topleft_ += offset;
}

void Polyomino::RotateCW()
{
    shape_ = &PolyominoShapePool::Instance().RotateShapeCW(*shape_);
}

void Polyomino::RotateCCW()
{
    shape_ = &PolyominoShapePool::Instance().RotateShapeCCW(*shape_);
}