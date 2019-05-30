/*
	 DSP Library
  
	 Author: Gary Grutzek
	 gary.grutzek@ib-gru.de
*/

#include "yummyDSP.h"


DSP::DSP() {
  fs = 48000;
  scale = 1<<15; // 16 Bits signed
// set headroom 6dB
}

DSP::DSP(int fs) {
	this->begin(fs);
}

DSP::~DSP() {
	;
}

void DSP::begin(int sampleRate) {
  	fs = sampleRate;
}

void DSP::addNode(AudioNode *node) {
	nodelist.push_back(node);
}

float DSP::process(float x) {
	float y = x;
	// process all nodes
	std::list<AudioNode *>::iterator it;
	for (it = nodelist.begin(); it != nodelist.end(); ++it){
		AudioNode *node = *it;
		y = node->processSample(y);
	}
	return y;
}
