#include "dagGenerator.h"
#include "dagUtil.h"

#include <random>
#include <algorithm>
#include <stdexcept>

using namespace std;

typedef std::mt19937 MyRNG;
MyRNG rng;

void set_seed(int seed) {
    rng.seed(seed);
}

/* generates a graph with number_of_nodes nodes and number_of_edges edges randomly distributed between the nodes
 * by setting should_be_dag to true, the generated graph will be a dag
 * if should_be_dag is set to false, the generated graph will most likely not be a dag, but it is not guaranteed
 *
 * input specifications:
 * number_of_nodes >= 2
 * number_of_edges >= 0
 * number_of_edges < number_of_nodes * (number_of_nodes-1) if should_be_dag is false
 * number_of_edges < number_of_nodes * (number_of_nodes-1)/2 if should_be_dag is true
 */
graph generate_graph(int number_of_nodes, int number_of_edges, bool should_be_dag) {
    if(number_of_nodes < 2) {
        throw std::invalid_argument( "the number of nodes needs to be at least 2" );
    }
    if(should_be_dag && number_of_edges > number_of_nodes * (number_of_nodes-1)/2 ||
            !should_be_dag && number_of_edges > number_of_nodes * (number_of_nodes-1) ||
            number_of_edges < 0) {
        throw std::invalid_argument( "too many edges" );
    }
    std::vector<node*> nodes;
    std::vector<Edge> edges;
    std::uniform_int_distribution<int> node_distribution(0,number_of_nodes-1);
    nodes.resize(number_of_nodes);
    edges.resize(number_of_edges);

    graph dag = {
            .nodes_ = nodes,
            .number_of_edges_ = number_of_edges,
    };

    // generate edges
    for(int i = 0; i < number_of_nodes; ++i) {
        dag.nodes_[i] = new node;
        dag.nodes_[i]->index_ = i;
    }

    std::vector<std::tuple<int, int>> generated_edges = {};
    // add random edges to nodes
    // edges are added from any node to any other node
    for(int i = 0; i < number_of_edges; ++i) {
        // generate two random indexes
        int a, b;
        int from, to;
        do {
            a = node_distribution(rng);
            b = node_distribution(rng);

            if(should_be_dag) {
                from = std::min(a, b);
                to = std::max(a, b);
            } else {
                from = a;
                to = b;
            }
        } while (from == to
                || (!generated_edges.empty() && count(generated_edges.begin(), generated_edges.end(), std::make_tuple(from, to)) > 0));

        // if the graph should be   a dag then the edge will be set from the node of the smaller index_ to the node of the bigger index_
        // if the graph should not be a dag then the edges will be set in random order
        generated_edges.emplace_back(from, to);

        dag.nodes_[from]->outgoing_edges_.push_back(dag.nodes_[to]); // add the destination node to the outgoing edges of the origin node
        dag.nodes_[to]->incoming_edges_.push_back(dag.nodes_[from]); // add the origin node to the incoming edges of the destination node

    }
    return dag;
}

std::vector<Edge> generate_extra_edges(graph& dag, int number_of_edges) {
    std::uniform_int_distribution<int> node_distribution(0,dag.nodes_.size()-1);
    std::vector<Edge> generated_edges = {};
    // generate random new edges that don't invalidate current topological order
    for(int i = 0; i < number_of_edges; ++i) {
        // generate two random indexes
        int a, b;
        node *from, *to;

        do {
            a = node_distribution(rng);
            b = node_distribution(rng);

            from = dag.nodes_[std::min(a, b)];
            to = dag.nodes_[std::max(a, b)];
        // if the generated edge is already in the graph, or was already generated before or is a self pointing edge,
        // then the edge is invalid and needs to be regenerated
        } while (from == to
            || (!generated_edges.empty() && count(generated_edges.begin(), generated_edges.end(), std::make_tuple(from, to)) > 0)
            || (!from->outgoing_edges_.empty() && count(from->outgoing_edges_.begin(), from->outgoing_edges_.end(), to) > 0));

        generated_edges.emplace_back(from, to);
    }
    return generated_edges;
}


// shuffles the order of the nodes of the given graph
// leaves the nodes index values in an inconsistent state
void shuffle_graph(graph& graph) {
    std::shuffle(std::begin(graph.nodes_), std::end(graph.nodes_), rng);
}