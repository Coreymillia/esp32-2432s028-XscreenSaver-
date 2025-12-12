# EMERGENCY FLASH INSTRUCTIONS - WORKING 39 EFFECTS

## ⚠️ CRITICAL: THIS FIRMWARE WORKS - NO BLANK SCREEN

### Quick Flash (from this directory):
```bash
cd /home/coreymillia/Documents/esp32-2432S028_video_player/XscreensEsp/GitXscreensAto35/1212GIT
pio run --target upload
```

### Manual Flash (if PlatformIO fails):
```bash
# Compile first
pio run

# Flash with esptool
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 write_flash -z 0x1000 .pio/build/esp32dev/firmware.bin
```

### What You Should See:
1. **Effects start immediately** - no blank screen
2. **Corner numbers 0-38** displayed
3. **Auto-scroll working** (changes every 30 seconds)
4. **Touch/button controls working**

### If Blank Screen:
1. Check backlight is initialized FIRST in setup()
2. Verify esptool flashed at 0x1000 address
3. Try hard reset (unplug USB, replug)

### Memory Usage (Confirmed):
- RAM: 7.1% (23,248 bytes)
- Flash: 31.9% (418,501 bytes)

**THIS IS THE SAFE WORKING VERSION - DO NOT MODIFY UNLESS ABSOLUTELY NECESSARY**