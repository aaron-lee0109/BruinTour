#ifndef ROUTER_H
#define ROUTER_H

#include "base_classes.h"
#include "geopoint.h"

class Router : public RouterBase
{
public:
	Router(const GeoDatabaseBase& geo_db) : database(geo_db) {}
	virtual ~Router() {}
	virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;
private:
	const GeoDatabaseBase& database;

	struct Node {
		GeoPoint point;
		double cost; // Cost from start to node
		double heuristic; // estimated cost to goal
		Node* parent; // Parent node in optimal path
		Node(GeoPoint pt, double co, double he, Node* pa) {
			point = pt;
			cost = co;
			heuristic = he;
			parent = pa;
		}
	};

	// Comparison function for priority queue
	struct CompareNodes {
		bool operator()(const Node* lhs, const Node* rhs) const {
			return (lhs->cost + lhs->heuristic) > (rhs->cost + rhs->heuristic);
		}
	};
};

#endif // ROUTER_H