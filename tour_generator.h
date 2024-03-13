#ifndef TOUR_GENERATOR_H
#define TOUR_GENERATOR_H

#include "base_classes.h"

class TourGenerator : public TourGeneratorBase
{
public:
	TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router) : m_database(geodb), m_router(router) {}
	virtual ~TourGenerator() {}
	virtual std::vector<TourCommand> generate_tour(const Stops& stops) const;
private:
	const GeoDatabaseBase& m_database;
	const RouterBase& m_router;
	std::string compute_direction(double angle) const;
};

#endif //TOUR_GENERATOR_H