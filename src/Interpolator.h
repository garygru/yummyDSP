/*
 * 	Interpolator
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#ifndef Interpolator_hpp
#define Interpolator_hpp

#include "Arduino.h"
#include <list>

enum smooth {
	kCurrent = 0,
	kTarget,
	kSmoothCount
};

typedef struct {
	float *current;
	float target;
	float increment;
} rampValue;

class Interpolator {
public:
	Interpolator();
	Interpolator(int fs, int ms);
	~Interpolator();
	
	void begin(int fs, int ms);
	void add(float *current, float target);
	void process();
	
protected:
	std::list <rampValue> values;
	int samplerate;
	float invFadeSteps;
};

#endif /* Interpolator_hpp */
