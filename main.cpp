#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;
using namespace cv;

struct Vertex { int id; Point coord; };

vector<Vertex> vertices;
vector<vector<int>> costMatrix;
vector<vector<int>> editedMatrix;

const int INF = 99999;
int cellSize = 60;
int editingRow = -1, editingCol = -1;
string currentInput = "";
bool editable = false;     // matrix cho phép sửa?
bool matrixOpen = false;   // Matrix Window đang mở?
vector<int> tspPath;       // đường đi TSP hiện tại
bool noHamiltonCycle = false; // flag báo không có cycle

string currentAlgorithm = "";    // tên thuật toán đang hiển thị
int nnLength = -1;               // độ dài NN

// Lưu kết quả hiển thị
string nnResult = "______";
string nn2OptResult = "______";
string greedyResult = "______";
string greedy2OptResult = "______";

string nnResultEdit = "______";
string nn2OptResultEdit = "______";
string greedyResultEdit = "______";
string greedy2OptResultEdit = "______";

string christofidesResult = "______";
string christofides2OptResult = "______";
string christofidesResultEdit = "______";
string christofides2OptResultEdit = "______";

//--------------- Sinh complete matrix --------------------
void generateCompleteMatrix() 
{
    int n = (int)vertices.size();
    costMatrix.assign(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) 
    {
        if (i == j) costMatrix[i][j] = 0;
        else 
        {
            int dx = vertices[i].coord.x - vertices[j].coord.x;
            int dy = vertices[i].coord.y - vertices[j].coord.y;
            costMatrix[i][j] = (int)lround(sqrt((double)dx*dx + (double)dy*dy));
        }
    }
    editedMatrix = costMatrix;
}

//--------------- Vẽ ma trận ------------------------------
Mat drawMatrixWindow(const vector<vector<int>> &matrix) 
{
    int n = (int)matrix.size();
    int size = n * cellSize + 100;
    Mat img(size, size, CV_8UC3, Scalar(255,255,255));

    for (int i = 0; i < n; i++) 
    {
        putText(img, to_string(i), Point(50 + i*cellSize + cellSize/2 - 8, 35),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,150), 1, LINE_AA);
        putText(img, to_string(i), Point(20, 50 + i*cellSize + cellSize/2 + 5),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,150), 1, LINE_AA);
    }

    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) 
    {
        Rect cell(j*cellSize+50, i*cellSize+50, cellSize, cellSize);
        rectangle(img, cell, Scalar(0,0,0), 1, LINE_AA);
        string text = (matrix[i][j] >= INF) ? "INF" : to_string(matrix[i][j]);
        putText(img, text, Point(cell.x+10, cell.y+35),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0), 1, LINE_AA);
    }

    if (editable && editingRow != -1 && editingCol != -1) 
    {
        Rect cell(editingCol*cellSize+50, editingRow*cellSize+50, cellSize, cellSize);
        rectangle(img, cell, Scalar(0,0,255), 2);
        putText(img, currentInput, Point(cell.x+10, cell.y+35),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255), 2);
    }
    return img;
}

// Vẽ đỉnh
void DrawVertices (Mat &br) 
{
    for (int i = 0; i < (int)vertices.size(); i++) 
    {
        const Point &p = vertices[i].coord;
        circle(br, p, 10, Scalar(0,255,0), -1, LINE_AA);
        putText(br, to_string(i), p + Point(-5,-13),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0), 1, LINE_AA);
    }
}

//--------------- Mouse callback: Graph --------------------
void mouseHandlerGraph(int event, int x, int y, int, void*) 
{
    if (matrixOpen) return;
    if (event == EVENT_LBUTTONDOWN) 
    {
        Vertex v{ (int)vertices.size(), Point(x,y) 
    };
        vertices.push_back(v);
        cout << "Added vertex " << v.id << " at (" << x << "," << y << ")\n";

        generateCompleteMatrix(); //always update matrix

        tspPath.clear();
        noHamiltonCycle = false;
        currentAlgorithm.clear();
        nnLength = -1;
    }
}

