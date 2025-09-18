#pragma once

#include <opencv2/opencv.hpp>
class WeightedUndirectedEdge
{
private:
    double weight;

public:

    class Vertex *v1;
    class Vertex *v2;

    WeightedUndirectedEdge() {};
    WeightedUndirectedEdge(class Vertex *v1, class Vertex *v2);
    void SetWeight(double w) { this->weight = (w < 0.0f ? 0.0f : w); };
    double GetWeight() { return this->weight; };
    ~WeightedUndirectedEdge();
};
