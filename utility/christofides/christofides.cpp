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
auto display = [](Graph &g, const int wait, const char *const name)
{
    std::vector<WeightedUndirectedEdge> empty;
    Graph fake(g.Vertices, empty);
    DeleteEdge(fake, ChristofidesCanvas, name);
    DisplayEdges(name, ChristofidesCanvas, g.Edges, wait);
};

void ChristofidesSolve(Graph &graph)
{
    const size_t size = graph.Vertices.size();
    graph.Edges.clear();
    graph.Edges.reserve(size);
    // Show empty canvas

    Graph ChristofideseGraph(graph.Vertices, graph.Edges);
    display(ChristofideseGraph, 1, ChristofidesWindowName);

    // Show canvas with fully connected vertices
    ConnectVertices(ChristofideseGraph);
    display(ChristofideseGraph, 50, ChristofidesWindowName);
    waitKey(0);

    // Show canvas with MST
    PrimMST(ChristofideseGraph);
    display(ChristofideseGraph, 200, ChristofidesWindowName);
    waitKey(0);

    // FindOddDegreeVertices(vertices, result);
    // display(fake, vertices, result, -1, ChristofidesWindowName);
}

void ConnectVertices(Graph &graph)
{
    graph.Edges.reserve(graph.Vertices.size() * graph.Vertices.size());
    for (size_t i = 0; i < graph.Vertices.size(); i++)
    {
        for (size_t j = i + 1; j < graph.Vertices.size(); j++)
        {
            graph.AddEdges(&graph.Vertices[i], &graph.Vertices[j]);
        }
    }
}

void PrimMST(Graph &graph)
{
    std::vector<bool> visited(graph.Vertices.size(), false);
    std::vector<SimpleEdge> simpleEdges;
    simpleEdges.reserve(graph.Vertices.size());

    visited[0] = true;

    while (!hasvitedAll(visited))
    {
        SimpleEdge edge;
        int candidateIndex = -1;
        for (size_t i = 0; i < visited.size(); i++)
        {
            if (!visited[i])
            {
                continue;
            }
            for (size_t j = 0; j < graph.Vertices[i].ConnectedEdges.size(); j++)
            {
                int evaluateIndex = FindIndex(
                    graph.Vertices[i].ConnectedEdges[j]->v1 == &graph.Vertices[i]
                        ? graph.Vertices[i].ConnectedEdges[j]->v2
                        : graph.Vertices[i].ConnectedEdges[j]->v1,
                    &graph.Vertices[0]);
                if ((edge.Distance == -1 ||
                     edge.Distance > graph.Vertices[i].ConnectedEdges[j]->GetWeight()) &&
                    !visited[evaluateIndex])
                {
                    // This ensure v2 will always be new vertex
                    edge.v1 = &graph.Vertices[i];
                    edge.v2 = &graph.Vertices[evaluateIndex];
                    edge.Distance = graph.Vertices[i].ConnectedEdges[j]->GetWeight();
                    candidateIndex = evaluateIndex;
                }
            }
        }
        if (edge.Distance == -1)
        {
            break;;
        }
        visited[candidateIndex] = true;
        simpleEdges.push_back(edge);
    }
    graph.Edges.clear();
    for (auto &&edge : simpleEdges)
    {
        graph.AddEdges(edge.v1, edge.v2);
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
        // display(fake, MST_Vertices, MST_Edges, -1, ChristofidesWindowName);
        MST_Edges.emplace_back(OddDegreeVertices[save_i], OddDegreeVertices[save_j]);
        visited[save_i] = true;
        visited[save_j] = true;
    }
}

void EulerTravel(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &edges, std::vector<WeightedUndirectedEdge> &path)
{
    std::vector<Vertex *> visited;
    visited.reserve(vertices.size());
    Vertex *current;
    while (edges.size() > 0)
    {
        visited.push_back(current);
        WeightedUndirectedEdge *shortestEdge = nullptr;
        for (size_t i = 0; i < current->ConnectedEdges.size(); i++)
        {
            if (!shortestEdge || shortestEdge->GetWeight() > current->ConnectedEdges[i]->GetWeight())
            {
                shortestEdge;
            }
        }
    }
}
