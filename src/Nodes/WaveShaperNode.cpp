/*
 * 	WaveShaperNode
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#include <Nodes/WaveShaperNode.h>


WaveShaperNode::WaveShaperNode() {
	; // be sure to call begin(fs)
}

WaveShaperNode::WaveShaperNode(int sampleRate, int channelCount) {
	this->begin(fs, channelCount);
}

WaveShaperNode::~WaveShaperNode() {
	if(interpolator) {
		delete interpolator;
	}
}

void WaveShaperNode::begin(int sampleRate, int channelCount) {
	fs = sampleRate;
	interpolator = new Interpolator(fs, 50);
	setDrive(0.f, false);
}


float WaveShaperNode::processSample(float sample, int channel) {
	interpolator->process();
	return fasttanh(sample * drive[kCurrent] * 30.f);
}


void WaveShaperNode::setDrive(float drv, bool fade) {
	drive[kTarget] = constrain(drv, 0.01, 1.0);
	if (fade == false) {
		drive[kCurrent] = drive[kTarget];
	}
	interpolator->add(&drive[kCurrent], drive[kTarget]);
}

