# 💾 STM32 Blackpill SD Card & FATFS Integration

![STM32](https://img.shields.io/badge/STM32-F411CEU6-blue.svg)
![FatFs](https://img.shields.io/badge/FatFs-Generic_FAT_FS-green.svg)
![Interface](https://img.shields.io/badge/Interface-SPI-orange.svg)

Hey there! 👋 Welcome to my STM32 Blackpill SD Card project. 

If you've ever wanted to add data logging, save settings, or just read files from an SD card using an STM32 microcontroller, you're in the right place. This project shows you how to hook up an SD card to the **STM32F411CEU6 (Blackpill)** board using the **FATFS (File Allocation Table File System)** over a standard **SPI bus**. 

I built this using **STM32CubeIDE**, so it should be pretty straightforward to open up, tweak, and flash to your own board!

---

## 📑 What's Inside?
1. [Why this project?](#-why-this-project)
2. [What You'll Need](#-what-youll-need)
3. [Wiring Guide](#-wiring-guide)
4. [How to Run It](#-how-to-run-it)
5. [Code Example: Writing a File](#-code-example-writing-a-file)
6. [Troubleshooting Tips](#-troubleshooting-tips)

---

## 🤔 Why this project?

Usually, SD Cards can run in a native "SDIO" mode or a slower (but much friendlier) "SPI" mode. Because the Blackpill board doesn't expose all the SDIO pins perfectly for a breadboard without bumping into other features, I decided to go with SPI. 

In this repo, I've written a custom, bare-metal SPI driver that handles all the gritty details of waking up the SD card and getting it ready to talk to the STM32. Once it's awake, **Chan's FatFs Module** (which comes bundled in STM32Cube) takes over so we can easily read and write normal files like `test.txt`.

---

## 🛠 What You'll Need

To get this up and running on your desk, grab these:
- An **STM32F411CEU6 "Blackpill"** Development Board.
- A **Micro SD Card Adapter Module** (make sure it supports SPI!).
- A **Micro SD Card** (Formatted as FAT32, usually 32GB or smaller works best).
- An **ST-Link V2** Programmer so you can flash the code.
- *(Optional)* A USB to TTL Serial Converter if you want to read the debugging logs.
- Some jumper wires and a breadboard.

---

## 🔌 Wiring Guide

We're going to use **SPI1** to talk to the SD card. Try to keep your jumper wires relatively short so the signals don't get messy at high speeds!

| Blackpill Pin | SD Card Module | What it does |
| :---: | :---: | :--- |
| **PA5** | `SCK` | The clock signal that keeps everything in sync. |
| **PA6** | `MISO` | Data going *from* the SD card *to* the STM32. |
| **PA7** | `MOSI` | Data going *from* the STM32 *to* the SD card. |
| **PA3** | `CS` (or `SS`) | "Chip Select" – tells the SD card it's time to talk. |
| **3.3V** | `VCC` | Power (Double check if your specific module needs 3.3V or 5V!). |
| **GND** | `GND` | Ground. |

**Want to see the serial logs? (USART1):**
- Connect **PA9** to the `RX` of your Serial Monitor.
- Connect **PA10** to the `TX` of your Serial Monitor.
- Set your baud rate to `115200`.

---

## 🚀 How to Run It

### 1. Download the code
```bash
git clone https://github.com/your-username/stm32_blackpill_sdcard_fatfs.git
cd stm32_blackpill_sdcard_fatfs
```

### 2. Open it up in STM32CubeIDE
1. Open **STM32CubeIDE**.
2. Go to `File` ➔ `Import...` ➔ `General` ➔ `Existing Projects into Workspace`.
3. Find the folder you just downloaded and click **Finish**.

### 3. Flash it to your board
1. Hook up your ST-Link to the `GND`, `3.3V`, `SWDIO`, and `SWCLK` pins on your Blackpill.
2. Hit the 🔨 icon (or `Project` ➔ `Build Project`) to compile.
3. Hit the 🐛 debug icon (or `Run` ➔ `Debug`) to flash the firmware onto the board.

---

## 💻 Code Example: Writing a File

Once the card is initialized, using the FATFS library is pretty easy. If you want to test it out, you can drop this snippet into your `main.c` file right inside the `USER CODE BEGIN 2` section:

```c
FATFS fs;
FIL fil;
FRESULT fres;
UINT bytesWritten;

// 1. Mount the SD Card
fres = f_mount(&fs, "", 1);
if (fres == FR_OK) {
    // 2. Open a file called test.txt (it will create it if it doesn't exist)
    fres = f_open(&fil, "test.txt", FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    
    if (fres == FR_OK) {
        // 3. Write a friendly message!
        char myData[] = "Hello from the STM32 Blackpill!\r\n";
        f_write(&fil, myData, strlen(myData), &bytesWritten);
        
        // 4. Save and close the file
        f_close(&fil);
    }
    // 5. Unmount the drive when you're done
    f_mount(NULL, "", 1);
}
```

---

## ⚠️ Troubleshooting Tips

Running into a wall? Don't stress, hardware can be finicky. Here are a few common hiccups and how to fix them:

* **The card just won't initialize (Fails to enter IDLE state):** 
  * Take a deep breath and double-check those jumper wires! A loose or swapped wire (especially MOSI/MISO) is the culprit 90% of the time. We've all been there.
  * Sometimes older SD cards are super picky about how fast you talk to them during initialization. Try lowering the SPI Baud Rate in your `.ioc` file and see if that wakes it up.
* **`f_mount` keeps throwing an error at you:** 
  * Pop your SD card into your computer and make sure it's actually formatted as `FAT32`. Newer file systems like exFAT or NTFS won't work right out of the box with this setup.
* **You're seeing alien hieroglyphics on the Serial Monitor:** 
  * No, the board isn't possessed! Just make sure your terminal software is set to a baud rate of exactly `115200` to match the USART1 configuration.
