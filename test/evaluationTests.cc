#include "gtest/gtest.h"
#include "insertionAlgorithm.h"
#include "dagGenerator.h"
#include "dagUtil.h"

#include <fstream>
#include <chrono>
#include <numeric>
#include <filesystem>

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

// runs a test set on the PK2 algorithm and returns the duration
std::chrono::microseconds run_test_pk2(test_set test_set) {
    auto start = std::chrono::high_resolution_clock::now();
    // insert new edges
    insert_edges(test_set.dag_, test_set.new_edges_);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "inserted " << test_set.new_edges_.size() << " and sorted with PK2 algorithm. TIME: " << duration.count() << "microseconds\n";
    return duration;
}

// runs a test set on the Kahn's algorithm and returns the duration
std::chrono::microseconds run_test_kahn(test_set test_set) {
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
    std::cout << "inserted " << test_set.new_edges_.size() << " and sorted with Kahn's algorithm. TIME: " << duration.count() << "microseconds\n";
    return duration;
}


// runs a test with the given parameters 10 times returns the all the durations
// the 10 runs have the same number_of_nodes, number_of_edges, num_of_new_edges params, but different seeds
std::vector<std::chrono::microseconds> run_test_repeated(int num_of_nodes, int num_of_edges, int num_of_new_edges, int start_seed, bool pk2) {
    std::vector<std::chrono::microseconds> durations(10);
    for(int i = 0; i < 10; ++i) {
        auto test_set = generate_test_set(num_of_nodes, num_of_edges, num_of_new_edges, start_seed);
        start_seed++;
        if(pk2) {
            durations[i] = run_test_pk2(test_set);
        } else {
            durations[i] = run_test_kahn(test_set);
        }
    }
    return durations;
}

void run_full_test(std::vector<int> num_of_nodes_options, std::vector<double> density_options, std::vector<double> insertion_density_options, int seed, bool pk2) {
    std::ofstream resultsFile(pk2 ? "./resultsPK2.csv" : "./resultsKahn.csv"); // When running this test over CLion the csv lands in cmake-build-debug/test/resultsPK2.csv

    if (!resultsFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
    }
    resultsFile << "number of nodes, number of edges, number of new edges, duration in microseconds\n";

    for(int num_of_nodes : num_of_nodes_options) {
        for(double density : density_options) {
            int const max_num_of_edges = num_of_nodes * (num_of_nodes-1)/2;
            int const num_of_edges = max_num_of_edges * density;

            for(double insertion_density : insertion_density_options) {
                int const num_of_new_edges = max_num_of_edges * insertion_density;
                if(num_of_new_edges == 0) continue;

                auto durations = run_test_repeated(num_of_nodes, num_of_edges, num_of_new_edges, seed, pk2);
                double mean = 0;
                for(auto duration : durations) {
                    resultsFile << num_of_nodes << ", " << num_of_edges << ", " << num_of_new_edges << ", "  << duration.count() << "\n";
                    mean += duration.count();
                }
                mean = mean/durations.size();
                resultsFile << mean << "\n";
            }
        }
    }
}

TEST(evaluate, KahnsAlgorithmApproach) {
    std::vector<int> num_of_nodes_options = {1000000};
    std::vector<double> density_options = {0.001, 0.01};
    std::vector<double> insertion_density_options = {0.000001, 0.0001, 0.001};
    int seed = 11032024;

    run_full_test(num_of_nodes_options, density_options, insertion_density_options, seed, false);
}

TEST(evaluate, PK2Approach) {
    std::vector<int> num_of_nodes_options = {1000000};
    std::vector<double> density_options = {0.001, 0.01};
    std::vector<double> insertion_density_options = {0.000001, 0.0001, 0.001};
    int seed = 11032024;

    run_full_test(num_of_nodes_options, density_options, insertion_density_options, seed, true);
}
