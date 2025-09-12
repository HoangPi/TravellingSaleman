#include "mathLib.h"

int DistanceSquare(Point a, Point b)
{
    const int deltaX = (a.x - b.x);
    const int deltaY = (a.y - b.y);
    return deltaX * deltaX + deltaY * deltaY;
}

void GreedyAlgorithm(vector<Vertex> &Vertices, vector<Edge> &Edges)
{
    vector<int> visited;
    visited.reserve(Vertices.size());
    Edges.clear();
    Edges.reserve(Vertices.size());
    for (int i = 0; i < Vertices.size(); i++)
    {
        int minDistance = 0x7fffffff;
        Vertex *nextVertex = nullptr;
        for (int j = i + 1; j < Vertices.size(); j++)
        {
            int temp = DistanceSquare(Vertices[i].p, Vertices[j].p);
            if (temp < minDistance)
            {
                minDistance = temp;
                nextVertex = &Vertices[j];
            }
        }
        if(!nextVertex)
        {
            exit(-1);
        }
        Vertex::MakeEdge(Vertices[i], *nextVertex);
    }
}
