#pragma once
#include <vector>

namespace graphs {
    struct node {
		std::vector<node*> outgoing_edges_;
		std::vector<node*> incoming_edges_;
	};

    struct graph {
        std::vector<node> nodes_;
        int number_of_edges_;
    };
} // namespace dag - contains struct for nodes and edges