/*
 * 	Interpolator
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#include "Interpolator.h"

Interpolator::Interpolator() {
	;
}

Interpolator::Interpolator(int fs, int ms) {
	this->begin(fs, ms);
}

Interpolator::~Interpolator() {
	values.clear();
}

void Interpolator::begin(int fs, int ms) {
	samplerate = fs;
	invFadeSteps = 1.f / ((float)fs / 1000.0f * (float)constrain(ms, 1, 1000)); // e.g. 1440 samples for 30 ms @ 48kHz
}

void Interpolator::add(float *current, float target) {

	// check if exists
	std::list<rampValue>::iterator it = values.begin();
	while (it != values.end()) {
		if ((*it).current == current) {
			// char buf[100];
			// sprintf(buf, "Adr: %p  exists", (void*)v2.current);
			// Serial.println(buf);
			(*it).target = target; // not really thread safe?
			(*it).increment = (target - *current) * invFadeSteps;
			return;
		}
		it++;
	}

	rampValue v;
	v.target = target;
	v.current = current;
	v.increment = (target - *current) * invFadeSteps;
	values.push_back(v);

	// char buf[100];
	// sprintf(buf, "adding c: %p  t: %f i: %f", (void*)v.current, v.target, v.increment);
	// Serial.println(buf);
	// Serial.println(values.size());
}

void Interpolator::process() {

	std::list<rampValue>::iterator it = values.begin();
	while (it != values.end()) {
		rampValue v = *it;
		// ramping
		*v.current += v.increment;
		// remove when ramped to target
		if ( (*(v.current) >= v.target && v.increment > 0) || (*(v.current) <= v.target && v.increment < 0) ) {
			*(v.current) = v.target;
			it = values.erase(it); 	// returns iterator to the next element
		}
		else {
			it++;
		}
	}
}
