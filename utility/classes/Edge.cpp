#include "Edge.h"

Edge::Edge(Point p1, Point p2)
{
    this->p1 = p1;
    this->p2 = p2;
}

int Edge::DistanceSquare()
{
    const int deltaX = p1.x = p2.x;
    const int deltaY = p1.y = p2.y;
    return deltaX * deltaX + deltaY * deltaY;
}
