#include "Vertex.h"

Vertex::Vertex(Point &p)
{
    this->p = p;
    this->ConnectedEdges.reserve(2);
}

bool Vertex::CheckNeigborExist(Vertex &v)
{
    for (auto &&e : this->ConnectedEdges)
    {
        if(e.p1->p == v.p || e.p2->p == v.p)
        {
            return true;
        }
    }
    return false;
}

void Vertex::MakeEdge(Vertex &v1, Vertex &v2)
{
    if(v1.ConnectedEdges.size() >= 2 || v2.ConnectedEdges.size() >= 2)
    {
        return;
    }
    if(v1.CheckNeigborExist(v2) || v2.CheckNeigborExist(v1))
    {
        return;
    }
    v1.ConnectedEdges.emplace_back(&v1, &v2);
    v2.ConnectedEdges.emplace_back(&v2, &v1);
}
