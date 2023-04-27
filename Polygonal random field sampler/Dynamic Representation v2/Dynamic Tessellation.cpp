#include "Dynamic Tessellation.h"
#include <matplot/matplot.h>
#include <unordered_map>

namespace plt = matplot;

DynamicTessellation::DynamicTessellation(double width, double height) : width{ width }, height{ height }, vertices{}, birthSites{} {

}

void DynamicTessellation::addBirthsite(const BirthSite& b) {
	if (b.boundarySite) {
		double velocity{ b.velocities[0] };
		double displacement{ b.getDisplacement() };
		double time{ b.getTime() };
		std::shared_ptr<Vertex> currParticle = std::make_shared<Vertex>(displacement, time);
		std::list<std::shared_ptr<Vertex>>::iterator i{ vertices.begin() };

		while (i != vertices.end() && (*i)->time <= time) {
			++i;
		}

		i = vertices.insert(i, currParticle);
		birthSites.push_back(*i);
		double jumpTime{ jumpNewTime(velocity) };
		displacement += jumpTime * velocity;
		time += jumpTime;
		currParticle = std::make_shared<Vertex>( displacement, time, *i) ;
		(*i)->child = currParticle;
		++i;

		std::shared_ptr<Vertex> temp{};

		while (currParticle) {
			updateParticle(currParticle, i, displacement, time, velocity, temp);
			
		}

		//while (currParticle) {
		//	addParticleToEndOfList(currParticle, displacement, time, velocity);
		//}

	}
	else {
		double velocity1{ b.velocities[0] };
		double velocity2{ b.velocities[1] };
		double displacement1{ b.getDisplacement() };
		double displacement2{ b.getDisplacement() };
		double time1{ b.getTime() };
		double time2{ b.getTime() };

		std::shared_ptr<Vertex> particle1 = std::make_shared<Vertex>(displacement1, time1);
		std::list<std::shared_ptr<Vertex>>::iterator i{ vertices.begin() };

		while (i != vertices.end() && (*i)->time <= time1) {
			++i;
		}

		i = vertices.insert(i, particle1);
		birthSites.push_back(*i);
		double jumpTime1{ jumpNewTime(velocity1) };
		displacement1 += jumpTime1 * velocity1;
		time1 += jumpTime1;
		particle1 = std::make_shared<Vertex>(displacement1, time1, *i);

		double jumpTime2{ jumpNewTime(velocity2) };
		displacement2 += jumpTime2 * velocity2;
		time2 += jumpTime2;
		std::shared_ptr<Vertex> particle2 = std::make_shared<Vertex>(displacement2, time2, *i);
		
		
		(*i)->child = particle1;
		(*i)->child2 = particle2;
		++i;

		addTwoParticles(particle1, particle2, displacement1, time1, velocity1, displacement2, time2, velocity2);
	}
}

