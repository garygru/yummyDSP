/*
 *  AK455x.cpp
 *	AKM AK4552 / AK4556 
 * 
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 *
 */
#include "AK455x.h"


/* AKM AK4552 Audio Serial Interface Format / Timing
 *
 *  | HPF 	| M/S 	| MCLK 	| Format|
 *	| ON	| SLAVE | AUTO	| LJ/RJ	|
 */
int AK4552::setup(int fs, int channelCount, int bitClkPin, int lrClkPin, int dataOutPin, int dataInPin, int enablePin, i2s_port_t i2s_port) {
	this->i2sPort = constrain(i2s_port, I2S_NUM_0, I2S_NUM_MAX);

	int mclk_fs = 384;
	int err = setFormat(fs, channelCount, I2S_BITS_PER_SAMPLE_32BIT, I2S_COMM_FORMAT_PCM, CODEC_LJ_RJ_ALIGN, mclk_fs);
	err += setPins(bitClkPin, lrClkPin, dataOutPin, dataInPin, enablePin);
	err += start();
	return err;
}


/* AKM AK4556 Audio Serial Interface Format / Timing
 *
 * Operating modes are selected by the CKS3-0 pins as shown in the table
 *
 * | Mode 	| CKS3 	| CKS2 	| CKS1 	| CKS0 	| HPF 	| M/S 	| MCLK **	| Format|
 * |	0	|	L	|	L	|	L	|	L	|	ON	| SLAVE | 	1		| LJ/RJ	|
 * |	1	|	L	|	L	|	L	|	H	|	ON	| SLAVE |	2		| LJ/RJ	|
 * |	2	|	L	|	L	|	H	|	L	|	OFF	| SLAVE |	1		| LJ/RJ	|
 * |	3	|	L	|	L	|	H	|	H	|	OFF	| SLAVE |	2		| LJ/RJ	|
 * |	4	|	L	|	H	|	L	|	L	|	ON	| SLAVE |	1		|	I2S	|
 * |	5	|	L	|	H	|	L	|	H	|	ON	| SLAVE |	2		|	I2S	|
 * |	6	|	L	|	H	|	H	|	L	|	OFF	| SLAVE |	1		|	I2S	|
 * |	7	|	L	|	H	|	H	|	H	|	OFF	| SLAVE |	2		|	I2S	|
 * |	8	|	L	|	L	|	L	|	L	|	ON	| SLAVE |	1		|	LJ	|
 * |	9	|	H	|	L	|	L	|	H	|	ON	| SLAVE |	2		|	LJ	|
 * |	10	|	H	|	L	|	H	|	L	|	OFF	| SLAVE |	1		|	LJ	|
 * |	11	|	H	|	L	|	H	|	H	|	OFF	| SLAVE |	2		|	LJ	|
 * |	12	|	H	|	H	|	L	|	L	|	ON	| MASTER|	256fs	|	I2S	|
 * |	13	|	H	|	H	|	L	|	H	|	ON	| MASTER|	512fs	|	I2S	|
 * |	14	|	H	|	H	|	H	|	L	|	ON	| MASTER|	128fs	|	I2S	|
 * |	15	|	H	|	H	|	H	|	H	|	ON	| MASTER|	256fs	|	I2S	|
 *
 * ** MCLK rate
 *
 * 1:
 * 	128/192fs (Quad Speed)
 * 	256/384fs (Double Speed)
 * 	512/768fs (Normal Speed)
 *
 * 2:  256/384/512/768fs (Normal Speed)
 *
 */
int AK4556::setup(int mode, int fs, int channelCount, int bitClkPin, int lrClkPin, int dataOutPin, int dataInPin, int enablePin, i2s_port_t i2s_port) {
	this->i2sPort = constrain(i2s_port, I2S_NUM_0, I2S_NUM_MAX);
	
	int mclk_fs = 384;
	int err = ESP_OK;
	switch (mode)
	{
		case 0: 
		case 1:
		case 2:
		case 3:
			err += setFormat(fs, channelCount, I2S_BITS_PER_SAMPLE_32BIT, I2S_COMM_FORMAT_PCM, CODEC_LJ_RJ_ALIGN, mclk_fs);
		 	break;
		case 4:
		case 5:
		case 6:
		case 7: 
			err += setFormat(fs, channelCount, I2S_BITS_PER_SAMPLE_32BIT, I2S_COMM_FORMAT_I2S_MSB, CODEC_I2S_ALIGN, mclk_fs); 
			break;
		default:
            err += setFormat(fs, channelCount, I2S_BITS_PER_SAMPLE_32BIT, I2S_COMM_FORMAT_PCM, CODEC_LJ_RJ_ALIGN, mclk_fs);
            break;
	}
	
	err += setPins(bitClkPin, lrClkPin, dataOutPin, dataInPin, enablePin);
	err += start();
	return err;
}