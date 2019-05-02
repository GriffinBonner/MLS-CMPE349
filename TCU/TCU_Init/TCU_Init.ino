/* Title: MLS Timing Control Unit (TCU) Memory Initialization
 * Author: Griffin bonner, Brendan Cain, Malakhi Hopkins, Bruno Mfuh
 * Date: 4/20/2019
 * Description: Initialize flash memory file with TCU system constants
 */

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

    File writeFile = fatfs.open("/site_personality/tcu_constants.txt", FILE_WRITE);
    if(!writeFile){
      Serial.println("Error, failed to open tcu_constants.txt for writing");
      while(1);
    }
    Serial.println("Opened file site_personality/tcu_constants.txt for writing...");
    // TODO: Write TCU data to tcu_constants flash memory file
    writeFile.close();
  }
}



void setup() {

  serial_init();          // initialize serial connection (Baud: 115200)                                         
  fatfs_init();           // mount FAT filesystem

}

void loop() {


}
