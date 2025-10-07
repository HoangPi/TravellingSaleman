#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <queue>
#include <algorithm>
#include <map>
#include <numeric>
using namespace std;
using namespace cv;

// Tạo Struct chứa thông tin của đỉnh và cạnh
struct Vertex
{
    int id;
    Point coord;
};

struct Edge
{
    int u, v;
    vector<Point> path;
    double length;
};

// Biến toàn cục
vector<Vertex> vertices;
vector<Edge> edges;
bool drawing = false;
bool moving = false;
int movingVertex = -1;
vector<Point> currentPath;
const int INF = 99999;

// bộ nhớ tạm thời lưu lại đang ở trạng thái của đồ thị nào
enum LastAlg
{
    ALG_NONE = 0,
    ALG_NN,
    ALG_GREEDY,
    ALG_CHR
};
LastAlg lastAlg = ALG_NONE;
vector<int> lastTour;
double lastLen = numeric_limits<double>::infinity();
bool graphChange = false;

// Khai báo chỗ nhận kết quả
string nnResult = "______";
string nn2OptResult = "______";
string greedyResult = "______";
string greedy2OptResult = "______";
string chResult = "______";
string ch2OptResult = "______";

// hàm xóa kết quả của tour cũ nếu có sự thay đổi trong việc lựa chọn thuật toán để xét và thay đổi trong đồ thị
vector<int> currentdisplayTour;
void RemoveResults()
{
    graphChange = true;
    lastAlg = ALG_NONE;
    lastTour.clear();
    lastLen = numeric_limits<double>::infinity();
    currentdisplayTour.clear();
    nnResult = nn2OptResult = greedyResult = greedy2OptResult = chResult = ch2OptResult = "______";
}

// Tính khoảng cách gần nhất giữa chuột tới đường đi
double distancetoEdge(const Edge &e, const vector<Vertex> &vertices, Point2d P)
{
    double best = 1e9;     // khai báo khoảng cách nhỏ nhất
    int n = e.path.size(); // cạnh là đường cong
    if (n >= 2)
    {
        for (int i = 1; i < n; i++)
        {
            Point2d A = e.path[i - 1];
            Point2d B = e.path[i];

            Point2d AB = B - A;
            Point2d AP = P - A;

            // tìm hình chiếu
            double t = AP.dot(AB) / AB.dot(AB);
            t = max(0.0, min(1.0, t));
            Point2d H = A + t * AB; // tọa độ hình chiếu H
            double dist = norm(P - H);
            best = min(best, dist);
        }
        return best;
    }
    Point2d A = vertices[e.u].coord;
    Point2d B = vertices[e.v].coord;
    Point2d AB = B - A;
    Point2d AP = P - A;
    double t = AP.dot(AB) / AB.dot(AB);
    t = max(0.0, min(1.0, t));
    return norm(P - (A + t * AB)); // distance PH
}

// Tìm đỉnh gần nhất
int findvertex(Point p)
{
    double r = 25.0;
    int n = vertices.size();
    for (int i = 0; i < n; i++)
    {
        if (norm(vertices[i].coord - p) < r)
        {
            return i;
        }
    }
    return -1;
}

