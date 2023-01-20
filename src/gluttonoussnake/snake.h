#ifndef GLUTTONOUSSNAKE_SNAKE_H
#define GLUTTONOUSSNAKE_SNAKE_H

#include <deque>
#include "ashes/coord.h"
#include "ashes/direction4.h"

class Snake
{
public:

    struct BodyNode 
    { 
        ashes::Coord      pos;
        ashes::Direction4 dir;
    };

    Snake();
    ~Snake();

    const BodyNode& Head() const;
    const BodyNode& Tail() const;
    const BodyNode& Node(int index) const;
    int Length() const;

    ashes::Coord GetForwardCoord() const;
    bool SetForwardDirection(ashes::Direction4 dir);
    void MoveForward();
    void GrowForward();
    
    bool Validate() const;
    void Reset(const BodyNode& head);

private:

    std::deque<BodyNode> body_;
};

#endif