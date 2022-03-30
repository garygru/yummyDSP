#include <YummyDSP.h>
#include <AudioDriver.h>
#include <Codecs/ES8388/ES8388.h>
 
#define I2S_BCK_PIN     27
#define I2S_LRCLK_PIN   25
#define I2S_DOUT_PIN    26
#define I2S_DIN_PIN     35

#define GPIO_PA_EN       GPIO_NUM_21
#define GPIO_SEL_PA_EN   GPIO_SEL_21

static ES8388 i2sCodec;
YummyDSP dsp;
FilterNode lp;
FilterNode hp;
FilterNode bp;
WaveShaperNode sat;

// I2S
const int fs = 44100;
const int channelCount = 2;

int volume = 3;

/*
  i2sCodec.SetInputCh(uint8_t ch); // 1 or 0
  i2sCodec.SetMixInCh(uint8_t ch, float var);
  i2sCodec.SetADCVOL(uint8_t vol); // 1-192
  i2sCodec.SetDACVOL(uint8_t vol); // 1-192
  i2sCodec.SetIn2OoutVOL(uint8_t unused, float vol);
  i2sCodec.SetOUT1VOL(uint8_t vol); //1-33
  i2sCodec.SetOUT2VOL(uint8_t vol); //1-33
  i2sCodec.SetPGAGain(uint8_t vol); //1-8
*/

void audioTask(void *);

void setup() {
  Serial.begin(115200);
  Serial.print("\n\nSetup ");
  i2sCodec.setup(fs, channelCount, I2S_BCK_PIN, I2S_LRCLK_PIN, I2S_DOUT_PIN, I2S_DIN_PIN, GPIO_PA_EN);
  i2sCodec.mute(false);
  
  dsp.begin(fs);
  lp.begin(fs, channelCount);  
  lp.setupFilter(FilterNode::LPF, 500, 1.0);
  hp.begin(fs, channelCount);  
  hp.setupFilter(FilterNode::HPF, 50, 1.0);
  sat.begin(fs, channelCount);  
  sat.setDrive(0.3f);
  
  dsp.addNode(&hp);
  dsp.addNode(&sat);
  dsp.addNode(&lp);
  i2sCodec.DumpRegisters();
  //i2sCodec.SetDACVOL(1,volume);
  //
  i2sCodec.SetOUT1VOL(25); //1-33
  i2sCodec.SetOUT2VOL(25); //1-33
  i2sCodec.SetADCVOL(volume);
  i2sCodec.SetDACVOL(volume); 
  i2sCodec.SetPGAGain(1); //1-8
  delay(2000);
  xTaskCreatePinnedToCore(audioTask, "audioTask", 10000, NULL, 10, NULL, 1);
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
      //sample *= 2.0f;
      sample = dsp.process(sample, ch);
      i2sCodec.writeSample(sample, i, ch);
      }
    }
    i2sCodec.writeBlock();
  }
  vTaskDelete(NULL);
}

void loop(){ 
  //i2sCodec.SetADCVOL(volume);
  //i2sCodec.SetDACVOL(volume);
  //i2sCodec.SetOUT1VOL(volume); //1-33
  //i2sCodec.SetOUT2VOL(volume); //1-33
  //i2sCodec.SetPGAGain(volume); //1-8
  delay(500);
  volume+=1;
  if (volume>8) 
    volume = 0;
}
