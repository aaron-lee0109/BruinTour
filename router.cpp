#include "router.h"
#include "geotools.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>

std::vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const
{
	// If pt1 and pt2 are the same
	if (pt1.longitude == pt2.longitude && pt1.latitude == pt2.latitude)
		return { pt1 };

	// Create priority queue and open set
	std::priority_queue < Node*, std::vector<Node*>, CompareNodes> open_set;
	std::unordered_set<std::string> closed_set;

	// Initialize start node
	Node* start_node = new Node(pt1, 0.0, distance_earth_km(pt1, pt2), nullptr );
	open_set.push(start_node);

	// Main loop
	while (!open_set.empty()) {
		// Get the node with the lowest total cost from the priority queue
		Node* current = open_set.top();
		open_set.pop();

		// Check if currentnnode is the goal
		if (current->point.latitude == pt2.latitude && current->point.longitude == pt2.longitude) {
			// Reconstruct the path
			std::vector<GeoPoint> path;
			while (current != nullptr) {
				path.push_back(current->point);
				current = current->parent;
			}
			std::reverse(path.begin(), path.end());
			return path;
		}

		// Add current node to the closed set
		closed_set.insert(current->point.to_string());

		std::vector<GeoPoint> connected_points = database.get_connected_points(current->point);
		// Explore neighbors of current node
		for (const auto& neighbor : connected_points) {
			if (closed_set.find(neighbor.to_string()) != closed_set.end()) {
				continue; // Skip nodes already visited
			}

			double tentative_cost = current->cost + distance_earth_km(current->point, neighbor);

			// Check if neighbor is not in the open set or if new cost is lower
			Node* neighbor_node = new Node(neighbor, tentative_cost, distance_earth_km(neighbor, pt2), current);
			open_set.push(neighbor_node);
		}
	}
	// No path found
	return {};
}
