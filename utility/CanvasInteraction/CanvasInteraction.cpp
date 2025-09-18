#include "CanvasInteraction.h"

void DeleteEdge(std::vector<WeightedUndirectedEdge> &Edges, std::vector<Vertex> Vertices, cv::Mat &canvas, const cv::String &winname)
{
    constexpr int radius = 10;
    canvas.setTo(cv::Scalar::all(255));
    Edges.clear();
    for (auto &&vertex : Vertices)
    {
        circle(canvas, vertex.p, radius, Scalar(0, 0, 0), FILLED);
    }
    imshow(winname, canvas);
}

double DisplayEdges(const char *const WindowName, Mat &canvas, std::vector<WeightedUndirectedEdge> &edges, const int WaitTime, const int offset)
{
    double totalWeight = 0.0f;
    for (int i = 0; i < edges.size(); i++)
    {
        waitKey(WaitTime < 0 ? 1 : WaitTime);
        DisplaySingleEdge(WindowName, canvas, edges[i], offset);
        totalWeight += edges[i].GetWeight();
        imshow(WindowName, canvas);
    }
    return totalWeight;
}

void DisplaySingleEdge(const char *const WindowName, Mat &canvas, const WeightedUndirectedEdge &edge, const int offset)
{
    Point start = PointAtDistance(edge.v1->p, edge.v2->p, offset);
    Point end = PointAtDistance(edge.v2->p, edge.v1->p, offset);
    line(canvas, start, end, Scalar(50, 50, 50), 2);
}