// Kiểm tra đồ thị liên thông
bool GraphConnected(const vector<vector<int>> &mat)
{
    int n = mat.size();
    if (n == 0)
        return false;
    vector<int> visit(n);
    queue<int> q;
    q.push(0);
    visit[0] = 1;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        for (int v = 0; v < n; v++)
        {
            if (mat[u][v] < INF && !visit[v])
            {
                visit[v] = 1;
                q.push(v);
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        if (!visit[i])
            return false;
    }
    return true;
}

// Mouse Callback
void mouse(int event, int x, int y, int flags, void *)
{
    Point p(x, y);
    // tạo đỉnh
    if (event == EVENT_LBUTTONDOWN && (flags & EVENT_FLAG_SHIFTKEY))
    {
        vertices.push_back({(int)vertices.size(), p});
        RemoveResults();
        return;
    }
    // xóa tất cả cạnh
    if (event == EVENT_RBUTTONDOWN && (flags & EVENT_FLAG_CTRLKEY))
    {
        if (!edges.empty())
        {
            edges.clear();
            RemoveResults();
        }
        return;
    }
    // xóa từng cạnh theo ý muốn
    if (event == EVENT_LBUTTONDOWN && (flags & EVENT_FLAG_CTRLKEY))
    {
        int best = -1;
        double md = 1e9;
        int n = edges.size();
        for (int i = 0; i < n; i++)
        {
            double d = distancetoEdge(edges[i], vertices, Point2d(x, y));
            if (d < md)
            {
                md = d;
                best = i;
            }
        }
        if (best != -1 && md < 10.0)
        {
            edges.erase(edges.begin() + best);
            RemoveResults();
        }
        return;
    }
    // Dời đỉnh
    if (event == EVENT_LBUTTONDOWN && (flags & EVENT_FLAG_ALTKEY))
    {
        int idx = findvertex(p);
        if (idx != -1)
        {
            moving = true;
            movingVertex = idx;
            return;
        }
    }
    else if (event == EVENT_MOUSEMOVE && moving)
    {
        vertices[movingVertex].coord = p;
    }
    else if (event == EVENT_LBUTTONUP && moving)
    {
        moving = false;
        movingVertex = -1;
        RemoveResults();
        return;
    }
    // Vẽ cạnh
    if (event == EVENT_LBUTTONDOWN && !(flags & (EVENT_FLAG_ALTKEY | EVENT_FLAG_CTRLKEY | EVENT_FLAG_SHIFTKEY)))
    {
        drawing = true;
        currentPath.clear();
        currentPath.push_back(p);
    }
    else if (event == EVENT_MOUSEMOVE && drawing)
    {
        currentPath.push_back(p);
    }
    else if (event == EVENT_LBUTTONUP && drawing)
    {
        drawing = false;
        currentPath.push_back(p);
        int u = findvertex(currentPath.front());
        int v = findvertex(currentPath.back());
        if (u != -1 && v != -1 && u != v)
        {
            double L = 0;
            int n = currentPath.size();
            for (int i = 1; i < n; i++)
            {
                L += norm(currentPath[i] - currentPath[i - 1]);
            }
            edges.push_back({u, v, currentPath, L});
            cout << "Canh " << u << " <-> " << v << " (len=" << L << ")" << endl;
            RemoveResults();
        }
        currentPath.clear();
    }
}

// Hàm sinh ma trận kề từ vẽ cạnh
vector<vector<int>> AdjMatrix(const vector<Vertex> &vertices, const vector<Edge> &edges)
{
    int n = vertices.size();
    vector<vector<int>> mat(n, vector<int>(n, INF));
    for (int i = 0; i < n; i++)
        mat[i][i] = 0;

    for (const auto &e : edges)
    {
        if (e.u >= 0 && e.v >= 0 && e.u < n && e.v < n)
        {
            int w = (int)round(e.length); // Làm tròn để giữ int
            mat[e.u][e.v] = w;
            mat[e.v][e.u] = w;
        }
    }
    return mat;
}

// Hàm complete graph
void GenerateCompleteGraph()
{
    if (vertices.size() < 2)
        return;

    // Xóa toàn bộ các cạnh cũ
    edges.clear();
    int n = vertices.size();

    // Nối tất cả các cặp đỉnh với nhau
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            Point p1 = vertices[i].coord;
            Point p2 = vertices[j].coord;
            vector<Point> path = {p1, p2}; // đường nối thẳng giữa 2 đỉnh
            double len = norm(p1 - p2);    // độ dài Euclidean
            edges.push_back({i, j, path, len});
        }
    }

    cout << "Da tao Complete Graph voi " << edges.size() << " canh." << endl;
    RemoveResults();
}

