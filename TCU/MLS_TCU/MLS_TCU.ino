/* Title: MLS Timing Control Main 
 * Author: Griffin bonner, Brendan Cain, Malakhi Hopkins, Bruno Mfuh
 * Date: 4/20/2019
 * Description: See TCU System Design Document (SDD) 
 */

#include <stdbool.h>                                                      
#include <stdio.h>
#include <timer.h>                                                        // Timing library
#include <SPI.h>                                                          // SPI interface library
#include <Wire.h>                                                         // I2C interface library
#include <Adafruit_SPIFlash.h>                                            // SPI flash library
#include <Adafruit_SPIFlash_FatFs.h>                                      // SPI flash FAT filesystem library


/* ----------------- SPI FLASH definitions ----------- */
#define FLASH_TYPE    SPIFLASHTYPE_W25Q16BV                               // Configuration of flash fatfs object
#if defined(__SAMD51__)
  Adafruit_SPIFlash flash(PIN_QSPI_SCK, PIN_QSPI_IO1, PIN_QSPI_IO0, PIN_QSPI_CS);
#else
  #if (SPI_INTERFACES_COUNT == 1)
    #define FLASH_SS       SS                                             // Flash chip SS pin.
    #define FLASH_SPI_PORT SPI                                            // What SPI port is Flash on?
  #else
    #define FLASH_SS       SS1                                            // Flash chip SS pin.
    #define FLASH_SPI_PORT SPI1                                           // What SPI port is Flash on?
  #endif
Adafruit_SPIFlash flash(FLASH_SS, &FLASH_SPI_PORT);
#endif
Adafruit_W25Q16BV_FatFs fatfs(flash);

/* ----------------- Global Variables ---------------- */
enum FUNCTION{AZ, BAZ, EL, BDWD1, BDWD2, BDWD3, BDWD4, BDWD5, BDWD6, ADWDA, ADWDB, ADWDC};
byte FUNCTION_ID[12] = {0x32, 0x92, 0xC2, 0x50, 0x78, 0xA0, 0x88, 0xD8, 0x1A, 0xE4, 0xAE, 0xF0};

// TODO: Potentially create artificial timing data bytes)

/* ----------------- Test Bytes ---------------------- */


/* ----------------- Signal Definitions -------------- */
const int TIME_SYNC_OUT = 1;                                               // Time synchronization output (1ms positive pulse corresponding to time-zero of transmission cycle)
const int SB_TO_FRO = 7;                                                   // Scan TO - FRO 
const int SB_START = 2;                                                    // Pulse on start of scanning beam
const int PHASE = 9;                                                       // Phase Select (DPSK)
const int ANT0 = 10;                                                       // Antenna Select bit-0
const int ANT1 = 11;                                                       // Antenna Select bit-1
const int ANT2 = 12;                                                       // Antenna Select bit-2
const int TX_EN = 13;                                                      // Transmit Enable 


/* ----------------- System Timer -------------------- */
Timer<1, micros> TX_SEQ_TIMER;                                             // Full transmission sequence timer (615 ms period)
Timer<2, micros> PULSE_TIMER;                                              // Positive Pulse timer (1 ms period)
Timer<8, micros> AZ_TIMER;                                                 // Azimuth Function Timer (76875 usec period)
Timer<10, micros> GEN_TIMER;                                                // General Timer
//Timer<4, micros> BAZ_TIMER;
//Timer<24, micros> EL_TIMER;



/* ----------------- TCU Functions ------------------- */
// Initialize Serial Communication 
void serial_init(){
  Serial.begin(115200);
    while(!Serial){
      delay(100);
    }
}

