#include "insertionAlgorithm.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

// insertion algorithm based on PK2
void insert_edges(graph& dag, std::vector<Edge>& edges) {
    // setup
    std::map<node*, int> order;
    std::vector<node*> inv_order(dag.nodes_.size());
    std::map<node*, bool> vacant;


    for(int i = 0; i<dag.nodes_.size(); ++i) {
        order[dag.nodes_[i]] = i;
        inv_order[i] = dag.nodes_[i];

        vacant[dag.nodes_[i]] = false;
    }

    // add edges to graph
    std::cout << "Inserting " << edges.size() << " edges\n";
    for(auto [x, y] : edges) {
        x->outgoing_edges_.push_back(y);
        y->incoming_edges_.push_back(x);
    }
    dag.number_of_edges_ += edges.size();

    // calculate new topological order (PK2)
    int deleted_edges = 0;
    for(int i = 0; i<edges.size() + deleted_edges; ++i) {
        auto [x, y] = edges[i - deleted_edges];
        if(order[x] < order[y]) { // remove forward edges from B
            edges.erase(edges.begin() + i - deleted_edges);
            ++deleted_edges;
        }
    }
    std::cout << "of which " << edges.size() << " are invalidating\n";

    if(edges.empty()) return;

    // sort invalidating edges into descending order by index of tail
    std::sort (edges.begin(), edges.end(), [&order](auto n1, auto n2) -> bool {
        return order[std::get<0>(n1)] > order[std::get<0>(n2)];
    });
    int lower_bound = dag.nodes_.size();
    int start = 0;

    for(int i = 0; i < edges.size(); ++i) {
        auto [x, y] = edges[i];
        if(order[x] < lower_bound && i > 0) {
            std::vector<Edge> edge_region = {edges.begin() + start, edges.begin() + i};
            auto queue = discover(edge_region, order, inv_order, vacant);
            shift(lower_bound, queue, order, inv_order, vacant);
            start = i; // set the start of new region
        }
        lower_bound = std::min(order[y], lower_bound);
    }
    // Process final region
    std::vector<Edge> edge_region = {edges.begin() + start, edges.end()};
    auto queue = discover(edge_region, order, inv_order, vacant);
    shift(lower_bound, queue, order, inv_order, vacant);

    // set the new topological order
    dag.nodes_ = inv_order;
}

void shift(int starting_index, EdgeQueue& queue, std::map<node*, int>& order, std::vector<node*>& inv_order, std::map<node*, bool>& vacant) {
    int num_of_removed_nodes = 0;
    int i = starting_index;
    while(!queue.empty()) {
        node* current_node = inv_order[i];
        if(vacant[current_node]) {
            ++num_of_removed_nodes;
            vacant[current_node] = false;
        } else {
            place_node(current_node, i - num_of_removed_nodes, order, inv_order);
        }

        // insert all nodes associated with index i
        Edge head = queue.top();
        while(!queue.empty() && current_node == std::get<1>(head)) {
            --num_of_removed_nodes;
            place_node(std::get<0>(head), i - num_of_removed_nodes, order, inv_order);
            queue.pop();
            if(!queue.empty()) head = queue.top();
        }
        ++i;
    }
}

void place_node(node* n, int i, std::map<node*, int>& order, std::vector<node*>& inv_order) {
    order[n] = i;
    inv_order[i] = n;
}

EdgeQueue discover(std::vector<Edge>& edge_insertions, std::map<node*, int>& order, std::vector<node*>& inv_order, std::map<node*, bool>& vacant) {
    EdgeQueue queue;

    // sort invalidating edges into descending order by index of tail
    std::sort (edge_insertions.begin(), edge_insertions.end(), [&order](auto n1, auto n2) -> bool {
        return order[std::get<0>(n1)] > order[std::get<0>(n2)];
    });
    for(auto [x, y] : edge_insertions) {
        if(!vacant[y]) {
            depth_first_search(y, order[x], queue, order, inv_order, vacant);
        }
    }
    return queue;
}

std::map<node*, bool> visited = {};

void depth_first_search(node* v, int ub, EdgeQueue& queue, std::map<node*, int>& order, std::vector<node*>& inv_order, std::map<node*, bool>& vacant) {
    vacant[v] = true;
    visited[v] = true;
    for(auto dest : v->outgoing_edges_) {
        if(visited[dest]) throw std::runtime_error("cycle detected");
        if(!vacant[dest] && order[dest] < ub) {
            depth_first_search(dest, ub, queue, order, inv_order, vacant);
        }
    }
    visited[v] = false;
    queue.emplace(v, inv_order[ub]);
}
