#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include "Edge.h"

using namespace std;
using namespace cv;

class Vertex
{
public:
    Point p;
    // For the travelling saleman problem ,the length should be at most 2
    vector<Edge> ConnectedEdges;

    Vertex(Point &p);
    bool CheckNeigborExist(Vertex &v);
    static void MakeEdge(Vertex &v1, Vertex &v2);
};