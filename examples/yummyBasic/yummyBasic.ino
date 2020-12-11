/*
    yummyDSP – Basic example

    Author: Gary Grutzek

*/

#include "Arduino.h"
#include "driver/gpio.h"
#include <YummyDSP.h>
#include <AudioDriver.h>

AudioDriver i2sCodec;
YummyDSP dsp;
FilterNode lp;
FilterNode hp;
WaveShaperNode sat;
DelayNode dly;

// I2S
const int fs = 48000;
const int channelCount = 2;

// Audio Amp Pins
#define I2S_BCK_PIN 26
#define I2S_LRCLK_PIN 27
#define I2S_DOUT_PIN 14
#define I2S_DIN_PIN 13
#define CODEC_ENABLE_PIN 33

// a potentiometer at GPIO 4
#define POT_PIN 4
float pot=0.f;


void audioTask(void *);


void setup() {

  Serial.begin(115200);
  Serial.print("\n\nSetup ");

  // setup audio codec
  i2sCodec.setup(fs, channelCount, I2S_BCK_PIN, I2S_LRCLK_PIN, I2S_DOUT_PIN, I2S_DIN_PIN, CODEC_ENABLE_PIN);
  i2sCodec.mute(false);

  // setup audio lib
  dsp.begin(fs);

  // setup some filter nodes
  lp.begin(fs, channelCount);
  lp.setupFilter(FilterNode::LPF, 5000, 0.7);

  hp.begin(fs, channelCount);
  hp.setupFilter(FilterNode::HPF, 30, 1.0);

  dly.begin(fs, channelCount);
  dly.setDelayMs(150.f);

  sat.begin(fs, channelCount);
  sat.setDrive(0.2f);

  // add nodes to audio processing tree
  // I2S in => hp => delay => saturation => lp => I2S out
  dsp.addNode(&hp);
  dsp.addNode(&dly);
  dsp.addNode(&sat);
  dsp.addNode(&lp);

  // run audio in dedicated task on cpu core 1
  xTaskCreatePinnedToCore(audioTask, "audioTask", 10000, NULL, 10, NULL, 1);
  // run control task on another cpu  core with lower priority

  Serial.print("\nSetup done ");
}



void audioTask(void *) {

  Serial.print("\nAudio task");

  float sample=0;

  while (true) {

    i2sCodec.readBlock();

    for (int i = 0; i < AudioDriver::BufferSize; i++) {

      for (int ch = 0; ch < channelCount; ch++) {

      sample = i2sCodec.readSample(i, ch);
      
      sample = dsp.process(sample, ch);

      i2sCodec.writeSample(sample, i, ch);
      }
    }

    i2sCodec.writeBlock();
  }
  vTaskDelete(NULL);
}


// control stuff here
void loop() 
{

  // sample the potentiometer multiple times, adc is too noisy 
  int readCnt = 50;
  float p = analogRead(POT_PIN);
  for (int i=0; i<readCnt; i++) {
    p += analogRead(POT_PIN);
  }
  // low pass filtered and scaled between 0-1
  pot = pot*0.99 + (float)p/4096/(1+readCnt) * 0.01;

  delay(1);

  //lp.updateFilter(pot * 10000.f);
//  sat.setDrive(pot);
  dly.setDelayMs(pot * dly.maxDelayMs());

}