void DynamicTessellation::removeParticle(int pos) {
	BirthSite b{ 1, 1, false };

	std::shared_ptr<Vertex> toRem{ birthSites[pos].lock()};

	birthSites.erase(birthSites.begin() + pos);

	if (toRem->child2.expired()) {
		std::shared_ptr<Vertex> pre{ };

		std::list<std::shared_ptr<Vertex>>::iterator j{ vertices.begin() }, lastJ{ vertices.begin() };
		while (toRem && j != vertices.end()) {
			if (toRem == *j) {

				if (!toRem->child.expired()) {
					pre = toRem;
					toRem = toRem->child.lock();
				}

				j = vertices.erase(j);
				lastJ = j;
			}
			else {
				++j;
			}
		}

		if (!toRem->parent2.expired()) {
			if (toRem->parent.lock() == pre) {
				toRem->parent = toRem->parent2;
				toRem->parent2 = std::weak_ptr<Vertex>();
			}
			else {
				toRem->parent2 = std::weak_ptr<Vertex>();
			}

			vertices.insert(lastJ, toRem);

			double displacement = toRem->displacement;
			double time = toRem->time;
			double velocity = b.velocities[0];
			jumpNewTimeVelocity(time, b.velocities[0], displacement);
			std::shared_ptr<Vertex> currParticle = std::make_shared<Vertex>(displacement, time, toRem);
			toRem->child = currParticle;

			std::list<std::shared_ptr<Vertex>>::iterator i{ vertices.begin() };

			std::shared_ptr<Vertex> temp{};

			while (currParticle) {
				updateParticle(currParticle, i, displacement, time, velocity, temp);
			}
		}
	}
	else {
		double displacement1{}, displacement2{}, time1{}, time2{}, velocity1{}, velocity2{};
		std::shared_ptr<Vertex> particle1{}, particle2{};
		
		std::shared_ptr<Vertex> pre{ }, temp{toRem};



		std::list<std::shared_ptr<Vertex>>::iterator j{ vertices.begin() }, lastJ{ vertices.begin() };
		while (toRem && j != vertices.end()) {
			if (toRem == *j) {

				if (!toRem->child.expired()) {
					pre = toRem;
					toRem = toRem->child.lock();
				}

				j = vertices.erase(j);
				lastJ = j;
			}
			else {
				++j;
			}
		}

		if (!toRem->parent2.expired()) {
			if (toRem->parent.lock() == pre) {
				toRem->parent = toRem->parent2;
				toRem->parent2 = std::weak_ptr<Vertex>();
			}
			else {
				toRem->parent2 = std::weak_ptr<Vertex>();
			}

			vertices.insert(lastJ, toRem);

			displacement1 = toRem->displacement;
			time1 = toRem->time;
			velocity1 = b.velocities[0];
			jumpNewTimeVelocity(time1, b.velocities[0], displacement1);
			particle1= std::make_shared<Vertex>(displacement1, time1, toRem);
			toRem->child = particle1;
		}
		else {
			particle1 = std::shared_ptr<Vertex>{};
		}

		toRem = temp->child2.lock();
		j = vertices.begin();
		lastJ = vertices.begin();

		while (toRem && j != vertices.end()) {
			if (toRem == *j) {

				if (!toRem->child.expired() && toRem->child.lock() == particle1) {
					particle1 = std::shared_ptr<Vertex>{};
				}

				if (!toRem->child.expired()) {
					pre = toRem;
					toRem = toRem->child.lock();
				}

				j = vertices.erase(j);
				lastJ = j;
			}
			else {
				++j;
			}
		}

		if (!toRem->parent2.expired()) {
			if (toRem->parent.lock() == pre) {
				toRem->parent = toRem->parent2;
				toRem->parent2 = std::weak_ptr<Vertex>();
			}
			else {
				toRem->parent2 = std::weak_ptr<Vertex>();
			}

			vertices.insert(lastJ, toRem);


			displacement2 = toRem->displacement;
			time2 = toRem->time;
			velocity2 = b.velocities[1];
			jumpNewTimeVelocity(time2, b.velocities[1], displacement2);
			particle2 = std::make_shared<Vertex>(displacement2, time2, toRem);
			toRem->child = particle2;
		}
		else {
			particle2 = std::shared_ptr<Vertex>{};
		}

		addTwoParticles(particle1, particle2, displacement1, time1, velocity1, displacement2, time2, velocity2);
	}
	
}

