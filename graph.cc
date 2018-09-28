#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template <class T>
struct Edge {
    T _src; // source node
    T _dst; // destination node

    Edge(T source, T dest) : _src(source), _dst(dest) {}
};

template <class T>
class Graph {
public:
    Graph() {}
    Graph(const list<Edge<T>>& edges) { parse_edges(edges); }

    void add_edge(const Edge<T>& edge);
    void detect_loop();
    void print_loop_free_paths();
    bool is_leaf(const T & node);

private:
    unordered_map<T, list<Edge<T>> _nbl; // neigbhour lists of source vertices
    unordered_map<T, int> _incnt; // incoming edges count of a vertex
    T _start;

    bool parse_edges(const list<Edge<T>>& edges);
    void print_path(const list<T>& path);
};

template <class T>
bool Graph::parse_edges(const list<Edge<T>>& edges)
{
    size_t cnt = 0;
    for (const auto & e : edges) {
        if (_nbl.find(e._src) == _nbl.end())
            _nbl[e._src] = list();
        _nbl[e._src].push_back(e._dst);
        _incnt[e._dst]++;

        if (cnt == 0)
            _start = e._src; // update start node
        cnt++;
    }
    return true;
}

template <class T>
void Graph::add_edge(const Edge<T>& edge)
{
}

template <class T>
void Graph::detect_loop()
{
}

template <class T>
bool Graph::is_leaf(const T & node)
{
    // A leaf node should not be a source node.
    // TODO: make sure the given node is in the graph
    return !_nbl.count(node);
}

template <class T>
void Graph::print_path(const list<T>& path)
{
}

template <class T>
void Graph::print_loop_free_paths()
{
    typedef pair<T,list<T>> node_path_type;
    unordered_map<T, int> explored;
    list<node_path_type> q; // queue
    q.emplace_front(node_path_type(_start, list<T>(_start)));
    while (!q.empty()) {
        auto node_path = q.back();
        q.pop_back();
        auto node = node_path.first;
        auto path = node_path.second;
        explored[node]++;
        if (is_leaf(node))
            print_path(path);
        for (const auto& nd : _nbl[node]) {
            if ((explored.find(nd) == explored.end()) ||
                    explored[]{
                auto p = path;
                p.emplace_back(nd);
                q.emplace_front(node_path_type(nd, p)); // nd not explored
            }
        }
    }
}

int main()
{
}
