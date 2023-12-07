#pragma once
#include <vector>

namespace graphs {
    struct edge;
    typedef struct edge edge;

    struct node {
		std::vector<edge*> outgoing_edges_;
		std::vector<edge*> incoming_edges_;
	};

	struct edge {
		node* from_;
		node* to_;
	};

    struct graph {
        std::vector<node> nodes_;
        std::vector<edge> edges_;

    public :
        bool equals(graph other) {
            if(edges_.size() != other.edges_.size() || nodes_.size() != other.nodes_.size()) return false;
            for(int i = 0; i < nodes_.size(); ++i) {
                if(nodes_[i].outgoing_edges_.size() != other.nodes_[i].outgoing_edges_.size() || nodes_[i].incoming_edges_.size() != other.nodes_[i].incoming_edges_.size()) return false;
//                for(int j = 0; j < nodes_[i].outgoing_edges_.size(); j++) {
//                    node to = nodes_[i].outgoing_edges_[j].to_;
//                    node otherTo = other.nodes_[i].outgoing_edges_[j].to_;
//
//                }
            }
            return true;
        }
    };
} // namespace dag - contains struct for nodes and edges