#include "insertionAlgorithm.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

// insertion algorithm based on PK2
void insert_edges(graph& dag, std::vector<Edge>& edges) {
    // setup
    std::unordered_map<node*, int> order;
    std::vector<node*>* inv_order = &dag.nodes_;
    std::unordered_map<node*, bool> vacant;
    std::vector<Edge> invalidating_edges;

    for(int i = 0; i<dag.nodes_.size(); ++i) {
        order[dag.nodes_[i]] = i;
    }

    // add edges to graph
    for(auto [x, y] : edges) {
        x->outgoing_edges_.push_back(y);
        y->incoming_edges_.push_back(x);

        if(order[x] >= order[y]) { // remove forward edges from B
            invalidating_edges.emplace_back(x, y);
        }
    }
    dag.number_of_edges_ += edges.size();

    std::cout << "Inserting " << edges.size() << " edges\n";
    std::cout << "of which " << invalidating_edges.size() << " are invalidating\n";

    if(invalidating_edges.empty()) return;

    // calculate new topological order (PK2)

    // sort invalidating edges into descending order by index of tail
    std::sort (invalidating_edges.begin(), invalidating_edges.end(), [&order](auto n1, auto n2) -> bool {
        return order[std::get<0>(n1)] > order[std::get<0>(n2)];
    });
    int lower_bound = dag.nodes_.size();
    int start = 0;

    for(int i = 0; i < invalidating_edges.size(); ++i) {
        auto [x, y] = invalidating_edges[i];
        if(order[x] < lower_bound && i > 0) {
            std::vector<Edge> edge_region = {invalidating_edges.begin() + start, invalidating_edges.begin() + i};
            auto queue = discover(edge_region, order, *inv_order, vacant);
            shift(lower_bound, queue, order, *inv_order, vacant);
            start = i; // set the start of new region
        }
        lower_bound = std::min(order[y], lower_bound);
    }
    // Process final region
    std::vector<Edge> edge_region = {invalidating_edges.begin() + start, invalidating_edges.end()};
    auto queue = discover(edge_region, order, *inv_order, vacant);
    shift(lower_bound, queue, order, *inv_order, vacant);
}

void shift(int starting_index, EdgeQueue& queue, std::unordered_map<node*, int>& order, std::vector<node*>& inv_order, std::unordered_map<node*, bool>& vacant) {
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

void place_node(node* n, int i, std::unordered_map<node*, int>& order, std::vector<node*>& inv_order) {
    order[n] = i;
    inv_order[i] = n;
}

EdgeQueue discover(std::vector<Edge>& edge_insertions, std::unordered_map<node*, int>& order, std::vector<node*>& inv_order, std::unordered_map<node*, bool>& vacant) {
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

std::unordered_map<node*, bool> visited = {};

void depth_first_search(node* start, int ub, EdgeQueue& queue, std::unordered_map<node*, int>& order, std::vector<node*>& inv_order, std::unordered_map<node*, bool>& vacant) {
    std::stack<node*> stack;
    stack.push(start);
    vacant[start] = true;
    visited[start] = true;

    while(!stack.empty()) {
        node* v = stack.top();
        bool has_unvisited_neighbor = false;

        for (auto dest : v->outgoing_edges_) {
            if (visited[dest]) {
                throw std::runtime_error("cycle detected");
            }
            if (!vacant[dest] && order[dest] < ub) {
                stack.push(dest);
                vacant[dest] = true;
                visited[dest] = true;
                has_unvisited_neighbor = true;
                break; // Move to the next unvisited neighbor
            }
        }

        if (!has_unvisited_neighbor) {
            stack.pop();
            visited[v] = false; // TODO check if this can be done here
            queue.emplace(v, inv_order[ub]);
        }
    }
}
