#pragma once
#include <vector>

namespace graphs {
    struct node {
		std::vector<node*> outgoing_edges_;
		std::vector<node*> incoming_edges_;
	};

    struct graph {
        std::vector<node*> nodes_;
        int number_of_edges_;
    };

    using Edge = std::tuple<node*, node*>;
} // namespace dag - contains struct for nodes and edges