// Working 3-Effect Screensaver - Full Screen
// abstractile.c, aligned_malloc.c, alpha.c

#include <Arduino_GFX_Library.h>
#include <XPT2046_Touchscreen.h>
#include <SD.h>
#include <math.h>

// Hardware pins
#define BL_PIN 21
#define BOOT_PIN 0
#define SD_CS 5
#define SD_MISO 19  
#define SD_MOSI 23
#define SD_SCK 18
#define SD_SPI_SPEED 80000000L

// Display setup
Arduino_DataBus *bus = new Arduino_HWSPI(2 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, 12 /* MISO */);
Arduino_GFX *gfx = new Arduino_ILI9341(bus);

// SD Card reader is on a separate SPI
SPIClass sd_spi(VSPI);

// Touch setup
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
SPIClass touchSPI = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

// SD Card driven - no hardcoded effects

// Global variables
int currentMode = 0;
float animationTime = 0;
bool bootPressed = false;
unsigned long lastPress = 0;
unsigned long bootHoldStart = 0;
bool autoScroll = false;
int autoScrollSeconds = 30;
int totalEffects = 0; // Will be counted from SD card
bool sdCardReady = false;

// Count effects on SD card
void countEffectsOnSD() {
  totalEffects = 0;
  if (!sdCardReady) return;
  
  File dir = SD.open("/effects");
  if (!dir) {
    Serial.println("Failed to open effects directory");
    return;
  }
  
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    if (!entry.isDirectory() && String(entry.name()).endsWith(".txt")) {
      totalEffects++;
    }
    entry.close();
  }
  dir.close();
  
  Serial.printf("Found %d effects on SD card\n", totalEffects);
}

// Read effect name from SD card file
String getEffectNameFromSD(int effectIndex) {
  if (!sdCardReady || effectIndex >= totalEffects) return "NO SD CARD";
  
  File dir = SD.open("/effects");
  if (!dir) return "SD ERROR";
  
  int fileIndex = 0;
  String effectName = "UNKNOWN";
  
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    
    if (!entry.isDirectory() && String(entry.name()).endsWith(".txt")) {
      if (fileIndex == effectIndex) {
        // Read first line of file for effect name
        String line = entry.readStringUntil('\n');
        line.trim();
        effectName = line;
        entry.close();
        break;
      }
      fileIndex++;
    }
    entry.close();
  }
  dir.close();
  
  return effectName;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Working 3-Effect Screensaver - Full Screen");
  Serial.printf("STARTUP: autoScroll = %s\n", autoScroll ? "TRUE" : "FALSE");
  
  // Initialize display
  gfx->begin();
  gfx->setRotation(1); // Landscape mode
  gfx->fillScreen(BLACK);
  
  Serial.printf("Display size: %dx%d\n", gfx->width(), gfx->height());
  
  // Initialize touch
  touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin();
  ts.setRotation(1);
  
  // Boot button
  pinMode(BOOT_PIN, INPUT_PULLUP);
  
  // Backlight
  pinMode(BL_PIN, OUTPUT);
  digitalWrite(BL_PIN, HIGH);
  
  // Initialize SD card
  Serial.println("SD Card initialization");
  if (!SD.begin(SD_CS, sd_spi, SD_SPI_SPEED, "/sd")) {
    Serial.println("SD Card initialization failed!");
    sdCardReady = false;
  } else {
    Serial.println("SD Card initialized successfully");
    sdCardReady = true;
    countEffectsOnSD();
  }
  
  Serial.printf("Starting with mode %d\n", currentMode);
}

