/*
 * 	AudioDriver.h
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#ifndef AUDIODRIVER_H_
#define AUDIODRIVER_H_

#include "Arduino.h"
// include espressif hw files
#include "driver/gpio.h"
#include "driver/i2s.h"


// TODO: abstraction layer for ADC / DAC / PDM / CODEC Shields

int32_t float2Int(float sample);
float int2Float(int32_t sample);


class AudioDriver {

public:

	static const int BufferSize=32; // increase to 64 samples to avoid drop outs (at 192 kHz)
	static const float ScaleFloat2Int;
	static const float ScaleInt2Float;

	int setup(int fs=48000, int channelCount=1, int bitClkPin=26, int lrClkPin=27, int dataOutPin=14, int dataInPin=13, int enablePin=33, i2s_port_t i2sPort=I2S_NUM_0);

	bool enable(bool powerOn);

	// TODO:  CKS configuration pin support
	// hpf on/off
	// mode ...

	int readBlock();
	int writeBlock();

//	float readSample(int n, int channel);
//	void writeSample(float sample, int n, int channel);

	inline float readSample(int n, int channel) {
		return int2Float(i2sReadBuffer[channelCount * n + channel]);
	}


	inline void writeSample(float sample, int n, int channel) {
		 i2sWriteBuffer[channelCount * n + channel] = float2Int(sample);
	}

protected:

	int fs;
	int channelCount;
	int enablePin;

	i2s_port_t i2sPort;
	int i2sBufferSize;
	int32_t* i2sReadBuffer;
	int32_t* i2sWriteBuffer;

};



#endif /* AUDIODRIVER_H_ */
