//
//  AudioNode.hpp
//  
//
//  Created by Gary Grutzek on 10.11.18.
//

#ifndef AudioNode_hpp
#define AudioNode_hpp

#include <stdio.h>
#include "dspHelpers.h"

class AudioNode {
public:
	virtual ~AudioNode() {};
	virtual void begin(int fs, int channelCount) = 0;
	virtual float processSample(float inSample, int channelCount) = 0;
};

#endif /* AudioNode_hpp */
