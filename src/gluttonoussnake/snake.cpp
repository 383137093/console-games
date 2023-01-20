#include "snake.h"
#include <cassert>
#include <algorithm>

Snake::Snake()
{
    Reset({{0, 0}, ashes::Direction4::Forward});
}

Snake::~Snake()
{
}

const Snake::BodyNode& Snake::Head() const
{
    return body_.front();
}

const Snake::BodyNode& Snake::Tail() const
{
    return body_.back();
}

const Snake::BodyNode& Snake::Node(int index) const
{
    assert(0 <= index && index < Length());
    return body_[index];
}

int Snake::Length() const
{
    return static_cast<int>(body_.size());
}

ashes::Coord Snake::GetForwardCoord() const
{
    return Head().pos.Adjacency(Head().dir);
}

bool Snake::SetForwardDirection(ashes::Direction4 dir)
{
    if (body_.size() <= 1 || dir != ashes::direction4::NegativeOf(body_[1].dir))
    {
        body_[0].dir = dir;
        return true;
    }
    return false;
}

void Snake::MoveForward()
{
    GrowForward();
    body_.pop_back();
}

void Snake::GrowForward()
{
    body_.push_front({GetForwardCoord(), Head().dir});
    assert(Validate());
}

bool Snake::Validate() const
{
    auto iter = std::adjacent_find(body_.rbegin(), body_.rend(),
        [](const BodyNode& next, const BodyNode& prev) {
            return next.pos.Adjacency(next.dir) != prev.pos; });
    return iter == body_.rend();
}

void Snake::Reset(const BodyNode& head)
{
    body_.assign(1, head);
}