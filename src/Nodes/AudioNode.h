/*
 * 	AudioNode
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#ifndef AUDIONODE
#define AUDIONODE

#include <stdio.h>
#include "dspHelpers.h"

class AudioNode {
public:
	virtual ~AudioNode() {};
	virtual void begin(int fs, int channelCount) = 0;
	virtual float processSample(float inSample, int channelCount) = 0;
};

#endif
