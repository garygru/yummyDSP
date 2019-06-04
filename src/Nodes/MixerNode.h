/*
 * 	MixerNode
 *  (currently just a smoothed fader)
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#include "Arduino.h"
#include "Nodes/AudioNode.h"
#include "Interpolator.h"

#ifndef MIXER_H
#define MIXER_H


// TODO: extend to more than one input, currently only ramped volume
class MixerNode : public AudioNode {

public:

	MixerNode();
	MixerNode(int fs, int channelCount);
	~MixerNode();

	void begin(int fs, int channelCount);

	float processSample(float sample, int channel);


	float getVolume() {
		return volume[kCurrent];
	}
	void setVolume(float volume, bool fade = true);

	float getVolumedB() {
		return 20 * log10(volume[kCurrent]);
	}
	void setVolumedB(float dB, bool fade = true);


protected:
	int fs; // sample rate
	Interpolator *interpolator;
	float volume[kSmoothCount];

};

#endif

