#pragma once
#include "Birth Site old.h"

class DynamicGraph {
private:
	std::function<std::pair<double, double>(double)> shape;
	std::vector<Particle*> particles;
	double maxTime;

	void initBirthSites(double time);
	// In case you have boundary birth site put in same particle pointers
	void ChangeGraph(double startTime, double timeChange, Particle* aboveParticle, Particle* belowParticle);

	double calculateDeathPoint(Particle* p1, Particle* p2, double time);
public:
	std::vector<BirthSiteOld> birthSites;
	DynamicGraph() = default;
	DynamicGraph(std::function<std::pair<double, double>(double)>& shape, double maxTime);

	void CreateGraph(double startTime, double timeChange);
	void AddBirthSite(double startTime, double timeChange, BirthSiteOld birthSite);
	void RemoveBirthSite(double startTime, double timeChange, int pos);
};