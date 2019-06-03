/*
  	DelayNode

    Author: Gary Grutzek
    gary.grutzek@ib-gru.de
 */

#include "Arduino.h"
#include "Nodes/AudioNode.h"
#include "Nodes/FilterNode.h"
#include "Interpolator.h"


#ifndef DELAYNODE_H
#define DELAYNODE_H


#define MAX_DELAY_LEN (12000) // 320kB RAM is the show stopper here

class DelayNode : public AudioNode {

public:

	DelayNode();
	DelayNode(int fs, int channelCount);
	~DelayNode();

	void begin(int fs, int channelCount);

	float processSample(float sample, int channel);

	float delayMs() {
		return delaySamples[kCurrent];
	}

	float wet() {
		return mix[kCurrent];
	}

	void setMix(float ms, bool fade = true);

	void setDelayMs(float ms, bool fade = true);

protected:
	int fs; // sample rate
	Interpolator *interpolator;
	float delaySamples[kSmoothCount];
	float mix[kSmoothCount];

	FilterNode lp;

	float buffer[MAX_DELAY_LEN + 1];
	int readCnt;
	int writeCnt;
	int maxDelayTimeMs;

};

#endif

