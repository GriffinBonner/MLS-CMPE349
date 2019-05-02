/* Title: MLS Timing Control Main 
 * Author: Griffin bonner, Brendan Cain, Malakhi Hopkins, Bruno Mfuh
 * Date: 4/20/2019
 * Description: See TCU System Design Document (SDD) 
 */

#include <stdbool.h>  
#include <SPI.h>                                                          // SPI interface library
#include <Wire.h>                                                         // I2C interface library
#include <Adafruit_SPIFlash.h>                                            // SPI flash library
#include <Adafruit_SPIFlash_FatFs.h>                                      // SPI flash FAT filesystem library


/* ----------------- SPI FLASH definitions ----------------- */
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


/* ----------------- Signal Definitions -------------- */
const int SLAVE_SELECT = 10;                                               // Active low slave select control signal
const int TIME_SYNC_OUT = 3;                                               // Time synchronization output (1ms positive pulse corresponding to time-zero of transmission cycle)
static uint8_t TX_BYTE;                                                    // Byte transmitted to SSIM
      


/* ----------------- TCU Functions -------------------- */

// Transmit data byte from TCU to SSIM via SPI interface
void TX_SPI(uint8_t TX_byte){
  digitalWrite(SLAVE_SELECT, LOW);                                         // Enable SPI slave
  SPI.transfer(TX_BYTE);                                                   // Transfer TX_byte via SPI to slave
  digitalWrite(SLAVE_SELECT, HIGH);                                        // Disable SPI slave
}

// Initialize Serial Communication 
void serial_init(){
  Serial.begin(115200);
    while(!Serial){
      delay(100);
    }
}

// Initialize - Setup fatfs flash memory filesystem
void fatfs_init(){

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

  // Verify-Create /site_personality directory
  if(!fatfs.exists("/site_personality")){
    Serial.println("site_personality directory not found, creating...");
    if(!fatfs.mkdir("/site_personality")){
      Serial.println("Error, failed to create site_personality directory");
      while(1);
    }
    Serial.println("Created site_personality directory");
  }
  
  File writeFile = fatfs.open("/site_personality/tcu_constants.txt", FILE_WRITE);
  if(!writeFile){
    Serial.println("Error, failed to open tcu_constants.txt for writing");
      while(1);
  }else{
    Serial.println("Opened file site_personality/tcu_constants.txt for writing...");
    // TODO: Write TCU data to tcu_constants flash memory file
    writeFile.close();
  }

  // Open tcu_constants.txt file for reading
  File readFile = fatfs.open("site_personality/tcu_constants.txt", FILE_READ);
  if (!readFile) {
    Serial.println("Error, failed to open test.txt for reading!");
    while(1);
  }else{
    Serial.println("Opened file stite_personality/tcu_constants.txt for reading...");
  }
}


/* ----------------- TCU Setup -------------------- */
void setup() {

  serial_init();                                                          // Initialize serial connection             
  fatfs_init();                                                           // Initialize fatfs flash memory

  pinMode(SLAVE_SELECT, OUTPUT);                                          // Configure SlaveSelect pin: 10 as output
  pinMode(TIME_SYNC_OUT, OUTPUT);                                         // Configure TIME_SYNC_OUT pin: 3 as output
  SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0));       // CPOL: 0, CPHA: 0, Output Edge: Falling, Data Capture: Rising

}

void loop(){

  digitalWrite(TIME_SYNC_OUT, HIGH);                                      // 1ms positive pulse

}

// TODO: Initialize timer overflow ISR for full transmission sequence (615 ms)
void TRANS_ISR(){

}

// TODO: Initialize timer overflow ISR for AZ, BAZ, EL functions
void FUNC_ISR(){
  
}

// TODO: Write constants for transmission sequence ~ AZ, BAZ, EL functions to SPI Flash Memory
void TCU_WriteConstants(){

}
