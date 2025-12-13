// Working 3-Effect Screensaver - Full Screen
// abstractile.c, aligned_malloc.c, alpha.c

#include <Arduino_GFX_Library.h>
#include <XPT2046_Touchscreen.h>
#include <SD.h>
#include <math.h>

// Hardware pins
#define BL_PIN 21
#define BOOT_PIN 0
#define SD_CS_PIN 5

// Display setup
Arduino_DataBus *bus = new Arduino_HWSPI(2 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, 12 /* MISO */);
Arduino_GFX *gfx = new Arduino_ILI9341(bus);

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
int autoScrollSeconds = 5;
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
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    sdCardReady = false;
  } else {
    Serial.println("SD Card initialized successfully");
    sdCardReady = true;
    countEffectsOnSD();
  }
  
  Serial.printf("Starting with mode %d\n", currentMode);
}

// SD Card driven display - no hardcoded effects
void displaySDStatus() {
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  
  if (!sdCardReady) {
    gfx->setCursor(50, 100);
    gfx->printf("INSERT SD CARD");
    gfx->setTextSize(1);
    gfx->setCursor(80, 140);
    gfx->printf("No effects found");
    return;
  }
  
  if (totalEffects == 0) {
    gfx->setCursor(60, 100);
    gfx->printf("NO EFFECTS");
    gfx->setTextSize(1);
    gfx->setCursor(70, 140);
    gfx->printf("Check /effects/ folder");
    return;
  }
  
  // Show current effect info
  String effectName = getEffectNameFromSD(currentMode);
  gfx->setCursor(20, 80);
  gfx->printf("Effect %d/%d", currentMode + 1, totalEffects);
  
  gfx->setTextSize(1);
  gfx->setCursor(20, 120);
  gfx->printf("Name: %s", effectName.c_str());
  
  gfx->setCursor(20, 140);
  gfx->printf("From SD: /effects/");
  
  // Simple animation while showing status
  int x = 50 + sin(animationTime * 2) * 30;
  int y = 180 + cos(animationTime * 1.5) * 20;
  gfx->fillCircle(x, y, 5, GREEN);
}

void loop() {
  // Handle auto scroll
  static unsigned long lastAutoScroll = 0;
  if (autoScroll && (millis() - lastAutoScroll > autoScrollSeconds * 1000)) {
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
  
  // Check touch
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
  
  // Draw SD card driven display
  displaySDStatus();
  
  animationTime += 0.05;
  delay(50);
}