/*
 * 	StatisticsNode
 *
 *  Author: Ben Marshall
 * 
 */

#include <Nodes/StatisticsNode.h>

StatisticsNode::StatisticsNode() {
	; // be sure to call begin(fs)
}

StatisticsNode::StatisticsNode(int fs, int channelCount) {
	this->begin(fs, channelCount);
}

StatisticsNode::~StatisticsNode() {
	;
}

void StatisticsNode::begin(int sampleRate, int channelCount) {
	this->channelCount = constrain(channelCount, 1, 2);
	this->reset();
}


float StatisticsNode::processSample(float sample, int channel) {
	channel = constrain(channel, 0, this->channelCount-1);
	if (sample < minValue[channel]) {
		minValue[channel] = sample;
	}
	if (sample > maxValue[channel]) {
		maxValue[channel] = sample;
	}
}


float StatisticsNode::getMaxValue(int channel) {
	channel = constrain(channel, 0, this->channelCount-1);
	return maxValue[channel];
}



float StatisticsNode::getMinValue(int channel) {
	channel = constrain(channel, 0, this->channelCount-1);
	return minValue[channel];
}


void StatisticsNode::reset() {
	memset(maxValue, 0, sizeof(maxValue));
	memset(minValue, 0, sizeof(minValue));
}
