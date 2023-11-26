#pragma once
#include <vector>

namespace dag {
    struct edge;
    typedef struct edge edge;

    struct node {
		std::vector<edge> outgoing_edges_;
		std::vector<edge> incoming_edges_;
	};

	struct edge {
		node from_;
		node to_;
	};
} // namespace dag - contains struct for nodes and edges