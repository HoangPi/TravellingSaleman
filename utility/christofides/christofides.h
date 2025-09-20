#pragma once

#include <opencv2/opencv.hpp>
#include "../classes/Graph.h"
#include <unordered_set>
#include <memory>
#include <vector>

struct SimpleEdge
{
    class Vertex *v1 = nullptr;
    class Vertex *v2 = nullptr;
    long long Distance = -1;
};

class EulerNode
{
public:
    Vertex *Node;
    int visited;
    EulerNode() : Node(nullptr), visited(-1) {};
    EulerNode(Vertex *v) : Node(v), visited(-1) {};
    EulerNode(Vertex *v, int visit) : Node(v), visited(visit) {};
    void AddEdge() { this->visited++; }
    WeightedUndirectedEdge *NextEdge()
    {
        if (visited < Node->ConnectedEdges.size() - 1)
        {
            return Node->ConnectedEdges[++visited];
        }
        return nullptr;
    }
};

extern cv::Mat ChristofidesCanvas;
extern const char *const ChristofidesWindowName;

void ChristofidesSolve(Graph &graph);
void ConnectVertices(Graph &graph);
void PrimMST(Graph &graph);
bool HasVisited(const Vertex *VertexToCheck, const std::vector<bool> &Visited, const Vertex *Begin);
inline int FindIndex(const Vertex *Target, const Vertex *Base) { return Target - Base; }
void MakeEulerCircuit(Graph &graph);
void EulerTravel(Graph &ChristofideGraph, Graph &Result);