/*
    Filter Library

    Smoothed LP-, HP-, BP-Filter

    Author: Gary Grutzek
    gary.grutzek@ib-gru.de
 */

#include <Nodes/AudioNode.h>
#include "Arduino.h"
#include "Interpolator.h"

#ifndef FILTER_H
#define FILTER_H


class FilterNode : public AudioNode {

	enum coeffs {
		cB0 = 0,
		cB1,
		cB2,
		cA1,
		cA2
	};

public:

	enum filterType {
		LPF = 0,
		HPF,
		BPF
	};

	FilterNode();
	FilterNode(int sampleRate, int channelCount);
	~FilterNode();

	void begin(int sampleRate, int channelCount);

	void setupFilter(int type, float f0, float BW, bool smooth = false, bool resetStates = true);
	void updateFilter(float f0);
	float processSample(float sample, int channel);

	int getSampleRate() { return fs; }
	float getFilterFreq() { return f0; }

protected:
	int fs; // sample rate
	Interpolator *interpolator;


	float f0; // filter cutoff
	float q; // reziprok bandwidth
	int type; // filter type aka hi pass, lo pass, ..
	float filterStates[2][2];
	float filterCoeffs[5][2];
	float coeffDelta[5];
};

#endif

