/*
 * 	WaveShaperNode
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#include "Arduino.h"
#include "Nodes/AudioNode.h"
#include "Interpolator.h"


#ifndef WAVESHAPER_H
#define WAVESHAPER_H


class WaveShaperNode : public AudioNode {

public:

	WaveShaperNode();
	WaveShaperNode(int fs, int channelCount);
	~WaveShaperNode();

	void begin(int fs, int channelCount);

	float processSample(float sample, int channel);

	float getDrive() {
		return drive[kCurrent];
	}

	void setDrive(float drive, bool fade = true);

protected:
	int fs; // sample rate
	Interpolator *interpolator;
	float drive[kSmoothCount];

};

#endif