// Vẽ đồ thị(hiển thị cạnh đã vẽ từ chuột) và vẽ đường đi của các thuật toán
void DrawGraph(Mat &img)
{
    for (auto &e : edges)
    {
        polylines(img, e.path, false, Scalar(200, 200, 200), 2, LINE_AA);
    }
    for (auto &v : vertices)
    {
        circle(img, v.coord, 15, Scalar(255, 0, 0), FILLED, LINE_AA);
        putText(img, to_string(v.id), v.coord + Point(-8, 6), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 2, LINE_AA);
    }
}

void DrawTour(Mat &img, const vector<int> &tour)
{
    int n = tour.size();
    for (int i = 0; i + 1 < n; i++)
    {
        int u = tour[i];
        int v = tour[i + 1];
        for (auto &e : edges)
        {
            if ((e.u == u && e.v == v) || (e.u == v && e.v == u))
            {
                polylines(img, e.path, false, Scalar(0, 200, 0), 3, LINE_AA);
            }
        }
    }
}

// Hamilton Cycle check
bool HasHamiltonCycle(const vector<vector<int>> &mat)
{
    int n = mat.size();
    for (int i = 0; i < n; ++i)
    {
        bool connected = false;
        for (int j = 0; j < n; ++j)
            if (i != j && mat[i][j] < INF)
                connected = true;
        if (!connected)
            return false;
    }
    return true;
}

// Deep First Search - DFS
void DFS(const vector<vector<int>> &matrix, int u, int start, vector<int> &path, vector<bool> &visit, double currentLength, double &bestLength, vector<int> &bestTour)
{
    if (currentLength >= bestLength)
        return;

    if ((int)path.size() == matrix.size())
    {
        if (matrix[u][start] < INF)
        {
            double total = currentLength + matrix[u][start];
            if (total < bestLength)
            {
                bestLength = total;
                bestTour = path;
                bestTour.push_back(start);
            }
        }
        return;
    }

    vector<pair<int, int>> candidate;
    for (int v = 0; v < matrix.size(); v++)
    {
        if (!visit[v] && v != start && matrix[u][v] < INF)
        {
            candidate.push_back({matrix[u][v], v});
        }
    }
    sort(candidate.begin(), candidate.end());

    for (auto [distance, v] : candidate)
    {
        visit[v] = true;
        path.push_back(v);
        DFS(matrix, v, start, path, visit, currentLength + distance, bestLength, bestTour);
        path.pop_back();
        visit[v] = false;
    }
}

// NearestNeighborBasic
pair<vector<int>, double> NearestNeighborBasic(const vector<vector<int>> &matrix)
{
    int start = 0;
    int n = matrix.size();
    if (n < 3)
    {
        return {{}, 0};
    }

    // Gọi kiểm tra Hamilton sớm trước khi chạy
    if (!HasHamiltonCycle(matrix))
    {
        cout << "Do thi khong thoa hamilton Cycle." << endl;
        return {{}, INF};
    }

    vector<bool> visit(n, false); // đỉnh chưa thăm
    vector<int> tour;
    double total = 0;
    int u = start;
    tour.push_back(u);
    visit[u] = true;

    for (int i = 1; i < n; i++)
    {
        int next = -1;
        int best = INF;
        for (int j = 0; j < n; j++)
        {
            if (!visit[j] && matrix[u][j] < best)
            {
                best = matrix[u][j];
                next = j;
            }
        }

        if (next == -1)
        {
            return {{}, INF};
        }

        total += best;
        u = next;
        visit[u] = true;
        tour.push_back(u);
    }

    if (matrix[u][start] >= INF)
    {
        return {{}, INF};
    }

    total += matrix[u][start];
    tour.push_back(start);

    cout << "Do dai cua Nearest Neighbor co ban la: " << total << endl;
    return {tour, total};
}

