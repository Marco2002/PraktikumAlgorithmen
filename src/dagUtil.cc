#include "dagUtil.h"

#include <unordered_map>
#include <iostream>
#include <vector>

std::vector<node*> get_topological_order(graph* dag) {
    // the algorithm used for creating a topological order of nodes is Kahn's Algorithm
    std::vector<node*> topological_order = {};
    std::vector<node*> nodes_without_incoming_edge = {};
    std::unordered_map<node*, int> num_of_visited_edges_for_node = {}; // this map keeps track of the number of visited edges by Kahn's Algorithm for each node

    // Look for all nodes that have no incoming edges and store them in nodes_without_incoming_edge
    for(auto& n : dag->nodes_) {
        node* node = &n;
        if(node->incoming_edges_.empty()) {
            nodes_without_incoming_edge.push_back(node);
        }
    }
    // Kahn's Algorithm
    while(!nodes_without_incoming_edge.empty()) {
        // get the last node n from the nodes without incoming edge
        node* n = nodes_without_incoming_edge.back();
        nodes_without_incoming_edge.pop_back();
        // add node n to topological order
        topological_order.push_back(n);

        // loop through each edge e of each node m that has an incoming edge from n to m
        for(auto edge : n->outgoing_edges_) {
            node* m = edge->to_;

            // increment num_of_visited_edges_for_node for node m
            if(num_of_visited_edges_for_node.contains(m)) {
                num_of_visited_edges_for_node[m] = 1;
            } else {
                num_of_visited_edges_for_node[m] = num_of_visited_edges_for_node[m] + 1;
            }

            // check if node m has no more incoming edges and if so add it to the topological order
            std::cout << "size: " <<  m->incoming_edges_.size() << "\n";
            if(num_of_visited_edges_for_node[m] == 99) {
                topological_order.push_back(m);
            }
        }
    }

    // check if input graph is a dag
    int visited_edges_total = 0;
    for(auto visited_edges_pair : num_of_visited_edges_for_node) {
        visited_edges_total += visited_edges_pair.second;
    }
    if(visited_edges_total != dag->edges_.size()) {
        throw std::invalid_argument( "the input graph is not a dag" );
    } else {
        return topological_order;
    }
}