// Working 3-Effect Screensaver - Full Screen
// abstractile.c, aligned_malloc.c, alpha.c

#include <Arduino_GFX_Library.h>
#include <XPT2046_Touchscreen.h>
#include <math.h>

// Hardware pins
#define BL_PIN 21
#define BOOT_PIN 0

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

// Animation modes - ONLY 3 EFFECTS
enum AnimationMode {
  ABSTRACTILE = 0,
  ALIGNED_MALLOC = 1, 
  ALPHA = 2,
  MODE_COUNT = 3
};

// Global variables
AnimationMode currentMode = ABSTRACTILE;
float animationTime = 0;
bool bootPressed = false;
unsigned long lastPress = 0;

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
  
  Serial.printf("Starting with mode %d\n", currentMode);
}

void drawAbstractile() {
  // Simple tile pattern using full screen
  int tileSize = 20;
  for (int x = 0; x < gfx->width(); x += tileSize) {
    for (int y = 0; y < gfx->height(); y += tileSize) {
      int color = (int)(sin(animationTime + x * 0.1 + y * 0.1) * 127 + 128);
      gfx->fillRect(x, y, tileSize-1, tileSize-1, 
                   gfx->color565(color, color/2, 255-color));
    }
  }
}

void drawAlignedMalloc() {
  // Memory allocation visualization using full screen
  for (int i = 0; i < 50; i++) {
    int x = (int)(sin(animationTime + i * 0.5) * (gfx->width()/2 - 50) + gfx->width()/2);
    int y = (int)(cos(animationTime * 1.3 + i * 0.3) * (gfx->height()/2 - 50) + gfx->height()/2);
    int size = (int)(sin(animationTime * 2 + i) * 10 + 15);
    gfx->fillCircle(x, y, size, gfx->color565(i*5, 255-i*5, 128));
  }
}

void drawAlpha() {
  // Alpha transparency simulation using full screen
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 20; i++) {
    int x = (int)(sin(animationTime + i * 0.8) * (gfx->width()/2 - 60) + gfx->width()/2);
    int y = (int)(cos(animationTime * 0.7 + i * 0.6) * (gfx->height()/2 - 60) + gfx->height()/2);
    int alpha = (int)(sin(animationTime * 3 + i) * 127 + 128);
    gfx->fillCircle(x, y, 30, gfx->color565(alpha, alpha/2, 255-alpha/2));
  }
}

void loop() {
  // Check boot button for mode switch
  if (digitalRead(BOOT_PIN) == LOW) {
    if (!bootPressed && (millis() - lastPress > 300)) {
      currentMode = (AnimationMode)((currentMode + 1) % MODE_COUNT);
      Serial.printf("Switched to mode %d\n", currentMode);
      lastPress = millis();
      gfx->fillScreen(BLACK);
    }
    bootPressed = true;
  } else {
    bootPressed = false;
  }
  
  // Check touch
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    if (millis() - lastPress > 300) {
      currentMode = (AnimationMode)((currentMode + 1) % MODE_COUNT);
      Serial.printf("Touch: Switched to mode %d\n", currentMode);
      lastPress = millis();
      gfx->fillScreen(BLACK);
    }
  }
  
  // Draw current effect
  switch (currentMode) {
    case ABSTRACTILE:
      drawAbstractile();
      break;
    case ALIGNED_MALLOC:
      drawAlignedMalloc();
      break;
    case ALPHA:
      drawAlpha();
      break;
  }
  
  animationTime += 0.05;
  delay(50);
}