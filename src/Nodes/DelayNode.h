/*
 * 	DelayNode
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#include "Arduino.h"
#include "Nodes/AudioNode.h"
#include "Nodes/FilterNode.h"
#include "Interpolator.h"


#ifndef DELAYNODE_H
#define DELAYNODE_H


#define MAX_DELAY_LEN (12000) // 520kB RAM is a show stopper

class DelayLine {

public:

	DelayLine();

	void begin(int channelCount);
	void push(float sample, int channel);
	float pop(int channel);

	void setSampleDelay(int delay);

protected:

	float buffer[MAX_DELAY_LEN + 1];
	int length;
	int channelCount;
	int sampleDelay;
	int readIndex;
	int writeIndex;

};


class DelayNode : public AudioNode {

public:

	DelayNode();
	DelayNode(int fs, int channelCount);
	~DelayNode();

	void begin(int fs, int channelCount);

	float processSample(float sample, int channel);

	void setMix(float ms, bool fade = true);
	float wet() { return mix[kCurrent]; }

	void setDelayMs(float ms, bool fade = true);
	float delayMs() { return delayMillis[kCurrent]; }

	int maxDelayMs() { return maxDelayTimeMs; }

protected:
	int fs; // sample rate
	Interpolator *interpolator;
	float delayMillis[kSmoothCount];
	float mix[kSmoothCount];

	FilterNode lp;

	DelayLine delayLine;
	int maxDelayTimeMs;

};

#endif

