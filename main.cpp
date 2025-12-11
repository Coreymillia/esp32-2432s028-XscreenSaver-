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
  switch (currentMode % (totalEffects > 0 ? totalEffects : 1)) {
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
      
    case 6: // ANEMOTAXIS
      {
        gfx->fillScreen(gfx->color565(10, 10, 30));
        for (int i = 0; i < 12; i++) {
          float baseAngle = i * PI * 2 / 12;
          float centerX = gfx->width() / 2 + cos(baseAngle + animationTime * 0.5) * 50;
          float centerY = gfx->height() / 2 + sin(baseAngle + animationTime * 0.5) * 30;
          
          for (int j = 0; j < 6; j++) {
            float tentacleAngle = baseAngle + j * PI / 3 + sin(animationTime * 2 + j) * 0.5;
            float tentacleLength = 15 + sin(animationTime * 3 + i + j) * 8;
            
            int x = centerX + cos(tentacleAngle) * tentacleLength;
            int y = centerY + sin(tentacleAngle) * tentacleLength;
            
            uint16_t color = gfx->color565(150, 100, 200);
            gfx->fillCircle(x, y, 3, color);
          }
          gfx->fillCircle(centerX, centerY, 5, gfx->color565(255, 200, 150));
        }
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("6");
      }
      break;
      
    case 7: // ANT
      {
        static int antX = 160, antY = 120;
        static int antDir = 0;
        static bool grid[80][60] = {false};
        
        gfx->fillScreen(BLACK);
        
        if ((int)(animationTime * 10) % 3 == 0) {
          int gridX = antX / 4;
          int gridY = antY / 4;
          
          if (gridX >= 0 && gridX < 80 && gridY >= 0 && gridY < 60) {
            if (grid[gridX][gridY]) {
              antDir = (antDir + 1) % 4;
              grid[gridX][gridY] = false;
            } else {
              antDir = (antDir + 3) % 4;
              grid[gridX][gridY] = true;
            }
          }
          
          switch(antDir) {
            case 0: antY -= 4; break;
            case 1: antX += 4; break;
            case 2: antY += 4; break;
            case 3: antX -= 4; break;
          }
          
          antX = constrain(antX, 0, 316);
          antY = constrain(antY, 0, 236);
        }
        
        for (int x = 0; x < 80; x++) {
          for (int y = 0; y < 60; y++) {
            if (grid[x][y]) {
              gfx->fillRect(x * 4, y * 4, 4, 4, WHITE);
            }
          }
        }
        
        gfx->fillRect(antX, antY, 4, 4, RED);
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("7");
      }
      break;
      
    case 8: // ATTRACTION
      {
        gfx->fillScreen(BLACK);
        static float particles[20][4];
        
        for (int i = 0; i < 20; i++) {
          if (particles[i][0] == 0) {
            particles[i][0] = random(20, gfx->width()-20);
            particles[i][1] = random(20, gfx->height()-20);
            particles[i][2] = (random(-100, 100) / 100.0) * 2;
            particles[i][3] = (random(-100, 100) / 100.0) * 2;
          }
          
          float centerX = gfx->width() / 2;
          float centerY = gfx->height() / 2;
          
          float dx = centerX - particles[i][0];
          float dy = centerY - particles[i][1];
          float dist = sqrt(dx*dx + dy*dy);
          
          if (dist > 10) {
            particles[i][2] += dx * 0.01 / dist;
            particles[i][3] += dy * 0.01 / dist;
          }
          
          particles[i][0] += particles[i][2];
          particles[i][1] += particles[i][3];
          
          if (particles[i][0] < 0 || particles[i][0] > gfx->width() || 
              particles[i][1] < 0 || particles[i][1] > gfx->height()) {
            particles[i][0] = 0;
          }
          
          gfx->fillCircle(particles[i][0], particles[i][1], 3, 
                         gfx->color565(255-i*10, 128+i*5, i*10));
        }
        
        gfx->fillCircle(gfx->width()/2, gfx->height()/2, 8, YELLOW);
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("8");
      }
      break;
      
    case 9: // APOLLONIAN
      {
        gfx->fillScreen(BLACK);
        int centerX = gfx->width() / 2;
        int centerY = gfx->height() / 2;
        
        for (int level = 0; level < 4; level++) {
          float radius = 80 - level * 15;
          int numCircles = 8 + level * 4;
          
          for (int i = 0; i < numCircles; i++) {
            float angle = (i * PI * 2 / numCircles) + animationTime * (0.5 + level * 0.2);
            int x = centerX + cos(angle) * radius;
            int y = centerY + sin(angle) * radius;
            int r = 10 - level * 2;
            
            gfx->fillCircle(x, y, r, gfx->color565(255 - level * 60, 100 + level * 40, 150 + level * 20));
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("9");
      }
      break;
      
    case 10: // APPLE2
      {
        gfx->fillScreen(BLACK);
        gfx->setTextColor(GREEN);
        gfx->setTextSize(1);
        
        int line = 0;
        gfx->setCursor(5, line++ * 10);
        gfx->printf("APPLE ][");
        gfx->setCursor(5, line++ * 10);
        gfx->printf("48K SYSTEM");
        gfx->setCursor(5, line++ * 10);
        gfx->printf("COPYRIGHT APPLE 1977");
        gfx->setCursor(5, line++ * 10);
        gfx->printf("");
        gfx->setCursor(5, line++ * 10);
        
        int blinkTime = (int)(animationTime * 2) % 2;
        if (blinkTime) {
          gfx->printf("]_");
        } else {
          gfx->printf("] ");
        }
        
        // Apple logo simulation with rectangles
        for (int i = 0; i < 6; i++) {
          int color = gfx->color565(255 - i * 40, i * 40, 128);
          gfx->fillRect(250 + i * 2, 50 + i * 8, 15, 8, color);
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("10");
      }
      break;
      
    case 11: // ATLANTIS  
      {
        gfx->fillScreen(gfx->color565(0, 0, 80)); // Deep blue
        
        // Bubbles
        for (int i = 0; i < 15; i++) {
          int x = (int)(sin(animationTime * 0.8 + i) * (gfx->width()/2 - 20) + gfx->width()/2);
          int y = (int)(fmod(animationTime * 30 + i * 20, gfx->height()));
          int size = 3 + (i % 4);
          
          gfx->fillCircle(x, y, size, gfx->color565(150, 200, 255));
        }
        
        // Fish
        for (int i = 0; i < 6; i++) {
          float fishAngle = animationTime * 0.5 + i * PI / 3;
          int fishX = gfx->width()/2 + cos(fishAngle) * (60 + i * 10);
          int fishY = gfx->height()/2 + sin(fishAngle) * (30 + i * 5);
          
          // Fish body
          gfx->fillCircle(fishX, fishY, 8, gfx->color565(255, 150, 0));
          // Fish tail
          gfx->fillCircle(fishX - 10, fishY, 4, gfx->color565(200, 100, 0));
        }
        
        // Coral at bottom
        for (int x = 0; x < gfx->width(); x += 20) {
          int height = 20 + sin(x * 0.1 + animationTime) * 10;
          gfx->fillRect(x, gfx->height() - height, 15, height, 
                       gfx->color565(255, 100, 150));
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("11");
      }
      break;
      
    case 12: // ATOMS
      {
        gfx->fillScreen(BLACK);
        
        // Central nucleus
        gfx->fillCircle(gfx->width()/2, gfx->height()/2, 15, RED);
        
        // Electron orbits
        for (int orbit = 0; orbit < 3; orbit++) {
          float radius = 40 + orbit * 25;
          int numElectrons = 2 + orbit;
          
          for (int e = 0; e < numElectrons; e++) {
            float angle = (animationTime * (1.0 + orbit * 0.3)) + (e * PI * 2 / numElectrons);
            int x = gfx->width()/2 + cos(angle) * radius;
            int y = gfx->height()/2 + sin(angle) * radius;
            
            gfx->fillCircle(x, y, 5, gfx->color565(100 + orbit * 50, 150, 255 - orbit * 50));
          }
          
          // Draw orbit path
          for (int a = 0; a < 360; a += 10) {
            float rad = a * PI / 180;
            int x = gfx->width()/2 + cos(rad) * radius;
            int y = gfx->height()/2 + sin(rad) * radius;
            gfx->drawPixel(x, y, gfx->color565(50, 50, 50));
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("12");
      }
      break;
      
    case 13: // BARCODE
      {
        gfx->fillScreen(WHITE);
        
        // Generate barcode pattern
        for (int x = 50; x < gfx->width() - 50; x += 4) {
          int barHeight = gfx->height() - 100;
          int pattern = (int)(sin(x * 0.1 + animationTime) * 127 + 128) % 3;
          
          if (pattern == 0) {
            gfx->fillRect(x, 50, 2, barHeight, BLACK);
          } else if (pattern == 1) {
            gfx->fillRect(x, 50, 4, barHeight, BLACK);
          }
        }
        
        gfx->setTextColor(BLACK);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("13");
      }
      break;
      
    case 14: // BINARYRING
      {
        gfx->fillScreen(BLACK);
        int centerX = gfx->width() / 2;
        int centerY = gfx->height() / 2;
        
        for (int ring = 0; ring < 3; ring++) {
          float radius = 40 + ring * 30;
          int numBits = 32 + ring * 16;
          
          for (int i = 0; i < numBits; i++) {
            float angle = (i * PI * 2 / numBits) + animationTime * (1.0 + ring * 0.5);
            int x = centerX + cos(angle) * radius;
            int y = centerY + sin(angle) * radius;
            
            int bit = ((int)(animationTime * 10 + i + ring) % 2);
            uint16_t color = bit ? GREEN : gfx->color565(0, 50, 0);
            
            gfx->fillRect(x-2, y-2, 4, 4, color);
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("14");
      }
      break;
      
    case 15: // BLASTER
      {
        gfx->fillScreen(BLACK);
        
        // Starfield
        for (int i = 0; i < 50; i++) {
          int x = (int)(sin(animationTime * 0.1 + i) * gfx->width()/2 + gfx->width()/2);
          int y = (int)(fmod(animationTime * 100 + i * 10, gfx->height()));
          gfx->drawPixel(x, y, WHITE);
        }
        
        // Player ship at bottom
        int shipX = gfx->width()/2 + sin(animationTime) * 50;
        int shipY = gfx->height() - 30;
        gfx->fillRect(shipX-5, shipY, 10, 15, CYAN);
        gfx->fillRect(shipX-2, shipY-5, 4, 5, CYAN);
        
        // Enemy ships
        for (int i = 0; i < 5; i++) {
          int enemyX = 50 + i * 50;
          int enemyY = 50 + sin(animationTime + i) * 20;
          gfx->fillRect(enemyX-4, enemyY, 8, 8, RED);
        }
        
        // Lasers
        for (int i = 0; i < 10; i++) {
          int laserX = 30 + i * 30;
          int laserY = (int)(fmod(animationTime * 150 + i * 20, gfx->height()));
          gfx->fillRect(laserX, laserY, 2, 10, YELLOW);
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("15");
      }
      break;
      
    case 16: // BLINKBOX
      {
        gfx->fillScreen(BLACK);
        
        int boxSize = 30;
        int cols = gfx->width() / (boxSize + 5);
        int rows = gfx->height() / (boxSize + 5);
        
        for (int x = 0; x < cols; x++) {
          for (int y = 0; y < rows; y++) {
            int blinkPhase = (int)(animationTime * 3 + x + y) % 4;
            
            if (blinkPhase < 2) {
              uint16_t color = gfx->color565(
                (x * 50) % 255,
                (y * 50) % 255,
                ((x + y) * 30) % 255
              );
              
              gfx->fillRect(
                x * (boxSize + 5) + 5,
                y * (boxSize + 5) + 5,
                boxSize,
                boxSize,
                color
              );
            }
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("16");
      }
      break;
      
    case 17: // BOING
      {
        gfx->fillScreen(gfx->color565(0, 0, 100));
        
        static float ballX = 160, ballY = 120;
        static float velX = 3, velY = 2;
        
        // Ball physics
        ballX += velX;
        ballY += velY;
        
        // Bounce off walls
        if (ballX <= 15 || ballX >= gfx->width() - 15) {
          velX = -velX;
        }
        if (ballY <= 15 || ballY >= gfx->height() - 15) {
          velY = -velY;
        }
        
        // Keep ball in bounds
        ballX = constrain(ballX, 15, gfx->width() - 15);
        ballY = constrain(ballY, 15, gfx->height() - 15);
        
        // Draw ball with trail
        for (int i = 0; i < 5; i++) {
          int trailX = ballX - velX * i * 2;
          int trailY = ballY - velY * i * 2;
          int alpha = 255 - i * 50;
          
          gfx->fillCircle(trailX, trailY, 15 - i * 2, 
                         gfx->color565(alpha, alpha/2, alpha));
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("17");
      }
      break;
      
    case 18: // BOUBOULE
      {
        gfx->fillScreen(BLACK);
        int centerX = gfx->width() / 2;
        int centerY = gfx->height() / 2;
        
        for (int i = 0; i < 8; i++) {
          float angle = animationTime + i * PI / 4;
          float radius = 30 + sin(animationTime * 2 + i) * 15;
          
          int x = centerX + cos(angle) * radius;
          int y = centerY + sin(angle) * radius;
          
          int size = 8 + sin(animationTime * 3 + i) * 5;
          uint16_t color = gfx->color565(255 - i * 30, 128 + i * 15, i * 30);
          
          gfx->fillCircle(x, y, size, color);
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("15");
      }
      break;
      
    case 19: // BUBBLES
      {
        gfx->fillScreen(gfx->color565(0, 100, 150));
        
        for (int i = 0; i < 20; i++) {
          int x = (int)(sin(animationTime * 0.3 + i) * (gfx->width()/2 - 20) + gfx->width()/2);
          int y = (int)(fmod(animationTime * 50 + i * 15, gfx->height()));
          int size = 5 + (i % 8);
          
          // Bubble with shine
          gfx->fillCircle(x, y, size, gfx->color565(150, 200, 255));
          gfx->fillCircle(x - size/3, y - size/3, size/3, WHITE);
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("16");
      }
      break;
      
    case 20: // CAGE
      {
        gfx->fillScreen(BLACK);
        
        // Draw rotating cage
        float rotation = animationTime * 0.5;
        int centerX = gfx->width() / 2;
        int centerY = gfx->height() / 2;
        
        // Cage bars
        for (int i = 0; i < 8; i++) {
          float angle = rotation + i * PI / 4;
          
          int x1 = centerX + cos(angle) * 40;
          int y1 = centerY + sin(angle) * 40;
          int x2 = centerX + cos(angle) * 80;
          int y2 = centerY + sin(angle) * 80;
          
          gfx->drawLine(x1, y1, x2, y2, gfx->color565(200, 200, 200));
        }
        
        // Horizontal rings
        for (int ring = 0; ring < 3; ring++) {
          int radius = 40 + ring * 20;
          for (int a = 0; a < 360; a += 15) {
            float rad = (a + rotation * 30) * PI / 180;
            int x = centerX + cos(rad) * radius;
            int y = centerY + sin(rad) * radius;
            gfx->drawPixel(x, y, gfx->color565(150, 150, 150));
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("17");
      }
      break;
      
    case 21: // CCURVE
      {
        gfx->fillScreen(BLACK);
        
        // Dragon curve / C-curve fractal
        int centerX = gfx->width() / 2;
        int centerY = gfx->height() / 2;
        
        for (int level = 0; level < 6; level++) {
          float scale = 2 + level * 0.5;
          float rotation = animationTime + level * PI / 3;
          
          for (int i = 0; i < (1 << level); i++) {
            float t = (float)i / (1 << level);
            float angle = rotation + t * PI * 2;
            
            int x = centerX + cos(angle) * (30 + level * 10);
            int y = centerY + sin(angle) * (20 + level * 8);
            
            uint16_t color = gfx->color565(
              255 - level * 40,
              100 + level * 25,
              50 + level * 35
            );
            
            gfx->fillRect(x-1, y-1, 2, 2, color);
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("21");
      }
      break;
      
    case 22: // CORAL
      {
        gfx->fillScreen(gfx->color565(0, 0, 60));
        
        // Growing coral structure
        for (int branch = 0; branch < 6; branch++) {
          float baseAngle = branch * PI / 3;
          int baseX = gfx->width()/2 + cos(baseAngle) * 20;
          int baseY = gfx->height()/2 + sin(baseAngle) * 20;
          
          // Draw branching coral
          for (int segment = 0; segment < 8; segment++) {
            float segmentAngle = baseAngle + sin(animationTime + segment * 0.5) * 0.8;
            float segmentLength = segment * 8 + sin(animationTime * 2 + branch) * 5;
            
            int x = baseX + cos(segmentAngle) * segmentLength;
            int y = baseY + sin(segmentAngle) * segmentLength;
            
            int thickness = 6 - segment;
            if (thickness > 0) {
              uint16_t color = gfx->color565(
                255 - segment * 20,
                100 + segment * 15,
                150 + branch * 15
              );
              
              gfx->fillCircle(x, y, thickness, color);
            }
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("22");
      }
      break;
      
    case 23: // CRYSTAL
      {
        gfx->fillScreen(BLACK);
        int centerX = gfx->width() / 2;
        int centerY = gfx->height() / 2;
        
        // Crystal facets
        for (int face = 0; face < 6; face++) {
          float angle = face * PI / 3 + animationTime * 0.3;
          
          for (int layer = 0; layer < 4; layer++) {
            float radius = 20 + layer * 15;
            int x1 = centerX + cos(angle) * radius;
            int y1 = centerY + sin(angle) * radius;
            int x2 = centerX + cos(angle + PI/3) * radius;
            int y2 = centerY + sin(angle + PI/3) * radius;
            
            uint16_t color = gfx->color565(
              200 + sin(animationTime + face) * 50,
              150 + layer * 25,
              255 - layer * 40
            );
            
            gfx->drawLine(x1, y1, x2, y2, color);
            gfx->drawLine(centerX, centerY, x1, y1, color);
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("23");
      }
      break;
      
    case 24: // CYNOSURE
      {
        gfx->fillScreen(gfx->color565(10, 10, 40));
        
        // Radiating lines from center
        int centerX = gfx->width() / 2;
        int centerY = gfx->height() / 2;
        
        for (int i = 0; i < 24; i++) {
          float angle = i * PI / 12 + animationTime * 0.2;
          float length = 60 + sin(animationTime * 2 + i * 0.5) * 30;
          
          int x = centerX + cos(angle) * length;
          int y = centerY + sin(angle) * length;
          
          uint16_t color = gfx->color565(
            255 - i * 8,
            100 + sin(animationTime + i) * 100,
            150 + i * 4
          );
          
          gfx->drawLine(centerX, centerY, x, y, color);
          gfx->fillCircle(x, y, 3, color);
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("24");
      }
      break;
      
    case 25: // DELUXE
      {
        gfx->fillScreen(BLACK);
        
        // Deluxe patterns - geometric art
        for (int layer = 0; layer < 3; layer++) {
          for (int i = 0; i < 8; i++) {
            float angle = i * PI / 4 + animationTime * (0.5 + layer * 0.2);
            float radius = 40 + layer * 25;
            
            int x = gfx->width()/2 + cos(angle) * radius;
            int y = gfx->height()/2 + sin(angle) * radius;
            
            // Diamond shapes
            int size = 8 + layer * 3;
            uint16_t color = gfx->color565(
              255 - layer * 80,
              layer * 100,
              200 - i * 20
            );
            
            gfx->fillRect(x-size/2, y-size/2, size, size, color);
            
            // Rotate the diamond
            int offset = size/3;
            gfx->fillRect(x-offset, y-offset, offset*2, offset*2, BLACK);
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("25");
      }
      break;
      
    case 26: // DEMON
      {
        gfx->fillScreen(gfx->color565(50, 0, 0));
        
        // Demonic cellular automaton
        static bool cells[40][30];
        static bool initialized = false;
        
        if (!initialized) {
          for (int x = 0; x < 40; x++) {
            for (int y = 0; y < 30; y++) {
              cells[x][y] = random(0, 100) < 30;
            }
          }
          initialized = true;
        }
        
        // Update cells based on neighbors (demon rule)
        if ((int)(animationTime * 5) % 3 == 0) {
          bool newCells[40][30];
          
          for (int x = 1; x < 39; x++) {
            for (int y = 1; y < 29; y++) {
              int neighbors = 0;
              for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                  if (dx != 0 || dy != 0) {
                    if (cells[x+dx][y+dy]) neighbors++;
                  }
                }
              }
              
              // Demon rule: birth on 2, survive on 1-2
              if (cells[x][y]) {
                newCells[x][y] = (neighbors >= 1 && neighbors <= 2);
              } else {
                newCells[x][y] = (neighbors == 2);
              }
            }
          }
          
          memcpy(cells, newCells, sizeof(cells));
        }
        
        // Draw cells
        for (int x = 0; x < 40; x++) {
          for (int y = 0; y < 30; y++) {
            if (cells[x][y]) {
              uint16_t color = gfx->color565(255, 50 + y * 5, 0);
              gfx->fillRect(x * 8, y * 8, 8, 8, color);
            }
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("26");
      }
      break;
      
    case 27: // DISCRETE
      {
        gfx->fillScreen(BLACK);
        
        // Discrete mathematical visualization
        for (int step = 0; step < 20; step++) {
          float t = animationTime + step * 0.3;
          
          // Discrete function points
          int x = 20 + step * 14;
          int y = gfx->height()/2 + sin(t) * cos(t * 1.3) * 60;
          
          // Connect discrete points
          if (step > 0) {
            int prevX = 20 + (step-1) * 14;
            int prevY = gfx->height()/2 + sin(animationTime + (step-1) * 0.3) * 
                       cos((animationTime + (step-1) * 0.3) * 1.3) * 60;
            
            gfx->drawLine(prevX, prevY, x, y, gfx->color565(0, 255, 100));
          }
          
          // Draw discrete point
          uint16_t color = gfx->color565(
            255 - step * 10,
            step * 12,
            200
          );
          
          gfx->fillCircle(x, y, 4, color);
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("27");
      }
      break;
      
    case 28: // DISTORT
      {
        gfx->fillScreen(BLACK);
        
        // Distortion effect
        for (int y = 0; y < gfx->height(); y += 4) {
          for (int x = 0; x < gfx->width(); x += 4) {
            float distortX = x + sin(animationTime + y * 0.02) * 10;
            float distortY = y + cos(animationTime + x * 0.02) * 8;
            
            uint16_t color = gfx->color565(
              (int)(sin(distortX * 0.1) * 127 + 128),
              (int)(cos(distortY * 0.1) * 127 + 128),
              (int)(sin(distortX * 0.05 + distortY * 0.05) * 127 + 128)
            );
            
            gfx->fillRect(x, y, 4, 4, color);
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("28");
      }
      break;
      
    case 29: // DRIFT
      {
        gfx->fillScreen(gfx->color565(0, 0, 20));
        
        // Drifting particles
        static float particles[30][4]; // x, y, vx, vy
        static bool initialized = false;
        
        if (!initialized) {
          for (int i = 0; i < 30; i++) {
            particles[i][0] = random(0, gfx->width());
            particles[i][1] = random(0, gfx->height());
            particles[i][2] = (random(-100, 100) / 100.0) * 2;
            particles[i][3] = (random(-100, 100) / 100.0) * 2;
          }
          initialized = true;
        }
        
        // Update and draw particles
        for (int i = 0; i < 30; i++) {
          // Drift physics
          particles[i][0] += particles[i][2];
          particles[i][1] += particles[i][3];
          
          // Wrap around screen
          if (particles[i][0] < 0) particles[i][0] = gfx->width();
          if (particles[i][0] > gfx->width()) particles[i][0] = 0;
          if (particles[i][1] < 0) particles[i][1] = gfx->height();
          if (particles[i][1] > gfx->height()) particles[i][1] = 0;
          
          // Draw particle with trail
          uint16_t color = gfx->color565(100 + i * 5, 150, 255 - i * 5);
          gfx->fillCircle(particles[i][0], particles[i][1], 3, color);
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("29");
      }
      break;
      
    case 30: // EPICYCLE
      {
        gfx->fillScreen(BLACK);
        
        // Epicycle animation
        int centerX = gfx->width() / 2;
        int centerY = gfx->height() / 2;
        
        for (int cycle = 0; cycle < 4; cycle++) {
          float radius1 = 30 + cycle * 15;
          float radius2 = 10 + cycle * 5;
          float speed1 = animationTime * (0.5 + cycle * 0.2);
          float speed2 = animationTime * (2.0 + cycle * 0.5);
          
          // Main circle center
          int x1 = centerX + cos(speed1) * radius1;
          int y1 = centerY + sin(speed1) * radius1;
          
          // Epicycle point
          int x2 = x1 + cos(speed2) * radius2;
          int y2 = y1 + sin(speed2) * radius2;
          
          uint16_t color = gfx->color565(
            255 - cycle * 50,
            100 + cycle * 30,
            150 + cycle * 25
          );
          
          // Draw circles
          gfx->drawCircle(x1, y1, radius2, gfx->color565(50, 50, 50));
          gfx->fillCircle(x2, y2, 4, color);
          gfx->drawLine(x1, y1, x2, y2, color);
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("30");
      }
      break;
      
    case 31: // ERUPTION
      {
        gfx->fillScreen(gfx->color565(20, 0, 0));
        
        // Volcanic eruption
        int baseY = gfx->height() - 20;
        
        // Lava particles
        for (int i = 0; i < 40; i++) {
          float age = fmod(animationTime * 2 + i * 0.2, 3.0);
          
          if (age < 2.5) {
            float x = gfx->width()/2 + sin(i) * 30 + sin(animationTime + i) * age * 20;
            float y = baseY - age * age * 30 + cos(animationTime * 2 + i) * 10;
            
            if (y >= 0 && y < gfx->height()) {
              int intensity = 255 - (age / 2.5) * 200;
              uint16_t color = gfx->color565(
                intensity,
                intensity / 3,
                0
              );
              
              int size = 2 + (2.5 - age);
              gfx->fillCircle(x, y, size, color);
            }
          }
        }
        
        // Mountain silhouette
        for (int x = 0; x < gfx->width(); x += 5) {
          int height = 20 + sin(x * 0.1) * 8;
          gfx->fillRect(x, gfx->height() - height, 5, height, gfx->color565(40, 20, 0));
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("31");
      }
      break;
      
    case 32: // FADEPLOT
      {
        gfx->fillScreen(BLACK);
        
        // Fading plot visualization
        static int plotHistory[320];
        static bool initialized = false;
        
        if (!initialized) {
          for (int i = 0; i < 320; i++) {
            plotHistory[i] = gfx->height() / 2;
          }
          initialized = true;
        }
        
        // Shift plot history
        for (int i = 0; i < 319; i++) {
          plotHistory[i] = plotHistory[i + 1];
        }
        
        // Add new plot point
        plotHistory[319] = gfx->height()/2 + 
                          sin(animationTime * 2) * 40 + 
                          cos(animationTime * 3.7) * 20;
        
        // Draw fading plot
        for (int x = 0; x < 320; x++) {
          int fade = x * 255 / 320; // Fade from left to right
          uint16_t color = gfx->color565(fade, fade/2, 255-fade/2);
          
          if (x > 0) {
            gfx->drawLine(x-1, plotHistory[x-1], x, plotHistory[x], color);
          }
          
          gfx->drawPixel(x, plotHistory[x], WHITE);
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("32");
      }
      break;
      
    case 33: // FIREWORKX
      {
        gfx->fillScreen(gfx->color565(0, 0, 20));
        
        // Fireworks display
        for (int firework = 0; firework < 5; firework++) {
          float age = fmod(animationTime * 1.5 + firework * 1.2, 4.0);
          
          if (age < 3.0) {
            int centerX = 50 + firework * 50;
            int centerY = 60 + sin(firework) * 30;
            
            // Explosion particles
            for (int particle = 0; particle < 16; particle++) {
              float angle = particle * PI * 2 / 16;
              float radius = age * age * 15;
              
              int x = centerX + cos(angle) * radius;
              int y = centerY + sin(angle) * radius;
              
              if (x >= 0 && x < gfx->width() && y >= 0 && y < gfx->height()) {
                int brightness = 255 - (age / 3.0) * 255;
                uint16_t color = gfx->color565(
                  brightness,
                  brightness * (firework % 2),
                  brightness * ((firework + 1) % 2)
                );
                
                gfx->fillCircle(x, y, 2, color);
              }
            }
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("33");
      }
      break;
      
    case 34: // FLAME
      {
        gfx->fillScreen(BLACK);
        
        // Fire/flame effect
        int baseY = gfx->height() - 10;
        
        for (int x = 0; x < gfx->width(); x += 3) {
          for (int flameLevel = 0; flameLevel < 8; flameLevel++) {
            float flameHeight = sin(animationTime * 3 + x * 0.1) * 20 + 
                               cos(animationTime * 2 + x * 0.05 + flameLevel) * 15 + 
                               flameLevel * 15;
            
            int y = baseY - flameHeight;
            
            if (y >= 0 && y < gfx->height()) {
              // Fire colors - red to yellow to white
              int intensity = 255 - flameLevel * 20;
              uint16_t color;
              
              if (flameLevel < 3) {
                color = gfx->color565(intensity, intensity/4, 0); // Red
              } else if (flameLevel < 6) {
                color = gfx->color565(intensity, intensity/2, 0); // Orange
              } else {
                color = gfx->color565(intensity, intensity, intensity/2); // Yellow
              }
              
              gfx->fillRect(x, y, 3, 4, color);
            }
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("34");
      }
      break;
      
    case 35: // FLOW
      {
        gfx->fillScreen(gfx->color565(0, 0, 40));
        
        // Flowing field visualization
        for (int y = 0; y < gfx->height(); y += 8) {
          for (int x = 0; x < gfx->width(); x += 8) {
            // Vector field
            float vx = sin(animationTime + x * 0.02) * cos(y * 0.02);
            float vy = cos(animationTime + y * 0.02) * sin(x * 0.02);
            
            // Flow lines
            int x2 = x + vx * 6;
            int y2 = y + vy * 6;
            
            // Color based on flow strength
            float strength = sqrt(vx*vx + vy*vy);
            uint16_t color = gfx->color565(
              (int)(strength * 255),
              100 + (int)(strength * 100),
              255 - (int)(strength * 200)
            );
            
            gfx->drawLine(x, y, x2, y2, color);
            gfx->fillCircle(x2, y2, 2, color);
          }
        }
        
        gfx->setTextColor(WHITE);
        gfx->setTextSize(1);
        gfx->setCursor(5, 5);
        gfx->printf("35");
      }
      break;
      
    default:
      {
        gfx->fillScreen(BLACK);
        gfx->setTextColor(WHITE);
        gfx->setTextSize(2);
        gfx->setCursor(50, 100);
        gfx->printf("Effect %d", currentMode);
        
        int x = 100 + sin(animationTime) * 50;
        int y = 100 + cos(animationTime) * 30;
        gfx->fillCircle(x, y, 20, RED);
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