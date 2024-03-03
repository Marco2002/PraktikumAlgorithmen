#include "gtest/gtest.h"
#include "dagGenerator.h"
#include "dagUtil.h"

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
    // assert that all the node's incoming and outgoing edges lead to nodes that are also part of the graph
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

TEST(dagGenerator, canGenerateDag) {
    int num_of_nodes = 1000;
    int num_of_edges = 2000;
    set_seed(21012024);
    graph dag = generate_graph(num_of_nodes, num_of_edges, true);
    ASSERT_TRUE(graph_is_in_topological_order(&dag));
}

TEST(dagGenerator, canGenerateNonDag) {
    int num_of_nodes = 1000;
    int num_of_edges = 2000;
    set_seed(21012024);
    graph non_dag = generate_graph(num_of_nodes, num_of_edges, false);
    ASSERT_FALSE(graph_is_in_topological_order(&non_dag));
    EXPECT_THROW(std::vector<node*> topological_order = get_topological_order(&non_dag), std::invalid_argument);
}
