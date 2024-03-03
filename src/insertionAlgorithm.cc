#include "insertionAlgorithm.h"
#include <algorithm>
#include <stack>
#include <stdexcept>

// insertion algorithm based on PK2
void insert_edges(graph& dag, std::vector<std::tuple<node*, node*>> edges) {
    // setup
    std::map<node*, int> order;
    std::vector<node*> inv_order;
    std::map<node*, bool> vacant;

    // add edges to graph
    // increment edge count
    for(int i = 0; i<dag.nodes_.size(); ++i) {
        order[&dag.nodes_[i]] = i;
        inv_order[i] = &dag.nodes_[i];

        vacant[&dag.nodes_[i]] = false;
    }

    // calculate new topological order
    for(int i = 0; i<edges.size(); ++i) {
        auto [x, y] = edges[i];
        if(order[x] < order[y]) { // remove forward edges from B
            edges.erase(edges.begin() + i);
        }
    }
    if(edges.empty()) return;

    // sort invalidating edges into descending order by index of tail
    std::sort (edges.begin(), edges.end(), [&order](auto n1, auto n2) -> bool {
        return order[std::get<0>(n1)] > order[std::get<0>(n2)];
    });


}

void shift(int starting_index, std::stack<std::tuple<node*, node*>>& queue, std::map<node*, int>& order, std::vector<node*>& inv_order, std::map<node*, bool>& vacant) {
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
        std::tuple<node*, node*> head = queue.top();
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

std::stack<std::tuple<node*, node*>> discover(std::vector<std::tuple<node*, node*>>& edge_insertions, std::map<node*, int>& order, std::vector<node*>& inv_order, std::map<node*, bool>& vacant) {
    std::stack<std::tuple<node*, node*>> queue;

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

void depth_first_search(node* v, int ub, std::stack<std::tuple<node*, node*>>& queue, std::map<node*, int>& order, std::vector<node*>& inv_order, std::map<node*, bool>& vacant) {
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
