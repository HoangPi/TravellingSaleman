#include "Graph.h"

Graph::Graph(std::vector<Vertex> &Vertices, std::vector<WeightedUndirectedEdge> &Edges)
{
    const Vertex *oldBase = &Vertices[0];
    auto FindIndex = [oldBase](Vertex *v)
    { return (v - oldBase); };
    this->Vertices = Vertices;
    for (auto &&oldEdge : Edges)
    {
        int v1Index = FindIndex(oldEdge.v1);
        int v2Index = FindIndex(oldEdge.v2);
        this->Edges.emplace_back(&this->Vertices[v1Index], &this->Vertices[v2Index]);
    }
    if (!IsValid())
    {
        exit(-1);
    }
}

bool Graph::IsValid()
{
    for (const auto &edge : Edges)
    {
        if (!IsValid(edge.v1) || !IsValid(edge.v2))
            return false;
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
