#pragma once

#include <opencv2/opencv.hpp>
#include "classes/Graph.h"
#include "christofides/christofides.h"
#include <vector>

using namespace cv;
enum ESOLVE_TYPE : uint8_t
{
    NEAREST_NEIGBOR,
    CRHISTOFIDES,
    GREEDY,
};

long long DistanceSquare(Point a, Point b);

cv::Point PointAtDistance(const cv::Point& a, const cv::Point& b, double n);

void NearestNeighbor(Graph &graph);
void Solve(Graph &graph, ESOLVE_TYPE Type = NEAREST_NEIGBOR);