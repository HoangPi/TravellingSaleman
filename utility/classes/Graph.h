#pragma once

#include "Edge.h"
#include "Vertex.h"

class Graph
{
    std::vector<Vertex> Vertices;
    std::vector<WeightedUndirectedEdge> Edges;
};