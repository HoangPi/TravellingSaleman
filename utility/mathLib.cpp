#include "mathLib.h"

int DistanceSquare(Point a, Point b)
{
    const int deltaX = (a.x - b.x);
    const int deltaY = (a.y - b.y);
    return deltaX * deltaX + deltaY * deltaY;
}