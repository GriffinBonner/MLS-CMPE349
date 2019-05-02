#include <SPI.h>                                                          // SPI interface library
#include <Wire.h>                                                         // I2C interface library
#include <stdint.h>

const float FREQ = 6.0; //frequency in khz
const float BW = 10; //3db beamwidth in degrees
float TR = 66; //angle of plane's receiver in the coverage volume 
float TS; //scanning beam angle (should be function of time)
double volt; //output voltage
bool antennas[3]; //0:AZ 1:BAZ 2:EL
float peak; //peak set by TCU given function
int phase; //1 or -1
float t; //counter for time?

  
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  //get code from TCU

  //set peak, boolean array, phase shift 
  
  //Simulate signal
  volt = phase*peak*sin(2*3.14159*FREQ*t)*sin((TR-TS)/(1.12*BW))/((TR-TS)/(1.12*BW));
}
