#include "graphs.h"

#include <stack>
#include <tuple>
#include <map>

using namespace graphs;

void insert_edges(graph& dag, std::vector<std::tuple<node*, node*>> edges);

void shift(int starting_index, std::stack<std::tuple<node*, node*>>& queue, std::map<node*, int>& order, std::vector<node*>& inv_order, std::map<node*, bool>& vacant);

void place_node(node* n, int i, std::map<node*, int>& order, std::vector<node*>& inv_order);

std::stack<std::tuple<node*, node*>> discover(std::vector<std::tuple<node*, node*>>& edge_insertions, std::map<node*, int>& order, std::vector<node*>& inv_order, std::map<node*, bool>& vacant);

void depth_first_search(node* v, int ub, std::stack<std::tuple<node*, node*>>& queue, std::map<node*, int>& order, std::vector<node*>& inv_order, std::map<node*, bool>& vacant);