#include "insertionAlgorithm.h"
#include "dagGenerator.h"
#include "dagUtil.h"

#include <chrono>
#include <iostream>

struct test_set {
    graph dag_;
    std::vector<Edge> new_edges_;
};

// generates a dag with num_of_nodes nodes and num_of_edges edges
// and a set of additional edges of size num_of_new_edges
// some edges will invalidate the topological order of the dag, but the graph will remain a dag
test_set generate_test_set(int num_of_nodes, int num_of_edges, int num_of_new_edges, int seed) {
    set_seed(seed);

    graph dag = generate_graph(num_of_nodes, num_of_edges, true);
    // generate new edges (all of them won't invalidate the current topological order)
    std::vector<Edge> new_edges = generate_extra_edges(dag, num_of_new_edges);

    // bring the dag into another topological order by shuffling and reordering
    // in the new topological order a part of the new edges will be invalidating
    shuffle_graph(dag);
    set_to_topological_order(dag);
    return {
            dag,
            new_edges
    };
}


int main(int argc, char** argv) {
    int num_of_nodes = 5000;
    int num_of_edges = 5000;
    int num_of_new_edges = 40;
    int seed = 10032024;

    test_set test_set = generate_test_set(num_of_nodes, num_of_edges, num_of_new_edges, seed);

    auto start = std::chrono::high_resolution_clock::now();
    // insert new edges
    for(auto [x, y] : test_set.new_edges_) {
        x->outgoing_edges_.push_back(y);
        y->incoming_edges_.push_back(x);
    }
    test_set.dag_.number_of_edges_ += test_set.new_edges_.size();
    set_to_topological_order(test_set.dag_);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "inserted " << num_of_new_edges << " and sorted with Kahn's algorithm. TIME: " << duration.count() << "microseconds\n";

    test_set = generate_test_set(num_of_nodes, num_of_edges, num_of_new_edges, seed);

    start = std::chrono::high_resolution_clock::now();
    // insert new edges
    insert_edges(test_set.dag_, test_set.new_edges_);
    stop = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "inserted " << num_of_new_edges << " and sorted with PK2 algorithm. TIME: " << duration.count() << "microseconds\n";
    return 0;
}