void DynamicTessellation::addTwoParticles(std::shared_ptr<Vertex>& particle1, std::shared_ptr<Vertex>& particle2, double& displacement1, double& time1, double& velocity1, double& displacement2, double& time2, double& velocity2) {

	std::list<std::shared_ptr<Vertex>>::iterator i{ vertices.begin() };

	while ((i != vertices.end()) && (particle1 || particle2)) {
		bool temp{};

		if (particle1 && (!particle2 || time1 < time2) && i != vertices.end()) {
			updateParticle(particle1, i, displacement1, time1, velocity1, particle2);
		}
		else {
			updateParticle(particle2, i, displacement2, time2, velocity2, particle1);
		}
	}

	while (particle1 || particle2) {

		bool temp{};
		if (((particle1 && particle1->parent.lock()->child2.expired()) || (particle2 && particle2->parent.lock()->child2.expired())) && collision(particle1, particle2, temp)) {
			std::pair<double, double> intersection{ intersectionPoint(particle1, particle2, temp) };

			if (intersection.first > 0 && intersection.first < height && intersection.second < width) {

				std::shared_ptr<Vertex> newParticle{ std::make_shared<Vertex>(intersection.first, intersection.second, particle1->parent, std::weak_ptr<Vertex>{} , particle2->parent) };

				if (particle1->parent.lock()->child.lock() == particle1) {
					particle1->parent.lock()->child = newParticle;
				}
				else {
					particle1->parent.lock()->child2 = newParticle;
				}

				if (particle2->parent.lock()->child.lock() == particle2) {
					particle2->parent.lock()->child = newParticle;
				}
				else {
					particle2->parent.lock()->child2 = newParticle;
				}

				vertices.push_back(newParticle);
				break;
			}
		}

		if (particle1 && (!particle2 || time1 < time2)) {
			if (((particle1 && particle1->parent.lock()->child2.expired()) && (particle2 && !particle2->parent.lock()->parent.expired() && particle2->parent.lock()->parent.lock()->child2.expired()))) {
				auto parent{ particle2->parent.lock() };
				bool temp{ false };
				if (collision(particle1, parent, temp)) {
					std::pair<double, double> intersection{ intersectionPoint(particle1, parent, temp) };

					std::shared_ptr<Vertex> newParticle{ std::make_shared<Vertex>(intersection.first, intersection.second, particle1->parent, std::weak_ptr<Vertex>{} , parent->parent) };

					if (particle1->parent.lock()->child.lock() == particle1) {
						particle1->parent.lock()->child = newParticle;
					}
					else {
						particle1->parent.lock()->child2 = newParticle;
					}

					if (parent->parent.lock()->child.lock() == parent) {
						parent->parent.lock()->child = newParticle;
					}
					else {
						parent->parent.lock()->child2 = newParticle;
					}

					for (auto i{ vertices.begin() }; i != vertices.end(); ++i) {
						if (*i == parent) {
							vertices.erase(i);
							break;
						}
					}

					vertices.push_back(newParticle);
					break;
				}
			}

			updateParticle(particle1, i, displacement1, time1, velocity1, particle2);
		}
		else {
			if (((particle2 && particle2->parent.lock()->child2.expired()) && (particle1 && !particle1->parent.lock()->parent.expired() && particle1->parent.lock()->parent.lock()->child2.expired()))) {
				auto parent{ particle1->parent.lock() };
				bool temp{ false };
				if (collision(particle2, parent, temp)) {
					std::pair<double, double> intersection{ intersectionPoint(particle2, parent, temp) };

					std::shared_ptr<Vertex> newParticle{ std::make_shared<Vertex>(intersection.first, intersection.second, parent->parent, std::weak_ptr<Vertex>{} , particle2->parent) };

					if (particle2->parent.lock()->child.lock() == particle2) {
						particle2->parent.lock()->child = newParticle;
					}
					else {
						particle2->parent.lock()->child2 = newParticle;
					}

					if (parent->parent.lock()->child.lock() == parent) {
						parent->parent.lock()->child = newParticle;
					}
					else {
						parent->parent.lock()->child2 = newParticle;
					}

					for (auto i{ vertices.begin() }; i != vertices.end(); ++i) {
						if (*i == parent) {
							vertices.erase(i);
							break;
						}
					}

					vertices.push_back(newParticle);
					break;
				}
			}

			updateParticle(particle2, i, displacement2, time2, velocity2, particle1);
		}

	}
}

