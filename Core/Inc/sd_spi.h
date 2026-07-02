#ifndef SD_SPI_H_

#define SD_SPI_H_
#include "main.h" // project-wide includes and HAL handles
#include "ffconf.h" // FatFs core types
#include "user_diskio.h" // FatFs disk I/O interface (DSTATUS, DRESULT, etc.)
// Ensure the SPI_HandleTypeDef used is declared in main.c (CubeMX generated)
extern SPI_HandleTypeDef hspi1; // SPI handle for the port we use (SPI2)
// Chip select pin and port for HW-125 (adjust if you choose different pin)
#define SD_CS_PORT GPIOA
#define SD_CS_PIN GPIO_PIN_3
// Macros to control the CS line (active low)
#define SD_CS_HIGH() HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_SET)
#define SD_CS_LOW() HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_RESET)
// Public prototypes required by FatFs (diskio.c contract)
DSTATUS disk_initialize(BYTE pdrv);
DSTATUS disk_status(BYTE pdrv);
DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff);
#endif // SD_SPI_H_
