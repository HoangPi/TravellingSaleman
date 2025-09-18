#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "utility/mathLib.h"
#include "utility/CanvasInteraction/CanvasInteraction.h"
#include "utility/classes/Vertex.h"

#include "utility/classes/Edge.h"

using namespace cv;
using namespace std;

constexpr int radius = 10;
constexpr int distance_threshold = 650;
constexpr int DELETE = 0xff;
constexpr int BACKSPACE = 0x08;
constexpr int ESCAPE = 0x1b;
constexpr int ZERO_BUTTON = 0x30;

auto const WindowName = "White Screen";

Mat canvas;
vector<Vertex> Vertices; // Global vector to store clicked points
int InteractiveVertexIndex = -1;
vector<WeightedUndirectedEdge> edges;

// Mouse callback function
void onMouse(int event, int x, int y, int, void *)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        Point p(x, y);

        auto color = canvas.at<cv::Vec3b>(p);
        if (InteractiveVertexIndex != -1)
        {
            circle(canvas, Vertices[InteractiveVertexIndex].p, radius, Scalar(0, 0, 0), FILLED);
            imshow(WindowName, canvas);
            InteractiveVertexIndex = -1;
            return;
        }
        constexpr int R2 = radius * radius + distance_threshold;
        for (int i = 0; i < Vertices.size(); i++)
        {

            if (DistanceSquare(p, Vertices[i].p) < R2)
            {
                InteractiveVertexIndex = i;
                circle(canvas, Vertices[i].p, radius, Scalar(127, 127, 127), FILLED);
                imshow(WindowName, canvas);
                return;
            }
        }
        Vertices.emplace_back(p); // Store the point

        // Draw a dark circle at the click position
        circle(canvas, p, radius, Scalar(0, 0, 0), FILLED);

        // If there is more than one point, draw a line from the last to the new one
        // if (clickPoints.size() > 1) {
        //     line(canvas, clickPoints[clickPoints.size() - 2], p, Scalar(50, 50, 50), 2);
        // }

        imshow(WindowName, canvas);

        // cout << "Stored point: (" << p.x << ", " << p.y << ")\n";
    }
}

void onSolveCallback(int state, void *userdata)
{
    printf("State is %d\n", state);
}

int main()
{
    // Create a fixed-size white screen (e.g., 600x400)
    canvas = Mat(400, 600, CV_8UC3, Scalar(255, 255, 255));

    namedWindow(WindowName);
    setMouseCallback(WindowName, onMouse);

    imshow(WindowName, canvas);

    // Wait until user presses a key
    while (true)
    {
        int key = waitKey(0);
        if (key == ESCAPE)
        {
            break;
        }
        if (key == DELETE || key == BACKSPACE)
        {
            if (InteractiveVertexIndex != -1)

            {
                circle(canvas, Vertices[InteractiveVertexIndex].p, radius, Scalar(255, 255, 255), FILLED);
                Vertices.erase(Vertices.begin() + InteractiveVertexIndex);
                InteractiveVertexIndex = -1;
                imshow(WindowName, canvas);
            }
        }
        else if (key == ZERO_BUTTON)
        {
            DeleteEdge(edges, Vertices, canvas, WindowName);
        }
        else
        {
            Solve(Vertices, edges, ESOLVE_TYPE::NEAREST_NEIGBOR);
            double totalWeight = DisplayEdges(WindowName, canvas, edges, 100);
            printf("Total weight is %lf\n", totalWeight);
        }
    }

    // Print all stored points after exiting
    cout << "\nAll clicked points:\n";
    for (const auto &pt : Vertices)
    {
        cout << "(" << pt.p.x << ", " << pt.p.y << ")\n";
    }

    return 0;
}
