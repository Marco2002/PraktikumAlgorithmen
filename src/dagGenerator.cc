#include "dagGenerator.h"

#include <iostream>
#include <random>
#include <algorithm>

typedef std::mt19937 MyRNG;
MyRNG rng;

void set_seed(int seed) {
    rng.seed(seed);
}

graph generate_graph(int number_of_nodes, int number_of_edges, bool should_be_dag) {
    std::vector<node> nodes;
    std::vector<edge> edges;
    std::uniform_int_distribution<int> node_distribution(0,number_of_nodes-1);
    nodes.resize(number_of_nodes);
    edges.resize(number_of_edges);

    graph dag = {
            .nodes_ = nodes,
            .edges_ = edges,
    };

    // generate edges
    for(int i = 0; i < number_of_nodes; ++i) {
        dag.nodes_[i] = {};
    }

    // add random edges to nodes
    // edges are added from any node to any other node
    for(int i = 0; i < number_of_edges; ++i) {
        // generate two random indexes
        int a, b;
        a = node_distribution(rng);
        do {
            b = node_distribution(rng);
        } while (a == b);

        // if the graph should be a dag then the edge will be set from the node of the smaller index to the node of the bigger index
        // if the graph should not be a dag then the edges will be set in random order
        int from, to;
        if(should_be_dag) {
            from = std::min(a, b);
            to = std::max(a, b);
        } else {
            from = a;
            to = b;
        }
        dag.edges_[i] = {
             .from_ = &dag.nodes_[from],
             .to_ = &dag.nodes_[to]
        };
        dag.nodes_[from].outgoing_edges_.push_back(&dag.edges_[i]);
        dag.nodes_[to].incoming_edges_.push_back(&dag.edges_[i]);

    }
    return dag;
}

void shuffle_graph(graph* graph) {
    std::shuffle(std::begin((graph->nodes_)), std::end((graph->nodes_)), rng);
}