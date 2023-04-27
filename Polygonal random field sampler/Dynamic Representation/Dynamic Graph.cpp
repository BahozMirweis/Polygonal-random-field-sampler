#include "Dynamic Graph.h"

DynamicGraph::DynamicGraph(std::function<std::pair<double, double>(double)>& shape, double maxTime) : shape{ shape }, maxTime{maxTime} {
	//birthSites.push_back(BirthSite(1, 1, false));
	//birthSites.push_back(BirthSite(2, 1, false));
	//birthSites.push_back(BirthSite(3, 2, true, -10.0));
}

void DynamicGraph::initBirthSites(double time) {
	for (BirthSiteOld& b : birthSites) {
		if (!b.created && time >= b.getTime()) {
			bool created = false;

			for (int i = 0; i < particles.size(); ++i) {
				// Preserve increasing order of particles
				if (b.getDisplacement() < particles[i]->getDisplacement()) {
					if (!b.boundarySite) {
						particles.insert(particles.begin() + i, &b.particles[1]);
					}

					particles.insert(particles.begin() + i, &b.particles[0]);
					created = true;
					break;
				}
			}

			if (!created) {
				particles.push_back(&b.particles[0]);
				if (!b.boundarySite) {
					particles.push_back(&b.particles[1]);
				}
			}

			b.created = true;
		}
	}
}

void DynamicGraph::CreateGraph(double startTime, double timeChange) {
	double time = startTime;

	while (time < maxTime) {
		std::pair<double, double> boundary = shape(time);
		initBirthSites(time);

		for (auto i = particles.begin(); i != particles.end();) {
			if (!(*i)->updateParticle(time, boundary.first, boundary.second)) {
				(*i)->endMotion();
				i = particles.erase(i);
			}
			else if (i != particles.begin() && (*i)->collision(*(*(i - 1)))) {
				(*i)->endMotion();
				(*(i - 1))->endMotion();
				i = particles.erase(i - 1);
				i = particles.erase(i);
			}
			else {
				++i;
			}
		}

		time += timeChange;
	}

	for (Particle* p : particles) {
		p->endMotion();
	}

	while (particles.size() != 0) {
		particles.pop_back();
	}
}

// needs more thought as points could have already moved
double DynamicGraph::calculateDeathPoint(Particle* p1, Particle* p2, double time) {
	return 0.0;
}

void ChangeGraphChangeParticle(Particle*& p1, Particle*& p2, std::vector<Particle *>::iterator i ,bool sample) { 

}

