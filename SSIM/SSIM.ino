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

uint8_t bytes[3];

#define SDA_PIN 4
#define SCL_PIN 5

const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;  

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
  
  //determine function type
    
  //determine phase shift
    
  //determine timing
    
  //Simulate signal
  volt = phase*peak*sin(2*3.14159*FREQ*t)*sin((TR-TS)/(1.12*BW))/((TR-TS)/(1.12*BW));
}

