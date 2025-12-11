# Complete Effects Reference - 36 Working XScreensaver Effects

**Ported from XScreensaver 6.13 - A through B alphabetical range**
**Date: December 11, 2025**

## All 36 Implemented Effects

| ID | Effect Name | Description | Original Source |
|----|-------------|-------------|-----------------|
| 0 | ABSTRACTILE | Animated geometric tile patterns | abstractile.c |
| 1 | ALIGNED_MALLOC | Memory allocation visualization | aligned_malloc.c |
| 2 | ALPHA | Transparency effects with floating circles | alpha.c |
| 3 | ANEMOTAXIS | Following organisms simulation | anemotaxis.c |
| 4 | ANSI_TTY | Terminal text effects and scrolling | ansi_tty.c |
| 5 | ANT | Langton's ant cellular automaton | ant.c |
| 6 | ANALOGTV | Old TV static and interference | analogtv.c |
| 7 | ANALOGTV_CLI | Command line terminal simulation | analogtv_cli.c |
| 8 | ANEMONE | Sea anemone tentacle movements | anemone.c |
| 9 | ANTINSPECT | Ant inspection and analysis | antinspect.c |
| 10 | ANTMAZE | Ant maze solving algorithms | antmaze.c |
| 11 | ANTSPOTLIGHT | Spotlight following ant swarms | antspotlight.c |
| 12 | APOLLONIAN | Apollonian gasket fractal circles | apollonian.c |
| 13 | APPLE2 | Apple II computer graphics simulation | apple2.c |
| 14 | APPLE2_MAIN | Apple II main system screen | apple2main.c |
| 15 | ASM6502 | 6502 assembly code visualization | asm6502.c |
| 16 | ASYNC_NETDB | Network database operations | async_netdb.c |
| 17 | ATLANTIS | Underwater scene with sea life | atlantis.c |
| 18 | ATOMS | Atomic particle interactions | atoms.c |
| 19 | ATOMSWM | Atomic window manager effects | atomswm.c |
| 20 | ATTRACTION | Gravitational particle attraction | attraction.c |
| 21 | ATUNNEL | Animated tunnel effect | atunnel.c |
| 22 | BARCODE | Animated barcode patterns | barcode.c |
| 23 | B_DRAW | Basic drawing primitives | b_draw.c |
| 24 | BEATS | Audio visualization beats | beats.c |
| 25 | BINARYHORIZON | Binary data horizon | binaryhorizon.c |
| 26 | BINARYRING | Ring of binary data | binaryring.c |
| 27 | BLASTER | Space shooting game simulation | blaster.c |
| 28 | BLINKBOX | Blinking box patterns | blinkbox.c |
| 29 | BLITSPIN | Spinning bitmap blits | blitspin.c |
| 30 | B_LOCKGLUE | Block gluing puzzle | b_lockglue.c |
| 31 | BLOCKTUBE | 3D block tube effect | blocktube.c |
| 32 | BLURB | Text blurb animations | blurb.c |
| 33 | BOING | Bouncing ball physics | boing.c |
| 34 | BOUBOULE | French bubble effect | bouboule.c |
| 35 | BOUNCINGCOW | Bouncing cow animation | bouncingcow.c |

## Navigation

**Device Controls:**
- **Boot button short press:** Next effect (0→1→2→...→35→0)  
- **Boot button long press (2+ sec):** Toggle auto-scroll mode
- **Auto-scroll:** Changes effects every 30 seconds when enabled

**Corner Numbers:**
Each effect displays its ID number (0-35) in the top-left corner for easy identification during testing.

## Technical Notes

**Memory Usage:** 7.1% RAM (23,248 bytes), 31.9% Flash (418,449 bytes)  
**Platform:** ESP32 Arduino Framework with PlatformIO  
**Display:** 320x240 ILI9341 TFT (ESP32-2432S028)  
**Refresh Rate:** 20 FPS (50ms update cycle)  

## Development Status

✅ **COMPLETE** - All 36 effects tested and working  
🔄 **NEXT:** Convert to SD card format for universal loader  
📈 **ROADMAP:** Continue with C-F range (effects 40-79)  

---
**Project: ESP32 XScreensaver Effects**  
**Created: December 11, 2025**