#include "christofides.h"
#include "../CanvasInteraction/CanvasInteraction.h"

cv::Mat ChristofidesCanvas = cv::Mat(400, 600, CV_8UC3, cv::Scalar(255, 255, 255));
const char *const ChristofidesWindowName = "Hello everyone, my name is Markplier";

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
auto display = [](std::vector<WeightedUndirectedEdge> &fake, std::vector<Vertex> &verticesCopy, std::vector<WeightedUndirectedEdge> &edges, const int wait, const char *const name)
{
    DeleteEdge(fake, verticesCopy, ChristofidesCanvas, name);
    DisplayEdges(name, ChristofidesCanvas, edges, wait);
};

void ChristofidesSolve(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &result)
{
    const size_t size = vertices.size();
    std::vector<Vertex> ConnectedVertices = vertices;
    std::vector<WeightedUndirectedEdge> ConnectedEdges;
    std::vector<WeightedUndirectedEdge> fake;
    result.clear();
    result.reserve(size);
    ConnectedEdges.reserve(size * size);
    // Show empty canvas

    display(fake, ConnectedVertices, ConnectedEdges, 1, ChristofidesWindowName);

    // Show canvas with fully connected vertices
    ConnectVertices(ConnectedVertices, ConnectedEdges);
    display(fake, ConnectedVertices, ConnectedEdges, 50, ChristofidesWindowName);
    waitKey(0);

    // Show canvas with MST
    PrimMST(ConnectedVertices, vertices, result);
    display(fake, vertices, result, 200, ChristofidesWindowName);
    waitKey(0);

    FindOddDegreeVertices(vertices, result);
    display(fake, vertices, result, -1, ChristofidesWindowName);
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
    MST_Edges.clear();
    std::vector<bool> visited(FullyConntectedVertices.size(), false);

    visited[0] = true;

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

void FindOddDegreeVertices(std::vector<Vertex> &MST_Vertices, std::vector<WeightedUndirectedEdge> &MST_Edges)
{
    std::vector<Vertex *> OddDegreeVertices;
    auto IsOverLap = [&MST_Edges](Vertex *v1, Vertex *v2)
    {
        for (auto &&e : MST_Edges)
        {
            if ((e.v1 == v1 && e.v2 == v2) || (e.v2 == v1 && e.v1 == v2))
            {
                return true;
            }
        }
        return false;
    };

    OddDegreeVertices.reserve(MST_Vertices.size());
    // Find all odd degree vertices
    for (size_t i = 0; i < MST_Vertices.size(); i++)
    {
        if ((MST_Vertices[i].ConnectedEdges.size() & 1) != 0)
        {
            OddDegreeVertices.push_back(&MST_Vertices[i]);
        }
    }
    std::vector<SimpleEdge> subgraphEdges;
    subgraphEdges.reserve(OddDegreeVertices.size() / 2);

    std::vector<bool> visited(OddDegreeVertices.size(), false);
    while (!hasvitedAll(visited))
    {
        SimpleEdge simpleEdge;
        int save_i = 0;
        int save_j;
        for (int i = 0; i < OddDegreeVertices.size(); i++)
        {
            for (int j = i + 1; j < OddDegreeVertices.size(); j++)
            {
                if (visited[i] || visited[j])
                {
                    continue;
                }
                double weight = WeightedUndirectedEdge(OddDegreeVertices[i], OddDegreeVertices[j]).GetWeight();
                if (!IsOverLap(OddDegreeVertices[i], OddDegreeVertices[j]))
                {
                    if (
                        simpleEdge.Distance == -1 ||
                        simpleEdge.Distance > weight)
                    {
                        simpleEdge.v1 = OddDegreeVertices[i];
                        simpleEdge.v2 = OddDegreeVertices[j];
                        simpleEdge.Distance = weight;
                        save_i = i;
                        save_j = j;
                    }
                }
            }
        }
        if (simpleEdge.Distance == -1)
        {
            break;
        }
        std::vector<WeightedUndirectedEdge> fake;
        display(fake, MST_Vertices, MST_Edges, -1, ChristofidesWindowName);
        MST_Edges.emplace_back(OddDegreeVertices[save_i], OddDegreeVertices[save_j]);
        visited[save_i] = true;
        visited[save_j] = true;
    }
}
