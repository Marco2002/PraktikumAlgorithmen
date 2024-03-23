#include "graphs.h"

#include <stack>
#include <tuple>
#include <unordered_map>

using namespace graphs;

using EdgeQueue = std::stack<Edge>;

void insert_edges(graph& dag, std::vector<Edge>& edges);

void shift(int starting_index, EdgeQueue& queue, std::unordered_map<node*, int>& order, std::vector<node*>& inv_order, std::unordered_map<node*, bool>& vacant);

void place_node(node* n, int i, std::unordered_map<node*, int>& order, std::vector<node*>& inv_order);

EdgeQueue discover(std::vector<Edge>& edge_insertions, std::unordered_map<node*, int>& order, std::vector<node*>& inv_order, std::unordered_map<node*, bool>& vacant);

void depth_first_search(node* v, int ub, EdgeQueue& queue, std::unordered_map<node*, int>& order, std::vector<node*>& inv_order, std::unordered_map<node*, bool>& vacant);