// Áp dụng DFS vào NN
pair<vector<int>, double> NN_DFS(const vector<vector<int>> &mat, int start, double upperBound)
{
    if (!HasHamiltonCycle(mat))
    {
        cout << "Hamilton pre-check failed: graph has isolated vertex(es)." << endl;
        return {{}, INF};
    }
    // Kiểm tra liên thông
    if (!GraphConnected(mat))
    {
        cout << "Graph not connected (no Hamilton cycle possible)." << endl;
        return {{}, INF};
    }

    // Lấy tour NN làm điểm khởi đầu
    auto [nnTour, nnLen] = NearestNeighborBasic(mat);
    double upper = (nnTour.empty() ? INF : nnLen);

    // cải thiện bằng DFS
    vector<int> bestTour;
    double bestLen = (upperBound < INF ? upperBound : upper);
    vector<int> path;
    path.reserve(mat.size() + 1);
    vector<bool> visit(mat.size(), false);

    visit[start] = true;
    path.push_back(start);

    DFS(mat, start, start, path, visit, 0, bestLen, bestTour);

    // kết quả cuối
    if (bestTour.empty())
    {
        cout << "DFS khong tim thay hamilton cycle, tra ve ket qua cua NN truoc do" << endl;
        return {nnTour, nnLen};
    }

    cout << "Do dai tot nhat la = " << bestLen << endl;
    return {bestTour, bestLen};
}

// GREEDY
pair<vector<int>, double> GreedyBasic(const vector<vector<int>> &mat)
{
    int n = mat.size();
    if (n < 3)
    {
        cout << "Khong du dinh de tao chu trinh!" << endl;
        return {{}, 0};
    }

    if (!HasHamiltonCycle(mat))
    {
        cout << "Do thi bi co lap hoac khong co hamilton cycle" << endl;
        return {{}, INF};
    }

    if (!GraphConnected(mat))
    {
        cout << "Do thi khong lien thong!" << endl;
        return {{}, INF};
    }

    // Greedy Basic
    struct Edge
    {
        int u, v, w;
    };
    vector<Edge> edges;

    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (mat[i][j] < INF)
                edges.push_back({i, j, (int)mat[i][j]});
        }
    }

    // Sắp xếp cạnh theo độ dài tăng dần
    sort(edges.begin(), edges.end(), [](auto &a, auto &b)
         { return a.w < b.w; });

    vector<int> degree(n, 0);
    vector<vector<int>> adj(n);
    vector<int> parent(n);
    iota(parent.begin(), parent.end(), 0);

    function<int(int)> findp = [&](int x)
    {
        return parent[x] == x ? x : parent[x] = findp(parent[x]);
    };

    vector<pair<int, int>> selected;

    for (auto &e : edges)
    {
        int u = e.u;
        int v = e.v;
        int w = e.w;

        if (degree[u] == 2 || degree[v] == 2)
            continue; // không vượt quá bậc 2

        int pu = findp(u);
        int pv = findp(v);

        // Tránh tạo chu trình sớm (chỉ cho phép khép khi đã đủ n cạnh)
        if (pu == pv && (int)selected.size() < n - 1)
            continue;

        parent[pu] = pv;
        selected.push_back({u, v});
        degree[u]++;
        degree[v]++;
        adj[u].push_back(v);
        adj[v].push_back(u);

        if ((int)selected.size() == n)
            break;
    }

    // Tạo tour
    if ((int)selected.size() < n)
    {
        cout << "Greedy khong the tao thanh chu trinh Hamilton" << endl;
        return {{}, INF};
    }

    vector<int> tour;
    int cur = 0, prev = -1;

    for (int step = 0; step < n; ++step)
    {
        tour.push_back(cur);
        int nxt = (adj[cur][0] == prev ? adj[cur][1] : adj[cur][0]);
        prev = cur;
        cur = nxt;
    }

    tour.push_back(tour[0]); // khép vòng

    // Tính độ dài
    double total = 0;
    for (int i = 0; i + 1 < (int)tour.size(); ++i)
        total += mat[tour[i]][tour[i + 1]];

    cout << "Do dai cua Greedy la: " << total << endl;
    return {tour, total};
}

