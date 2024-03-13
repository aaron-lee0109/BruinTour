#include "tour_generator.h"
#include "geotools.h"

std::vector<TourCommand> TourGenerator::generate_tour(const Stops& stops) const
{
	// Initialize vector to hold the tour commands
	std::vector<TourCommand> instructions;

	// Go through stops
	for (int i = 0; i < stops.size(); i++) {
		std::string poi, talking_points;
		stops.get_poi_data(i, poi, talking_points);

		TourCommand commentary;
		commentary.init_commentary(poi, talking_points);
		instructions.push_back(commentary);

		// If there is another point of interest after, generate route
		if (i + 1 < stops.size()) {
			std::string next_poi, next_talking_points;
			stops.get_poi_data(i + 1, next_poi, next_talking_points);
			GeoPoint g1, g2;
			m_database.get_poi_location(poi, g1);
			m_database.get_poi_location(next_poi, g2);
			std::vector<GeoPoint> route = m_router.route(g1, g2);

			// Generate instructions based on route
			for (int j = 0; j < route.size() - 1; j++) {
				const GeoPoint& currentPoint = route[j];
				const GeoPoint& nextPoint = route[j + 1];
				std::string streetName = m_database.get_street_name(currentPoint, nextPoint);

				// Generate proceed command
				double distance = distance_earth_miles(currentPoint, nextPoint);
				const std::string direction = compute_direction(angle_of_line(currentPoint, nextPoint));
				TourCommand proceed;
				proceed.init_proceed(direction, streetName, distance, currentPoint, nextPoint);
				instructions.push_back(proceed);

				// Check if turn command is needed
				if (j + 2 < route.size()) {
					const GeoPoint& nextNextPoint = route[j + 2];
					std::string nextStreetName = m_database.get_street_name(nextPoint, nextNextPoint);
					double angle = angle_of_turn(currentPoint, nextPoint, nextNextPoint);

					if (streetName != nextStreetName && angle != 0){
						// Generate turn command
						std::string turn_direction;
						if (angle >= 1 and angle < 180)
							turn_direction = "left";
						else if (angle >= 180 && angle <= 359)
							turn_direction = "right";
						TourCommand turn;
						turn.init_turn(turn_direction, nextStreetName);
						instructions.push_back(turn);
					}
				}
			}
		}
	}
	return instructions;
}

std::string TourGenerator::compute_direction(double angle) const{
	if (angle >= 337.5)
		return "east";
	else if (angle >= 292.5)
		return "southeast";
	else if (angle >= 247.5)
		return "south";
	else if (angle >= 202.5)
		return "southwest";
	else if (angle >= 157.5)
		return "west";
	else if (angle >= 112.5)
		return "northwest";
	else if (angle >= 67.5)
		return "north";
	else if (angle >= 22.5)
		return "northeast";
	else
		return "east";
}
