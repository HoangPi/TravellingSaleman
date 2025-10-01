void twoOpt(vector<int> &tour, const vector<Point>& V) 
{
    int n = tour.size();
    bool improved = true;
    while (improved) 
    {
        improved = false;
        for (int i = 1; i < n - 2; i++) 
        {
            for (int j = i + 1; j < n - 1; j++) 
            {
                int a = tour[i-1], b = tour[i];
                int c = tour[j],   d = tour[j+1];
                int before = dist(V[a], V[b]) + dist(V[c], V[d]);
                int after  = dist(V[a], V[c]) + dist(V[b], V[d]);
                if (after < before) {
                    reverse(tour.begin() + i, tour.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
}
