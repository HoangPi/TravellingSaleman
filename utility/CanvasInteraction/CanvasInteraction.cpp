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
