# ESP32 XScreensaver Project - Universal Screensaver Collection

**Converting 490+ XScreensaver effects to run on ESP32-2432S028 (Cheap Yellow Display)**

## 🎯 Project Vision

**ULTIMATE GOAL:** Convert all 490+ XScreensaver effects to run from SD card on ESP32 hardware, creating a universal retro screensaver device that can display any classic computer screensaver effect.

## 📊 Current Status (December 2025)

### ✅ Phase 1: COMPLETE - Firmware Implementation (36 Effects)
- **36 working effects** implemented in firmware (A-B alphabetical range)
- **Tested and stable** on ESP32-2432S028 hardware
- **Memory optimized** - 7.1% RAM, 31.9% Flash usage
- **Ready-to-flash firmware** available

### 🔄 Phase 2: IN PROGRESS - SD Card Universal Loader
- **Goal:** Convert all 36 effects to SD card format
- **Universal loader firmware** that reads effects from `/effects/` directory
- **Unlimited effects** without reflashing firmware
- **Hot-swappable** effect collections

### 📈 Phase 3: PLANNED - Complete Collection (490+ Effects)
- **Batch development** - 40 effects per batch to stay within memory limits
- **Complete A-Z coverage** of all XScreensaver effects
- **Multiple firmware versions** if SD card approach fails
- **Full XScreensaver library** preserved for ESP32

## 🛠️ Hardware Requirements

- **ESP32-2432S028** (Cheap Yellow Display / CYD)
- **2.8" ILI9341 TFT Display** (320x240)
- **MicroSD Card** (for universal loader version)
- **Boot button** for navigation
- **USB-C cable** for programming

## 🎮 Features

- **Boot button control** - Navigate through effects manually
- **Auto-scroll mode** - Automatically cycles every 30 seconds
- **Corner ID numbers** - Each effect shows its identifier
- **Memory optimized** - Efficient rendering for smooth animation
- **Full-screen effects** - Utilizes entire 320x240 display

## 📦 Current Release - 36 Effects Working

### Ready-to-Use Files
- `XscreensEsp_40EFFECTS_BATCH1_COMPLETE.bin` - Flash-ready firmware
- `src/main.cpp` - Complete source code
- `GithubEspScreenLoader/` - SD card universal loader project

### 🎨 Implemented Effects (A-B Range)

| ID | Effect Name | Description | Status |
|----|-------------|-------------|---------|
| 0 | **ABSTRACTILE** | Animated geometric tile patterns | ✅ Working |
| 1 | **ALIGNED_MALLOC** | Memory allocation visualization | ✅ Working |
| 2 | **ALPHA** | Transparency effects with floating circles | ✅ Working |
| 3 | **ANEMOTAXIS** | Following organisms simulation | ✅ Working |
| 4 | **ANSI_TTY** | Terminal text effects and scrolling | ✅ Working |
| 5 | **ANT** | Langton's ant cellular automaton | ✅ Working |
| 6 | **ANALOGTV** | Old TV static and interference | ✅ Working |
| 7 | **ANALOGTV_CLI** | Command line terminal simulation | ✅ Working |
| 8 | **ANEMONE** | Sea anemone tentacle movements | ✅ Working |
| 9 | **ANTINSPECT** | Ant inspection and analysis | ✅ Working |
| 10 | **ANTMAZE** | Ant maze solving algorithms | ✅ Working |
| 11 | **ANTSPOTLIGHT** | Spotlight following ant swarms | ✅ Working |
| 12 | **APOLLONIAN** | Apollonian gasket fractal circles | ✅ Working |
| 13 | **APPLE2** | Apple II computer graphics simulation | ✅ Working |
| 14 | **APPLE2_MAIN** | Apple II main system screen | ✅ Working |
| 15 | **ASM6502** | 6502 assembly code visualization | ✅ Working |
| 16 | **ASYNC_NETDB** | Network database operations | ✅ Working |
| 17 | **ATLANTIS** | Underwater scene with sea life | ✅ Working |
| 18 | **ATOMS** | Atomic particle interactions | ✅ Working |
| 19 | **ATOMSWM** | Atomic window manager effects | ✅ Working |
| 20 | **ATTRACTION** | Gravitational particle attraction | ✅ Working |
| 21 | **ATUNNEL** | Animated tunnel effect | ✅ Working |
| 22 | **BARCODE** | Animated barcode patterns | ✅ Working |
| 23 | **B_DRAW** | Basic drawing primitives | ✅ Working |
| 24 | **BEATS** | Audio visualization beats | ✅ Working |
| 25 | **BINARYHORIZON** | Binary data horizon | ✅ Working |
| 26 | **BINARYRING** | Ring of binary data | ✅ Working |
| 27 | **BLASTER** | Space shooting game simulation | ✅ Working |
| 28 | **BLINKBOX** | Blinking box patterns | ✅ Working |
| 29 | **BLITSPIN** | Spinning bitmap blits | ✅ Working |
| 30 | **B_LOCKGLUE** | Block gluing puzzle | ✅ Working |
| 31 | **BLOCKTUBE** | 3D block tube effect | ✅ Working |
| 32 | **BLURB** | Text blurb animations | ✅ Working |
| 33 | **BOING** | Bouncing ball physics | ✅ Working |
| 34 | **BOUBOULE** | French bubble effect | ✅ Working |
| 35 | **BOUNCINGCOW** | Bouncing cow animation | ✅ Working |

