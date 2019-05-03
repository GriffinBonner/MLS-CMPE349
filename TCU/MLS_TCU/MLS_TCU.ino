/* Title: MLS Timing Control Main 
 * Author: Griffin bonner, Brendan Cain, Malakhi Hopkins, Bruno Mfuh
 * Date: 4/20/2019
 * Description: See TCU System Design Document (SDD) 
 */

#include <stdbool.h>                                                      
#include <timer.h>                                                        // Timing library
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
Timer<1, micros> TX_SEQ;


/* ----------------- TCU Functions -------------------- */


// Initialize Serial Communication 
void serial_init(){
  Serial.begin(115200);
    while(!Serial){
      delay(100);
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

  TX_SEQ.tick();                                                          // Full transmission sequence timer (615 ms period)

  digitalWrite(TIME_SYNC_OUT, HIGH);                                      // 1ms positive pulse

}


