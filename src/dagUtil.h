#include "graphs.h"

using namespace graphs;

void set_to_topological_order(graph& dag);

int find_node_in_graph(node* node, graph& graph);

bool graph_is_in_topological_order(graph& graph);