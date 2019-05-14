/*
  SSIM code for transmitting signnal
  I2c protocal utility with wire library
  Authors: Zak Kempler, Jd Bryon, Taylor Bear, Cole Schmidt
  Reference code ----------
  master sender / slave receiver: https://www.arduino.cc/en/Tutorial/MasterWriter
  timer: https://github.com/contrem/arduino-timer
  plot://https://learn.adafruit.com/experimenters-guide-for-metro/circ08-using%20the%20arduino%20serial%20plotter
*/
#include <timer.h>        // timer library
//#include <stdint.h>

const float FREQ = 150; //frequency in khz
const float BW = 1; //3db beamwidth in degrees
float TR = 10; //angle of plane's receiver in the coverage volume
float TS = -62; //scanning beam angle (should be function of time)
double volt = 0; //output voltage
int phase = 1; //1 or -1
double time = 0;
double freqtime = 0;
double amplitude;

const int SB_TO_FRO = 7;                                                   // Scan TO - FRO 
const int SB_START = 2;                                                    // Pulse on start of scanning beam
const int PHASE = 9;                                                       // Phase Select (DPSK)
const int ANT0 = 10;                                                       // Antenna Select bit-0
const int ANT1 = 11;                                                       // Antenna Select bit-1
const int ANT2 = 12;                                                       // Antenna Select bit-2
const int TX_EN = 13;                                                      // Transmit Enable

Timer<2, micros> timer;

uint8_t bytes;
uint8_t function = 0;
int reset = 0;
int i = 0;

void setup() {
  pinMode(SB_TO_FRO, INPUT);                                             // Set SB_TO_FRO pin as input (scanning beam to/fro)
  pinMode(SB_START, INPUT);                                              // Set SB_START pin as input (scanning bream start)
  pinMode(PHASE, INPUT);                                                 // Set PHASE pin as input
  pinMode(ANT0, INPUT);                                                  // Set ANT0 pin as input
  pinMode(ANT1, INPUT);                                                  // Set ANT1 pin as input
  pinMode(ANT2, INPUT);                                                  // Set ANT2 pin as input
  pinMode(TX_EN, INPUT);
  
  Serial.begin(9600);           // start serial for output
  timer.every(50, TS_Inc);
  timer.every(25, freq_Inc);
}

bool freq_Inc(void *)
{
  freqtime++;
  return true;
  }

bool TS_Inc(void *) { 
  if(reset == 0 && digitalRead(SB_START))
  {
    time = 0;
  }
  if(digitalRead(TX_EN))
  {
  time = time + 67; //94
  if(digitalRead(SB_TO_FRO) ) {
  TS  = -62+.02*time; }
  else{
  TS = 62 - .02*time;  
  }      
  if(digitalRead(SB_START))
  {
  reset = 1;  
    }
   else
   {
    reset = 0;
    }
  }
  
  if(digitalRead(ANT2) && digitalRead(ANT1) && digitalRead(ANT0)) 
  {
    amplitude = .1; // test
  }
  else if(digitalRead(ANT2) && digitalRead(ANT1) && !digitalRead(ANT0))
  {
    amplitude = 0; // off
  }
  else if(digitalRead(ANT2) && !digitalRead(ANT1) && digitalRead(ANT0))
  {
    amplitude = 10; // scanning
  }
  else if(digitalRead(ANT2) && !digitalRead(ANT1) && !digitalRead(ANT0))
  {
    amplitude = .2; // R CLR
  }
  else if(!digitalRead(ANT2) && digitalRead(ANT1) && digitalRead(ANT0))
  {
    amplitude = .3; // L CLR
  }
  else if(!digitalRead(ANT2) && digitalRead(ANT1) && !digitalRead(ANT0))
  {
    amplitude = .6; // R/U-OCI
  }
  else if(!digitalRead(ANT2) && !digitalRead(ANT1) && digitalRead(ANT0))
  {
    amplitude = .7; // L-OCI
  }
  else if(!digitalRead(ANT2) && !digitalRead(ANT1) && !digitalRead(ANT0))
  {
    amplitude = 1; //IDENT/DATA
  }
  volt = cos(FREQ*2*3.14159*.025*freqtime)*sin((TR - TS) / (1.12 * BW)) / ((TR - TS) / (1.12 * BW));
  Serial.println(10*volt);
  Serial.print(" ");
  return true;
}

void loop() {
  timer.tick();
}