// Read TCU constants from SPI flash memory to local SRAM
void fatfs_read(){

  // Initialize SPI Flash
  if(!flash.begin(FLASH_TYPE)){
    Serial.println("Error, failed to initialize Flash Memory");
    while(1);
  }
  Serial.print("Flash chip JEDEC ID: 0x"); Serial.println(flash.GetJEDECID(), HEX);

  // Mount fatfs Flash filesystem
  if(!fatfs.begin()){
    Serial.println("Error mounting filesystem");
    while(1);
  }
  Serial.println("Filesystem Mounted...");

  // Open file for reading
  File readFile = fatfs.open("site_personality/tcu_constants.txt", FILE_READ);
  if (!readFile) {
    Serial.println("Error, failed to open tcu_constants.txt for reading!");
    while(1);
  }else{
    Serial.println("Opened file stite_personality/tcu_constants.txt for reading...");
    Serial.print("Total size of tcu_constants.txt (bytes): "); Serial.println(readFile.size(), DEC);
    Serial.print("Current position in tcu_constants.txt: "); Serial.println(readFile.position(), DEC);
    Serial.print("Available data to read in tcu_constants.txt: "); Serial.println(readFile.available(), DEC);

    // Read function ID's into SRAM array
    /*
    for(int i = 0; i < 11; i++){
      String line = readFile.readStringUntil('\n');
      function_id[i] = line[0];
      Serial.println(function_id[i]);
    }
    */
  }
}

// Full transmission sequence function (615 ms period)
bool TX_SEQUENCE(void *){
  Serial.println("Full Transmission Sequence Started");

  // 1 ms positive pulse 
  PULSE_TIMER.in(0, TOGGLE_PULSE);                        
  PULSE_TIMER.in(1000, TOGGLE_PULSE);
  return true;
}

// Approach Azimuth Function
bool AZ_FNC(void *){
  Serial.println("Approach Azimuth Function Started");

  // Transmit Approach Azimuth function ID to slave SSIM
  Wire.beginTransmission(8);
  Wire.write(FUNCTION_ID[AZ]);

  
  
  // Handle timing for TX_EN signal
  GEN_TIMER.in(2560, TOGGLE_TX_EN);
  GEN_TIMER.in(8760, TOGGLE_TX_EN);
  GEN_TIMER.in(9360, TOGGLE_TX_EN);
  GEN_TIMER.in(15560, TOGGLE_TX_EN);

  // Handle timing for SB_START signal
  GEN_TIMER.in(2560, TOGGLE_SB_START);
  GEN_TIMER.in(3560, TOGGLE_SB_START);
  GEN_TIMER.in(9360, TOGGLE_SB_START);
  GEN_TIMER.in(10360, TOGGLE_SB_START);

  // Handle timing for SB_TO_FRO signal
  GEN_TIMER.in(2560, TOGGLE_SB_TF);
  GEN_TIMER.in(8760, TOGGLE_SB_TF);
  return true;
}

// Back Azimuth Function
bool BAZ_FNC(void *){
  Serial.println("Back Azimuth Function Started");
  return true;
}

// Approach Elevation Function
bool EL_FNC(void *){
  Serial.println("Approach Elevation Function Started");
  return true;
}

// Basic Data Word 
bool BDWD_FNC(void *){
  Serial.println("Basic Data Word Function Started");
  return true;
}

// Auxiliary Data Word
bool ADWD_FNC(void *){
  Serial.println("Auxiliary Data Word Function Started");
  return true;
}

bool SEQ1_FNC(void *){
  Serial.println("Sequence 1 Function Started");
  return true;
}

bool SEQ2_FNC(void *){
  Serial.println("Sequence 2 Function Started");
  return true;
}

// Toggle 1 ms positive pulse
bool TOGGLE_PULSE(void *){
  digitalWrite(TIME_SYNC_OUT, !digitalRead(TIME_SYNC_OUT));
  return true;
}

// Toggle SB_TO_FRO (scanning beam to/fro) output
bool TOGGLE_SB_TF(void *){
  digitalWrite(SB_TO_FRO, !digitalRead(SB_TO_FRO));
  return true;
}

// Toggle SB_START (scanning beam start)
bool TOGGLE_SB_START(void *){
  digitalWrite(SB_START, !digitalRead(SB_START));
  return true;
}

// Toggle PHASE (differential phase shift keying) output
bool TOGGLE_PHASE(void *){
  digitalWrite(PHASE, !digitalRead(PHASE));
  return true;
}


