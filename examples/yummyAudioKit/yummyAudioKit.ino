/*      
  This example is ripped off the AC101 Arduino library for the AIThinker ESP32-A1S: 
  https://github.com/Yveaux/AC101
      
  AC101 Codec driver library example.
  Uses the ESP32-A1S module with integrated AC101 codec, mounted on the ESP32 Audio Kit board:
  https://wiki.ai-thinker.com/esp32-audio-kit
  
  Copyright (C) 2019, Ivo Pullens, Emmission
  
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


#include <YummyDSP.h>
#include <AudioDriver.h>
#include <Codecs/AC101/AC101.h>

#define I2S_BCK_PIN                 27
#define I2S_LRCLK_PIN               26
#define I2S_DOUT_PIN                25
#define I2S_DIN_PIN             13 // ?

#define GPIO_PA_EN                  GPIO_NUM_21
#define GPIO_SEL_PA_EN              GPIO_SEL_21

#define PIN_PLAY                    (23)      // KEY 4
#define PIN_VOL_UP                  (18)      // KEY 5
#define PIN_VOL_DOWN                (5)       // KEY 6

static AC101 i2sCodec;
YummyDSP dsp;
WaveSynth synth;
FilterNode lp;
FilterNode hp;

// I2S
const int fs = 96000;
const int channelCount = 2;

static uint8_t volume = 10;
const uint8_t volume_step = 2;
int note = 0;
int arpCnt = 0;

int debounce = 0;

void audioTask(void *);


void setup()
{
  Serial.begin(115200);

  Serial.printf("Connect to AC101 codec... ");

  // setup audio codec
  i2sCodec.setup(fs, channelCount, I2S_BCK_PIN, I2S_LRCLK_PIN, I2S_DOUT_PIN, I2S_DIN_PIN, GPIO_PA_EN);

  i2sCodec.SetVolumeSpeaker(volume);
  i2sCodec.SetVolumeHeadphone(volume);
  //  ac.DumpRegisters();

  // Enable amplifier
  pinMode(GPIO_PA_EN, OUTPUT);
  digitalWrite(GPIO_PA_EN, HIGH);

  // Configure keys on ESP32 Audio Kit board
  pinMode(PIN_PLAY, INPUT_PULLUP);
  pinMode(PIN_VOL_UP, INPUT_PULLUP);
  pinMode(PIN_VOL_DOWN, INPUT_PULLUP);

  Serial.printf("Use KEY5/KEY6 for volume Up/Down\n");

  // setup audio lib
  dsp.begin(fs);

  synth.begin(fs);
  synth.noteOff();
  synth.setWaveform(SAW);
  synth.setGlide(0);
  synth.setAttack(50);
  synth.setSustain(0.6);

  // setup some filter nodes
  lp.begin(fs, channelCount);
  lp.setupFilter(FilterNode::LPF, 5000, 1.7);

  hp.begin(fs, channelCount);
  hp.setupFilter(FilterNode::HPF, 20, 1.0);

  // add nodes to audio processing tree
  // Synth => hp => lp => I2S out
  // dsp.addNode(&hp);
  dsp.addNode(&lp);


  // run audio in dedicated task on cpu core 1
  xTaskCreatePinnedToCore(audioTask, "audioTask", 10000, NULL, 10, NULL, 1);
  // run control task on another cpu  core with lower priority
  Serial.print("\nSetup done ");

}

bool pressed( const int pin )
{
  if (millis() > (debounce + 500))
  {
    if (digitalRead(pin) == LOW)
    {
      debounce = millis();
      return true;
    }
  }
  return false;
}


void audioTask(void *) {

  Serial.print("\nAudio task");

  float sample = 0;

  while (true) {

    for (int i = 0; i < AudioDriver::BufferSize; i++) {

      float sampleMono = synth.getSample();

      for (int ch = 0; ch < channelCount; ch++) {

        // upmix to stereo
        sample = sampleMono;

        sample = dsp.process(sample, ch);

        i2sCodec.writeSample(sample, i, ch);
      }
    }

    i2sCodec.writeBlock();
  }
  vTaskDelete(NULL);
}


void loop()
{
  bool updateVolume = false;

  delay(2);

  //  uncomment to change low pass filter frequency with a pot
  //    lp.updateFilter(pot * 10000.f);

  // playin some notes
  int len = 100;
  arpCnt++;
  switch (note) {
    case 0: {
        if (arpCnt > len) {
          arpCnt = 0;
          synth.note(40);
          note++;
        }
      }
    case 1: {
        if (arpCnt > len) {
          arpCnt = 0;
          synth.note(47);
          note++;
        }
      }
    case 2: {
        if (arpCnt > len) {
          arpCnt = 0;
          synth.note(43);
          note++;
        }
      }
    case 3: {
        if (arpCnt > len) {
          arpCnt = 0;
          synth.note(47);
          note = 0;
        }
      }
  }

  if (pressed(PIN_VOL_UP))
  {
    if (volume <= (63 - volume_step))
    {
      // Increase volume
      volume += volume_step;
      updateVolume = true;
    }
  }
  if (pressed(PIN_VOL_DOWN))
  {
    if (volume >= volume_step)
    {
      // Decrease volume
      volume -= volume_step;
      updateVolume = true;
    }
  }
  if (updateVolume)
  {
    // Volume change requested
    Serial.printf("Volume %d\n", volume);
    i2sCodec.SetVolumeSpeaker(volume);
    i2sCodec.SetVolumeHeadphone(volume);
  }
}