// Execute actual effects based on SD card
void executeEffect() {
  if (!sdCardReady || totalEffects == 0) {
    gfx->fillScreen(BLACK);
    gfx->setTextColor(RED);
    gfx->setTextSize(2);
    gfx->setCursor(50, 100);
    gfx->printf("NO SD CARD");
    return;
  }
  
  // Execute effect based on current mode (0-14 maps to our 15 effects)
  switch (currentMode % 15) {
    case 0: // ABSTRACTILE
      {
        int tileSize = 20;
        for (int x = 0; x < gfx->width(); x += tileSize) {
          for (int y = 0; y < gfx->height(); y += tileSize) {
            int color = (int)(sin(animationTime + x * 0.1 + y * 0.1) * 127 + 128);
            gfx->fillRect(x, y, tileSize-1, tileSize-1, 
                         gfx->color565(color, color/2, 255-color));
          }
        }
      }
      break;
      
    case 1: // ALIGNED_MALLOC
      {
        for (int i = 0; i < 50; i++) {
          int x = (int)(sin(animationTime + i * 0.5) * (gfx->width()/2 - 50) + gfx->width()/2);
          int y = (int)(cos(animationTime * 1.3 + i * 0.3) * (gfx->height()/2 - 50) + gfx->height()/2);
          int size = (int)(sin(animationTime * 2 + i) * 10 + 15);
          gfx->fillCircle(x, y, size, gfx->color565(i*5, 255-i*5, 128));
        }
      }
      break;
      
    case 2: // ALPHA
      {
        gfx->fillScreen(BLACK);
        for (int i = 0; i < 20; i++) {
          int x = (int)(sin(animationTime + i * 0.8) * (gfx->width()/2 - 60) + gfx->width()/2);
          int y = (int)(cos(animationTime * 0.7 + i * 0.6) * (gfx->height()/2 - 60) + gfx->height()/2);
          int alpha = (int)(sin(animationTime * 3 + i) * 127 + 128);
          gfx->fillCircle(x, y, 30, gfx->color565(alpha, alpha/2, 255-alpha/2));
        }
      }
      break;
      
    case 3: // ANALOGTV
      {
        gfx->fillScreen(BLACK);
        for (int y = 0; y < gfx->height(); y += 3) {
          for (int x = 0; x < gfx->width(); x += 2) {
            int noise = random(0, 256);
            int scanline = (int)(sin(animationTime * 5 + y * 0.1) * 50 + 128);
            gfx->drawPixel(x, y, gfx->color565(noise, scanline, noise/2));
          }
        }
        int barY = (int)(animationTime * 20) % gfx->height();
        gfx->drawFastHLine(0, barY, gfx->width(), WHITE);
      }
      break;
      
    case 4: // ANALOGTV_CLI
      {
        gfx->fillScreen(BLACK);
        gfx->setTextColor(GREEN);
        gfx->setTextSize(1);
        int lineHeight = 10;
        int numLines = gfx->height() / lineHeight;
        int offset = (int)(animationTime * 10) % numLines;
        for (int i = 0; i < numLines; i++) {
          int y = i * lineHeight;
          gfx->setCursor(5, y);
          if ((i + offset) % 4 == 0) gfx->printf("C:\\> dir /w");
          else if ((i + offset) % 4 == 1) gfx->printf("Loading...");
          else if ((i + offset) % 4 == 2) gfx->printf("*** SYSTEM OK ***");
          else gfx->printf("_");
        }
      }
      break;
      
    case 5: // ANEMONE
      {
        gfx->fillScreen(gfx->color565(0, 0, 50));
        int centerX = gfx->width() / 2;
        int centerY = gfx->height() / 2;
        for (int i = 0; i < 16; i++) {
          float angle = (i * PI * 2 / 16) + animationTime * 0.5;
          float length = 60 + sin(animationTime * 2 + i) * 20;
          for (int j = 0; j < 8; j++) {
            float segmentAngle = angle + sin(animationTime * 3 + j * 0.5) * 0.3;
            float segmentLength = length * (j + 1) / 8.0;
            int x = centerX + cos(segmentAngle) * segmentLength;
            int y = centerY + sin(segmentAngle) * segmentLength;
            uint16_t color = gfx->color565(255 - j * 20, 100 + j * 10, 200);
            gfx->fillCircle(x, y, 3 - j/3, color);
          }
        }
      }
      break;
      
    default:
      // Simple plasma for other effects until we add more
      {
        for (int x = 0; x < gfx->width(); x += 4) {
          for (int y = 0; y < gfx->height(); y += 4) {
            int color = (int)(sin(animationTime + x * 0.1) * sin(animationTime + y * 0.1) * 127 + 128);
            gfx->fillRect(x, y, 4, 4, gfx->color565(color, color/2, 255-color/2));
          }
        }
      }
      break;
  }
}

void loop() {
  // Handle auto scroll
  static unsigned long lastAutoScroll = 0;
  if (autoScroll && (millis() - lastAutoScroll > autoScrollSeconds * 1000)) {
    Serial.printf("AUTO-SCROLL TRIGGERED - Mode: %d\n", currentMode);
    currentMode = (currentMode + 1) % (totalEffects > 0 ? totalEffects : 1);
    Serial.printf("Auto-switched to mode %d\n", currentMode);
    lastAutoScroll = millis();
    gfx->fillScreen(BLACK);
  }
  
  // Check boot button - simplified logic
  static bool buttonHandled = false;
  
  if (digitalRead(BOOT_PIN) == LOW) {
    if (!bootPressed) {
      bootHoldStart = millis();
      bootPressed = true;
      buttonHandled = false;
    }
    
    // Check for long hold (2+ seconds) - only trigger once
    if (!buttonHandled && (millis() - bootHoldStart > 2000)) {
      autoScroll = !autoScroll;
      Serial.printf("Auto scroll: %s (%d seconds)\n", autoScroll ? "ON" : "OFF", autoScrollSeconds);
      lastAutoScroll = millis();
      buttonHandled = true;
      gfx->fillScreen(BLACK);
    }
  } else {
    // Button released
    if (bootPressed && !buttonHandled && (millis() - lastPress > 300)) {
      // Short press - change mode
      currentMode = (currentMode + 1) % (totalEffects > 0 ? totalEffects : 1);
      Serial.printf("Switched to mode %d\n", currentMode);
      lastPress = millis();
      lastAutoScroll = millis();
      gfx->fillScreen(BLACK);
    }
    bootPressed = false;
    buttonHandled = false;
  }
  
  // Touch disabled - boot button and auto-scroll work fine
  /*
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    if (millis() - lastPress > 300) {
      currentMode = (currentMode + 1) % (totalEffects > 0 ? totalEffects : 1);
      Serial.printf("Touch: Switched to mode %d\n", currentMode);
      lastPress = millis();
      lastAutoScroll = millis(); // Reset auto scroll timer
      gfx->fillScreen(BLACK);
    }
  }
  */
  
  // Execute actual effects from SD card
  executeEffect();
  
  animationTime += 0.05;
  delay(50);
}