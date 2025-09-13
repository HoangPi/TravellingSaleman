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

void NearestNeighbor(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &result)
{
    result.clear();
    result.reserve(vertices.size() - 1);
    std::vector<int> visited;
    visited.reserve(vertices.size());
    visited.push_back(0);
    int current = 0;
    while (visited.size() < visited.capacity())
    {
        double minDistance = 0x7fffffffffffffff;
        int next = -1;
        for (int i = 0; i < vertices.size(); i++)
        {
            if (!ExistInVector(visited, i))
            {
                const double temp = DistanceSquare(vertices[current].p, vertices[i].p);
                if (temp < minDistance)
                {
                    next = i;
                    minDistance = temp;
                }
            }
        }
        if (next == -1)
        {
            result.emplace_back(&vertices[current], &vertices[0]);
            return;
        }
        visited.push_back(next);
        result.emplace_back(&vertices[current], &vertices[next]);
        current = next;
    }
    result.emplace_back(&vertices[current], &vertices[0]);
}
