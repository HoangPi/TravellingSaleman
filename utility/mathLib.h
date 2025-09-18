#pragma once

#include <opencv2/opencv.hpp>
#include "classes/Edge.h"
#include "classes/Vertex.h"
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

void NearestNeighbor(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &result);
void Solve(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &result, ESOLVE_TYPE Type = NEAREST_NEIGBOR);