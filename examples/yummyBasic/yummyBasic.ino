/*
    yummyDSP – Basic example

    Author: Gary Grutzek

*/

#include "Arduino.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include <YummyDSP.h>
#include <AudioDriver.h>


// I2S
int fs = 48000;
const int channelCount = 2;
int32_t sampleBuffer[channelCount * AudioDriver::BufferSize];

#define I2S_PORT_NUM (I2S_NUM_0)

// Audio Amp Pins
#define I2S_BCK_PIN 26
#define I2S_LRCLK_PIN 27
#define I2S_DOUT_PIN 14
#define I2S_DIN_PIN 13
#define CODEC_ENABLE_PIN 33
#define POT_PIN 4 

AudioDriver codec;
YummyDSP dsp;
// the processing nodes
FilterNode lp;
FilterNode hp;
WaveShaperNode sat;

float pot = 0.f;


void audioTask(void *);

void setupAudio() {
  delay(100);
  // setup audio codec
  codec.setup(fs, I2S_BCK_PIN, I2S_LRCLK_PIN, I2S_DOUT_PIN, I2S_DIN_PIN, CODEC_ENABLE_PIN, I2S_PORT_NUM);
  codec.enable(true);
}

void setup() {

  Serial.begin(115200);
  Serial.print("\n\nSetup ");

  // setup I2S
  setupAudio();

  // setup audio lib
  dsp.begin(fs);

  // setup some filter nodes
  // low pass
  lp.begin(fs, channelCount);
  lp.setupFilter(FilterNode::LPF, 5000, 0.7);
  // hi pass
  hp.begin(fs, channelCount);
  hp.setupFilter(FilterNode::HPF, 10, 1.0);
  // saturation
  sat.begin(fs, channelCount);
  sat.setDrive(0.1f);

  // add nodes to audio processing tree
  // I2S in => hp => saturation => lp => I2S out
  dsp.addNode(&hp);
  dsp.addNode(&sat);
  dsp.addNode(&lp);

  // run audio in dedicated task
  xTaskCreatePinnedToCore(audioTask, "audioTask", 10000, NULL, 10, NULL, 1);
  // run control task on another cpu  core with lower priority

  Serial.print("\nSetup done ");
}


void audioTask(void *) {
  //
  Serial.print("\nAudio task");

  int i2s_buf_len =  channelCount * AudioDriver::BufferSize;
  int32_t* i2s_read_buff = (int32_t*) calloc(i2s_buf_len, sizeof(int32_t));
  int32_t* i2s_write_buff = (int32_t*) calloc(i2s_buf_len, sizeof(int32_t));

  int err = 0;
  uint bytesWritten = 0;
  uint bytesRead = 0;
  float sample = 0;

  while (true) {

    err = i2s_read(I2S_PORT_NUM, (void*) i2s_read_buff, i2s_buf_len * sizeof(int32_t), &bytesRead, 500);

    // sample loop
    for (int i = 0; i < AudioDriver::BufferSize; i++) {

      // channel loop
      for (int ch = 0; ch < channelCount; ch++) {
        // read from I2S read buffer and convert to float
        sample = int2Float(i2s_read_buff[2*i+ch]);

        // processing is done sequentially in the nodes. that's all!
        sample = dsp.process(sample, ch);

        // convert back to signed integer
        sampleBuffer[2*i+ch] = float2Int(sample);
      }
    }

    err = i2s_write(I2S_PORT_NUM, (const char *)&sampleBuffer, i2s_buf_len * sizeof(int32_t), &bytesWritten, 500);

  }

  // should never be executed
  free(i2s_read_buff);
  i2s_read_buff = NULL;
  free(i2s_write_buff);
  vTaskDelete(NULL);
}


// control stuff here
void loop() {

  delay(5);

  // a potentiometer on GPIO 4, scaled and filtered
  pot = pot * 0.9 + (float)analogRead(POT_PIN) / 4096 * 0.1;

//  float f0 = pot * 10000.f;
//  lp.updateFilter(f0);
//  sat.setDrive(pot);

}


