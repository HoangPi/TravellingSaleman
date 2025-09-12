#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

class Edge
{
private:
    double weight;

public:
    class Vertex *p1;
    class Vertex *p2;

    Edge(class Vertex *p1, class Vertex *p2);
    int DistanceSquare();
    inline double GetWeight() { return this->weight; };
    inline void SetWeight(double w) { this->weight = (w < 0.0f ? 0.0f : w); }
};
