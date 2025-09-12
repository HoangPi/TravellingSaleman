#pragma once

#include <opencv2/opencv.hpp>
#include "classes/Edge.h"
#include "classes/Vertex.h"
#include <vector>

using namespace cv;

long long DistanceSquare(Point a, Point b);

void GreedySolve(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &result);