//--------------- Mouse callback: Matrix -------------------
void mouseHandlerMatrix(int event, int x, int y, int, void*) 
{
    if (!matrixOpen || !editable) return;
    int n = (int)editedMatrix.size();
    if (event == EVENT_LBUTTONDOWN) 
    {
        int row = (y-50) / cellSize;
        int col = (x-50) / cellSize;
        if (row >= 0 && row < n && col >= 0 && col < n) 
        {
            editingRow = row;
            editingCol = col;
            currentInput.clear();
            cout << "Editing cell (" << row << "," << col << ")\n";
        }
    }
}

//--------------- Key input cho Matrix ---------------------
void keyHandlerMatrix(int key) 
{
    if (!matrixOpen || !editable) return;
    if (editingRow == -1 || editingCol == -1) return;

    if (key >= '0' && key <= '9') 
    {
        currentInput.push_back((char)key);
    } else if (key == 8 && !currentInput.empty()) 
    {
        currentInput.pop_back();
    } else if (key == 13) 
    { 
        if (!currentInput.empty()) 
        {
            int val = stoi(currentInput);
            if (val >= INF) val = INF;
            editedMatrix[editingRow][editingCol] = val;
            editedMatrix[editingCol][editingRow] = val;
        }
        editingRow = editingCol = -1;
        currentInput.clear();
        tspPath.clear();
        noHamiltonCycle = false;
        currentAlgorithm.clear();
        nnLength = -1;
    }
}

//--------------- TSP: Nearest Neighbor --------------------
pair<vector<int>, int> tspNearestNeighbor(const vector<vector<int>> &mat) 
{
    int n = (int)mat.size();
    if (n == 0) return {{}, 0};

    vector<bool> vis(n, false);
    vector<int> path; path.reserve(n+1);
    int total = 0, cur = 0;

    path.push_back(cur);
    vis[cur] = true;

    for (int step = 1; step < n; step++) 
    {
        int next = -1, best = INF;
        for (int j = 0; j < n; j++) {
            if (!vis[j] && mat[cur][j] < best) 
            {
                best = mat[cur][j];
                next = j;
            }
        }
        if (next == -1 || best >= INF) 
        {
            return {{}, INF};
        }
        vis[next] = true;
        path.push_back(next);
        total += best;
        cur = next;
    }

    if (mat[cur][0] >= INF) 
    {
        cout << "Hamilton path ton tai nhung khong the dong thanh chu trinh.\n";
        return {{}, INF};
    }

    total += mat[cur][0];
    path.push_back(0);
    return {path, total};
}

//--------------- TSP: Greedy Algorithm --------------------
pair<vector<int>, int> tspGreedy(const vector<vector<int>>& mat) 
{
    int n = (int)mat.size();
    if (n == 0) return {{}, 0};

    struct Edge { int u,v,w; };
    vector<Edge> edges;
    for (int i=0;i<n;i++)
    {
        for (int j=i+1;j<n;j++)
        {
            if (mat[i][j] < INF) edges.push_back({i,j,mat[i][j]});
        }
    }

    sort(edges.begin(), edges.end(), [](auto &a, auto &b){ return a.w < b.w; });

    vector<int> degree(n,0);
    vector<vector<int>> adj(n);
    int added = 0, total = 0;

    for (auto &e : edges) 
    {
        if (degree[e.u]==2 || degree[e.v]==2) continue;

        vector<bool> vis(n,false);
        queue<int> q; q.push(e.u); vis[e.u]=true;
        bool connected=false;
        while(!q.empty())
        {
            int x=q.front(); q.pop();
            if(x==e.v) 
            {
                connected=true; break;
            }
            for(int nei: adj[x]) if(!vis[nei]) 
            {
                vis[nei]=true; q.push(nei);
            }
        }
        if (connected && added < n-1) continue;

        adj[e.u].push_back(e.v);
        adj[e.v].push_back(e.u);
        degree[e.u]++; degree[e.v]++;
        total += e.w;
        added++;
        if (added==n) break;
    }

    if (added < n) 
    {
    auto [fallbackTour, fallbackLen] = tspNearestNeighbor(mat);
    return {fallbackTour, fallbackLen};
    }

    vector<int> path;
    int cur = 0, prev=-1;
    for(int step=0;step<n;step++)
    {
        path.push_back(cur);
        int nxt = (adj[cur][0]==prev? adj[cur][1] : adj[cur][0]);
        prev=cur; cur=nxt;
    }
    path.push_back(0);
    return {path,total};
}

