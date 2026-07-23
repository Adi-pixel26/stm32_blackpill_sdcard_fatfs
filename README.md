# STM32 Blackpill SD Card & FATFS Integration

This repository demonstrates how to interface a micro SD Card with the STM32F411CEU6 (Blackpill) development board. It utilizes the FATFS (File Allocation Table File System) middleware over a standard SPI bus to enable robust file read and write operations.

Built entirely within STM32CubeIDE, this project provides a solid foundation for embedded developers looking to integrate data logging, configuration management, or external storage capabilities into their STM32 applications without relying on the native SDIO interface.

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Hardware Requirements](#hardware-requirements)
3. [Wiring & Pin Configuration](#wiring--pin-configuration)
4. [Project Architecture](#project-architecture)
5. [Getting Started](#getting-started)
6. [Code Example: FATFS Operations](#code-example-fatfs-operations)
7. [Troubleshooting](#troubleshooting)

---

## Project Overview

While SD Cards can operate via a native SDIO interface, the STM32 Blackpill board does not expose all necessary SDIO pins optimally for breadboard use without conflicting with other peripherals. This project overcomes that limitation by utilizing the more universally supported SPI mode.

This repository features a custom, bare-metal SPI driver responsible for the low-level initialization and communication sequence required to bring the SD card out of its idle state. Once initialized, the project leverages Chan's FatFs Module (integrated via STM32Cube) to manage the file system seamlessly.

---

## Hardware Requirements

To replicate or build upon this project, you will need the following components:
- **STM32F411CEU6 "Blackpill"** Development Board.
- **Micro SD Card Adapter Module** (Ensure it supports SPI mode).
- **Micro SD Card** (Formatted to FAT32, preferably 32GB or smaller).
- **ST-Link V2** Programmer (For flashing and debugging).
- USB to TTL Serial Converter (Optional, for monitoring USART logs).
- Standard jumper wires and a breadboard.

---

## Wiring & Pin Configuration

The system uses the **SPI1** peripheral for communication with the SD card. It is recommended to use short jumper wires to maintain signal integrity at higher SPI clock frequencies.

| STM32 Blackpill Pin | SD Card Module Pin | Function Description |
| :---: | :---: | :--- |
| **PA5** | `SCK` | SPI Clock – Synchronizes data transmission. |
| **PA6** | `MISO` | Master In Slave Out – Data from the SD card to the STM32. |
| **PA7** | `MOSI` | Master Out Slave In – Data from the STM32 to the SD card. |
| **PA3** | `CS` (or `SS`) | Chip Select – Activates the SD card for communication. |
| **3.3V** | `VCC` | Power Supply (Verify if your specific module requires 3.3V or 5V). |
| **GND** | `GND` | Common Ground. |

**Serial Debugging (USART1):**
- Connect **PA9** to the `RX` pin of your Serial Monitor.
- Connect **PA10** to the `TX` pin of your Serial Monitor.
- Set your terminal baud rate to **115200**.

---

## Project Architecture

### Directory Structure
- `Core/Src/main.c`: Contains the main application loop and the custom low-level SPI SD Card driver.
- `FATFS/`: Contains Chan's FatFs Middleware files and the STM32 target configurations for disk I/O.
- `Drivers/`: Contains the standard STM32 HAL (Hardware Abstraction Layer) drivers.
- `sd_card.ioc`: The STM32CubeMX configuration file.

### SPI Driver Initialization Sequence
The `main.c` file executes the following sequence to properly initialize the SD card over SPI:
1. **Clock Train**: Sends 80+ clock pulses with the Chip Select line held high to wake the card.
2. **Software Reset (CMD0)**: Places the SD Card into SPI mode.
3. **Voltage Check (CMD8)**: Validates the operating voltage conditions.
4. **Initialization (ACMD41)**: Polls the card until the initialization process is complete.
5. **Capacity Check (CMD58)**: Identifies whether the card is Standard Capacity (SDSC) or High Capacity (SDHC/SDXC).

---

## Getting Started

### 1. Clone the Repository
Open your terminal and clone the repository to your local machine:
```bash
git clone https://github.com/your-username/stm32_blackpill_sdcard_fatfs.git
cd stm32_blackpill_sdcard_fatfs
```

### 2. Import into STM32CubeIDE
1. Launch **STM32CubeIDE**.
2. Navigate to `File` ➔ `Import...` ➔ `General` ➔ `Existing Projects into Workspace`.
3. Browse to the cloned folder and click **Finish**.

### 3. Build and Flash
1. Connect your ST-Link to the `GND`, `3.3V`, `SWDIO`, and `SWCLK` pins on your Blackpill board.
2. Build the project by selecting `Project` ➔ `Build Project` from the top menu.
3. Select `Run` ➔ `Debug` to flash the compiled firmware onto the microcontroller.

---

## Code Example: FATFS Operations

Once the low-level SPI driver successfully initializes the SD card, you can use standard FATFS functions to manipulate files. Below is an example snippet demonstrating how to mount the drive and write a file. 

This code can be placed within the `USER CODE BEGIN 2` section of your `main.c`:

```c
FATFS fs;
FIL fil;
FRESULT fres;
UINT bytesWritten;

// 1. Mount the SD Card file system
fres = f_mount(&fs, "", 1);
if (fres == FR_OK) {
    // 2. Open a file for writing (creates the file if it does not exist)
    fres = f_open(&fil, "log.txt", FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    
    if (fres == FR_OK) {
        // 3. Write data to the file
        char logMessage[] = "System Initialized successfully.\r\n";
        f_write(&fil, logMessage, strlen(logMessage), &bytesWritten);
        
        // 4. Ensure data is saved and close the file
        f_close(&fil);
    }
    // 5. Unmount the drive when operations are complete
    f_mount(NULL, "", 1);
}
```

---

## Troubleshooting

If you encounter issues during setup or operation, consider the following common scenarios:

* **SD Card Fails to Initialize (Cannot enter IDLE state):** 
  * Verify all hardware connections. A loose jumper wire or reversed MISO/MOSI connection is the most frequent cause.
  * SD cards require a low clock frequency (typically < 400kHz) during the initial startup phase. Try increasing the SPI Baud Rate Prescaler in your `.ioc` file to lower the initialization speed.
* **`f_mount` Returns an Error:** 
  * Ensure your micro SD card is formatted to `FAT32`. File systems such as exFAT or NTFS are not supported by the default FatFs configuration in this project.
* **Garbage Characters in the Serial Monitor:** 
  * Verify that your serial terminal software is configured to exactly `115200` baud, matching the USART1 configuration on the STM32.
