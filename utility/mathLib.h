#pragma once

#include <opencv2/opencv.hpp>
#include "classes/Vertex.h"

using namespace cv;

int DistanceSquare(Point a, Point b);

void GreedyAlgorithm(vector<Vertex> &Vertices, vector<Edge> &Edges);