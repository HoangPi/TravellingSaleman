#include <opencv2/opencv.hpp>
#include <vector>
#include "../classes/Edge.h"
#include "../classes/Vertex.h"
#include "../mathLib.h"

using namespace cv;

void HighlightVertex(Point p, Mat &canvas, const cv::String &winname = "White Screen");

// Delete edges by redraw vertices without edges
void DeleteEdge(std::vector<WeightedUndirectedEdge> &Edges, std::vector<Vertex> Vertices, cv::Mat &canvas, const cv::String &winname);