void DynamicTessellation::addParticleToEndOfList(std::shared_ptr<Vertex>& particle, double& displacement, double& time, double& velocity) {
	if (time < width && displacement > 0 && displacement < height) {
		vertices.push_back(particle);
		jumpNewTimeVelocity(time, velocity, displacement);
		particle = std::make_shared<Vertex>(displacement, time, vertices.back());
		vertices.back()->child = particle;
	}
	else {
		auto col = boundaryCollision(particle);
		std::weak_ptr<Vertex> parent{ particle->parent };
		particle = std::make_shared<Vertex>(col.first, col.second, parent);
		
		if (parent.lock()->child.expired()) {
			parent.lock()->child = particle;
		}
		else {
			parent.lock()->child2 = particle;
		}
		
		
		for (auto i{ vertices.end() }; ; --i) {
			if (i == vertices.end()) continue;

			if ((*i)->time < col.second) {
				i++;
				vertices.insert(i, particle);
				break;
			}
		}

		//vertices.push_back(particle);
		particle = std::shared_ptr<Vertex>{};
		time = width + 1;
	}
}

void DynamicTessellation::jumpNewTimeVelocity(double& time, double& velocity, double& displacement) {
	double newVelocity{ jumpNewVelocity(velocity) };
	double newTime{ time + jumpNewTime(newVelocity) };

	double newDisplacement{ newVelocity * newTime - newVelocity * time + displacement };
	displacement = newDisplacement;
	time = newTime;
	velocity = newVelocity;
}

std::pair<double, double> DynamicTessellation::boundaryCollision(const std::shared_ptr<Vertex>& u) const {
	if (u->displacement > height || u->displacement < 0 || u->time > width) {
		auto uparent = u->parent.lock();

		double m = (u->displacement - uparent->displacement) / (u->time - uparent->time);
		double c = u->displacement - u->time * m;

		double rightDisplacement = width * m + c;
		if (rightDisplacement <= height && rightDisplacement >= 0) {
			return { rightDisplacement, width };
		}

		double intersectionTime{};
		if (u->displacement >= height) {
			intersectionTime = (height - c) / m;
			return { height, intersectionTime };
		}
		else {
			intersectionTime = -c / m;
			return { 0, intersectionTime };
		}
	}
	else {
		return { -1, -1 };
	}
}

bool DynamicTessellation::collision(const std::shared_ptr<Vertex>& u, const std::shared_ptr<Vertex>& v, bool& secondParent) const {
	if (!u || !v || u->parent.expired() || v->parent.expired()) return false;

	auto uparent = u->parent.lock(), vparent = v->parent.lock();

	auto inter1{ intersectionPoint(u, v) };
	std::pair<double, double> inter2{};

	double intersectionTime{ inter1.second };
	bool firstParent{ intersectionTime > max(uparent->time, vparent->time) && intersectionTime < min(u->time, v->time) };

	bool par2{ false };
	if (!u->parent2.expired()) {
		uparent = u->parent2.lock();
		inter2 = intersectionPoint(u, v, true);
		double intersectionTime2{ inter2.second };
		par2 = intersectionTime2 > max(uparent->time, vparent->time) && intersectionTime2 < min(u->time, v->time);
	}

	if (firstParent && par2) {
		if (std::abs(inter1.first - v->parent.lock()->displacement) > std::abs(inter2.first - v->parent.lock()->displacement)) {
			secondParent = true;
		}
	}
	else if (firstParent) {
		secondParent = false;
	}
	else if(par2) {
		secondParent = true;
	}

	return firstParent || par2;
}

