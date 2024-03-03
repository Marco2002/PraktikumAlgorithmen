#include "gtest/gtest.h"
#include "insertionAlgorithm.h"

TEST(insertionAlgorithm, ShiftTestDeterministic) {
    int starting_index = 0;
    int number_of_nodes = 9;
    // hard coded test case based on graph from Figure 3
    // in "A Batch Algorithm for Maintaining a Topological Order" by David J Pearce
    std::vector<node> nodes;
    nodes.resize(number_of_nodes);
    for(int i = 0; i < number_of_nodes; ++i) {
        nodes[i] = {};
    }
    std::map<node*, int> order;
    std::vector<node*> inv_order;
    std::map<node*, bool> vacant;
    for(int i = 0; i < number_of_nodes; ++i) {
        order[&nodes[i]] = i;
        inv_order.push_back(&nodes[i]);
        vacant[&nodes[i]] = false;
    }

    // new edges in queue
    std::stack<std::tuple<node*, node*>> queue;

    queue.emplace(&nodes[2], &nodes[8]);
    queue.emplace(&nodes[4], &nodes[8]);
    queue.emplace(&nodes[0], &nodes[6]);

    // vacant slots
    vacant[&nodes[0]] = true;
    vacant[&nodes[2]] = true;
    vacant[&nodes[4]] = true;

    shift(starting_index,queue, order, inv_order, vacant);
    // compare solution with result in paper
    ASSERT_EQ(inv_order[0], &nodes[1]);
    ASSERT_EQ(inv_order[1], &nodes[3]);
    ASSERT_EQ(inv_order[2], &nodes[5]);
    ASSERT_EQ(inv_order[3], &nodes[6]);
    ASSERT_EQ(inv_order[4], &nodes[0]);
    ASSERT_EQ(inv_order[5], &nodes[7]);
    ASSERT_EQ(inv_order[6], &nodes[8]);
    ASSERT_EQ(inv_order[7], &nodes[4]);
    ASSERT_EQ(inv_order[8], &nodes[2]);
}

TEST(insertionAlgorithm, DiscoverTestDeterministic) {
    int number_of_nodes = 9;
    // hard coded test case based on graph from Affected Region 3 in Figure 2
    // in "A Batch Algorithm for Maintaining a Topological Order" by David J Pearce
    std::vector<node> nodes;
    std::vector<std::tuple<node*, node*>> edge_insertions;
    nodes.resize(number_of_nodes);
    for(int i = 0; i < number_of_nodes; ++i) {
        nodes[i] = {};
    }
    std::map<node*, int> order;
    std::vector<node*> inv_order;
    std::map<node*, bool> vacant;
    for(int i = 0; i < number_of_nodes; ++i) {
        order[&nodes[i]] = i;
        inv_order.push_back(&nodes[i]);
        vacant[&nodes[i]] = false;
    }

    nodes[3].outgoing_edges_.push_back(&nodes[5]);
    nodes[5].incoming_edges_.push_back(&nodes[3]);
    nodes[6].outgoing_edges_.push_back(&nodes[8]);
    nodes[8].incoming_edges_.push_back(&nodes[6]);
    // invalidating edges
    edge_insertions.emplace_back(&nodes[8], &nodes[4]);
    nodes[8].outgoing_edges_.push_back(&nodes[4]);
    nodes[4].incoming_edges_.push_back(&nodes[8]);
    edge_insertions.emplace_back(&nodes[4], &nodes[2]);
    nodes[4].outgoing_edges_.push_back(&nodes[2]);
    nodes[2].incoming_edges_.push_back(&nodes[4]);
    edge_insertions.emplace_back(&nodes[6], &nodes[0]);
    nodes[6].outgoing_edges_.push_back(&nodes[0]);
    nodes[0].incoming_edges_.push_back(&nodes[6]);

    auto queue = discover(edge_insertions, order, inv_order, vacant);

    for(int i = 0; i<number_of_nodes; ++i) {
        ASSERT_TRUE((i == 0 || i == 2 || i == 4) ? vacant[inv_order[i]] : !vacant[inv_order[i]]);
    }

    auto [n1, n2] = queue.top();
    ASSERT_EQ(n1, &nodes[0]);
    ASSERT_EQ(n2, &nodes[6]);
    queue.pop();
    auto [n3, n4] = queue.top();
    ASSERT_EQ(n3, &nodes[4]);
    ASSERT_EQ(n4, &nodes[8]);
    queue.pop();
    auto [n5, n6] = queue.top();
    ASSERT_EQ(n5, &nodes[2]);
    ASSERT_EQ(n6, &nodes[8]);
    queue.pop();
    ASSERT_TRUE(queue.empty());
}
