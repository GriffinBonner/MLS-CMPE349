/*
SSIM code for transmitting signnal 
I2c protocal utilizes with wire library
Authors: Zak Kempler, Jd Bryon, Taylor Bear, Cole Schmidt
Reference code ----------
master sender / slave receiver: https://www.arduino.cc/en/Tutorial/MasterWriter
plot://https://learn.adafruit.com/experimenters-guide-for-metro/circ08-using%20the%20arduino%20serial%20plotter
*/

#include <Wire.h>         // I2C interface library
#include <timer.h>        // timer library
#include <stdint.h>

auto timer = timer_create_default(); 
int time = 0;

const float FREQ = 6.0; //frequency in khz
const float BW = 10; //3db beamwidth in degrees
const float SCANVEL = 2.0; //degree per microsecond
float TR = 10; //angle of plane's receiver in the coverage volume 
float TS; //scanning beam angle (should be function of time)
double volt; //output voltage
bool antennas[3]; //0:AZ 1:BAZ 2:E
int phase; //1 or -1
float t; //counter for time?
int timedelay;

uint8_t bytes[3];
uint8_t function;

float Tzero[3] = {6800, 4800, 33500};

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  timer.every(1/FREQ, increment_time);
}

void receiveEvent(int howMany) {
  int i = 0; 
  while (1 < Wire.available()) { // loop through all but the last
    bytes[i]  = Wire.read(); // receive byte as a character
    //Serial.print(bytes[i]);         // print the character
    ++i;
  }
}

bool increment_time(void *){
  time++;
  return true;
}
  
void loop() {
  
  timer.tick();
    
  //reset antennas
  antennas[0] == 0;
  antennas[1] == 0;
  antennas[2] == 0;
    
  //determine function type, then set antenna array
  function = bytes[0]&0b11111110;
    
  if(function == 0x32) //AZ function
  {
    antennas[0] == 1;
    antennas[1] == 0;
    antennas[2] == 0;
    
  }
  else if(function == 0x92) //BAZ
  {
    
    antennas[0] == 0;
    antennas[1] == 1;
    antennas[2] == 0;
  }
  else if (function == 0xC2) //EL
  {
    
    antennas[0] == 0;
    antennas[1] == 0;
    antennas[2] == 1;
  }  
  
  //Determine timing t (in microseconds)
  timedelay = bytes[1]*256 + bytes[2]&0b11111100; //assuming byte3 has msb starting at bit 8 (note 22 bits used out of 3 bytes
    
  //Set Scanning Beam Angle  
  if(antennas[0] == 1)
  {
    TS = (Tzero[0] - timedelay)*SCANVEL/2;
  }
  else if(antennas[1] == 1)
  {
    TS = (Tzero[1] - timedelay)*SCANVEL/2;
  }
  
  else if(antennas[2] == 1)
  {
    TS = (Tzero[2] - timedelay)*SCANVEL/2;
  }
    
  //determine phase shift
  if(bytes[0] & 0b00000001 == 1)
  {
    phase = 1;
  }
  else 
  {
    phase = -1;
  }
      
  //Simulate signal
  if(antennas[0] == 1 || antennas[2] == 1 || antennas[2] == 1 )
  {
    volt = phase*sin(time)*sin((TR-TS)/(1.12*BW))/((TR-TS)/(1.12*BW));
  }
  else 
  {
    volt = 0;
  }
    
  Serial.println(volt); //plot signal
  Serial.print(" "); 
}