// CHRISTOFIDES
bool IsValidHamiltonTour(const vector<vector<int>> &mat, const vector<int> &tour)
{
    int n = mat.size();
    if ((int)tour.size() != n + 1)
        return false;
    if (tour.front() != tour.back())
        return false;

    vector<char> seen(n, 0);
    for (int i = 0; i < n; ++i)
    {
        int u = tour[i];
        int v = tour[i + 1];

        if (u < 0 || u >= n || v < 0 || v >= n)
            return false;
        if (mat[u][v] >= INF)
            return false;

        if (i < n)
        {
            if (seen[u])
                return false; // đỉnh lặp trước khi khép vòng
            seen[u] = 1;
        }
    }
    return true;
}

// Minimum spanning tree - PRIM
vector<vector<int>> MST_Prim(const vector<vector<int>> &mat)
{
    int n = mat.size();
    vector<int> key(n, INF), parent(n, -1);
    vector<bool> inMST(n, false);
    key[0] = 0;

    for (int count = 0; count < n - 1; ++count)
    {
        int u = -1, minKey = INF;
        for (int v = 0; v < n; ++v)
            if (!inMST[v] && key[v] < minKey)
                minKey = key[v], u = v;

        if (u == -1)
            break;
        inMST[u] = true;

        for (int v = 0; v < n; ++v)
        {
            if (!inMST[v] && mat[u][v] < key[v])
            {
                parent[v] = u;
                key[v] = mat[u][v];
            }
        }
    }

    vector<vector<int>> mst(n, vector<int>(n, INF));
    for (int v = 1; v < n; ++v)
    {
        if (parent[v] != -1)
        {
            mst[v][parent[v]] = mst[parent[v]][v] = mat[v][parent[v]];
        }
    }
    return mst;
}

// Tìm các bậc lẻ
vector<int> FindOddDegreeVertices(const vector<vector<int>> &mst)
{
    int n = mst.size();
    vector<int> odd;

    for (int i = 0; i < n; ++i)
    {
        int deg = 0;
        for (int j = 0; j < n; ++j)
        {
            if (mst[i][j] < INF && i != j)
                deg++;
        }
        if (deg % 2 == 1)
            odd.push_back(i);
    }
    return odd;
}

// Matching các đỉnh bậc
vector<pair<int, int>> MinimumWeightMatching(const vector<vector<int>> &mat, const vector<int> &odd)
{
    vector<pair<int, int>> matching;
    if (odd.empty())
        return matching;

    vector<bool> used(odd.size(), false);
    for (int i = 0; i < (int)odd.size(); ++i)
    {
        if (used[i])
            continue;

        int best = -1, bestW = INF;
        for (int j = i + 1; j < (int)odd.size(); ++j)
        {
            if (!used[j] && mat[odd[i]][odd[j]] < bestW)
            {
                bestW = mat[odd[i]][odd[j]];
                best = j;
            }
        }

        if (best != -1)
        {
            used[i] = used[best] = true;
            matching.push_back({odd[i], odd[best]});
        }
    }
    return matching;
}

// Eulerian Graph
vector<vector<int>> CombineGraphs(const vector<vector<int>> &mst, const vector<pair<int, int>> &matching, const vector<vector<int>> &mat)
{
    int n = mst.size();
    vector<vector<int>> combined = mst;
    for (auto [u, v] : matching)
    {
        if (mat[u][v] < INF)
        {
            combined[u][v] = combined[v][u] = mat[u][v]; // SỬA: luôn thêm cạnh matching
        }
    }
    return combined;
}

// Convert Euler -> Hamilton
vector<int> EulerToHamilton(const vector<vector<int>> &g)
{
    int n = g.size();
    if (n == 0)
        return {};

    vector<vector<int>> adj = g;
    vector<int> circuit;
    vector<int> st = {0};

    while (!st.empty())
    {
        int u = st.back();
        int v = -1;

        for (int i = 0; i < n; ++i)
        {
            if (adj[u][i] < INF && u != i)
            {
                v = i;
                adj[u][i] = adj[i][u] = INF;
                st.push_back(v);
                break;
            }
        }

        if (v == -1)
        {
            circuit.push_back(u);
            st.pop_back();
        }
    }

    reverse(circuit.begin(), circuit.end());
    vector<bool> seen(n, false);
    vector<int> tour;
    for (int v : circuit)
        if (!seen[v])
        {
            seen[v] = true;
            tour.push_back(v);
        }

    if ((int)tour.size() < n)
        return {};
    tour.push_back(tour[0]);
    return tour;
}

