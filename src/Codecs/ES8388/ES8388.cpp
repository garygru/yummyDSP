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

#include <Wire.h>
#include "ES8388.h"

#define ES8388_ADDR 0b00010000 //0x10

/* ES8388 register  DEC */
#define ES8388_CONTROL1     0
#define ES8388_CONTROL2     1
#define ES8388_CHIPPOWER    2
#define ES8388_ADCPOWER     3
#define ES8388_DACPOWER     4
#define ES8388_CHIPLOPOW1   5
#define ES8388_CHIPLOPOW2   6
#define ES8388_ANAVOLMANAG  7
#define ES8388_MASTERMODE   8
 
/* ADC */ 
#define ES8388_ADCCONTROL1  9
#define ES8388_ADCCONTROL2  10
#define ES8388_ADCCONTROL3  11
#define ES8388_ADCCONTROL4  12
#define ES8388_ADCCONTROL5  13
#define ES8388_ADCCONTROL6  14
#define ES8388_ADCCONTROL7  15
#define ES8388_ADCCONTROL8  16
#define ES8388_ADCCONTROL9  17
#define ES8388_ADCCONTROL10 18
#define ES8388_ADCCONTROL11 19
#define ES8388_ADCCONTROL12 20
#define ES8388_ADCCONTROL13 21
#define ES8388_ADCCONTROL14 22
 
/* DAC */ 
#define ES8388_DACCONTROL1  23
#define ES8388_DACCONTROL2  24
#define ES8388_DACCONTROL3  25
#define ES8388_DACCONTROL4  26
#define ES8388_DACCONTROL5  27
#define ES8388_DACCONTROL6  28
#define ES8388_DACCONTROL7  29
#define ES8388_DACCONTROL8  30
#define ES8388_DACCONTROL9  31
#define ES8388_DACCONTROL10 32
#define ES8388_DACCONTROL11 33
#define ES8388_DACCONTROL12 34
#define ES8388_DACCONTROL13 35
#define ES8388_DACCONTROL14 36
#define ES8388_DACCONTROL15 37
#define ES8388_DACCONTROL16 38
#define ES8388_DACCONTROL17 39
#define ES8388_DACCONTROL18 40
#define ES8388_DACCONTROL19 41
#define ES8388_DACCONTROL20 42
#define ES8388_DACCONTROL21 43
#define ES8388_DACCONTROL22 44
#define ES8388_DACCONTROL23 45
#define ES8388_DACCONTROL24 46
#define ES8388_DACCONTROL25 47
#define ES8388_DACCONTROL26 48
#define ES8388_DACCONTROL27 49
#define ES8388_DACCONTROL28 50
#define ES8388_DACCONTROL29 51
#define ES8388_DACCONTROL30 52

//byte val_volume = 5;
#define pilih_input  1
#define pilih_MixInCh  2

//#define bitsampelnya I2S_BITS_PER_SAMPLE_32BIT
#define bitsampelnya I2S_BITS_PER_SAMPLE_24BIT
//#define bitsampelnya I2S_BITS_PER_SAMPLE_16BIT


