/* Title: MLS Timing Control Unit (TCU) Memory Initialization
 * Author: Griffin bonner, Brendan Cain, Malakhi Hopkins, Bruno Mfuh
 * Date: 4/20/2019
 * Description: Initialize flash memory file with TCU system constants
 */

#include <SPI.h>
#include <MsTimer2.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_SPIFlash_FatFs.h>


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

    // TCU Function Identification Codes (7-bits with appended 1-bit DPSK bit)
    if(writeFile.size() <= 0){
      writeFile.println(0x32, HEX);    // Approach Azimuth (AZ) function ID
      writeFile.println(0x92, HEX);    // Back Azimuth (BAZ) function ID
      writeFile.println(0xC2, HEX);    // Approach Elevation (EL) function ID
      writeFile.println(0x50, HEX);    // Basic Data Word 1 (BDW1) function ID
      writeFile.println(0x78, HEX);    // Basic Data Word 2 (BDW1) function ID
      writeFile.println(0xA0, HEX);    // Basic Data Word 3 (BDW1) function ID
      writeFile.println(0x88, HEX);    // Basic Data Word 4 (BDW1) function ID
      writeFile.println(0x1A, HEX);    // Basic Data Word 6 (BDW1) function ID
      writeFile.println(0xE4, HEX);    // Auxiliary Data Word A (ADWA) function ID
      writeFile.println(0xAE, HEX);    // Auxiliary Data Word B (ADWB) function ID
      writeFile.println(0xF1, HEX);    // Auxiliary Data Word C (ADWC) function ID
    }

    writeFile.close();
  }




  // Open tcu_constants.txt file for reading
  File readFile = fatfs.open("site_personality/tcu_constants.txt", FILE_READ);
  if (!readFile) {
    Serial.println("Error, failed to open test.txt for reading!");
    while(1);
  }else{
    Serial.println("Opened file stite_personality/tcu_constants.txt for reading...");

    String line = readFile.readStringUntil('\n');
    Serial.print("Approach Azimuth Function ID: 0x"); Serial.println(line);

    String line2 = readFile.readStringUntil('\n');
    Serial.print("Back Azimuth Function ID: 0x"); Serial.println(line2);

    String line3 = readFile.readStringUntil('\n');
    Serial.print("Approach Elevation Function ID: 0x"); Serial.println(line3);


    for(int i = 0; i < 8; i++){
      String line = readFile.readStringUntil('\n');
      Serial.println(line);
    }

    Serial.print("Total size of test.txt (bytes): "); Serial.println(readFile.size(), DEC);
    Serial.print("Current position in test.txt: "); Serial.println(readFile.position(), DEC);
    Serial.print("Available data to read in test.txt: "); Serial.println(readFile.available(), DEC);
  }
}


// 
void setup() {

  serial_init();          // initialize serial connection (Baud: 115200)                                         
  fatfs_init();           // mount FAT filesystem

}

void loop() {


}
