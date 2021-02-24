/*
 * 	StatisticsNode
 *
 *  Author: Ben Marshall
 * 
 */

#include "Arduino.h"
#include "Nodes/AudioNode.h"

#ifndef STATISTICS_H
#define STATISTICS_H


class StatisticsNode : public AudioNode {

public:

	StatisticsNode();
	StatisticsNode(int fs, int channelCount);
	~StatisticsNode();

	void begin(int fs, int channelCount);

	float processSample(float sample, int channel);

	float getMaxValue(int channel);

	float getMinValue(int channel);
	
	void reset();


protected:
	int fs; // sample rate
	int channelCount; // Channel Count
	float maxValue[2]; // Maximum detected value
	float minValue[2]; // Minimum detected value
};

#endif
