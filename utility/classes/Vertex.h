#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class Vertex
{
public:
    cv::Point p;
    std::vector<class WeightedUndirectedEdge *> ConnectedEdges;

    Vertex(cv::Point &p);
    Vertex() {};
    static int64 DistanceSquare(Vertex &v1, Vertex &v2);
};