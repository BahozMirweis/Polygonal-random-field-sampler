#pragma once

#include <memory>
#include<list>
#include "Birth Site.h"
#include "Metropolis Hastings.h"

struct Vertex {
	double displacement;
	double time;
	std::weak_ptr<Vertex> parent;
	std::weak_ptr<Vertex> child;
	std::weak_ptr<Vertex> parent2;
	std::weak_ptr<Vertex> child2;

	Vertex(double displacement, double time, std::weak_ptr<Vertex> parent = std::weak_ptr<Vertex>{}, std::weak_ptr<Vertex> child = std::weak_ptr<Vertex>{},
		std::weak_ptr<Vertex> parent2 = std::weak_ptr<Vertex>{}, std::weak_ptr<Vertex> child2 = std::weak_ptr<Vertex>{}) : displacement{ displacement }, time{ time },
		parent{ parent }, child{ child }, parent2{ parent2 }, child2{child2} {
	
	}
};

// We construct this for rectangle shape only
class DynamicTessellation {
private:
	double width, height;
public:
	DynamicTessellation() = default;
	DynamicTessellation(double width, double height);
	DynamicTessellation(const DynamicTessellation& d);
	DynamicTessellation& operator= (const DynamicTessellation&);
	std::vector<std::weak_ptr<Vertex>> birthSites;
	std::list<std::shared_ptr<Vertex>> vertices;

	void addBirthsite(const BirthSite& b);
	double jumpNewVelocity(double oldVelocity) const;
	double jumpTimeLogDensity(double u, double velocity) const;
	double jumpLogDensity(double u, double velocity) const;
	double jumpNewTime(double oldVelocity) const;
	bool collision(const std::shared_ptr<Vertex>& u, const std::shared_ptr<Vertex>& v, bool& secondParent) const;
	void jumpNewTimeVelocity(double& time, double& velocity, double& displacement);
	void drawTessellation();
	std::pair<double, double> boundaryCollision(const std::shared_ptr<Vertex>& u) const;

	void updateParticle(std::shared_ptr<Vertex>& currParticle, std::list<std::shared_ptr<Vertex>>::iterator& i, double& displacement, double& time, double& velocity, std::shared_ptr<Vertex>& particle2);

	void addParticleToEndOfList(std::shared_ptr<Vertex>& particle, double& displacement, double& time, double& velocity);

	void removeParticle(int pos);

	void addTwoParticles(std::shared_ptr<Vertex>& particle1, std::shared_ptr<Vertex>& particle2, double& displacement1, double& time1, double& velocity1, double& displacement2, double& time2, double& velocity2);

	// Displacement, time
	std::pair<double, double> intersectionPoint(const std::shared_ptr<Vertex>& u, const std::shared_ptr<Vertex>& v, bool usecondParent = false) const;

	void drawAllVertices();
};