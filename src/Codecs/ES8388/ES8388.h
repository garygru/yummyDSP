/*
	AC101 - An AC101 Codec driver library for Arduino
	Copyright (C) 2019, Ivo Pullens, Emmission

	Inspired by:
	https://github.com/donny681/esp-adf/tree/master/components/audio_hal/driver/AC101

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ES8388_H
#define ES8388_H

// #include <inttypes.h>
#include "Arduino.h"
#include "AudioDriver.h"

class ES8388: public AudioDriver {

public:
/*
	typedef enum {
		SAMPLE_RATE_8000	= 0x0000,
		SAMPLE_RATE_11052	= 0x1000,
		SAMPLE_RATE_12000	= 0x2000,
		SAMPLE_RATE_16000	= 0x3000,
		SAMPLE_RATE_22050	= 0x4000,
		SAMPLE_RATE_24000	= 0x5000,
		SAMPLE_RATE_32000	= 0x6000,
		SAMPLE_RATE_44100	= 0x7000,
		SAMPLE_RATE_48000	= 0x8000,
		SAMPLE_RATE_96000	= 0x9000,
		SAMPLE_RATE_192000	= 0xa000,
	} I2sSampleRate_t;

	typedef enum {
		MODE_MASTER			= 0x00,
		MODE_SLAVE			= 0x01,
	} I2sMode_t;

	typedef enum {
		WORD_SIZE_8_BITS	= 0x00,
		WORD_SIZE_16_BITS	= 0x01,
		WORD_SIZE_20_BITS	= 0x02,
		WORD_SIZE_24_BITS	= 0x03,
	} I2sWordSize_t;

	typedef enum {
		DATA_FORMAT_I2S		= 0x00,
		DATA_FORMAT_LEFT	= 0x01,
		DATA_FORMAT_RIGHT	= 0x02,
		DATA_FORMAT_DSP		= 0x03,
	} I2sFormat_t;

	typedef enum {
		BCLK_DIV_1			= 0x0,
		BCLK_DIV_2			= 0x1,
		BCLK_DIV_4			= 0x2,
		BCLK_DIV_6			= 0x3,
		BCLK_DIV_8			= 0x4,
		BCLK_DIV_12			= 0x5,
		BCLK_DIV_16			= 0x6,
		BCLK_DIV_24			= 0x7,
		BCLK_DIV_32			= 0x8,
		BCLK_DIV_48			= 0x9,
		BCLK_DIV_64			= 0xa,
		BCLK_DIV_96			= 0xb,
		BCLK_DIV_128		= 0xc,
		BCLK_DIV_192		= 0xd,
	} I2sBitClockDiv_t;

	typedef enum {
		LRCK_DIV_16			= 0x0,
		LRCK_DIV_32			= 0x1,
		LRCK_DIV_64			= 0x2,
		LRCK_DIV_128		= 0x3,
		LRCK_DIV_256		= 0x4,
	} I2sLrClockDiv_t;


	typedef enum {
		MODE_ADC,
		MODE_DAC,
		MODE_ADC_DAC,
		MODE_LINE,
		MODE_MIC
	} Mode_t;
*/
	// Constructor.
  	ES8388();

	// Convenience method
	int setup(int fs=44100, int channelCount=2, int bitClkPin=27, int lrClkPin=25, int dataOutPin=26, int dataInPin=35, int enablePin=GPIO_NUM_21, i2s_port_t i2sPort=I2S_NUM_0);

	void setI2CPins(int I2C_CLK = 32, int I2C_DATA = 33);
	bool begin(int sda, int scl, uint32_t frequency);
	
	void SetInputCh(uint8_t ch);
	void SetMixInCh(uint8_t ch, float var);
	void SetADCVOL(uint8_t vol); 
	void SetDACVOL(uint8_t vol);
	void SetIn2OoutVOL(uint8_t unused, float vol);
	void SetOUT1VOL(uint8_t vol);
	void SetOUT2VOL(uint8_t vol);
	void SetPGAGain(uint8_t vol);
 
 
	void DumpRegisters();
	
protected:
	bool WriteReg(uint8_t reg, uint8_t val);
	uint8_t ReadReg(uint8_t reg);

	int I2C_SCL;
	int I2C_SDA;

};

#endif
