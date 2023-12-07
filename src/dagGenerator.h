#include "graphs.h"

using namespace graphs;

void set_seed(int seed);

graph generate_graph(int number_of_nodes, int number_of_edges, bool should_be_dag);
void shuffle_graph(graph* graph);