/*
 DelayNode
 
 Author: Gary Grutzek
 gary.grutzek@ib-gru.de
 */

#include <Nodes/DelayNode.h>

DelayNode::DelayNode() {
	; // be sure to call begin(fs)
}

DelayNode::DelayNode(int sampleRate) {
	this->begin(fs);
}

DelayNode::~DelayNode() {
	;
}

void DelayNode::begin(int sampleRate) {
	fs = sampleRate;
	maxDelayTimeMs = (int)((MAX_DELAY_LEN * 1000) / fs);
	interpolator = new Interpolator(fs, 50);
	setDelayMs(144.f, false);
	setMix(0.3f, false);
	readCnt = MAX_DELAY_LEN - (int) delaySamples[kCurrent];
	writeCnt = 0;

	lp.begin(fs);
	lp.setupFilter(FilterNode::LPF, 1000, 0.7);

}


float DelayNode::processSample(float sample) {
	interpolator->process();

	float fb = 1.5f * buffer[readCnt];
	fb = lp.processSample(fb);

	buffer[writeCnt] = (sample + fb)/2;
	writeCnt++;
	if (writeCnt >= MAX_DELAY_LEN) {
		writeCnt = 0;
	}

	//readCnt = writeCnt - (int) delaySamples[kCurrent];
	readCnt++;
	if(readCnt >= MAX_DELAY_LEN) {
		readCnt -= MAX_DELAY_LEN;
	}

//	Serial.println(mix[kCurrent]);

	float y = buffer[readCnt];

	// wet signal low pass


	return mix[kCurrent] * y + (1.f-mix[kCurrent]) * sample;
}


void DelayNode::setMix(float val, bool fade) {
	mix[kTarget] = constrain(val, 0.f, 1.f);

	if (fade == false) {
		mix[kCurrent] = mix[kTarget];
	}
	interpolator->add(&mix[kCurrent], mix[kTarget]);
}


void DelayNode::setDelayMs(float ms, bool fade) {

	float delayMs = constrain(ms, 0.1f, maxDelayTimeMs);
	delaySamples[kTarget] = floor(delayMs * fs * 0.001f);

	if (fade == false) {
		delaySamples[kCurrent] = delaySamples[kTarget];
	}
	Serial.println(delaySamples[kCurrent]);

	interpolator->add(&delaySamples[kCurrent], delaySamples[kTarget]);
}

