# ESP32 SD Card Screensaver Loader

Universal firmware for ESP32-2432S028 (CYD) that reads and executes screensaver effects from SD card.

## Features

- **Universal Base Firmware** - Flash once, run any effects from SD card
- **SD Card Driven** - No hardcoded effects, completely customizable
- **Boot Button Control** - Navigate effects and toggle auto-scroll
- **Auto-scroll Mode** - Automatically cycles through effects every 30 seconds
- **Multiple Effect Support** - Currently supports 6+ different visual effects

## Hardware Requirements

- ESP32-2432S028 (Cheap Yellow Display)
- SD card formatted as FAT32
- Effects files in `/effects/` directory on SD card

## Installation

### Quick Start (Pre-built Binary)
1. Flash `ESP32_SD_Screensaver_Loader.bin` to your ESP32-2432S028
2. Insert SD card with effects files
3. Device automatically runs screensavers from SD card

### Build from Source
1. Install PlatformIO
2. Clone this repository
3. Build and upload with `pio run --target upload`

## Controls

- **Boot Button Short Press** - Next effect
- **Boot Button Long Hold (2+ seconds)** - Toggle auto-scroll ON/OFF
- **Auto-scroll** - Changes effects every 30 seconds when enabled

## SD Card Structure

Create an `/effects/` directory on your SD card with numbered effect files:
```
/effects/
  00_ABSTRACTILE.txt
  01_ALIGNED_MALLOC.txt
  02_ALPHA.txt
  03_ANALOGTV.txt
  04_ANALOGTV_CLI.txt
  05_ANEMONE.txt
  ...
```

Each `.txt` file should contain the effect name on the first line.

## Supported Effects

Currently includes 6 built-in effects:
- **Abstractile** - Animated tile patterns
- **Aligned Malloc** - Memory allocation visualization  
- **Alpha** - Transparency effects with floating circles
- **AnalogTV** - Old TV static and interference
- **AnalogTV CLI** - Terminal-style scrolling text
- **Anemone** - Sea anemone tentacle movements

Additional effects fall back to plasma visualization.

## Technical Details

- **Display**: 320x240 ILI9341 TFT
- **SD Card**: Separate VSPI bus (CS=5, MISO=19, MOSI=23, SCK=18)
- **Memory Usage**: 28.4% flash, 6.9% RAM
- **Effect Rate**: 50ms refresh cycle

## Hardware Pinout

```cpp
// Display SPI (Hardware SPI)
#define DC_PIN 2
#define CS_PIN 15
#define SCK_PIN 14
#define MOSI_PIN 13
#define MISO_PIN 12

// SD Card SPI (VSPI)
#define SD_CS 5
#define SD_MISO 19  
#define SD_MOSI 23
#define SD_SCK 18

// Controls
#define BOOT_PIN 0
#define BL_PIN 21
```

## License

Open source - use and modify freely for personal and commercial projects.

## Project Status

This firmware successfully reads SD card effects and provides a universal base for custom screensaver collections. The modular design allows for easy expansion and customization.