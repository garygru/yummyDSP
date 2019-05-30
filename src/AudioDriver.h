/*
 * AudioDriver.h
 *
 *  Created on: 30 Nov 2018
 *      Author: gary
 */

#ifndef AUDIODRIVER_H_
#define AUDIODRIVER_H_

#include "Arduino.h"
// include espressif hw files
#include "driver/gpio.h"
#include "driver/i2s.h"


// TODO: abstraction layer for ADC / DAC / PDM / CODEC Shields

int32_t float_to_int(float sample);


class AudioDriver {

public:
	int setup(int fs=48000, int bitClkPin=26, int lrClkPin=27, int dataOutPin=14, int dataInPin=13, int enablePin=33, i2s_port_t i2sPort=I2S_NUM_0);

	bool enable(bool powerOn);

	// TODO:  CKS configuration pin support
	// hpf on/off
	// mode ...

protected:

	int fs;
	i2s_port_t i2sPort;
	int enablePin;
	int bufferSize;

};



#endif /* AUDIODRIVER_H_ */
