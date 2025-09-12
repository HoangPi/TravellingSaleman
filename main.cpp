#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "utility/mathLib.h"

using namespace cv;
using namespace std;

constexpr int radius = 10;

Mat canvas;                // Global canvas
vector<Point> clickPoints; // Global vector to store clicked points
int InteractivePointIndex = -1;

// Mouse callback function
void onMouse(int event, int x, int y, int, void *)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        Point p(x, y);
        auto color = canvas.at<cv::Vec3b>(p);
        if (InteractivePointIndex != -1)
        {
            circle(canvas, clickPoints[InteractivePointIndex], radius, Scalar(0, 0, 0), FILLED);
            imshow("White Screen", canvas);
            InteractivePointIndex = -1;
            return;
        }
        if (color == cv::Vec3b::zeros())
        {
            for (int i = 0; i < clickPoints.size(); i++)
            {
                constexpr int R2 = radius * radius;
                if (DistanceSquare(clickPoints[i], p) < R2)
                {
                    InteractivePointIndex = i;
                    circle(canvas, clickPoints[i], radius, Scalar(127, 127, 127), FILLED);
                    imshow("White Screen", canvas);
                    return;
                }
            }
        }
        clickPoints.push_back(p); // Store the point

        // Draw a dark circle at the click position
        circle(canvas, p, radius, Scalar(0, 0, 0), FILLED);

        // If there is more than one point, draw a line from the last to the new one
        // if (clickPoints.size() > 1) {
        //     line(canvas, clickPoints[clickPoints.size() - 2], p, Scalar(50, 50, 50), 2);
        // }

        imshow("White Screen", canvas);

        cout << "Stored point: (" << p.x << ", " << p.y << ")\n";
    }
}

int main()
{
    // Create a fixed-size white screen (e.g., 600x400)
    canvas = Mat(400, 600, CV_8UC3, Scalar(255, 255, 255));

    namedWindow("White Screen");
    setMouseCallback("White Screen", onMouse);

    imshow("White Screen", canvas);

    // Wait until user presses a key
    waitKey(0);

    // Print all stored points after exiting
    cout << "\nAll clicked points:\n";
    for (const auto &pt : clickPoints)
    {
        cout << "(" << pt.x << ", " << pt.y << ")\n";
    }

    return 0;
}
