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
    void dump_neighbor_list();

private:
    unordered_map<T, list<T>> _nbl; // neigbhour lists of source vertices
    unordered_map<T, int> _incnt; // incoming edges count of a vertex
    T _start;

    bool parse_edges(const list<Edge<T>>& edges);
    void print_path(const list<T>& path);
};

template <class T>
bool Graph<T>::parse_edges(const list<Edge<T>>& edges)
{
    size_t cnt = 0;
    for (const auto & e : edges) {
        if (_nbl.find(e._src) == _nbl.end())
            _nbl[e._src] = list<T>();
        _nbl[e._src].push_back(e._dst);
        _incnt[e._dst]++;

        if (cnt == 0)
            _start = e._src; // update start node
        cnt++;
    }
    return true;
}

template <class T>
void Graph<T>::add_edge(const Edge<T>& edge)
{
    // TODO
}

template <class T>
void Graph<T>::detect_loop()
{
    // TODO
}

template <class T>
bool Graph<T>::is_leaf(const T & node)
{
    // A leaf node should not be a source node.
    // TODO: make sure the given node is in the graph
    return !_nbl.count(node);
}

template <class T>
void Graph<T>::dump_neighbor_list()
{
    for (const auto& nb : _nbl) {
        if (nb.first == _start)
            cout << "*";
        cout << nb.first << ":";
        int cnt = 0;
        for (const auto& node : nb.second) {
            if (cnt++ > 0)
                cout  << "->";
            cout << node;
        }
        cout << endl;
    }
}

template <class T>
void Graph<T>::print_path(const list<T>& path)
{
    int cnt = 0;
    for (const auto& nd : path) {
        if (cnt++ > 0)
            cout << "->";
        cout << nd;
    }
    cout << endl;
}

// Directed graph
template <class T>
void Graph<T>::print_loop_free_paths()
{
    typedef pair<T,list<T>> node_path_type;
    unordered_map<T, int> explored;
    list<node_path_type> q; // queue
    list<T> lst;
    lst.emplace_back(_start);
    q.emplace_front(node_path_type(_start, lst));
    while (!q.empty()) {
        auto node_path = q.back();
        q.pop_back();
        auto node = node_path.first;
        auto path = node_path.second;
        explored[node]++;
        if (is_leaf(node)) {
            print_path(path);
            continue;
        }
        for (const auto& nd : _nbl[node]) {
            if ((explored.find(nd) == explored.end()) || (explored[nd] < _incnt[nd])
                    || is_leaf(nd)) { // always add leaf node to path
                auto p = path;
                p.emplace_back(nd);
                q.emplace_front(node_path_type(nd, p)); // nd not explored
            }
        }
    } // end while
}

int main()
{
    list<Edge<char>> edges{
        Edge<char>('a','b'), Edge<char>('a','c'), Edge<char>('b','d'),
        Edge<char>('d','b'), Edge<char>('b','e'), Edge<char>('c','f'),
        Edge<char>('c','g'), Edge<char>('e','h'), Edge<char>('f','e'),
        Edge<char>('f','g'), Edge<char>('g','e'), Edge<char>('g','i')
    };
    Graph<char> g(edges);
    //g.dump_neighbor_list();
    g.print_loop_free_paths();

    /* Expected output:
        a->b->e->h
        a->c->g->i
        a->c->f->e->h
        a->c->f->g->i
        a->c->g->e->h
        a->b->d->b->e->h
        a->c->f->g->e->h
    */        

    return 0;
}