void DynamicGraph::ChangeGraph(double startTime, double timeChange, Particle* aboveParticle, Particle* belowParticle) {
	for (BirthSiteOld& b : birthSites) {
		b.created = false;
	}

	double time = startTime;
	bool finish = false;

	while (time < maxTime && !finish) {
		std::pair<double, double> boundary = shape(time);
		initBirthSites(time);

		for (auto i = particles.begin(); i != particles.end();) {
			if (!(*i)->updateParticle(time, boundary.first, boundary.second)) {
				// Check if we should have collided with a particle

				if (*i != belowParticle && i + 1 != particles.end() && (*i)->collision(*(*(i + 1))) && !(*(i + 1))->sample) {
					if (*(i + 1) == aboveParticle) {
						if (belowParticle == aboveParticle) {
							belowParticle = *i;
						}

						bool aboveSample = (*(i + 1))->sample;
						(*(i + 1))->endMotion(*i);
						aboveParticle = *i;
						aboveParticle->sample = !aboveSample;
						aboveParticle->control = true;

						aboveParticle->changeParticle(*(i + 1));

						i = particles.erase(i + 1);
						continue;
					}
					else if (*(i + 1) == belowParticle) {
						if (belowParticle == aboveParticle) {
							aboveParticle = *i;
						}

						bool belowSample = (*(i + 1))->sample;
						(*(i + 1))->endMotion(*i);
						belowParticle = *i;
						belowParticle->sample = !belowSample;
						belowParticle->control = true;

						belowParticle->changeParticle(*(i + 1));

						i = particles.erase(i + 1);
						continue;
					}
				}

				if (*i == aboveParticle) {
					if (i + 1 != particles.end() && !(*i)->sample && (*(i + 1))->collision(*(*i))) {
						if (belowParticle == aboveParticle) {
							belowParticle = *(i + 1);
						}

						bool aboveSample = (*i)->sample;
						(*i)->endMotion(*(i + 1));
						aboveParticle = *(i + 1);
						aboveParticle->sample = !aboveSample;
						aboveParticle->control = true;

						aboveParticle->changeParticle(*i);

						i = particles.erase(i);
						continue;
					}

					if (belowParticle == aboveParticle) {
						(*i)->endMotion();
						i = particles.erase(i);
						finish = true;
						break;
					}
					else {
						aboveParticle = belowParticle;
					}
				}
				else if (*i == belowParticle) {
					if (i + 1 != particles.end() && *(i + 1) != aboveParticle && !(*i)->sample && (*(i + 1))->collisionDeath(*(*i))) {
						if (belowParticle == aboveParticle) {
							aboveParticle = *(i + 1);
						}

						bool belowSample = (*i)->sample;
						(*i)->endMotion(*(i + 1));
						belowParticle = *(i + 1);
						belowParticle->sample = !belowSample;
						belowParticle->control = true;

						belowParticle->changeParticle();

						i = particles.erase(i);
						continue;
					}

					if (belowParticle == aboveParticle) {
						(*i)->endMotion();
						i = particles.erase(i);
						finish = true;
						break;
					}
					else {
						belowParticle = aboveParticle;
					}
				}
				(*i)->endMotion();
				i = particles.erase(i);
			}
			else if (i != particles.begin() && (*i)->collision(*(*(i - 1)))) {
				// Particle intersection
				if (((*i) == aboveParticle) && ((*(i - 1)) == belowParticle)) {
					if ((*i)->sample && (*(i - 1))->sample) {
						(*(i - 1))->endMotion(*i);
						(*i)->endMotion(*(i - 1));
						i = particles.erase(i - 1);
						i = particles.erase(i);
						finish = true;
						break;
					}
					else {
						belowParticle = *i;
						aboveParticle = *(i - 1);
						std::iter_swap(i, i - 1);

						++i;
					}
					/*else if (!(*i)->sample) {
						(*i)->endMotion();
						i = particles.erase(i);
						aboveParticle = belowParticle;
					}
					else {
						(*(i - 1))->endMotion();
						i = particles.erase(i - 1);
						belowParticle = aboveParticle;
					}*/
				}
				//else if ((*i) != belowParticle && *(i - 1) != aboveParticle) {
				//	(*i)->endMotion();
				//	(*(i - 1))->endMotion();
				//	i = particles.erase(i - 1);
				//	i = particles.erase(i);
				//}
				else if (*i == aboveParticle) {
					if (belowParticle == aboveParticle) {
						belowParticle = *(i - 1);
					}

					bool aboveSample = (*i)->sample;
					(*i)->endMotion(*(i - 1));
					aboveParticle = *(i - 1);
					aboveParticle->sample = !aboveSample;
					aboveParticle->control = true;

					aboveParticle->changeParticle(*i);

					i = particles.erase(i);
				}
				else if ((*i) == belowParticle) {
					if (belowParticle == aboveParticle) {
						aboveParticle = *(i - 1);
					}

					bool belowSample = (*i)->sample;
					(*i)->endMotion(*(i - 1));
					belowParticle = *(i - 1);
					belowParticle->sample = !belowSample;
					belowParticle->control = true;

					belowParticle->changeParticle(*i);

					i = particles.erase(i);
				}
				else if (*(i - 1) == belowParticle) {
					if (belowParticle == aboveParticle) {
						aboveParticle = *i;
					}

					bool belowSample = (*(i - 1))->sample;
					(*(i - 1))->endMotion(*i);
					belowParticle = *i;
					belowParticle->sample = !belowSample;
					belowParticle->control = true;

					belowParticle->changeParticle(*(i - 1));

					i = particles.erase(i - 1) + 1;
				}
				else if (aboveParticle == *(i - 1)) {
					if (belowParticle == aboveParticle) {
						belowParticle = *i;
					}

					bool aboveSample = (*(i - 1))->sample;
					(*(i - 1))->endMotion(*i);
					aboveParticle = *i;
					aboveParticle->sample = !aboveSample;
					aboveParticle->control = true;

					aboveParticle->changeParticle(*(i - 1));

					i = particles.erase(i - 1) + 1;
				}
				else {
					// Issue occurs when a particle jumps and then changes velocity. After we check collision but since the change in velocity has
					// occured, their intersection point will be calculated incorrectly.
					++i;
				}
			}
			else {
				++i;
			}
		}

		time += timeChange;
	}

	for (Particle* p : particles) {
		p->endMotion();
	}

	while (particles.size() != 0) {
		particles.pop_back();
	}
}

void DynamicGraph::AddBirthSite(double startTime, double timeChange, BirthSiteOld birthSite) {
	birthSites.push_back(birthSite);

	Particle* belowParticle = &birthSites.back().particles[0];
	Particle* aboveParticle = &birthSites.back().particles[0];
	if (!birthSites.back().boundarySite) {
		aboveParticle = &birthSites.back().particles[1];
	}

	ChangeGraph(startTime, timeChange, aboveParticle, belowParticle);
}

void DynamicGraph::RemoveBirthSite(double startTime, double timeChange, int pos) {
	BirthSiteOld& b{ birthSites[pos] };
	Particle* belowParticle = &b.particles[0];
	Particle* aboveParticle = &b.particles[0];

	if (!b.boundarySite) {
		aboveParticle = &b.particles[1];
	}

	aboveParticle->control = true;
	belowParticle->control = true;

	ChangeGraph(startTime, timeChange, aboveParticle, belowParticle);

	birthSites.erase(birthSites.begin() + pos);
}