#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

class Edge
{
public:
    Point p1;
    Point p2;

    Edge(Point p1, Point p2);
    int DistanceSquare();
};
