#pragma once

#include <opencv2/opencv.hpp>
#include "../classes/Edge.h"
#include "../classes/Vertex.h"
#include <vector>

void ChristofidesSolve(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &result);
void ConnectVertices(std::vector<Vertex> &vertices, std::vector<WeightedUndirectedEdge> &edges);