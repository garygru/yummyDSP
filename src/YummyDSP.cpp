/*
	 DSP Library
  
	 Author: Gary Grutzek
	 gary.grutzek@ib-gru.de
*/

#include <YummyDSP.h>


YummyDSP::YummyDSP() {
}

YummyDSP::YummyDSP(int fs) {
	this->begin(fs);
}

YummyDSP::~YummyDSP() {
	;
}

void YummyDSP::begin(int sampleRate) {
  	fs = sampleRate;
}

void YummyDSP::addNode(AudioNode *node) {
	nodelist.push_back(node);
}

float YummyDSP::process(float x, int channel) {
	float y = x;
	// process all nodes
	std::list<AudioNode *>::iterator it;
	for (it = nodelist.begin(); it != nodelist.end(); ++it){
		AudioNode *node = *it;
		y = node->processSample(y, channel);
	}
	return y;
}
