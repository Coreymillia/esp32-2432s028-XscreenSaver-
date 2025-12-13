# XScreensEsp - XScreensaver Effects for ESP32-2432S028

**A clean, optimized base project for exploring xscreensaver effects on ESP32 hardware**

## 🎯 Project Goals
- Systematic exploration of xscreensaver effects
- Clean, modular codebase
- Memory-optimized implementations
- Full hardware utilization (touch, web control, etc.)

## ✅ Base Features Implemented
- **Display:** ILI9341 320x240 optimized rendering
- **Touch:** XPT2046 working (no boot button holding!)
- **Web Control:** Full AP interface (192.168.4.1)
- **Status Bar:** Real-time system information
- **Button Control:** Interrupt-based navigation
- **Auto-scroll:** Configurable timing
- **Memory Optimized:** Based on 72-effect project lessons

## 🛠️ Hardware Configuration
- **ESP32-2432S028** development board
- **Display:** 2.8" ILI9341 TFT (320x240)
- **Touch:** XPT2046 resistive touch
- **LED:** RGB backlight control
- **Upload:** 921,600 baud (fast development)

## 📁 Project Structure
```
XscreensEsp/
├── src/
│   └── main.cpp           # Base framework
├── docs/                  # Documentation
├── firmware_builds/       # Compiled binaries
├── platformio.ini         # Hardware configuration
└── README.md             # This file
```

## 🎮 Controls
- **Boot Button:** Short press = next effect
- **Touch Screen:** Tap anywhere = next effect  
- **Web Interface:** Connect to "XScreensEsp" → 192.168.4.1
- **Auto-scroll:** 5-second intervals (configurable)

## 🌐 Web Interface Features
- Real-time system status
- Effect navigation (next/previous)
- Auto-scroll toggle
- Touch enable/disable
- Auto-refresh every 5 seconds

## 🎨 Effect System Ready
- Modular effect framework
- Easy to add new effects
- Memory usage monitoring
- Performance optimization

## 🚀 Next Steps
1. Extract effects from xscreensaver source
2. Port to ESP32 (one at a time)
3. Memory optimization
4. Build effect library

## 📊 Development Status
- ✅ Hardware initialization
- ✅ Web control panel
- ✅ Touch input working
- ✅ Status display
- ✅ Effect framework
- 🔄 Ready for xscreensaver porting

---
**Built on lessons learned from the ultimate 72-effect psychedelic clock project!** 🌈