#include "gtest/gtest.h"
#include "dagGenerator.h"

#include <algorithm>

// looks for the given node in the given graph
// return -1 if the node is not inside the graph and the index of the node in the graph's nodes if the node is inside the graph
int find_node_in_graph(node* node, graph* graph) {
    for(int i = 0; i < graph->nodes_.size(); i++) {
        if(&(graph->nodes_[i]) == node) return i;
    }
    return -1;
}

TEST(dagGenerator, isDeterministicWithSeed) {
    int num_of_nodes = 1000;
    int num_of_edges = 2000;
    set_seed(30112023);
    graph dag1 = generate_graph(num_of_nodes, num_of_edges, true);
    set_seed(30112023);
    graph dag2 = generate_graph(num_of_nodes, num_of_edges, true);

    for(int i = 0; i < num_of_nodes; i++) {
        ASSERT_EQ(dag1.nodes_[i].outgoing_edges_.size(), dag2.nodes_[i].outgoing_edges_.size());
        ASSERT_EQ(dag1.nodes_[i].incoming_edges_.size(), dag2.nodes_[i].incoming_edges_.size());

        for(int j = 0; j < dag1.nodes_[i].outgoing_edges_.size(); j++) {

        }
    }
}

TEST(dagGenerator, generatesConsitentGraphs) {
    int number_of_nodes = 1000;
    int number_of_edges = 2000;
    set_seed(30112023);
    graph dag = generate_graph(number_of_nodes, number_of_edges, true);
    graph non_dag = generate_graph(number_of_nodes, number_of_edges, false);

    ASSERT_TRUE(dag.nodes_.size() == number_of_nodes);
    ASSERT_TRUE(dag.number_of_edges_ == number_of_edges);

    ASSERT_TRUE(non_dag.nodes_.size() == number_of_nodes);
    ASSERT_TRUE(non_dag.number_of_edges_ == number_of_edges);
    // assert that all the nodes of all the incoming and outgoing edges of all the nodes are part of the dag
    for(auto& node : dag.nodes_) {
        for(auto n: node.outgoing_edges_) {
            ASSERT_TRUE(find_node_in_graph(n, &dag) >= 0);
        }
        for(auto n: node.outgoing_edges_) {
            ASSERT_TRUE(find_node_in_graph(n, &dag) >= 0);
        }
    }

    // repeat for the normal non_dag
    for(auto& node : non_dag.nodes_) {
        for(auto n: node.outgoing_edges_) {
            ASSERT_TRUE(find_node_in_graph(n, &non_dag) >= 0);
        }
        for(auto n : node.outgoing_edges_) {
            ASSERT_TRUE(find_node_in_graph(n, &non_dag) >= 0);
        }
    }
}
