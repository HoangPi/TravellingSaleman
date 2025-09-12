#pragma once

#include <opencv2/opencv.hpp>

class Vertex
{
public:
    cv::Point p;

    Vertex(cv::Point &p);
    Vertex() {};
    static int64 DistanceSquare(Vertex &v1, Vertex &v2);
};