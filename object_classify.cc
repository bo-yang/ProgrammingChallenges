
/*
A depth image indicates in each pixel the distance from the camera to the seen object.

Input:
10 10 2 2
0 0 0 0 0 0 0 0 0 0
0 10 12 0 0 0 22 40 41 0
0 14 11 13 0 0 0 0 0 0
0 11 13 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 4 3 0 0 0
0 0 0 0 0 6 3 5 0 0
0 0 0 0 0 10 0 7 0 0
35 37 38 39 0 8 0 7 0 0
0 0 0 0 0 0 7 6 0 0

expected output:
0 0 0 0 0 0 0 0 0 0
0 25 25 0 0 0 0 0 0 0
0 25 25 25 0 0 0 0 0 0
0 25 25 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 7 7 0 0 0
0 0 0 0 0 7 7 7 0 0
0 0 0 0 0 7 0 7 0 0
1 1 1 1 0 7 0 7 0 0
0 0 0 0 0 0 7 7 0 0
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <queue>

using namespace std;

typedef std::pair<size_t,size_t> node;
// Only for pairs of std::hash-able types for simplicity.
// You can of course template this struct to allow other hash functions
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;  
    }
};

static int get_tag(size_t tag)
{
    return tag;
}

static bool is_visited(node n, const unordered_map<node, size_t, pair_hash> &visited)
{
    auto found = visited.find(n);
    return (found != visited.end());
}

void bfs_explore(node nd, const vector<vector<int>> &data, unordered_map<node, size_t, pair_hash> &visited, size_t S, size_t D)
{
    // Start from the first non-zero pixel, search for all nearby non-zero pixels.
    // Put visited pixels into a hashtable - (x,y) : depth.
    // Maintain the average depth for each group, and use it to lable pixel group.
    unordered_map<node, size_t, pair_hash> nodes;
    size_t avg = 0;
    queue<node> Q;
    Q.push(nd);
    while (!Q.empty()) {
        node n = Q.front();
        Q.pop();
        size_t row = n.first;
        size_t col = n.second;
        //if (data[row][col] >= 0 && data[row][col] <= 10)
        //    cout << "exploring " << data[row][col] <<endl; // FIXME: TEST ONLY
        if (is_visited(n, visited) || is_visited(n, nodes)) {
            //cout << "bfs_explore(): Skipping " << data[row][col] << endl;
            continue;
        }
        // visit
        nodes[n] = data[row][col];
        avg += nodes[n];
        size_t depth = avg/nodes.size();
        auto same_depth = [D](int current, int target) {
            //cout << "current depth " << current << ", target " <<target <<endl; // FIXME: TEST ONLY
            return (current > 0) && (current + (int)D*5 >= target) && (current - (int)D*5 <= target);
        };
        // explore
        if (row - 1 > 0 && same_depth(data[row-1][col], depth))
            Q.push(node{row-1,col});
        if (row + 1 < data.size() && same_depth(data[row+1][col], depth))
            Q.push(node{row+1,col});
        if (col - 1 > 0 && same_depth(data[row][col-1], depth))
            Q.push(node{row, col-1});
        if (col + 1 < data[0].size() && same_depth(data[row][col+1], depth))
            Q.push(node{row, col+1});
    }

    size_t tag = 0;
    if (nodes.size() > S)
        tag = get_tag(avg / nodes.size());

    for (auto &n : nodes)
        visited[n.first] = tag;

    return;
}

// Input image data with size HxW.
// S is the threashhold of pixels to discard small objects
// if two nearby pixels differ image differ more than D milimeters in depth to camera, label them as two objects
vector<vector<int>> classify(size_t H, size_t W, size_t S, size_t D, vector<vector<int>> data) {
    vector<vector<int>> image(W,vector<int>(H,0));
    unordered_map<node, size_t, pair_hash> visited;

    // Find the first non-zero pixel
    for (int i=0; i<W; ++i) {
        for (int j=0; j<H; ++j) {
            if ((data[i][j] <= 0) || is_visited(node(i,j), visited)) {
                continue;
            }
            bfs_explore(node(i,j), data, visited, S, D);
        }
    }

    // set the pixel depth
    for (auto &pix : visited)
        image[pix.first.first][pix.first.second] = pix.second;

    return image;
}

void print_matrix(vector<vector<int>> &M)
{
    for (auto &vec: M) {
        for (auto &pix: vec)
            cout << std::setw(2) << pix << " ";
        cout << endl;
    }
}

int main()
{
    size_t H = 10;
    size_t W = 10;
    size_t S = 2;
    size_t D = 2;
    vector<vector<int>> data = {
        {0,0,0,0,0,0,0,0,0,0},
        {0,10,12,0,0,0,22,40,41,0},
        {0,14,11,13,0,0,0,0,0,0},
        {0,11,13,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,4,3,0,0,0},
        {0,0,0,0,0,6,3,5,0,0},
        {0,0,0,0,0,10,0,7,0,0},
        {35,37,38,39,0,8,0,7,0,0},
        {0,0,0,0,0,0,7,6,0,0}
    };

    cout << "Input:" << endl;
    print_matrix(data);

    cout << "\nOutput:" << endl;
    vector<vector<int>> newdata = classify(H, W, S, D, data);
    print_matrix(newdata);
}