## 🚀 Complete Target List (490+ Effects)

### A-B Range (36/40 complete)
✅ **ABSTRACTILE** through **BOUNCINGCOW** (36 working)  
🔄 **BOXED**, **BOXFIT**, **BRAID**, **BSOD** (next 4)

### C-F Range (0/120 planned)
📋 **CAGE**, **CAROUSEL**, **CCURVE**, **CELTIC**, **CIRCUIT**, **CITYFLOW**...  
📋 **DELAUNAY**, **DEMO**, **DNALOGO**, **DRIFT**, **ERUPTION**, **FLAME**...

### G-M Range (0/150 planned) 
📋 **GALAXY**, **GEARS**, **GIBSON**, **GRAV**, **HELIX**, **JULIA**...  
📋 **KALEIDESCOPE**, **LASER**, **LIGHTNING**, **MAZE**, **MOLECULE**...

### N-Z Range (0/180 planned)
📋 **NOOF**, **PACMAN**, **PETRI**, **PIPES**, **QIX**, **RIPPLES**...  
📋 **SPIRAL**, **STARFISH**, **SWIRL**, **TRIANGLE**, **WORM**, **ZOOM**...

## ⚡ Quick Start

### Option 1: Flash Ready Firmware (36 Effects)
1. Download `XscreensEsp_40EFFECTS_BATCH1_COMPLETE.bin`
2. Flash to ESP32-2432S028 using ESP32 flash tool
3. Device boots with all 36 effects ready

### Option 2: Build from Source
```bash
# Install PlatformIO
pip install platformio

# Clone repository
git clone https://github.com/Coreymillia/esp32-2432s028-XscreenSaver-.git
cd esp32-2432s028-XscreenSaver-

# Build and flash
pio run --target upload

# Monitor output
pio device monitor
```

### Option 3: SD Card Universal Loader (Future)
1. Flash universal loader firmware
2. Copy effect files to SD card `/effects/` directory  
3. Insert SD card - effects load automatically
4. Unlimited effects without reflashing

## 🎛️ Controls

- **Boot Button Short Press:** Next effect (cycles 0→1→2→...→35→0)
- **Boot Button Long Press (2+ sec):** Toggle auto-scroll ON/OFF
- **Auto-scroll Mode:** Changes effects every 30 seconds when enabled
- **Corner Numbers:** Each effect displays its ID for identification

## 📊 Technical Specifications

- **Platform:** ESP32 Arduino Framework with PlatformIO
- **Memory Usage:** 23,248 bytes RAM (7.1%), 418,449 bytes Flash (31.9%)
- **Display:** 320x240 ILI9341 TFT with SPI hardware acceleration
- **Refresh Rate:** 20 FPS (50ms update cycle)
- **Build Size:** 418KB firmware (current 36 effects)

