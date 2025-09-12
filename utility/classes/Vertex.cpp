#include "Vertex.h"

Vertex::Vertex(cv::Point &p)
{
    this->p = p;
}

int64 Vertex::DistanceSquare(Vertex &v1, Vertex &v2)
{
    const long long deltaX = v1.p.x - v2.p.x;
    const long long deltaY = v1.p.y - v2.p.y;
    return (deltaX * deltaX) + (deltaY * deltaY);
}
