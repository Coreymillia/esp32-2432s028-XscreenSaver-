# ESP32 XScreenSaver Project - Converting 400+ Classic Screensavers

![ESP32 XScreenSaver](https://img.shields.io/badge/ESP32-XScreenSaver-blue.svg)
![Effects](https://img.shields.io/badge/Effects-187%2F400-brightgreen.svg)
![Status](https://img.shields.io/badge/Status-Active%20Development-orange.svg)

## 🎯 Project Mission

**Converting all 400+ classic XScreenSaver effects to run natively on ESP32 hardware**

This project is systematically porting the entire XScreenSaver collection (originally written for Unix/Linux systems) to run on affordable ESP32-2432S028 "Cheap Yellow Display" hardware. Our goal is to bring decades of mesmerizing computer graphics to modern embedded systems.

### 📈 Current Progress: **187/400+ Effects** (46.75% Complete)

## 🚀 What We're Building

XScreenSaver has been captivating users since 1991 with over 400 mesmerizing visual effects. This project brings that entire collection to modern ESP32 microcontrollers, making decades of computer graphics accessible on $20 hardware.

### 🎯 Project Goals
1. **Complete Conversion**: Port all 400+ XScreenSaver effects to ESP32
2. **Hardware Optimization**: Maximize performance on 320KB RAM / 4MB Flash
3. **Universal Access**: Create an SD card loader system for easy effect management
4. **Documentation**: Maintain detailed progress and conversion notes

## 📊 Current Milestone: 187 Effects Working
- **Memory Usage**: RAM 17.0% (55,688 / 327,680 bytes) | Flash 42.6% (557,777 / 1,310,720 bytes)
- **Performance**: Stable 30+ FPS on most effects
- **Hardware**: ESP32-2432S028 "Cheap Yellow Display" ($20 on AliExpress)
- **Status**: Production-ready firmware with auto-scroll and touch controls

## 🛠️ Hardware Requirements

**ESP32-2432S028 Development Board** ("Cheap Yellow Display")
- **MCU**: ESP32-D0WD-V3 (240MHz dual-core, WiFi, Bluetooth)
- **Display**: 2.8" ILI9341 TFT (320×240, 16-bit color)
- **Touch**: XPT2046 resistive touchscreen
- **Storage**: 4MB Flash, 320KB RAM (+ optional SD card slot)
- **Price**: ~$20 USD on AliExpress/Amazon
- **Pinout**: DC=2, CS=15, SCK=14, MOSI=13, MISO=12, Backlight=21

## 🎮 User Experience
- **Auto-Scroll**: Effects change every 30 seconds automatically
- **Touch Control**: Tap anywhere to advance to next effect
- **Boot Button**: Short press = next effect, long press = toggle auto-scroll
- **Effect Counter**: Corner display shows current effect number (0-186)
- **Instant Boot**: Display activates immediately (proper backlight initialization)

## ⚡ Quick Start

### Method 1: PlatformIO (Source Build)
```bash
git clone https://github.com/Coreymillia/esp32-2432s028-XscreenSaver-.git
cd esp32-2432s028-XscreenSaver-
pio run --target upload
```

### Method 2: Pre-compiled Binary
```bash
# Download the working firmware
wget https://github.com/Coreymillia/esp32-2432s028-XscreenSaver-/raw/main/ESP32_XScreenSaver_187_EFFECTS_WORKING.bin

# Flash directly (Linux/Mac)
esptool.py --chip esp32 --port /dev/ttyUSB0 write_flash 0x1000 ESP32_XScreenSaver_187_EFFECTS_WORKING.bin

# Flash directly (Windows)  
esptool.exe --chip esp32 --port COM3 write_flash 0x1000 ESP32_XScreenSaver_187_EFFECTS_WORKING.bin
```

### ✅ Expected Results
- **Immediate backlight activation** (no blank screen!)
- **Effect #0 starts** displaying immediately after boot
- **Touch screen responds** to advance effects
- **Auto-scroll timer** begins 30-second intervals
- **Stable operation** with <20% memory usage

## 📱 SD Card Loader (Experimental)

We're developing a universal SD card loader system to manage all effects dynamically:

- **Current Status**: Basic SD loader exists but needs refinement
- **Goal**: Load effects from SD card files instead of compiled firmware
- **Benefits**: Add new effects without reflashing, mix-and-match collections
- **Challenge**: Memory constraints require careful optimization

*The SD loader is currently being reworked. This 187-effect firmware represents our stable baseline.*

## 🎨 Featured Effects (187 Total)

This build includes a diverse selection from the XScreenSaver collection:

### Classic Favorites
- **MATRIX** - The famous "Matrix digital rain" effect
- **GEARS** - Spinning mechanical gears
- **FLYINGTOASTERS** - Berkeley Systems classic
- **FLAME** - Fractal flame generator
- **GALAXY** - Spiral galaxy formation
- **MAZE** - Animated maze generation and solving

### Mathematical Visualizations  
- **JULIA** - Julia set fractals
- **MANDELBROT** - Mandelbrot set explorer
- **EULER2D** - Euler mathematics visualization
- **FIBONACCI** - Fibonacci sequence patterns
- **SIERPINSKI** - Sierpinski triangle fractals
- **LORENZ** - Lorenz attractor chaos

### 3D Graphics & Simulations
- **CUBE21** - Modern cube animations
- **SPHERE** - 3D sphere transformations
- **HELIX** - DNA helix structures
- **GRAV** - N-body gravitational physics
- **BUBBLE3D** - 3D bubble physics
- **ATLANTIS** - Underwater scene with sea life

### Retro Computer Graphics
- **APPLE2** - Apple II computer simulation
- **BSOD** - Windows blue screen parody
- **COLORBARS** - TV test pattern bars
- **TERMINAL** - Retro terminal effects
- **PHOSPHOR** - Green phosphor CRT simulation

*[Complete list of all 187 effects available in source code comments]*

## 🔧 Development Progress & Challenges

### ✅ Completed Milestones
- **187 effects** successfully ported and optimized
- **Memory optimization** - under 20% RAM usage on ESP32
- **Touch controls** - responsive interface system
- **Auto-scroll** - hands-free operation
- **Hardware abstraction** - clean separation of display/input layers
- **Build system** - PlatformIO configuration for easy compilation

### 🔄 Current Work
- **SD Card Loader**: Universal effect management system
- **Performance optimization**: Targeting 60 FPS on all effects
- **Memory profiling**: Identifying bottlenecks for remaining 213 effects
- **Effect categorization**: Organizing by complexity and visual style

### 🎯 Upcoming Conversions (213 Effects Remaining)
- **L-Z alphabetical range**: LASER through ZWOPTEX effects
- **Advanced 3D**: OpenGL-dependent effects requiring custom shaders
- **Audio visualizers**: Effects that respond to sound input
- **Network effects**: Screensavers that pull data from internet sources
- **Interactive games**: Effects with complex user interaction

## 🔬 Technical Deep Dive

### Memory Architecture
```
ESP32-2432S028 Specifications:
├── CPU: 240MHz dual-core Tensilica LX6
├── RAM: 320KB (SRAM) + 8MB (external PSRAM available)  
├── Flash: 4MB (program storage)
└── Display: 320×240×16bit = 153KB framebuffer
```

### Conversion Methodology
1. **Source Analysis**: Extract core algorithms from XScreenSaver C code
2. **Hardware Adaptation**: Replace X11/OpenGL calls with ESP32 equivalents
3. **Memory Optimization**: Reduce float precision, optimize buffers
4. **Performance Tuning**: Target 30+ FPS within memory constraints
5. **Visual Verification**: Ensure output matches original aesthetic

### Code Structure
```cpp
// Effect Framework
class XScreenEffect {
  virtual void init() = 0;      // Initialize effect parameters
  virtual void update() = 0;    // Calculate next frame
  virtual void render() = 0;    // Draw to framebuffer
  virtual void cleanup() = 0;   // Free resources
};
```

## 🤝 Contributing

We welcome contributions to help complete the remaining 213 effects!

### How to Help
1. **Choose an effect** from the [XScreenSaver source](https://github.com/jwz/xscreensaver)
2. **Fork this repository** and create a conversion branch  
3. **Port the algorithm** using our effect framework
4. **Test on hardware** - ensure stable performance
5. **Submit a pull request** with your conversion

### Conversion Guidelines
- Maintain visual fidelity to original effect
- Target <5MB flash usage per effect
- Optimize for 30+ FPS performance
- Include effect name/description comments
- Test memory stability over extended runtime

### Development Environment
```bash
# Required tools
pip install platformio
git clone https://github.com/Coreymillia/esp32-2432s028-XscreenSaver-.git
cd esp32-2432s028-XscreenSaver-

# Hardware connection
# Connect ESP32-2432S028 via USB
# No additional wiring required

# Build and flash
pio run --target upload
```

## 📚 Resources & References

### XScreenSaver Origins
- **Original Project**: [XScreenSaver by Jamie Zawinski](https://www.jwz.org/xscreensaver/)
- **Source Repository**: [GitHub XScreenSaver](https://github.com/jwz/xscreensaver)  
- **Documentation**: [Man pages and effect descriptions](https://linux.die.net/man/6/xscreensaver)
- **History**: 30+ years of development, 400+ effects, Unix/Linux/macOS

### ESP32 Development
- **Hardware Vendor**: [ESP32-2432S028 Specifications](https://github.com/Xinyuan-LilyGO/LilyGO-T-Display)
- **Arduino Framework**: [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- **PlatformIO**: [ESP32 Platform Documentation](https://docs.platformio.org/en/latest/platforms/espressif32.html)
- **Graphics Library**: [GFX Library for Arduino](https://github.com/moononournation/Arduino_GFX)

### Related Projects  
- **Adafruit GFX**: Graphics primitives and drivers
- **TFT_eSPI**: Alternative ESP32 display library
- **ESP32 Matrix**: LED matrix screensaver ports
- **Arduino Demos**: Classic computer graphics on microcontrollers

## 🚀 Project Roadmap

### Phase 1: Foundation (✅ Complete)
- ✅ Hardware platform selection and validation
- ✅ Display and touch driver implementation  
- ✅ Basic effect framework development
- ✅ Memory optimization strategies
- ✅ Build system and toolchain setup

### Phase 2: Core Effects (🔄 187/400 - In Progress)
- ✅ Alphabetical range A-L (187 effects)
- 🔄 Range M-Z (213 effects remaining)
- 🎯 Target: 300 effects by Q1 2025
- 🎯 Complete collection by Q2 2025

### Phase 3: Advanced Features (📅 Planned)
- 📅 SD card universal loader system
- 📅 WiFi configuration and OTA updates
- 📅 Custom effect creation tools
- 📅 Mobile app for remote control
- 📅 Multi-device synchronization

### Phase 4: Community & Distribution (📅 Future)
- 📅 Pre-flashed hardware sales
- 📅 Effect sharing marketplace  
- 📅 Educational workshops and tutorials
- 📅 Integration with home automation systems

## 🏆 Project Impact

**Preserving Computing History**: XScreenSaver represents 30+ years of creative programming and mathematical visualization. By porting these effects to modern, affordable hardware, we're ensuring this digital art remains accessible to future generations.

**Educational Value**: Each effect demonstrates different programming concepts - from particle physics and fractal mathematics to 3D graphics and cellular automata. The ESP32 port makes these concepts tangible and interactive.

**Maker Community**: At ~$20 per device, anyone can own a piece of computing history that fits in their pocket, powered by a modern microcontroller that's more powerful than the workstations that originally ran these effects.

---

*ESP32 XScreenSaver Project - Converting 400+ Classic Effects*  
*Current Milestone: 187 Effects Working (46.75% Complete)*  
*Next Update: Effect range M-Z in development*  
*Project Started: 2024 | Latest Update: December 2024*