## 🛣️ Development Roadmap

### Phase 1: ✅ COMPLETE - Firmware Foundation
- [x] Hardware initialization and display drivers
- [x] Effect framework and rendering system
- [x] 36 working effects (A-B alphabetical range)
- [x] Memory optimization and performance tuning
- [x] User controls and navigation

### Phase 2: 🔄 IN PROGRESS - Universal SD Loader
- [ ] Convert 36 effects to SD card file format
- [ ] Test universal loader with effect files
- [ ] Validate SD card performance and compatibility
- [ ] Optimize file reading and effect loading

### Phase 3: 📋 PLANNED - Complete Collection
- [ ] **Batch 2:** C-F range effects (40 more)
- [ ] **Batch 3:** G-M range effects (40 more)  
- [ ] **Batch 4:** N-Z range effects (40 more)
- [ ] Continue until all 490+ effects converted
- [ ] Multiple firmware versions for different effect sets

### Phase 4: 🎯 FUTURE - Enhancement Features
- [ ] WiFi web interface for effect selection
- [ ] Custom effect parameters and settings
- [ ] Effect playlist and scheduling
- [ ] Multiple display size support
- [ ] Real-time effect mixing and transitions

## 🔧 Development Strategy

### Primary Goal: SD Card Universal System
**If successful:** Unlimited effects, easy expansion, hot-swappable collections

### Fallback Goal: Multiple Firmware Versions  
**If SD fails:** Continue batch development, create specialized firmware builds for different effect collections

### Memory Management
- **40 effects maximum** per firmware build (memory constraint)
- **Batch development** to systematically cover all effects
- **Modular design** for easy effect addition/removal

## 📂 Repository Structure

```
esp32-2432s028-XscreenSaver-/
├── src/
│   └── main.cpp                    # 36-effect firmware source
├── GithubEspScreenLoader/          # Universal SD loader project
├── XscreensEsp_40EFFECTS_BATCH1_COMPLETE.bin  # Ready firmware
├── platformio.ini                  # Build configuration
├── EFFECTS_LIST.md                 # Complete effect reference
└── README.md                       # This file
```

## 🎨 Effect Categories

### Classic Screensavers
- **Flying Toasters**, **Pipes**, **Maze**, **Starfield**

### Mathematical Visualizations  
- **Mandelbrot**, **Julia**, **Sierpinski**, **Apollonian**

### Physics Simulations
- **Gravity**, **Fluidballs**, **Particle Systems**, **Bouncing Objects**

### Retro Computer Tributes
- **Apple II**, **Atari**, **Terminal Effects**, **Matrix Rain**

### Artistic Effects
- **Kaleidoscope**, **Spiral**, **Flame**, **Abstract Patterns**

## 🤝 Contributing

This project welcomes contributions! Ways to help:

- **Effect Conversion:** Port additional XScreensaver effects
- **Optimization:** Improve memory usage and performance  
- **Testing:** Verify effects on different ESP32 hardware
- **Documentation:** Improve guides and effect descriptions
- **Hardware Support:** Add support for other ESP32 displays

## 📄 License

MIT License - See LICENSE file for details

## 🙏 Credits

This project adapts visual effects from **XScreensaver**, created and maintained by **Jamie Zawinski**  
([jwz.org/xscreensaver](https://www.jwz.org/xscreensaver)).

XScreensaver has been the definitive collection of computer screensavers since 1992, preserving decades of digital art and mathematical visualizations. This ESP32 port aims to keep these classic effects alive on modern embedded hardware.

**Original XScreensaver:** Copyright © 1991-2025 Jamie Zawinski  
**ESP32 Port:** Copyright © 2025 ESP32 XScreensaver Project Contributors

## 📞 Support

- **GitHub Issues:** Report bugs and request features
- **Discussions:** Share your builds and modifications
- **Wiki:** Community documentation and tutorials

---

**🌟 Star this repository if you love retro screensavers on ESP32! 🌟**

*Preserving digital art history, one screensaver at a time.*