void DynamicTessellation::updateParticle(std::shared_ptr<Vertex>& currParticle, std::list<std::shared_ptr<Vertex>>::iterator& i, double& displacement, double& time, double& velocity, std::shared_ptr<Vertex>& particle2) {
	bool collide{ false };

	std::shared_ptr<Vertex> minParticle;
	std::pair<double, double> minIntersection{ };
	bool minSecondParent{ false };
	std::list<std::shared_ptr<Vertex>>::iterator minJ;

	for (auto j{ vertices.begin() }; j != vertices.end(); ++j) {
		bool secondParent{ false };
		if (collision((*j), currParticle, secondParent) && (*j)->parent.lock() != currParticle->parent.lock()) {

			std::pair<double, double> intersection{ intersectionPoint(*j, currParticle, secondParent) };
			//want to compare to the parents displacement rather than the potential vertex we're jumpting too
			if (!minParticle || std::abs(intersection.first - currParticle->parent.lock()->displacement) < std::abs(minIntersection.first - currParticle->parent.lock()->displacement)) {
				minParticle = *j;
				minIntersection = intersection;
				minSecondParent = secondParent;
				minJ = j;
			}
			collide = true;


		}
	}

	//case where the two new particles collide
	bool temp{};
	if (((currParticle && currParticle->parent.lock()->child2.expired()) || (particle2 && particle2->parent.lock()->child2.expired())) && collision(currParticle, particle2, temp)) {
		std::pair<double, double> intersection{ intersectionPoint(currParticle, particle2, temp) };
		if (std::abs(intersection.first - currParticle->parent.lock()->displacement) < std::abs(minIntersection.first - currParticle->parent.lock()->displacement)&&
			intersection.first > 0 && intersection.first < height && intersection.second < width) {
			std::shared_ptr<Vertex> newParticle{ std::make_shared<Vertex>(intersection.first, intersection.second, currParticle->parent) };

			if (currParticle->parent.lock()->child.lock() == currParticle) {
				currParticle->parent.lock()->child = newParticle;
			}
			else {
				currParticle->parent.lock()->child2 = newParticle;
			}
			newParticle->parent2 = particle2->parent;

			if (!temp) {
				if (particle2->parent.lock()->child.lock() == particle2) {
					particle2->parent.lock()->child = newParticle;
				}
				else {
					particle2->parent.lock()->child2 = newParticle;
				}
			}
			else {
				if (particle2->parent2.lock()->child.lock() == particle2) {
					particle2->parent2.lock()->child = newParticle;
				}
				else {
					particle2->parent2.lock()->child2 = newParticle;
				}
			}

			while (i != vertices.end() && (*i)->time <= intersection.second) {
				++i;
			}

			i = vertices.insert(i, newParticle);
			++i;
			particle2 = std::shared_ptr<Vertex>{};
			currParticle = std::shared_ptr<Vertex>{};
			return;
		}
	}

	if (!collide) {

		auto col = boundaryCollision(currParticle);

		if (col.first == -1) {
			while (i != vertices.end() && (*i)->time <= time) {
				++i;
			}

			i = vertices.insert(i, currParticle);
			jumpNewTimeVelocity(time, velocity, displacement);
			currParticle = std::make_shared<Vertex>(displacement, time, *i);
			(*i)->child = currParticle;
			++i;
		}
		else {
			std::shared_ptr<Vertex> parent{ currParticle->parent.lock() };
			currParticle = std::make_shared<Vertex>(col.first, col.second, currParticle->parent);
			displacement = col.first;
			time = col.second;


			if (parent->child.expired()) {
				parent->child = currParticle;
			}
			else {
				parent->child2 = currParticle;
			}

			i = vertices.begin();
			while (i != vertices.end() && (*i)->time <= time) {
				++i;
			}

			i = vertices.insert(i, currParticle);

			currParticle = std::shared_ptr<Vertex>{};
		}
	}
	else {
		std::shared_ptr<Vertex> newParticle{ std::make_shared<Vertex>(minIntersection.first, minIntersection.second, currParticle->parent) };

		if (currParticle->parent.lock()->child.lock() == currParticle) {
			currParticle->parent.lock()->child = newParticle;
		}
		else {
			currParticle->parent.lock()->child2 = newParticle;
		}

		if (!minSecondParent) {
			newParticle->parent2 = minParticle->parent;
			if (minParticle->parent.lock()->child.lock() == minParticle) {
				minParticle->parent.lock()->child = newParticle;
			}
			else {
				minParticle->parent.lock()->child2 = newParticle;
			}
		}
		else {
			newParticle->parent2 = minParticle->parent2;
			if (minParticle->parent2.lock()->child.lock() == minParticle) {
				minParticle->parent2.lock()->child = newParticle;
			}
			else {
				minParticle->parent2.lock()->child2 = newParticle;
			}
		}

		//when there are two particles the i iterator gets ahead of this min intersection making it add to vertices in the wrong place/
		i = vertices.begin();
		while (i != vertices.end() && (*i)->time <= minIntersection.second) {
			++i;
		}

		i = vertices.insert(i, newParticle);
		++i;

		std::shared_ptr<Vertex> pre{ minParticle->parent };

		if (minSecondParent) {
			pre = minParticle->parent2.lock();
		}

		std::list<std::shared_ptr<Vertex>>::iterator lastJ{ minJ };
		while (minParticle && minJ != vertices.end()) {
			if (minParticle == *minJ) {

				if (!(*minJ)->child.expired() && (*minJ)->child.lock() == particle2) {
					particle2 = std::shared_ptr<Vertex>{};
				}

				if (!minParticle->child.expired()) {
					pre = minParticle;
					minParticle = minParticle->child.lock();
				}

				if (i == minJ) {
					minJ = vertices.erase(minJ);
					i = minJ;
				}
				else {
					minJ = vertices.erase(minJ);
				}

				lastJ = minJ;
			}
			else {
				++minJ;
			}
		}

		if (!minParticle->parent2.expired()) {
			if (minParticle->parent.lock() == pre) {
				minParticle->parent = minParticle->parent2;
				minParticle->parent2 = std::weak_ptr<Vertex>();
			}
			else {
				minParticle->parent2 = std::weak_ptr<Vertex>();
			}

			vertices.insert(lastJ, minParticle);

			displacement = minParticle->displacement;
			time = minParticle->time;
			jumpNewTimeVelocity(time, velocity, displacement);
			currParticle = std::make_shared<Vertex>(displacement, time, minParticle);
			minParticle->child = currParticle;
		}
		else {
			currParticle = std::shared_ptr<Vertex>{};
		}
	}
}

