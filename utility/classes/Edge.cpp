#include "Vertex.h"
#include "Edge.h"

#include <math.h>

Edge::Edge(Vertex *p1, Vertex *p2)
{
    this->p1 = p1;
    this->p2 = p2;
    this->weight = sqrt(this->DistanceSquare());
}

int Edge::DistanceSquare()
{
    const int deltaX = p1->p.x - p2->p.x;
    const int deltaY = p1->p.y - p2->p.y;
    return deltaX * deltaX + deltaY * deltaY;
}