// Christofides
pair<vector<int>, double> ChristofidesBasic(const vector<vector<int>> &mat)
{
    int n = mat.size();
    if (n < 3)
    {
        cout << "Khong du dinh de tao thanh mot chu trinh" << endl;
        return {{}, INF};
    }

    if (!HasHamiltonCycle(mat) || !GraphConnected(mat))
    {
        cout << "Do thi khong lien thong!" << endl;
        return {{}, INF};
    }

    auto mst = MST_Prim(mat);
    auto odd = FindOddDegreeVertices(mst);
    auto matching = MinimumWeightMatching(mat, odd);
    auto combined = CombineGraphs(mst, matching, mat);
    auto tour = EulerToHamilton(combined);

    if (tour.empty())
    {
        cout << "Christofides khong tao duoc tour hoan chinh." << endl;
        return {{}, INF};
    }

    double total = 0;
    for (int i = 0; i + 1 < (int)tour.size(); ++i)
        total += mat[tour[i]][tour[i + 1]];

    cout << "Da tim thay duong di voi do dai la: " << total << endl;
    return {tour, total};
}

// 2-OPT
inline bool EdgeOk(const vector<vector<int>> &mat, int a, int b)
{
    return mat[a][b] < INF;
}

double TourLength(const vector<vector<int>> &mat, const vector<int> &tour)
{
    double s = 0;
    int n = tour.size();
    for (int i = 0; i + 1 < n; i++)
    {
        int w = mat[tour[i]][tour[i + 1]];
        if (w >= INF)
            return numeric_limits<double>::infinity();
        s += w;
    }
    return s;
}

pair<vector<int>, double> TwoOpt(const vector<vector<int>> &mat, vector<int> tour)
{
    int m = tour.size();
    if (m <= 3)
        return {tour, TourLength(mat, tour)};

    double best = TourLength(mat, tour);
    if (!isfinite(best))
        return {tour, best};

    bool improved = true;
    while (improved)
    {
        improved = false;
        for (int i = 1; i < m - 2; i++)
        {
            for (int k = i + 1; k < m - 1; k++)
            {
                int a = tour[i - 1];
                int b = tour[i];
                int c = tour[k];
                int d = tour[k + 1];

                if (!EdgeOk(mat, a, c) || !EdgeOk(mat, b, d))
                    continue;

                double delta = (mat[a][c] + mat[b][d]) - (mat[a][b] + mat[c][d]);

                if (delta < -1e-9)
                {
                    reverse(tour.begin() + i, tour.begin() + k + 1);
                    best += delta;
                    improved = true;
                }
            }
        }
    }
    return {tour, best};
}

