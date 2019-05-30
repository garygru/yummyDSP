/*
 MixerNode

 currently more like a smoothed fader
 
 Author: Gary Grutzek
 gary.grutzek@ib-gru.de
 */

#include <Nodes/MixerNode.h>

MixerNode::MixerNode() {
	; // be sure to call begin(fs)
}

MixerNode::MixerNode(int fs) {
	this->begin(fs);
}

MixerNode::~MixerNode() {
	;
}

void MixerNode::begin(int sampleRate) {
	fs = sampleRate;
	interpolator = new Interpolator(fs, 50);
	setVolume(0.f, false);
}


//
// return smoothed volume
//
float MixerNode::processSample(float sample) {
	interpolator->process();
	return sample * volume[kCurrent];
}


void MixerNode::setVolume(float vol, bool fade) {
	volume[kTarget] = constrain(vol, 0.0, 1.0);
	if (fade == false) {
		volume[kCurrent] = volume[kTarget];
	}
	interpolator->add(&volume[kCurrent], volume[kTarget]);
}


void MixerNode::setVolumedB(float dB, bool fade) {
	float x = constrain(dB, -120.f, 0.f);
	setVolume(pow(10, x / 20.f), fade);
}
