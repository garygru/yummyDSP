/*
 * 	AK455x.h
 *	AKM AK4552 / AK4556 
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#ifndef AK455x_H_
#define AK455x_H_

#include "Arduino.h"
#include "AudioDriver.h"

class AK4552: public AudioDriver {

public:

	int setup(int fs=48000, int channelCount=2, int bitClkPin=26, int lrClkPin=27, int dataOutPin=14, int dataInPin=13, int enablePin=33, i2s_port_t i2sPort=I2S_NUM_0);
};



class AK4556: public AudioDriver {

public:

	int setup(int fs=48000, int channelCount=2, int bitClkPin=26, int lrClkPin=27, int dataOutPin=14, int dataInPin=13, int enablePin=33, i2s_port_t i2sPort=I2S_NUM_0) {
		return setup(0, fs, channelCount, bitClkPin, lrClkPin, dataOutPin, dataInPin, enablePin, i2sPort);
	}

	int setup(int mode = 0, int fs=48000, int channelCount=2, int bitClkPin=26, int lrClkPin=27, int dataOutPin=14, int dataInPin=13, int enablePin=33, i2s_port_t i2sPort=I2S_NUM_0);

};

#endif /* AK455x_H_ */
