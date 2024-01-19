/*
 * 	AudioDriver.h
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#ifndef AUDIODRIVER_H_
#define AUDIODRIVER_H_

#include "dspHelpers.h"
#include "Arduino.h"
// include espressif hw files
#include "driver/gpio.h"
#include "driver/i2s.h"

#define BITS_PER_SAMPLE 32 // can be 8, 16, 24 or 32
#define BUF_SIZE 32 // buffer size in SAMPLES, increase to 64 to avoid drop outs at 192 kHz

enum i2s_alignment {
	CODEC_I2S_ALIGN = 0,
	CODEC_LJ_RJ_ALIGN
};

class AudioDriver {

public:
	static const uint32_t BitsPerSample = BITS_PER_SAMPLE;
	static constexpr float ScaleFloat2Int = (uint32_t)(1<<(BitsPerSample-1));
	static constexpr float ScaleInt2Float = 0,992f/ScaleFloat2Int;
	static const int BufferSize = BUF_SIZE; 

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

	inline int32_t float2Int(float fSample) {
		fSample = fclamp(fSample, -0.9999999f, 0.9999999f);
		int32_t y = ScaleFloat2Int * fSample;
		//Serial.println(y);
		return y;
	}

	inline float int2Float(int32_t iSample) {
		return ((float)iSample * ScaleInt2Float);
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