uint8_t ES8388::ReadReg(uint8_t reg){
    Wire.beginTransmission(ES8388_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    uint8_t val = 0u;
    if (1 == Wire.requestFrom(uint16_t(ES8388_ADDR), uint8_t(1), true)) { val = Wire.read();}
    Wire.endTransmission(false);
    return val;
}

bool ES8388::WriteReg(uint8_t reg, uint8_t val){
    Wire.beginTransmission(ES8388_ADDR);
    Wire.write(reg);
    Wire.write(val);
    return 0 == Wire.endTransmission(true);
}
ES8388::ES8388(){}

void ES8388::setI2CPins(int I2C_CLK, int I2C_DATA) {
	I2C_SCL = I2C_CLK;
	I2C_SDA = I2C_DATA;
}


int ES8388::setup(int fs, int channelCount, int bitClkPin, int lrClkPin, int dataOutPin, int dataInPin, int enablePin, i2s_port_t i2s_port) {
	this->i2sPort = constrain(i2s_port, I2S_NUM_0, I2S_NUM_MAX);
	this->setI2CPins();
    /*
	static uint8_t volume = 5;
	SetVolumeSpeaker(volume);
	SetVolumeHeadphone(volume);
*/
    Serial.println("Connect to ES8388 codec... ");  
    while (not begin(I2C_SDA, I2C_SCL, 400000)){
        Serial.printf("Failed!\n");
        delay(1000);
    }
    Serial.println("Nyambung ke Codec = Sukses/OK");
 	
	int mclk_fs = 256;//512;
	// i2s_comm_format_t comm_fmt = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB); // depricated
	i2s_comm_format_t comm_fmt = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S);
	int err = setFormat(fs, channelCount, I2S_BITS_PER_SAMPLE_24BIT, comm_fmt, CODEC_I2S_ALIGN, mclk_fs);
	err += setPins(bitClkPin, lrClkPin, dataOutPin, dataInPin, enablePin);
	err += start();	


	return err;
}
bool ES8388::begin(int sda, int scl, uint32_t frequency){
	
    bool ok = Wire.begin(sda, scl, frequency);

	// Reset all registers, readback default as sanity check
    //ok &= WriteReg(CHIP_AUDIO_RS, 0x123);
    delay(100);
    Serial.printf("0x00: 0x%02x\n", ReadReg(ES8388_CONTROL1));
    Serial.printf("0x01: 0x%02x\n", ReadReg(ES8388_CONTROL2));
 
    //Serial.printf("0x00: 0x%02x\n", ReadReg(ES8388_CONTROL1));
    //Serial.printf("0x01: 0x%02x\n", ReadReg(ES8388_CONTROL2));
    WriteReg(ES8388_CONTROL1, 1 << 7); //  do reset!  
    WriteReg(ES8388_CONTROL1, 0x06);
    WriteReg(ES8388_CONTROL2, 0x50);

    ok &= (0x06 == ReadReg(ES8388_CONTROL1));
    ok &= (0x50 == ReadReg(ES8388_CONTROL2));
/**/ 

    WriteReg(ES8388_CHIPPOWER, 0xFF);  //reset and stop es8388
    WriteReg(0x00, 0x80); // reset control port register to default  
    WriteReg(0x00, 0x06); // restore default value  
  
    //10.5 Power Down Sequence (To Standby Mode)
    WriteReg(0x0F, 0x34); // ADC Mute  
    WriteReg(0x19, 0x36); // DAC Mute  
    WriteReg(0x02, 0xF3); // Power down DEM and STM  
    
    //10.4 The sequence for Start up bypass mode
     
    
    WriteReg(0x08, 0x00);// Set Chip to Slave Mode    
    WriteReg(0x02, 0x3F);//* Power down DEM and STM   
    WriteReg(0x2B, 0x80);//* Set same LRCK   
    WriteReg(0x00, 0x05);//* Set Chip to Play&Record Mode   
    WriteReg(0x01, 0x40);//* Power Up Analog and Ibias  
	
    if (bitsampelnya==I2S_BITS_PER_SAMPLE_32BIT) {
      WriteReg(ES8388_ADCCONTROL4, 0b00010000);
      WriteReg(ES8388_DACCONTROL1, 0b00100000);
    }
    else if (bitsampelnya==I2S_BITS_PER_SAMPLE_24BIT){
      WriteReg(ES8388_ADCCONTROL4, 0b00000000);
      WriteReg(ES8388_DACCONTROL1, 0b00000000);    
    }
    else if (bitsampelnya==I2S_BITS_PER_SAMPLE_16BIT){
      WriteReg(ES8388_ADCCONTROL4, 0b00001100);
      WriteReg(ES8388_DACCONTROL1, 0b00011000);    
    }    
       

          WriteReg(0x03, 0x3F); // adc also on but no bias  
          WriteReg(0x03, 0x00); // PdnAINL, PdinAINR, PdnADCL, PdnADCR, PdnMICB, PdnADCBiasgen, flashLP, Int1LP

    // Power up DAC / Analog Output for Record  
    WriteReg(0x04, 0x3C);

    // Set ADC Digital Volume  
    WriteReg(0x10, 0x00); // LADCVOL
    WriteReg(0x11, 0x00); // RADCVOL

    // UnMute ADC  
    WriteReg(0x0F, 0x30); //
    WriteReg(0x12, 0x16);
    WriteReg(0x17, 0x18); // DACLRSWAP, DACLRP, DACWL, DACFORMAT
    WriteReg(0x18, 0x02); // DACFsMode , DACFsRatio

    // Set ADC Digital Volume
   
    WriteReg(0x1A, 0x00);
    WriteReg(0x1B, 0x02);
    WriteReg(0x19, 0x32);

    //Setup Mixer    
    WriteReg(0x26, 0x09);//  WriteReg(0x26, 0x00);
    WriteReg(0x27, 0xD0); // ES8388_DACCONTROL17
    WriteReg(0x28, 0x38);
    WriteReg(0x29, 0x38);
    WriteReg(0x2A, 0xD0);
    
    // Set Lout/Rout Volume /
    WriteReg(0x2E, 0x1E);
    WriteReg(0x2F, 0x1E);
    WriteReg(0x30, 0x1E);
    WriteReg(0x31, 0x1E);

    //Power up DEM and STM /
    WriteReg(0x02, 0x00);
    SetInputCh(pilih_input);	
    SetMixInCh(pilih_MixInCh, 1);
    
	//SetPGAGain(1, 190);
    //SetIn2OoutVOL(5, 5);
    
	Serial.printf("ES8388 setup finished!\n");   
  /* 
    WriteReg(ES8388_ADCCONTROL1,0b00110011);   //ADC gain input setingnya  L = bit 7 sd 4,  R = bit 3 sd 0 
    WriteReg(ES8388_ADCCONTROL6,0b00110000);  // ADC invert disable Highpass enabel opo disabel
    WriteReg(ES8388_ADCCONTROL8,192);   //ADC IN L att
    WriteReg(ES8388_ADCCONTROL9,0);   //ADC IN R att   
    WriteReg(ES8388_ADCCONTROL10, 0b11110000);   //ALC xx +  max xxx min xxx PGA

    WriteReg(ES8388_DACCONTROL4,0b00000000);    //L OUT att
    WriteReg(ES8388_DACCONTROL5,0b00000000);    //R OUT att
    //WriteReg(ES8388_DACCONTROL6, 0b10000000);    //deempasis 44.1 khz , ga invert Phase , ngarang2 dewe    
    WriteReg(ES8388_DACCONTROL24,0b00100001);    //L OUT 1 VOL
    WriteReg(ES8388_DACCONTROL25,0b00100001);    //R OUT 1 VOL
    WriteReg(ES8388_DACCONTROL26,0b00100001);    //L OUT 2 VOL
    WriteReg(ES8388_DACCONTROL27,0b00100001);    //R OUT 2 VOL
*/

	return ok;
}

