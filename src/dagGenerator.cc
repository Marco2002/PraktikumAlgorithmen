#include "dagGenerator.h"
#include "dagUtil.h"

#include <random>
#include <algorithm>
#include <tuple>

using namespace std;

typedef std::mt19937 MyRNG;
MyRNG rng;

void set_seed(int seed) {
    rng.seed(seed);
}

graph generate_graph(int number_of_nodes, int number_of_edges, bool should_be_dag) {
    std::vector<node> nodes;
    std::vector<std::tuple<node*, node*>> edges;
    std::uniform_int_distribution<int> node_distribution(0,number_of_nodes-1);
    nodes.resize(number_of_nodes);
    edges.resize(number_of_edges);

    graph dag = {
            .nodes_ = nodes,
            .number_of_edges_ = number_of_edges,
    };

    // generate edges
    for(int i = 0; i < number_of_nodes; ++i) {
        dag.nodes_[i] = {};
    }

    std::tuple<int, int> generated_edges[number_of_edges];
    auto array_size = sizeof(generated_edges) / sizeof(std::tuple<int, int>);
    auto array_end = generated_edges + array_size;
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
        } while (from == to || std::find(generated_edges, array_end, std::make_tuple(from, to)) != array_end);

        // if the graph should be   a dag then the edge will be set from the node of the smaller index to the node of the bigger index
        // if the graph should not be a dag then the edges will be set in random order
        generated_edges[i] = std::make_tuple(from, to);

        dag.nodes_[from].outgoing_edges_.push_back(&dag.nodes_[to]); // add the destination node to the outgoing edges of the origin node
        dag.nodes_[to].incoming_edges_.push_back(&dag.nodes_[from]); // add the origin node to the incoming edges of the destination node

    }
    return dag;
}

void shuffle_graph(graph* graph) {
    // generate a random mapping of indexes. The vector Maps the current index to the new index of each node
    vector<int> index_mapping;
    index_mapping.resize(graph->nodes_.size());
    for(int i = 0; i<graph->nodes_.size(); i++) {
        index_mapping[i] = i;
    }
    std::shuffle(std::begin((index_mapping)), std::end((index_mapping)), rng);

    // generate a vector of new edges that has the same edges than the ones in the graph but with the new indexes
    vector<tuple<int, int>> new_edges;
    for(int i = 0; i<graph->nodes_.size(); i++) {
        node current_node = graph->nodes_[i];
        for(auto to_node : current_node.outgoing_edges_) {
            int to_index = find_node_in_graph(to_node, graph);
            // map the old indexes of the source and target node to the new indexes
            new_edges.emplace_back(index_mapping[i], index_mapping[to_index]);
        }
    }

    auto x = new_edges.size();
    // empty all nodes incoming and outgoing edges
    for(auto & node : graph->nodes_) {
        node.outgoing_edges_.clear();
        node.incoming_edges_.clear();
    }

    // fill the new nodes
    for(auto edge : new_edges) {
        auto node_from = &graph->nodes_[std::get<0>(edge)];
        auto node_to= &graph->nodes_[std::get<1>(edge)];
        (*node_from).outgoing_edges_.push_back(node_to);
        (*node_to).incoming_edges_.push_back(node_from);
    }
}