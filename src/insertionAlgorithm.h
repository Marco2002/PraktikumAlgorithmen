#include "graphs.h"

#include <stack>

using namespace graphs;

using EdgeQueue = std::stack<Edge>;

void insert_edges(graph& dag, std::vector<Edge>& edges);

void shift(int starting_index, EdgeQueue& queue, std::vector<node*>& inv_order, std::vector<bool>& vacant);

void place_node(node* n, int i, std::vector<node*>& inv_order);

EdgeQueue discover(std::vector<Edge>& edge_insertions, std::vector<node*>& inv_order, std::vector<bool>& vacant);

void depth_first_search(node* v, int ub, EdgeQueue& queue, std::vector<node*>& inv_order, std::vector<bool>& vacant);