//--------------- TSP: Christofides -----------------------
pair<vector<int>, int> tspChristofides(const vector<vector<int>>& mat) 
{
    int n = (int)mat.size();
    if (n == 0) return {{}, 0};

    vector<int> parent(n, -1), key(n, INF);
    vector<bool> inMST(n, false);
    key[0] = 0;

    for (int count = 0; count < n - 1; count++) 
    {
        int u = -1;
        for (int i = 0; i < n; i++) 
        {
            if (!inMST[i] && (u == -1 || key[i] < key[u])) u = i;
        }
        inMST[u] = true;

        for (int v = 0; v < n; v++) 
        {
            if (mat[u][v] < key[v] && !inMST[v]) 
            {
                parent[v] = u;
                key[v] = mat[u][v];
            }
        }
    }

    vector<vector<int>> adj(n);
    for (int v = 1; v < n; v++) 
    {
        int u = parent[v];
        if (u != -1) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }


    vector<int> odd;
    for (int i = 0; i < n; i++) 
    {
        if (adj[i].size() % 2 == 1) odd.push_back(i);
    }

    vector<bool> used(n, false);
    for (int i = 0; i < (int)odd.size(); i++) 
    {
        if (used[odd[i]]) continue;
        int u = odd[i], best = -1;
        for (int j = i + 1; j < (int)odd.size(); j++) 
        {
            int v = odd[j];
            if (!used[v] && (best == -1 || mat[u][v] < mat[u][best])) best = v;
        }
        if (best != -1) 
        {
            adj[u].push_back(best);
            adj[best].push_back(u);
            used[u] = used[best] = true;
        }
    }

    vector<int> euler, stack = {0};
    vector<vector<int>> adjCopy = adj;
    while (!stack.empty()) 
    {
        int u = stack.back();
        if (!adjCopy[u].empty()) 
        {
            int v = adjCopy[u].back();
            adjCopy[u].pop_back();
            auto& vec = adjCopy[v];
            vec.erase(find(vec.begin(), vec.end(), u));
            stack.push_back(v);
        } 
        else 
        {
            euler.push_back(u);
            stack.pop_back();
        }
    }
    reverse(euler.begin(), euler.end());

    vector<bool> visited(n, false);
    vector<int> tour;
    int total = 0;
    for (int v : euler) 
    {
        if (!visited[v]) 
        {
            if (!tour.empty()) total += mat[tour.back()][v];
            tour.push_back(v);
            visited[v] = true;
        }
    }

    total += mat[tour.back()][tour.front()];
    tour.push_back(tour.front());

    return {tour, total};
}

