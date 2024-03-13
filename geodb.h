#ifndef GEODB_H
#define GEODB_H

#include "base_classes.h"
#include "geopoint.h"
#include "HashMap.h"
#include <string>
#include <vector>

class GeoDatabase : public GeoDatabaseBase
{
public:
	GeoDatabase() {}
	virtual ~GeoDatabase() {}
	virtual bool load(const std::string& map_data_file);
	virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;
	virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
	virtual std::string get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const;
private:
	HashMap<GeoPoint> poi_to_location;
	HashMap<std::vector<GeoPoint>> geopoint_to_connected;
	HashMap<std::string> location_to_street;
	void addToConnected(const std::string key, const GeoPoint& pt);

};

#endif // GEODB_H