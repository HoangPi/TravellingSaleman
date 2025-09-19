#include <opencv2/opencv.hpp>
#include <vector>
#include "../classes/Edge.h"
#include "../classes/Vertex.h"
#include "../mathLib.h"
#include "../classes/Graph.h"

using namespace cv;

void HighlightVertex(Point p, Mat &canvas, const cv::String &winname = "White Screen");

// Delete edges by redraw vertices without edges
void DeleteEdge(Graph &graph, cv::Mat &canvas, const cv::String &winname);
void DisplaySingleEdge(const char *const WindowName, Mat &canvas, const WeightedUndirectedEdge &edge, const int offset);
double DisplayEdges(const char *const WindowName, Mat &canvas, std::vector<WeightedUndirectedEdge> &edges, const int WaitTime = 100, const int offset = 15);