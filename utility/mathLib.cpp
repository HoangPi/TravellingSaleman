#include "mathLib.h"

inline bool ExistInVector(std::vector<int> &vec, int item)
{
    for (auto &&v : vec)
    {
        if (v == item)
        {
            return true;
        }
    }
    return false;
}

long long DistanceSquare(Point a, Point b)
{
    const long long deltaX = (a.x - b.x);
    const long long deltaY = (a.y - b.y);
    return deltaX * deltaX + deltaY * deltaY;
}

cv::Point PointAtDistance(const cv::Point &a, const cv::Point &b, double n)
{
    cv::Point2d dir = b - a;
    double len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0)
        return a; // Avoid division by zero
    cv::Point2d unit = dir / len;
    return a + Point(unit * n);
}

void NearestNeighbor(Graph &graph)
{
    graph.Edges.clear();
    graph.Edges.reserve(graph.Vertices.size() - 1);
    std::vector<int> visited;
    visited.reserve(graph.Vertices.size());
    visited.push_back(0);
    int current = 0;
    while (visited.size() < visited.capacity())
    {
        double minDistance = 0x7fffffffffffffff;
        int next = -1;
        for (int i = 0; i < graph.Vertices.size(); i++)
        {
            if (!ExistInVector(visited, i))
            {
                const double temp = DistanceSquare(graph.Vertices[current].p, graph.Vertices[i].p);
                if (temp < minDistance)
                {
                    next = i;
                    minDistance = temp;
                }
            }
        }
        if (next == -1)
        {
            graph.Edges.emplace_back(&graph.Vertices[current], &graph.Vertices[0]);
            return;
        }
        visited.push_back(next);
        graph.Edges.emplace_back(&graph.Vertices[current], &graph.Vertices[next]);
        current = next;
    }
    graph.Edges.emplace_back(&graph.Vertices[current], &graph.Vertices[0]);
}

void Solve(Graph &graph, ESOLVE_TYPE Type)
{
    switch (Type)
    {
    case ESOLVE_TYPE::NEAREST_NEIGBOR:
        NearestNeighbor(graph);
        break;
    case ESOLVE_TYPE::CRHISTOFIDES:
        ChristofidesSolve(graph);
        break;
    default:
        NearestNeighbor(graph);
        break;
    }
}
