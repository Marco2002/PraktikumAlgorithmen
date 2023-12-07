#include "gtest/gtest.h"
#include "dagGenerator.h"
#include "dagUtil.h"

TEST(dagUtil, temporary) {
    int num_of_nodes = 10;
    int num_of_edges = 20;
    set_seed(07122023);
    graph dag1 = generate_graph(num_of_nodes, num_of_edges, true);
    EXPECT_NO_THROW(get_topological_order(&dag1));
    //std::vector<node*> topological_order = get_topological_order(&dag1);
}

TEST(dagUtil, topologicalOrderFinderThrowsErrorIfInputIsNotADag) {
    int num_of_nodes = 10;
    int num_of_edges = 20;
    set_seed(07122023);
    graph non_dag = generate_graph(num_of_nodes, num_of_edges, false);
    EXPECT_THROW(std::vector<node*> topological_order = get_topological_order(&non_dag), std::invalid_argument);
}