// Toggle TX_EN (transmit enable) output
bool TOGGLE_TX_EN(void *){
  digitalWrite(TX_EN, !digitalRead(TX_EN));
  return true;
}

// IDENT/DATA (ALL)
bool ANT_DATA(void *){
  digitalWrite(ANT0, LOW);
  digitalWrite(ANT1, LOW);
  digitalWrite(ANT2, LOW);
  return true;
}

// L-OCI (AZ, BAZ)
bool ANT_LOCI(void *){
  digitalWrite(ANT0, HIGH);
  digitalWrite(ANT1, LOW);
  digitalWrite(ANT2, LOW);
  return true;
}

// R/U-OCI (AZ, BAZ, EL)
bool ANT_RUOCI(void *){
  digitalWrite(ANT0, LOW);
  digitalWrite(ANT1, HIGH);
  digitalWrite(ANT2, LOW);
  return true;
}

// L CLR (AZ, BAZ)
bool ANT_LCLR(void *){
  digitalWrite(ANT0, HIGH);
  digitalWrite(ANT1, HIGH);
  digitalWrite(ANT2, LOW);
  return true;
}

// R CLR (AZ, BAZ)
bool ANT_RCLR(void *){
  digitalWrite(ANT0, LOW);
  digitalWrite(ANT1, LOW);
  digitalWrite(ANT2, HIGH);
  return true;
}

// SCANNING (AZ, BAZ, EL)
bool ANT_SCANNING(void *){
  digitalWrite(ANT0, HIGH);
  digitalWrite(ANT1, LOW);
  digitalWrite(ANT2, HIGH);
  return true;
}

// OFF (Default Position)
bool ANT_OFF(void *){
  digitalWrite(ANT0, LOW);
  digitalWrite(ANT1, HIGH);
  digitalWrite(ANT2, HIGH);
  return true;
}

// TEST (Not used)
bool ANT_TEST(void *){
  digitalWrite(ANT0, HIGH);
  digitalWrite(ANT1, HIGH);
  digitalWrite(ANT2, HIGH);
}

/* ----------------- TCU Setup -------------------- */
void setup() {

  serial_init();                                                          // Initialize serial connection             
  fatfs_read();                                                           // Read data from tcu_constants.txt to local SRAM
  Wire.begin();                                                           // Join I2C Bus

  // Set pins as outputs
  pinMode(TIME_SYNC_OUT, OUTPUT);                                         // Configure time sync pin 3 as output 
  pinMode(SB_TO_FRO, OUTPUT);                                             // Set SB_TO_FRO pin as output (scanning beam to/fro)
  pinMode(SB_START, OUTPUT);                                              // Set SB_START pin as output (scanning bream start)
  pinMode(PHASE, OUTPUT);                                                 // Set PHASE pin as output
  pinMode(ANT0, OUTPUT);                                                  // Set ANT0 pin as output
  pinMode(ANT1, OUTPUT);                                                  // Set ANT1 pin as output
  pinMode(ANT2, OUTPUT);                                                  // Set ANT2 pin as output

  // Initialize periodic timers
  TX_SEQ_TIMER.every(615000, TX_SEQUENCE);                                // Restart full transmission sequence every 615 ms
  AZ_TIMER.every(76875, AZ_FNC);
  //BAZ_TIMER.every(153750, BAZ_FNC);
  //EL_TIMER.every(25625, EL_FNC);
}

// Tick system timers
void loop(){
  TX_SEQ_TIMER.tick();                                                     // Full transmission sequence timer (615 ms period)
  PULSE_TIMER.tick();                                                      // Full transmission sequence timer positive pulse (1 ms period)
  AZ_TIMER.tick();                                                         // Approach Azimuth Function (Freq: 13 Hz)
  GEN_TIMER.tick();
  //BAZ_TIMER.tick();                                                      // Back Azimuth Function (Freq: 6.5 Hz)
  //EL_TIMER.tick();                                                       // Approach Elevation Function (Freq: 39 Hz)
}


