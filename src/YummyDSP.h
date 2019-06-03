/*
    DSP Library

    Author: Gary Grutzek
    gary.grutzek@ib-gru.de
*/


#ifndef DSP_H
#define DSP_H

#include "Arduino.h"
#include "Interpolator.h"
#include "Nodes/AudioNode.h"
#include "Nodes/FilterNode.h"
#include "Nodes/MixerNode.h"
#include "Nodes/WaveShaperNode.h"
#include "Nodes/DelayNode.h"

#include "WaveSynth.h"
#include <list>



class YummyDSP {

  public:
    YummyDSP();
	YummyDSP(int fs);
    ~YummyDSP();

    void begin(int fs);
    int getSampleRate() { return fs; }
	
	// TODO:
//	void addSource();
//	void addSink(); ??
	
	void addNode(AudioNode *node);
	
	float process(float sample, int channel);

  protected:
	std::list<AudioNode *> nodelist;
	
    int fs; // sample rate

};

#endif