void ES8388::DumpRegisters(){
    for (int i = 0; i < 53; i++){
        uint8_t reg = 0;
        reg = ReadReg(i);
        Serial.printf("Reg 0x%02x = 0x%02x\n", i, reg);
    }
}
void ES8388::SetInputCh(uint8_t ch){  
        uint8_t in;
        switch (ch)        {
        case 0:
            in = 0;Serial.printf("AdcCh0!\n");break;
        case 1:
            in = 1;Serial.printf("AdcCh1!\n");break;
        default:
            Serial.printf("Illegal Input ch!\n");
            return;
        }
        // ES8388_
        WriteReg(ES8388_ADCCONTROL2, (in << 6) + (in << 4)); // LINSEL , RINSEL , DSSEL , DSR
}
void ES8388::SetMixInCh(uint8_t ch, float var){
    if (var > 0) {
        uint8_t in = 0;
        switch (ch){
        case 0:
            in = 0; Serial.printf("MixCh0!\n");break;
        case 1:
            in = 1;Serial.printf("MixCh1!\n");break;
        case 2:
            in = 3;Serial.printf("MixChAMPL!\n");break;
        default:
            Serial.printf("Illegal Mix Input ch!\n");
            return;
        }
 
        WriteReg(ES8388_DACCONTROL16, in + (in << 3)); // LMIXSEL, RMIXSEL
    }
}

void ES8388::SetADCVOL(uint8_t vol){ 
    if (vol > 192) vol = 192;
    uint8_t volu8 = 192-vol;
    if (volu8 < 0){volu8 = 0;}
    WriteReg(ES8388_ADCCONTROL8, volu8); // LADCVOL
    WriteReg(ES8388_ADCCONTROL9, volu8); // RADCVOL
    Serial.printf("ADC Volume = %d\n", vol);
}

void ES8388::SetDACVOL(uint8_t vol){
    if (vol > 192) vol = 192;
    uint8_t volu8 = 192-vol;
    if (volu8 < 0){volu8 = 0;}
      WriteReg(ES8388_DACCONTROL4, volu8); // LDACVOL
      WriteReg(ES8388_DACCONTROL5, volu8); // RDACVOL
      Serial.printf("DAC Volume = %d\n", vol);
}

void ES8388::SetIn2OoutVOL(uint8_t unused, float vol){
    vol *= -5;
    vol += 7;
    uint8_t volu8 = vol; if (volu8 > 7){volu8 = 7;}
    uint8_t var;
    var =  ReadReg(ES8388_DACCONTROL17) & 0xC0;
    if (volu8 == 7){ var &= ~ 0x40; }  else { var |= 0x40;}
    WriteReg(ES8388_DACCONTROL17, (volu8 << 3) + var); // LD2LO, LI2LO, LI2LOVOL

    var = ReadReg(ES8388_DACCONTROL20) & 0xC0;
    if (volu8 == 7){ var &= ~ 0x40; } else { var |= 0x40;}
    WriteReg(ES8388_DACCONTROL20, (volu8 << 3) + var); // RD2RO, RI2RO, RI2ROVOL
    Serial.printf("In to out volume = %f\n", vol);
    
}

void ES8388::SetOUT1VOL(uint8_t vol){
    if (vol > 33) vol = 33;
    WriteReg(ES8388_DACCONTROL24, vol); // LOUT1VOL
    WriteReg(ES8388_DACCONTROL25, vol); // ROUT1VOL
    Serial.printf("OUT1 VOL = %d\n", vol);
}

void ES8388::SetOUT2VOL(uint8_t vol){
    if (vol > 33) vol = 33;
    WriteReg(ES8388_DACCONTROL26, vol); // LOUT2VOL
    WriteReg(ES8388_DACCONTROL27, vol); // ROUT2VOL
    Serial.printf("OUT2 VOL = %d\n", vol);
}

void ES8388::SetPGAGain(uint8_t vol){
    uint8_t volu8 = vol + (vol*17);
    if (volu8 > 136) volu8 = 136;
    WriteReg(ES8388_ADCCONTROL1, volu8); // MicAmpL, MicAmpR
    Serial.printf("PGA Gain Vol = %d\n", vol);
}