std::pair<double, double> DynamicTessellation::intersectionPoint(const std::shared_ptr<Vertex>& u, const std::shared_ptr<Vertex>& v, bool usecondParent) const {
	if (u->parent.expired() || v->parent.expired()) return {0, 0};
	if (usecondParent && u->parent2.expired()) return { 0, 0 };

	auto uparent = u->parent.lock(), vparent = v->parent.lock();

	if (usecondParent) {
		uparent = u->parent2.lock();
	}

	double v1 = (u->displacement - uparent->displacement) / (u->time - uparent->time);
	double v2 = (v->displacement - vparent->displacement) / (v->time - vparent->time);

	double c1 = u->displacement - u->time * v1;
	double c2 = v->displacement - v->time * v2;

	double intersectionTime{ (c2 - c1) / (v1 - v2) };
	double intersectionDisplacement{ v1 * intersectionTime + c1 };

	return { intersectionDisplacement, intersectionTime };
}

double DynamicTessellation::jumpNewVelocity(double oldVelocity) const {
	std::function<double(double)> density{ [this, oldVelocity](double u) {return this->jumpLogDensity(u, oldVelocity); } };
	

	return MetropolisHastings::sample1D(density, 0.0, 0.25, 50000);
}

double DynamicTessellation::jumpNewTime(double oldVelocity) const {
	std::function<double(double)> timeDensity{ [this, oldVelocity](double t) {return this->jumpTimeLogDensity(t, oldVelocity); } };
	return MetropolisHastings::sample1D(timeDensity, 0.0, 0.5, 50000);
}

double DynamicTessellation::jumpTimeLogDensity(double x, double velocity) const {
	if (x < 0) {
		return -2147483648;
	}

	return -2 * std::sqrt(1 + velocity * velocity) * x;
}

double DynamicTessellation::jumpLogDensity(double u, double velocity) const {
	return log(std::abs(u - velocity)) - 1.5 * log(1 + u * u);
}