//--------------- 2-opt -----------------------------------
void twoOpt(vector<int> &tour, const vector<vector<int>>& mat) 
{
    int n = tour.size();
    if (n <= 3) return;

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

                if (a >= mat.size() || b >= mat.size() || c >= mat.size() || d >= mat.size()) continue;
                if (mat[a][c] >= INF || mat[b][d] >= INF) continue;

                int before = mat[a][b] + mat[c][d];
                int after  = mat[a][c] + mat[b][d];

                if (after < before) 
                {
                    reverse(tour.begin() + i, tour.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
}

//--------------- Vẽ tour ---------------------------------
void drawTour(Mat &img, const vector<int> &tour, const vector<vector<int>> &mat, Scalar color = Scalar(0,255,0)) 
{
    if (tour.empty()) return;
    for (int i = 0; i + 1 < (int)tour.size(); i++) 
    {
        int u = tour[i], v = tour[i+1];
        if (u < mat.size() && v < mat.size() && mat[u][v] < INF) 
        {
            line(img, vertices[u].coord, vertices[v].coord, color, 2);
        }
    }
}

int main() 
{
    const string graphWin  = "Graph Window";
    const string matrixWin = "Matrix Window";
    bool matrixWindowCreated = false;

    Mat window(900, 1280, CV_8UC3, Scalar(255,255,255));
    namedWindow(graphWin);
    setMouseCallback(graphWin, mouseHandlerGraph);

    while (true) 
    {
        Mat display = window.clone();

        DrawVertices(display);
        if (!tspPath.empty()) 
        {
            const auto &matRef = editable ? editedMatrix : costMatrix;
            drawTour(display, tspPath, matRef, Scalar(255,0,0));
        }

        // Tiêu đề
        putText(display, "Group 9: Travelling Salesman",
            Point(30, 60), FONT_HERSHEY_SIMPLEX, 1.0,
            Scalar(0,0,200), 2, LINE_AA);

        int baseY = 500;
        int lineHeight = 30;

        putText(display, "Truong hop 1: Khong co vat can", Point(30, baseY),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,0,150), 2, LINE_AA);
        putText(display, "Nearest Neighbor (n): " + nnResult + " / 2-opt: " + nn2OptResult,
            Point(30, baseY + lineHeight), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,0), 2, LINE_AA);
        putText(display, "Greedy (g): " + greedyResult + " / 2-opt: " + greedy2OptResult,
            Point(30, baseY + 2*lineHeight), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,0), 2, LINE_AA);
        putText(display, "Christofides (c): " + christofidesResult + " / 2-opt: " + christofides2OptResult,
            Point(30, baseY + 3*lineHeight), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,0), 2, LINE_AA);

        putText(display, "Truong hop 2: Co vat can", Point(30, baseY + 4*lineHeight),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,0,150), 2, LINE_AA);
        putText(display, "Nearest Neighbor (n): " + nnResultEdit + " / 2-opt: " + nn2OptResultEdit,
            Point(30, baseY + 5*lineHeight), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,0), 2, LINE_AA);
        putText(display, "Greedy (g): " + greedyResultEdit + " / 2-opt: " + greedy2OptResultEdit,
            Point(30, baseY + 6*lineHeight), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,0), 2, LINE_AA);
        putText(display, "Christofides (c): " + christofidesResultEdit + " / 2-opt: " + christofides2OptResultEdit,
            Point(30, baseY + 7*lineHeight), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,0), 2, LINE_AA);
        imshow(graphWin, display);

        // ===== Matrix Window =====
        if (matrixOpen) {
            if (!matrixWindowCreated) 
            {
                namedWindow(matrixWin);
                setMouseCallback(matrixWin, mouseHandlerMatrix);
                matrixWindowCreated = true;
            }
            const auto &matRef = editable ? editedMatrix : costMatrix;
            Mat matImg = drawMatrixWindow(matRef);
            imshow(matrixWin, matImg);

            if (getWindowProperty(matrixWin, WND_PROP_VISIBLE) < 1) 
            {
                destroyWindow(matrixWin);
                matrixOpen = false;
                matrixWindowCreated = false;
                editingRow = editingCol = -1;
                currentInput.clear();
            }
        }

        int key = waitKey(20);
        if (key == -1) continue;

        if (key == 27) 
        {
            if (matrixOpen) 
            {
                destroyWindow(matrixWin);
                matrixOpen = false;
                matrixWindowCreated = false;
                editingRow = editingCol = -1;
                currentInput.clear();
                continue;
            } else break;
        }

        int ch = tolower(key);

        if (ch == 'm') 
        {
            if (vertices.empty()) { cout << "Chua co dinh!\n"; continue; }
            generateCompleteMatrix();
            editable = false;
            matrixOpen = true;
            continue;
        }

        if (ch == 'e') 
        {
            if (vertices.empty()) { cout << "Chua co dinh!\n"; continue; }
            generateCompleteMatrix();
            editable = true;
            matrixOpen = true;
            continue;
        }

        // NN
        if (ch == 'n') 
        {
            const auto &matRef = editable ? editedMatrix : costMatrix;
            auto [tour, length] = tspNearestNeighbor(matRef);
            tspPath = tour;
            currentAlgorithm = "NN";
            if (length < INF && !tour.empty()) 
            {
                if (editable) nnResultEdit = to_string(length);
                else nnResult = to_string(length);
            } else 
            {
                if (editable) nnResultEdit = "Khong ton tai chu trinh";
                else nnResult = "Khong ton tai chu trinh";
            }
        }

        // Greedy
        if (ch == 'g') 
        {
            const auto &matRef = editable ? editedMatrix : costMatrix;
            auto [tour, length] = tspGreedy(matRef);
            tspPath = tour;
            currentAlgorithm = "Greedy";
            if (length < INF && !tour.empty()) 
            {
                if (editable) greedyResultEdit = to_string(length);
                else greedyResult = to_string(length);
            } else 
            {
                if (editable) greedyResultEdit = "Khong ton tai chu trinh";
                else greedyResult = "Khong ton tai chu trinh";
            }
        }

        // c: Christofides
        if (ch == 'c') 
        {
            const auto &matRef = editable ? editedMatrix : costMatrix;
            auto [tour, length] = tspChristofides(matRef);
            tspPath = tour;
            currentAlgorithm = "Christofides";
        if (length < INF && !tour.empty()) 
        {
            if (editable) christofidesResultEdit = to_string(length);
            else christofidesResult = to_string(length);
        } 
        else 
        {
            if (editable) christofidesResultEdit = "Khong ton tai chu trinh";
            else christofidesResult = "Khong ton tai chu trinh";
        }
        }



        // 2-opt
        if (ch == '2' && !tspPath.empty()) 
        {
            const auto &matRef = editable ? editedMatrix : costMatrix;
            twoOpt(tspPath, matRef);

            int newLen = 0;
            for (int i = 0; i + 1 < tspPath.size(); i++) 
            {
                int u = tspPath[i], v = tspPath[i+1];
                if (u >= matRef.size() || v >= matRef.size() || matRef[u][v] >= INF) 
            {
            newLen = INF; break;
        }
        newLen += matRef[u][v];
        }

        if (newLen < INF) 
        {
            if (editable) 
            {
                if (currentAlgorithm == "NN") nn2OptResultEdit = to_string(newLen);
                else if (currentAlgorithm == "Greedy") greedy2OptResultEdit = to_string(newLen);
                else if (currentAlgorithm == "Christofides") christofides2OptResultEdit = to_string(newLen);
            } 
            else 
            {
                if (currentAlgorithm == "NN") nn2OptResult = to_string(newLen);
                else if (currentAlgorithm == "Greedy") greedy2OptResult = to_string(newLen);
                else if (currentAlgorithm == "Christofides") christofides2OptResult = to_string(newLen);
            }
        } 
    else 
    {
        if (editable) 
        {
            if (currentAlgorithm == "NN") nn2OptResultEdit = "Khong ton tai chu trinh";
            else if (currentAlgorithm == "Greedy") greedy2OptResultEdit = "Khong ton tai chu trinh";
            else if (currentAlgorithm == "Christofides") christofides2OptResultEdit = "Khong ton tai chu trinh";
        } else {
            if (currentAlgorithm == "NN") nn2OptResult = "Khong ton tai chu trinh";
            else if (currentAlgorithm == "Greedy") greedy2OptResult = "Khong ton tai chu trinh";
            else if (currentAlgorithm == "Christofides") christofides2OptResult = "Khong ton tai chu trinh";
        }
    }
}


        if (matrixOpen) keyHandlerMatrix(key);
    }

    return 0;
}
