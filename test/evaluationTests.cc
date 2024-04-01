#include <fstream>
#include <filesystem>

#include "gtest/gtest.h"

#include "insertionAlgorithm.h"
#include "dagGenerator.h"
#include "dagUtil.h"

struct test_set_params {
    long number_of_nodes_;
    double density_;
    int batch_size_;
    int num_of_runs_;
};

struct test_set {
    graph dag_;
    std::vector<Edge> new_edges_;
};

// generates a dag with num_of_nodes nodes and num_of_edges edges
// and a set of additional edges of size num_of_new_edges
// some edges will invalidate the topological order of the dag, but the graph will remain a dag
test_set generate_test_set(long num_of_nodes, long long num_of_edges, long num_of_new_edges, int seed) {
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
std::chrono::microseconds run_test_pk2(test_set& test_set) {
    auto start = std::chrono::high_resolution_clock::now();
    // insert new edges
    insert_edges(test_set.dag_, test_set.new_edges_);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "inserted " << test_set.new_edges_.size() << " edges and sorted with PK2 algorithm. TIME: " << duration.count() << "microseconds\n";
    return duration;
}

// runs a test set on the Kahn's algorithm and returns the duration
std::chrono::microseconds run_test_kahn(test_set& test_set) {
    auto const start = std::chrono::high_resolution_clock::now();
    // insert new edges
    for(auto const [x, y] : test_set.new_edges_) {
        x->outgoing_edges_.push_back(y);
        y->incoming_edges_.push_back(x);
    }
    test_set.dag_.number_of_edges_ += test_set.new_edges_.size();
    set_to_topological_order(test_set.dag_);
    auto const stop = std::chrono::high_resolution_clock::now();
    auto const duration = duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "inserted " << test_set.new_edges_.size() << " edges and sorted with Kahn's algorithm. TIME: " << duration.count() << "microseconds\n";
    return duration;
}


// runs a test with the given parameters 10 times returns the all the durations
// the 10 runs have the same number_of_nodes, number_of_edges, num_of_new_edges params, but different seeds
std::vector<std::chrono::microseconds> run_test_repeated(long num_of_nodes, long long num_of_edges, long num_of_new_edges, int num_of_runs, int start_seed, bool pk2) {
    std::vector<std::chrono::microseconds> durations(num_of_runs);
    for(int i = 0; i < num_of_runs; ++i) {
        auto test_set = generate_test_set(num_of_nodes, num_of_edges, num_of_new_edges, start_seed);
        ++start_seed;
        if(pk2) {
            durations[i] = run_test_pk2(test_set);
        } else {
            durations[i] = run_test_kahn(test_set);
        }
    }
    return durations;
}

void run_full_tests(std::vector<test_set_params> test_params, int seed, bool pk2) {
    std::ofstream resultsFile(pk2 ? "./resultsPK2.csv" : "./resultsKahn.csv"); // When running this test over CLion the csv lands in cmake-build-debug/test/resultsPK2.csv

    if (!resultsFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
    }
    resultsFile << "number of nodes, number of edges, number of new edges, duration in microseconds\n";

    for(auto [num_of_nodes, density, num_of_new_edges, num_of_runs] : test_params) {
        long long max_num_of_edges = ((long long) num_of_nodes)/2 * (((long long) num_of_nodes)-1);
        long long num_of_edges = max_num_of_edges * density;

        auto const durations = run_test_repeated(num_of_nodes, num_of_edges, num_of_new_edges, num_of_runs, seed, pk2);
        double mean = 0;
        for(auto const duration : durations) {
            resultsFile << num_of_nodes << ", " << num_of_edges << ", " << num_of_new_edges << ", "  << duration.count() << "\n";
            mean += duration.count();
        }
        mean = mean/num_of_runs;
        resultsFile << mean << "\n";
    }
}

TEST(evaluate, evaluationTests) {
    std::vector<test_set_params> test_params = {};

    // small graph
    test_params.emplace_back(100, 0.001, 10, 10);
    test_params.emplace_back(100, 0.01, 10, 10);
    test_params.emplace_back(100, 0.1, 10, 10);
    test_params.emplace_back(100, 0.001, 100, 10);
    test_params.emplace_back(100, 0.01, 100, 10);
    test_params.emplace_back(100, 0.1, 100, 10);

    // medium graph
    test_params.emplace_back(10000, 0.001, 1000, 10);
    test_params.emplace_back(10000, 0.01, 1000, 10);
    test_params.emplace_back(10000, 0.1, 1000, 10);
    test_params.emplace_back(10000, 0.001, 5000, 10);
    test_params.emplace_back(10000, 0.01, 5000, 10);
    test_params.emplace_back(10000, 0.1, 5000, 10);

    // large graph
    test_params.emplace_back(1000000, 0.0001, 5000, 1);
    test_params.emplace_back(1000000, 0.0002, 5000, 1);
    test_params.emplace_back(1000000, 0.0001, 50000, 1);
    test_params.emplace_back(1000000, 0.0002, 50000, 1);



    int seed = 11032034;

    run_full_tests(test_params, seed, true);
    run_full_tests(test_params, seed, false);
}
