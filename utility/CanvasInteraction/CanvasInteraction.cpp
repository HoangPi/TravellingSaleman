#include "CanvasInteraction.h"

void DeleteEdge(Graph &graph, cv::Mat &canvas, const cv::String &winname)
{
    constexpr int radius = 10;
    canvas.setTo(cv::Scalar::all(255));
    graph.Edges.clear();
    for (auto &&vertex : graph.Vertices)
    {
        circle(canvas, vertex.p, radius, Scalar(0, 0, 0), FILLED);
    }
    imshow(winname, canvas);
}

double DisplayEdges(const char *const WindowName, Mat &canvas, std::vector<WeightedUndirectedEdge> &edges, const int WaitTime, const int offset, cv::Scalar Color)
{
    double totalWeight = 0.0f;
    for (int i = 0; i < edges.size(); i++)
    {
        if (WaitTime >= 0)
        {
            waitKey(WaitTime);
        }
        DisplaySingleEdge(WindowName, canvas, edges[i], offset, Color);
        totalWeight += edges[i].GetWeight();
        imshow(WindowName, canvas);
    }
    return totalWeight;
}

void DisplaySingleEdge(const char *const WindowName, Mat &canvas, const WeightedUndirectedEdge &edge, const int offset, cv::Scalar Color)
{
    Point start = PointAtDistance(edge.v1->p, edge.v2->p, offset);
    Point end = PointAtDistance(edge.v2->p, edge.v1->p, offset);
    line(canvas, start, end, Color, 2);
}
