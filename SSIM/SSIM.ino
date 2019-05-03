/*
SSIM code for transmitting signnal 
I2c protocal utilizes with wire library
Authors: Zak Kempler, Jd Bryon, Taylor Bear, Cole Schmidt

Reference code ----------
slave receiver: https://github.com/esp8266/Arduino/blob/master/libraries/Wire/examples/slave_receiver/slave_receiver.ino
master sender: https://github.com/esp8266/Arduino/blob/master/libraries/Wire/examples/master_writer/master_writer.ino
plot://https://learn.adafruit.com/experimenters-guide-for-metro/circ08-using%20the%20arduino%20serial%20plotter
*/
#include <Wire.h>         // I2C interface library
#include <stdint.h>

const float FREQ = 6.0; //frequency in khz
const float BW = 10; //3db beamwidth in degrees
const float SCANVEL = 2.0; //degree per microsecond
float TR = 66; //angle of plane's receiver in the coverage volume 
float TS; //scanning beam angle (should be function of time)
double volt; //output voltage
bool antennas[3]; //0:AZ 1:BAZ 2:EL
float peak; //peak set by TCU given function
int phase; //1 or -1
float t; //counter for time?
int timedelay;

uint8_t bytes[3];
uint8_t function;

const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;
#define SDA_PIN 4
#define SCL_PIN 5  

float Tzero[3] = {6800, 4800, 33500};

void setup() {
  Serial.begin(115200);           // start serial for output (serial used for plot)
  Wire.begin(SDA_PIN, SCL_PIN, I2C_SLAVE); // new syntax: join i2c bus (address required for slave)
  Wire.onReceive(receiveEvent); // register event
}

void loop() {
  void receiveEvent(size_t howMany) {
  int i = 0; 
  (void) howMany;
  while (1 < Wire.available()) { // loop through all but the last
    bytes[i]  = Wire.read(); // receive byte as a character
    //Serial.print(bytes[i]);         // print the character
    ++i;
  }
  
  //determine function type, then set antenna array
  function = byte[0]&0b11111110;
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
    
    
    
  //determine phase shift
  if(byte[0] & 0b00000001 == 1)
  {
    phase = 1;
  }
  else 
  {
    phase = -1;
  }
    
  //determine timing (in microseconds)
  timedelay = byte[1]*256 + byte[2]&0b11111100; //assuming byte3 has msb starting at bit 8 (note 22 bits used out of 3 bytes)
    
  //Simulate signal
  volt = phase*peak*sin(2*3.14159*FREQ*t)*sin((TR-TS)/(1.12*BW))/((TR-TS)/(1.12*BW));
    
  Serial.println(volt); //plot signal
  Serial.print(" "); 
}

