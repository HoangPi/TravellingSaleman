#include "Edge.h"

#include "Vertex.h"
#include "cmath"

WeightedUndirectedEdge::WeightedUndirectedEdge(Vertex *v1, Vertex *v2)
{
    this->v1 = v1;
    this->v2 = v2;
    const double deltaX = v1->p.x - v2->p.x;
    const double deltaY = v1->p.y - v2->p.y;
    this->weight = std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
}