int main()
{
    string graphWin = "Group 7: Travelling Salesman Problem";
    Mat bg(1280, 1920, CV_8UC3, Scalar(255, 255, 255));
    namedWindow(graphWin);
    setMouseCallback(graphWin, mouse);

    while (true)
    {
        Mat temp = bg.clone();
        putText(temp, graphWin, Point(30, 60), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 200), 2, LINE_AA);

        DrawGraph(temp);
        if (!currentdisplayTour.empty())
            DrawTour(temp, currentdisplayTour);

        if (drawing && currentPath.size() > 1)
            polylines(temp, currentPath, false, Scalar(0, 255, 0), 2, LINE_AA);

        // Hướng dẫn sử dụng phím tắt
        int baseY = 900;
        int baseX = 30;
        int lineHeight = 30;
        putText(temp, "Instructions:", Point(baseX, baseY), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(200, 0, 0), 2, LINE_AA);
        putText(temp, "Create Vertex: Shift + Left Click", Point(baseX, baseY + lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);
        putText(temp, "Create Edge: Drag with Left Click", Point(baseX, baseY + 2 * lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);
        putText(temp, "Remove Edge: Ctrl + Left Click", Point(baseX, baseY + 3 * lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);
        putText(temp, "Move Vertex: Alt + Left Click", Point(baseX, baseY + 4 * lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);
        putText(temp, "Remove All Edges: Ctrl + Right Click", Point(baseX, baseY + 5 * lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);
        putText(temp, "Draw Complete Graph: Press [k]", Point(baseX, baseY + 6 * lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);

        // Hiển thị thông tin kết quả của các thuật toán
        putText(temp, "Algorithm Results:", Point(baseX, baseY + 7 * lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(200, 0, 0), 2, LINE_AA);
        putText(temp, "Nearest Neighbor - Press [n]: " + nnResult + " / 2-opt - Press [2]: " + nn2OptResult, Point(baseX, baseY + 8 * lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);
        putText(temp, "Greedy - Press [g]: " + greedyResult + " / 2-opt - Press [2]: " + greedy2OptResult, Point(baseX, baseY + 9 * lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);
        putText(temp, "Christofides - Press [c]: " + chResult + " / 2-opt - Press [2]: " + ch2OptResult, Point(baseX, baseY + 10 * lineHeight), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0), 2, LINE_AA);

        imshow(graphWin, temp);
        int key = waitKey(20);
        if (key == 27)
            break;

        auto run_and_display = [&](const vector<int> &tour, double len, LastAlg which, string &resStr)
        {
            if (!tour.empty() && isfinite(len) && (int)tour.size() == (int)vertices.size() + 1)
            {
                resStr = to_string((int)len);
                lastAlg = which;
                lastTour = tour;
                lastLen = len;
                graphChange = false;
                currentdisplayTour = tour;
            }
            else
            {
                resStr = "No cycle";
                currentdisplayTour.clear();
            }
        };

        // COMPLETE GRAPH
        if (key == 'k')
        {
            GenerateCompleteGraph();
        }

        // NEAREST NEIGHBOR
        if (key == 'n')
        {
            auto mat = AdjMatrix(vertices, edges);
            if (GraphConnected(mat))
            {
                auto [t, l] = NN_DFS(mat, 0, INF);
                run_and_display(t, l, ALG_NN, nnResult);
            }
            else
                nnResult = "No cycle";
        }

        // GREEDY
        if (key == 'g')
        {
            auto mat = AdjMatrix(vertices, edges);
            if (GraphConnected(mat))
            {
                auto [t, l] = GreedyBasic(mat);
                run_and_display(t, l, ALG_GREEDY, greedyResult);
            }
            else
                greedyResult = "No cycle";
        }

        // CHRISTOFIDES
        if (key == 'c')
        {
            auto mat = AdjMatrix(vertices, edges);
            if (GraphConnected(mat))
            {
                auto [t, l] = ChristofidesBasic(mat);
                run_and_display(t, l, ALG_CHR, chResult);
            }
            else
                chResult = "No cycle";
        }

        // 2-OPT - chạy sau các thuật toán khác
        if (key == '2')
        {
            if (lastAlg == ALG_NONE || lastTour.empty() || graphChange)
                continue;

            auto mat = AdjMatrix(vertices, edges);
            auto [optTour, optLen] = TwoOpt(mat, lastTour);
            if (!isfinite(optLen))
                continue;

            if (lastAlg == ALG_NN)
                nn2OptResult = to_string((int)optLen);
            else if (lastAlg == ALG_GREEDY)
                greedy2OptResult = to_string((int)optLen);
            else if (lastAlg == ALG_CHR)
                ch2OptResult = to_string((int)optLen);

            currentdisplayTour = optTour;
            lastTour = optTour;
            lastLen = optLen;
        }
    }

    return 0;
}
