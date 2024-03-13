#include "geodb.h"
#include "geotools.h"
#include "HashMap.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>

bool GeoDatabase::load(const std::string& map_data_file) 
{
	std::ifstream file(map_data_file);
	if(!file.is_open())
		// File not found
		return false;

	std::string line;
	while (std::getline(file, line)) {
		// First line is street line
		std::string streetName = line;
		// Second line containst start and end GeoPoints
		std::getline(file, line);
		std::istringstream iss(line);
		double startLat, startLon, endLat, endLon;
		iss >> startLat >> startLon >> endLat >> endLon;

		std::string startLat_string = std::to_string(startLat);
		std::string startLon_string = std::to_string(startLon);
		std::string endLat_string = std::to_string(endLat);
		std::string endLon_string = std::to_string(endLon);

		// Third line contains number of POIs
		std::getline(file, line);
		int numPOIs = std::stoi(line);

		// Create keys for start, end, and the entire segment geopoints
		GeoPoint endpoint = GeoPoint(endLat_string, endLon_string);
		GeoPoint startpoint = GeoPoint(startLat_string, startLon_string);
		std::string start_point_key = startpoint.to_string();
		std::string end_point_key = endpoint.to_string();
		std::string segment_key = start_point_key + "," + end_point_key;

		// Keep track of connected points
		addToConnected(start_point_key, endpoint);
		addToConnected(end_point_key, startpoint);

		// Keep track of street name
		location_to_street[segment_key] = streetName;

		// Process POIs
		for (int i = 0; i < numPOIs; i++) {
			std::getline(file, line);
			std::istringstream issPoint(line);
			std::string poiName;
			std::string poiLat, poiLon;
			// Extract POI name
			std::getline(issPoint, poiName, '|');
			// Extract POI GeoPoint
			issPoint >> poiLat >> poiLon;

			GeoPoint poi = GeoPoint(poiLat, poiLon);
			std::string poi_key = poi.to_string();

			// Get midpoint
			GeoPoint mid = midpoint(startpoint, endpoint);
			std::string midpoint_key = mid.to_string();

			// Keep track of POI location
			poi_to_location[poiName] = poi;

			// Add additional connections
			addToConnected(start_point_key, mid);
			addToConnected(end_point_key, mid);
			addToConnected(poi_key, mid);
			addToConnected(midpoint_key, poi);
			addToConnected(midpoint_key, startpoint);
			addToConnected(midpoint_key, endpoint);

			// Add additional segments
			std::string start_to_mid_key = start_point_key + "," + midpoint_key;
			std::string end_to_mid_key = end_point_key + "," + midpoint_key;
			std::string poi_to_mid_key = start_point_key + "," + midpoint_key;
			location_to_street[start_to_mid_key] = streetName;
			location_to_street[end_to_mid_key] = streetName;
			location_to_street[poi_to_mid_key] = "a path";
		}
	}
	file.close();
	// File loaded successfully
	return true;
}

bool GeoDatabase::get_poi_location(const std::string& poi, GeoPoint& point) const 
{
	// Search for poi
	const GeoPoint* temp = poi_to_location.find(poi);
	if (temp != nullptr) {
		// If poi found
		point = *temp;
		return true;
	}
	// If poi not found
	return false;
}

std::vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const
{
	// Create key
	std::string key = pt.to_string();
	// Get vector of connected GeoPoints
	const std::vector<GeoPoint>* connected = geopoint_to_connected.find(key);

	// If no connected points
	if(connected == nullptr)
		return {};

	return *connected;
}

 std::string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const
{
	 std::string segment_key = pt1.to_string() + "," + pt2.to_string();
	 const std::string* street = location_to_street.find(segment_key);
	 if (street != nullptr)
		 // Connected
		 return *street;
	 // Not connected
	 return "";
}

void GeoDatabase::addToConnected(std::string key, const GeoPoint& pt)
{
	std::vector<GeoPoint>* connectedListPtr = geopoint_to_connected.find(key);
	if (connectedListPtr == nullptr) {
		geopoint_to_connected[key].push_back(pt);
		return;
	}
	std::vector<GeoPoint>& connectedList = *connectedListPtr;
	for (int i = 0; i < connectedList.size(); i++) {
		if (connectedList[i].latitude == pt.latitude && connectedList[i].longitude == pt.longitude)
			return;
	}
	geopoint_to_connected[key].push_back(pt);
}