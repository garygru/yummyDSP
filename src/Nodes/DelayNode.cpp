/*
 * 	DelayNode
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */
#define DEBUG_FX

#ifdef DEBUG_FX 
  #define DEB(...) Serial.print(__VA_ARGS__) 
  #define DEBF(...) Serial.printf(__VA_ARGS__) 
  #define DEBUG(...) Serial.println(__VA_ARGS__) 
#else
  #define DEB(...)
  #define DEBF(...)
  #define DEBUG(...) 
#endif

#include <Nodes/DelayNode.h>

DelayLine::DelayLine() {
	channelCount = 1;
	length = MAX_DELAY_LEN;
}


void DelayLine::begin(int channelCount) {
#ifdef BOARD_HAS_PSRAM 
	buffer = (float *)ps_malloc(sizeof(float) * MAX_DELAY_LEN);
	if( buffer == NULL){
		DEBUG("No more PSRAM for delay!\n");
	} else {
		DEBUG("PSRAM buffer allocated for delay");
	}
#else
	buffer = (float *)malloc(sizeof(float) * MAX_DELAY_LEN);
	if( buffer == NULL){
		DEBUG("No more RAM for delay!");
	} else {
		DEBUG("RAM buffer allocated for delay");
	}		
#endif
	memset(buffer, 0, sizeof(float) * MAX_DELAY_LEN);
	this->channelCount = constrain(channelCount, 1, 2);
	length = MAX_DELAY_LEN / channelCount;
	writeIndex = 0;
	sampleDelay = length/2;
}


void DelayLine::push(float sample, int channel) {
	buffer[writeIndex + length * channel] = sample;
	if (channel == channelCount-1) {
		writeIndex++;
	}
	if (writeIndex >= length) {
		writeIndex = 0;
	}
}


float DelayLine::pop(int channel) {
	readIndex = writeIndex - sampleDelay;
	if (readIndex < 0) {
		readIndex += length;
	}
	return buffer[readIndex + length * channel];
}


void DelayLine::setSampleDelay(int delay) {
	sampleDelay = constrain(delay, 0, MAX_DELAY_LEN);
}

DelayNode::DelayNode() {
	; // be sure to call begin(fs)
}

DelayNode::DelayNode(int sampleRate, int channelCount) {
	this->begin(fs, channelCount);
}

DelayNode::~DelayNode() {
	if(interpolator) {
		delete interpolator;
	}
}

void DelayNode::begin(int sampleRate, int channelCount) {
	fs = sampleRate;
	interpolator = new Interpolator(fs, 50);

	maxDelayTimeMs = (int)((MAX_DELAY_LEN * 1000) / (fs * channelCount));

	delayLine.begin(channelCount);

	setDelayMs(80.f, false);
	setMix(0.5f, false);

	lp.begin(fs, channelCount);
	lp.setupFilter(FilterNode::LPF, 1200, 0.5f);

}


float DelayNode::processSample(float sample, int channel) {
	interpolator->process();

	float y = delayLine.pop(channel);

	// feedback and low pass, still arbitrary
	float fb = 1.4f *  y;
	fb = lp.processSample((sample+fb)*0.5f, channel);

	delayLine.push(fb, channel);

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
	delayMillis[kTarget] = constrain(ms, 0.1f, maxDelayTimeMs);

	if (fade == false) {
		delayMillis[kCurrent] = delayMillis[kTarget];
	}
	interpolator->add(&delayMillis[kCurrent], delayMillis[kTarget]);

	delayLine.setSampleDelay(floor(delayMillis[kCurrent] * fs * 0.001));
	DEBF("delay %5.3f\r\n", ms);
}

