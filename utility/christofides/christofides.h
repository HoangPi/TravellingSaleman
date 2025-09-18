#pragma once

#include <opencv2/opencv.hpp>
#include "../classes/Edge.h"
#include "../classes/Vertex.h"
#include <memory>
#include <vector>

struct SimpleEdge
{
    class Vertex *v1 = nullptr;
    class Vertex *v2 = nullptr;
    long long Distance = -1;
};

struct SimpleVertexClone
{
    class std::shared_ptr<Vertex> Current = nullptr;
    class Vertex *Origin = nullptr;
};

extern cv::Mat ChristofidesCanvas;
extern const char *const ChristofidesWindowName;

void ChristofidesSolve(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &result);
void ConnectVertices(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &edges);
void PrimMST(std::vector<Vertex> &FullyConntectedVertices, std::vector<Vertex> &OriginVertices, std::vector<WeightedUndirectedEdge> &MST_Edges);
bool HasVisited(const Vertex *VertexToCheck, const std::vector<bool> &Visited, const Vertex *Begin);
inline int FindIndex(const Vertex *Target, const Vertex *Base) { return Target - Base; }
void FindOddDegreeVertices(std::vector<Vertex> &MST_Vertices, std::vector<WeightedUndirectedEdge> &MST_Edges);