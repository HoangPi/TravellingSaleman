#include "christofides.h"
#include "../CanvasInteraction/CanvasInteraction.h"

cv::Mat ChristofidesCanvas = cv::Mat(400, 600, CV_8UC3, cv::Scalar(255, 255, 255));
const char *const ChristofidesWindowName = "Hello everyone, my name is Markplier";

void ChristofidesSolve(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &result)
{
    const size_t size = vertices.size();
    std::vector<Vertex> verticesCopy = vertices;
    std::vector<WeightedUndirectedEdge> MST_Edges;
    std::vector<WeightedUndirectedEdge> fake;
    result.clear();
    result.reserve(size);
    MST_Edges.reserve(size * size);
    auto display = [](std::vector<WeightedUndirectedEdge> &fake, std::vector<Vertex> &verticesCopy, std::vector<WeightedUndirectedEdge> &edges, const int wait)
    {
        DeleteEdge(fake, verticesCopy, ChristofidesCanvas, ChristofidesWindowName);
        DisplayEdges(ChristofidesWindowName, ChristofidesCanvas, edges, wait);
        waitKey(0);
    };
    // Show empty canvas

    display(fake, verticesCopy, MST_Edges, 1);

    // Show canvas with fully connected vertices
    ConnectVertices(verticesCopy, MST_Edges);
    display(fake, verticesCopy, MST_Edges, 50);

    // Show canvas with MST
    PrimMST(verticesCopy, vertices, result);
    display(fake, vertices, result, 0);
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

void PrimMST(std::vector<Vertex> &FullyConntectedVertices, std::vector<Vertex> &OriginVertices, std::vector<WeightedUndirectedEdge> &MST_Edges)
{
    std::vector<bool> visited(FullyConntectedVertices.size(), false);

    visited[0] = true;
    auto hasvitedAll = [](std::vector<bool> &vectorToCheck)
    {
        for (auto &&v : vectorToCheck)
        {
            if (!v)
            {
                return false;
            }
        }
        return true;
    };

    while (!hasvitedAll(visited))
    {
        SimpleEdge simpleEdge;
        for (size_t i = 0; i < visited.size(); i++)
        {
            if (!visited[i])
            {
                continue;
            }
            for (size_t j = 0; j < FullyConntectedVertices[i].ConnectedEdges.size(); j++)
            {
                Vertex *v1Position = &FullyConntectedVertices[i];
                Vertex *v2Position = FullyConntectedVertices[i].ConnectedEdges[j]->v1 == &FullyConntectedVertices[i]
                                         ? FullyConntectedVertices[i].ConnectedEdges[j]->v2
                                         : FullyConntectedVertices[i].ConnectedEdges[j]->v1;
                if (HasVisited(v2Position, visited, &FullyConntectedVertices[0]))
                    continue;
                if (simpleEdge.Distance == -1 || simpleEdge.Distance > FullyConntectedVertices[i].ConnectedEdges[j]->GetWeight())
                {
                    // This ensure v2 will always be new vertex
                    simpleEdge.v1 = v1Position;
                    simpleEdge.v2 = v2Position;
                    simpleEdge.Distance = FullyConntectedVertices[i].ConnectedEdges[j]->GetWeight();
                }
            }
        }
        if (simpleEdge.Distance == -1)
        {
            return;
        }
        const int index1 = FindIndex(simpleEdge.v1, &FullyConntectedVertices[0]);
        const int index2 = FindIndex(simpleEdge.v2, &FullyConntectedVertices[0]);
        visited[index2] = true;
        MST_Edges.emplace_back(&OriginVertices[index1], &OriginVertices[index2]);
    }
}

bool HasVisited(const Vertex *VertexToCheck, const std::vector<bool> &Visited, const Vertex *Begin)
{
    return Visited[FindIndex(VertexToCheck, Begin)];
}
