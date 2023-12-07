#include "gtest/gtest.h"
#include "dagGenerator.h"


bool node_is_in_graph(node* n, graph* graph) {
    bool node_is_in_graph = false;
    for(auto& i : graph->nodes_) {
        node* current_node = &i;
        if(n == current_node) node_is_in_graph = true;
    }
    return node_is_in_graph;
}

bool edge_is_in_graph(edge* e, graph* graph) {
    bool edge_is_in_graph = false;
    for(auto& i:  graph->edges_) {
        edge* current_edge = &i;
        if(e == current_edge) edge_is_in_graph = true;
    }
    return edge_is_in_graph;
}

TEST(dagGenerator, isDeterministicWithSeed) {
    int num_of_nodes = 1000;
    int num_of_edges = 2000;
    set_seed(30112023);
    graph dag1 = generate_graph(num_of_nodes, num_of_edges, true);
    set_seed(30112023);
    graph dag2 = generate_graph(num_of_nodes, num_of_edges, true);

    ASSERT_TRUE(dag1.equals(dag2));
}

TEST(dagGenerator, generatesConsitentGraphs) {
    int num_of_nodes = 1000;
    int num_of_edges = 2000;
    set_seed(30112023);
    graph dag = generate_graph(num_of_nodes, num_of_edges, true);
    graph non_dag = generate_graph(num_of_nodes, num_of_edges, false);

    ASSERT_TRUE(dag.nodes_.size() == num_of_nodes);
    ASSERT_TRUE(dag.edges_.size() == num_of_edges);

    ASSERT_TRUE(non_dag.nodes_.size() == num_of_nodes);
    ASSERT_TRUE(non_dag.edges_.size() == num_of_edges);
    // assert that all the incoming and outgoing edges of all the nodes are part of the dag
    for(auto& node : dag.nodes_) {
        for(auto outgoing_edge: node.outgoing_edges_) {
            ASSERT_TRUE(edge_is_in_graph(outgoing_edge, &dag));
        }
        for(auto incoming_edge: node.outgoing_edges_) {
            ASSERT_TRUE(edge_is_in_graph(incoming_edge, &dag));
        }
    }
    // assert that the two nodes of each edge are also in the dag
    for(auto edge : dag.edges_) {
        ASSERT_TRUE(node_is_in_graph(edge.from_, &dag));
        ASSERT_TRUE(node_is_in_graph(edge.to_, &dag));
    }
    // with both conditions combined we have a guarantee
    // that all the edges of all the nodes lead to nodes that are inside the non_dag
    // and that the nodes of all the edges inside the edges_ member are also inside the non_dag

    // repeat for the normal non_dag
    for(auto& node : non_dag.nodes_) {
        for(auto outgoing_edge: node.outgoing_edges_) {
            ASSERT_TRUE(edge_is_in_graph(outgoing_edge, &non_dag));
        }
        for(auto incoming_edge: node.outgoing_edges_) {
            ASSERT_TRUE(edge_is_in_graph(incoming_edge, &non_dag));
        }
    }
    for(auto edge : non_dag.edges_) {
        ASSERT_TRUE(node_is_in_graph(edge.from_, &non_dag));
        ASSERT_TRUE(node_is_in_graph(edge.to_, &non_dag));
    }
}
