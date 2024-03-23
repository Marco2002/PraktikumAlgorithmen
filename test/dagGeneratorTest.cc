#include "gtest/gtest.h"
#include "dagGenerator.h"
#include "dagUtil.h"

TEST(dagGenerator, nodeIndexesAreConsistent) {
    int num_of_nodes = 1000;
    int num_of_edges = 2000;
    set_seed(30112023);
    graph dag1 = generate_graph(num_of_nodes, num_of_edges, false);
    for(int i = 0; i < num_of_nodes; ++i) {
        ASSERT_EQ(i, dag1.nodes_[i]->index_);
    }
}

TEST(dagGenerator, isDeterministicWithSeed) {
    int num_of_nodes = 1000;
    int num_of_edges = 2000;
    set_seed(30112023);
    graph dag1 = generate_graph(num_of_nodes, num_of_edges, true);
    set_seed(30112023);
    graph dag2 = generate_graph(num_of_nodes, num_of_edges, true);

    for(int i = 0; i < num_of_nodes; i++) {
        ASSERT_EQ(dag1.nodes_[i]->outgoing_edges_.size(), dag2.nodes_[i]->outgoing_edges_.size());
        ASSERT_EQ(dag1.nodes_[i]->incoming_edges_.size(), dag2.nodes_[i]->incoming_edges_.size());

        for(int j = 0; j < dag1.nodes_[i]->outgoing_edges_.size(); j++) {
            ASSERT_EQ(dag1.nodes_[i]->outgoing_edges_[j]->index_, dag2.nodes_[i]->outgoing_edges_[j]->index_);
        }
        for(int j = 0; j < dag1.nodes_[i]->incoming_edges_.size(); j++) {
            ASSERT_EQ(dag1.nodes_[i]->incoming_edges_[j]->index_, dag2.nodes_[i]->incoming_edges_[j]->index_);
        }
    }
}

TEST(dagGenerator, generatesConsitentGraphs) {
    int number_of_nodes = 1000;
    int number_of_edges = 2000;
    set_seed(30112023);
    graph dag = generate_graph(number_of_nodes, number_of_edges, true);
    graph non_dag = generate_graph(number_of_nodes, number_of_edges, false);

    ASSERT_EQ(dag.nodes_.size(), number_of_nodes);
    ASSERT_EQ(dag.number_of_edges_, number_of_edges);

    ASSERT_EQ(non_dag.nodes_.size(), number_of_nodes);
    ASSERT_EQ(non_dag.number_of_edges_, number_of_edges);
    // assert that all the node's incoming and outgoing edges lead to nodes that are also part of the graph
    for(auto& node : dag.nodes_) {
        for(auto n: node->outgoing_edges_) {
            ASSERT_TRUE(find_node_in_graph(n, dag) >= 0);
        }
        for(auto n: node->outgoing_edges_) {
            ASSERT_TRUE(find_node_in_graph(n, dag) >= 0);
        }
    }

    // repeat for the normal non_dag
    for(auto& node : non_dag.nodes_) {
        for(auto n: node->outgoing_edges_) {
            ASSERT_TRUE(find_node_in_graph(n, non_dag) >= 0);
        }
        for(auto n : node->outgoing_edges_) {
            ASSERT_TRUE(find_node_in_graph(n, non_dag) >= 0);
        }
    }
}

TEST(dagGenerator, canGenerateDag) {
    int num_of_nodes = 1000;
    int num_of_edges = 2000;
    set_seed(21012024);
    graph dag = generate_graph(num_of_nodes, num_of_edges, true);
    ASSERT_TRUE(graph_is_in_topological_order(dag));
}

TEST(dagGenerator, canGenerateNonDag) {
    int num_of_nodes = 1000;
    int num_of_edges = 2000;
    set_seed(21012024);
    graph non_dag = generate_graph(num_of_nodes, num_of_edges, false);
    ASSERT_FALSE(graph_is_in_topological_order(non_dag));
    EXPECT_THROW(set_to_topological_order(non_dag), std::invalid_argument);
}

TEST(dagGenerator, worksInEdgeCases) {
    int num_of_nodes = 2; // minimal input values
    int num_of_edges = 0;
    set_seed(21012024);
    graph g = generate_graph(num_of_nodes, num_of_edges, true);
    ASSERT_TRUE(graph_is_in_topological_order(g));

    num_of_nodes = 100;
    num_of_edges = num_of_nodes * (num_of_nodes-1); // full non dag (density = 1)
    set_seed(21012024);
    g = generate_graph(num_of_nodes, num_of_edges, false);
    ASSERT_FALSE(graph_is_in_topological_order(g));
    EXPECT_THROW(set_to_topological_order(g), std::invalid_argument);

    num_of_nodes = 100;
    num_of_edges = num_of_nodes * (num_of_nodes-1)/2; // full dag (density = 1)
    set_seed(21012024);
    g = generate_graph(num_of_nodes, num_of_edges, true);
    ASSERT_TRUE(graph_is_in_topological_order(g));
}

TEST(dagGenerator, throwsInvalidArgumentWhenArgumentsInvalid) {
    int num_of_nodes = 1; // too few nodes
    int num_of_edges = 0;
    set_seed(21012024);
    EXPECT_THROW(generate_graph(num_of_nodes, num_of_edges, false), std::invalid_argument);

    num_of_nodes = 2;
    num_of_edges = -1; // invalid edge number
    EXPECT_THROW(generate_graph(num_of_nodes, num_of_edges, false), std::invalid_argument);

    num_of_nodes = 100;
    num_of_edges = num_of_nodes * (num_of_nodes-1)/2 +1; // too many edges for dag
    EXPECT_THROW(generate_graph(num_of_nodes, num_of_edges, true), std::invalid_argument);
    graph g = generate_graph(num_of_nodes, num_of_edges, false); // expect no throw for non dag

    num_of_nodes = 100;
    num_of_edges = num_of_nodes * (num_of_nodes-1) +1; // too many edges for non dag
    EXPECT_THROW(generate_graph(num_of_nodes, num_of_edges, false), std::invalid_argument);
}


