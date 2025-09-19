#include "Graph.h"

bool Graph::IsValid()
{
    size_t begin = (size_t)(&this->Vertices[0]);
    size_t end = (size_t)(&this->Vertices.back());
    for (auto &&edge : this->Edges)
    {
        const size_t v1 = (size_t)edge.v1;
        const size_t v2 = (size_t)edge.v2;
        if (v1 > end || v1 < begin || v2 > end || v2 < begin)
        {
            return false;
        }
    }
    return true;
}

bool Graph::IsValid(Vertex *v)
{
    return !this->Vertices.empty() && (&this->Vertices.back() >= v) && (&this->Vertices[0] <= v);
}

void Graph::AddVertex(cv::Point &p)
{
    this->Vertices.emplace_back(p);
}

void Graph::AddEdges(Vertex *v1, Vertex *v2)
{
    if (this->IsValid(v1) && this->IsValid(v2))
    {
        this->Edges.emplace_back(v1, v2);
    }
}
