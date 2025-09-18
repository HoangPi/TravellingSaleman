
#include "Edge.h"

#include "Vertex.h"
#include "cmath"

WeightedUndirectedEdge::WeightedUndirectedEdge(Vertex *v1, Vertex *v2)
{
    this->v1 = v1;
    this->v2 = v2;
    const double deltaX = v1->p.x - v2->p.x;
    const double deltaY = v1->p.y - v2->p.y;
    this->weight = std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
    v1->ConnectedEdges.push_back(this);
    v2->ConnectedEdges.push_back(this);
}

WeightedUndirectedEdge::~WeightedUndirectedEdge()
{
    for (size_t i = 0; i < this->v1->ConnectedEdges.size(); i++)
    {
        if (this->v1->ConnectedEdges[i] == this)
        {
            this->v1->ConnectedEdges.erase(v1->ConnectedEdges.begin() + i);
        }
    }
    for (size_t i = 0; i < this->v2->ConnectedEdges.size(); i++)
    {
        if (this->v2->ConnectedEdges[i] == this)
        {
            this->v2->ConnectedEdges.erase(v2->ConnectedEdges.begin() + i);
        }
    }
    printf("Good byte\n");
}
