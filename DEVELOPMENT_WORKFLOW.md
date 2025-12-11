# ESP32 Screensaver Development Workflow

## Project Status
**Date Started:** December 10, 2025  
**Current Phase:** Batch Effect Development  
**Goal:** Convert 400+ xscreensaver effects to ESP32 SD card format

## Hardware Setup
- **Device 1 (SD Reader)** - MAC: 88:57:21:2e:6b:c4 - Tests effects from SD card
- **Device 2 (Development)** - MAC: 88:57:21:43:fa:ac - Builds effects in firmware
- **SD Card Location:** `/media/coreymillia/NEW VOLUME/effects`

## Batch Development Workflow

### WORKFLOW CONFIRMED:
- **Build 40 effects in Device 2 firmware** with corner numbers
- **Test all 40** - skip any with major conversion problems  
- **Copy/paste working effects** to `/media/coreymillia/NEW VOLUME/effects`
- **Test on Device 1** - verify SD versions work
- **Repeat** for next 40 effects

### Process Details
1. **Effect Development (Device 2)**
   - Add effects to firmware with corner numbers (1-40)
   - Test each effect for stability
   - Skip effects with major conversion issues
   - Flash and verify all 40 work without getting stuck

2. **SD Card Transfer**
   - Copy working effect code to SD card files
   - Replace existing 15 effects with new batch of 40
   - Use minimal conversion - mostly copy/paste

3. **SD Card Testing (Device 1)**
   - Insert SD card with new effects
   - Verify effects run properly from SD card
   - Identify any that get stuck or malfunction

4. **Next Batch**
   - Clear Device 2 firmware
   - Start next 40 effects with numbers 41-80
   - Repeat process

### Conversion Rules
- **Skip difficult conversions** - focus on easy ones
- **400+ total effects available** - can afford to skip problematic ones
- **Corner numbers essential** - for identification during testing
- **Batch size: 40 effects maximum** before memory limits

### Files and Locations
- **Source Effects:** `/home/coreymillia/Documents/esp32-2432S028_video_player/xscreensaver-6.13/`
- **Development Firmware:** Current working directory
- **SD Card Effects:** `/media/coreymillia/NEW VOLUME/effects/`
- **GitHub Project:** `/home/coreymillia/Documents/esp32-2432S028_video_player/XscreensEsp/GithubEspScreenLoader/`

### Current Status
- ✅ Universal SD card loader firmware completed
- ✅ Dual device setup ready
- ✅ Workflow documented
- 🔄 **READY FOR BATCH 1 (Effects 1-40)**

### Notes for Resume
- Device 2 ready for effect development
- SD card format and location confirmed
- Both devices flashed with working firmware
- Touch screen disabled (boot button control only)
- Auto-scroll: 30 seconds, toggle with long press

## Next Session Tasks
1. Get first 3 effects from user
2. Add to Device 2 firmware with corner numbers
3. Test for stability
4. Begin batch development process