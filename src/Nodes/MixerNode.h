/*
  	MixerNode
 	(currently more like a smoothed fader)

    Author: Gary Grutzek
    gary.grutzek@ib-gru.de
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
	MixerNode(int fs);
	~MixerNode();

	void begin(int fs);

	float processSample(float sample);


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

