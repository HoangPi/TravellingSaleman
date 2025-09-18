#include "christofides.h"

void ChristofidesSolve(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &result)
{
    const size_t size = vertices.size();
    result.clear();
    result.reserve(vertices.size());
    std::vector<WeightedUndirectedEdge> edges;
    ConnectVertices(vertices, edges);
}

void ConnectVertices(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &edges)
{
    for (size_t i = 0; i < vertices.size(); i++)
    {
        for (size_t j = i + 1; j < vertices.size(); j++)
        {
            edges.emplace_back(&vertices[i], &vertices[j]);
        }
    }
}