void DynamicTessellation::drawTessellation() {
	plt::cla();
	plt::hold(true);

	for (std::weak_ptr<Vertex> birthsite : birthSites) {
		std::vector<double> x{}, y{};

		std::weak_ptr<Vertex> temp{ birthsite };

		while(!temp.expired()) {
			x.push_back(temp.lock()->time);
			y.push_back(temp.lock()->displacement);
			temp = temp.lock()->child;
		}

		plt::plot(x, y)->color("black");

		if (!birthsite.lock()->child2.expired()) {
			x = { birthsite.lock()->time }, y = { birthsite.lock()->displacement };


			temp = birthsite.lock()->child2;

			while (!temp.expired()) {
				x.push_back(temp.lock()->time);
				y.push_back(temp.lock()->displacement);
				temp = temp.lock()->child;
			}

			plt::plot(x, y)->color("black");
		}
		
	}

	plt::show();
}

void DynamicTessellation::drawAllVertices() {
	plt::cla();
	plt::hold(true);

	for (std::shared_ptr<Vertex> v : vertices) {
		std::vector<double> x{v->time}, y{v->displacement};
		if (!v->parent.expired()) {
			x.push_back(v->parent.lock()->time);
			y.push_back(v->parent.lock()->displacement);
			plt::plot(x, y);
		}

		if (!v->parent2.expired()) {
			x = { v->time }, y = { v->displacement };
			x.push_back(v->parent2.lock()->time);
			y.push_back(v->parent2.lock()->displacement);
			plt::plot(x, y);
		}
	}
	//plt::save("C:/Users/bahoz/source/repos/Polygonal random field sampler/x64/Debug/images/balls.png");
	plt::show();
}

DynamicTessellation::DynamicTessellation(const DynamicTessellation& d) {
	std::unordered_map<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>> comp{};
	width = d.width;
	height = d.height;

	for (const std::shared_ptr<Vertex>& v : d.vertices) {
		std::shared_ptr<Vertex> toAdd{ std::make_shared<Vertex>(v->displacement, v->time) };

		if (!v->parent.expired()) {
			std::shared_ptr<Vertex> parent{ comp[v->parent.lock()]};
			toAdd->parent = parent;
			
			if (v->parent.lock()->child.lock() == v) {
				parent->child = toAdd;
			}
			else {
				parent->child2 = toAdd;
			}
		}

		if (!v->parent2.expired()) {
			std::shared_ptr<Vertex> parent{ comp[v->parent2.lock()] };
			toAdd->parent2 = parent;

			if (v->parent2.lock()->child.lock() == v) {
				parent->child = toAdd;
			}
			else {
				parent->child2 = toAdd;
			}
		}

		comp[v] = toAdd;
		vertices.push_back(toAdd);
	}

	for (std::weak_ptr<Vertex> v : d.birthSites) {
		birthSites.push_back(comp[v.lock()]);
	}
}

 DynamicTessellation& DynamicTessellation::operator= (const DynamicTessellation& d) {
	 std::unordered_map<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>> comp{};
	 width = d.width;
	 height = d.height;
	 vertices = std::list<std::shared_ptr<Vertex>>{};
	 birthSites = std::vector<std::weak_ptr<Vertex>>{};

	 for (const std::shared_ptr<Vertex>& v : d.vertices) {
		 std::shared_ptr<Vertex> toAdd{ std::make_shared<Vertex>(v->displacement, v->time) };

		 if (!v->parent.expired()) {
			 std::shared_ptr<Vertex> parent{ comp[v->parent.lock()] };
			 toAdd->parent = parent;

			 if (v->parent.lock()->child.lock() == v) {
				 parent->child = toAdd;
			 }
			 else {
				 parent->child2 = toAdd;
			 }
		 }

		 if (!v->parent2.expired()) {
			 std::shared_ptr<Vertex> parent{ comp[v->parent2.lock()] };
			 toAdd->parent2 = parent;

			 if (v->parent2.lock()->child.lock() == v) {
				 parent->child = toAdd;
			 }
			 else {
				 parent->child2 = toAdd;
			 }
		 }

		 comp[v] = toAdd;
		 vertices.push_back(toAdd);
	 }

	 for (std::weak_ptr<Vertex> v : d.birthSites) {
		 birthSites.push_back(comp[v.lock()]);
	 }

	 return *this;
}