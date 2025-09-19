#pragma once

#include "Edge.h"
#include "Vertex.h"

class Graph
{
public:
    std::vector<Vertex> Vertices;
    std::vector<WeightedUndirectedEdge> Edges;

    Graph() {};
    bool IsValid();
    bool IsValid(Vertex *v);
    void AddVertex(cv::Point &p);
    void AddEdges(Vertex *v1, Vertex *v2);
};