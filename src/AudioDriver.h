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


enum i2s_alignment {
	CODEC_I2S_ALIGN = 0,
	CODEC_LJ_RJ_ALIGN
};


class AudioDriver {

public:

	static const int BufferSize=32; // increase to 64 samples to avoid drop outs (at 192 kHz)
	static const float ScaleFloat2Int;
	static const float ScaleInt2Float;

public:

	int setPins(int bitClkPin=26, int lrClkPin=27, int dataOutPin=14, int dataInPin=13, int enablePin=33);

	void setI2sPort(i2s_port_t i2s_port);

	int setFormat(int fs, int channelCount, i2s_bits_per_sample_t bitsPerSample, i2s_comm_format_t commFormat, int alignment=CODEC_I2S_ALIGN, int mclkFactor = 384);
	
	bool start();

	// convenience constructor
	int setup(int fs=48000, int channelCount=2, int bitClkPin=26, int lrClkPin=27, int dataOutPin=14, int dataInPin=13, int enablePin=33, i2s_port_t i2sPort=I2S_NUM_0);

	bool mute(bool powerDown);

	int readBlock();
	int writeBlock();

	inline int32_t float2Int(float sample) {
		sample *= AudioDriver::ScaleFloat2Int;
		int32_t y = (int32_t)(sample >= 0.5)? sample+1 : sample;
		// y = constrain(y, -AudioDriver::ScaleFloat2Int, AudioDriver::ScaleFloat2Int-1);
		return (y << 8);
	}

	inline float int2Float(int32_t sample) {
		return (float)(sample * AudioDriver::ScaleInt2Float);
	}

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

	int lshift;

	i2s_port_t i2sPort;
	int i2sBufferSize;
	int32_t* i2sReadBuffer;
	int32_t* i2sWriteBuffer;

};



#endif /* AUDIODRIVER_H_ */
