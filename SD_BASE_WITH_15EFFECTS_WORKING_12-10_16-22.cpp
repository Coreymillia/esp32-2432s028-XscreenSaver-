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

// Animation modes - 15 EFFECTS
enum AnimationMode {
  ABSTRACTILE = 0,
  ALIGNED_MALLOC = 1, 
  ALPHA = 2,
  ANALOGTV = 3,
  ANALOGTV_CLI = 4,
  ANEMONE = 5,
  ANEMOTAXIS = 6,
  ANSI_TTY = 7,
  ANT = 8,
  ANTINSPECT = 9,
  ANTMAZE = 10,
  ANTSPOTLIGHT = 11,
  APOLLONIAN = 12,
  APPLE2 = 13,
  APPLE2_MAIN = 14,
  MODE_COUNT = 15
};

// Global variables
AnimationMode currentMode = ABSTRACTILE;
float animationTime = 0;
bool bootPressed = false;
unsigned long lastPress = 0;
unsigned long bootHoldStart = 0;
bool autoScroll = false;
int autoScrollSeconds = 5;
int totalEffects = 15; // Will be updated from SD card

// SD Card function - reads effect name from file
String getEffectNameFromSD(int effectIndex) {
  String filename = "/effects/" + String(effectIndex, DEC);
  if (effectIndex < 10) filename = "/effects/0" + String(effectIndex, DEC);
  filename += "_*.txt";
  
  // For now, return hardcoded names (we'll improve this)
  String names[] = {
    "ABSTRACTILE", "ALIGNED_MALLOC", "ALPHA", "ANALOGTV", "ANALOGTV_CLI",
    "ANEMONE", "ANEMOTAXIS", "ANSI_TTY", "ANT", "ANTINSPECT", 
    "ANTMAZE", "ANTSPOTLIGHT", "APOLLONIAN", "APPLE2", "APPLE2_MAIN"
  };
  
  if (effectIndex >= 0 && effectIndex < 15) {
    return names[effectIndex];
  }
  return "UNKNOWN";
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
  } else {
    Serial.println("SD Card initialized successfully");
  }
  
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

void drawAnalogTV() {
  // Old TV static and interference
  gfx->fillScreen(BLACK);
  for (int y = 0; y < gfx->height(); y += 3) {
    for (int x = 0; x < gfx->width(); x += 2) {
      int noise = random(0, 256);
      int scanline = (int)(sin(animationTime * 5 + y * 0.1) * 50 + 128);
      gfx->drawPixel(x, y, gfx->color565(noise, scanline, noise/2));
    }
  }
  // Rolling bars
  int barY = (int)(animationTime * 20) % gfx->height();
  gfx->drawFastHLine(0, barY, gfx->width(), WHITE);
}

void drawAnalogTVCLI() {
  // Command line interface simulation on old TV
  gfx->fillScreen(BLACK);
  gfx->setTextColor(GREEN);
  gfx->setTextSize(1);
  
  int lineHeight = 10;
  int numLines = gfx->height() / lineHeight;
  int offset = (int)(animationTime * 10) % numLines;
  
  for (int i = 0; i < numLines; i++) {
    int y = i * lineHeight;
    gfx->setCursor(5, y);
    if ((i + offset) % 4 == 0) {
      gfx->printf("C:\\> dir /w");
    } else if ((i + offset) % 4 == 1) {
      gfx->printf("Loading...");
    } else if ((i + offset) % 4 == 2) {
      gfx->printf("*** SYSTEM OK ***");
    } else {
      gfx->printf("_");
    }
  }
}

void drawAnemone() {
  // Sea anemone tentacle movements
  gfx->fillScreen(gfx->color565(0, 0, 50)); // Deep blue background
  
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

void drawAnemotaxis() {
  // Anemotaxis behavior - organisms following wind/current
  static int particles[30][4]; // x, y, vx, vy for each particle
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 30; i++) {
      particles[i][0] = random(gfx->width());
      particles[i][1] = random(gfx->height());
      particles[i][2] = random(-2, 3);
      particles[i][3] = random(-2, 3);
    }
    initialized = true;
  }
  
  gfx->fillScreen(gfx->color565(20, 40, 60));
  
  // Wind force
  float windX = sin(animationTime * 0.5) * 2;
  float windY = cos(animationTime * 0.3) * 1.5;
  
  for (int i = 0; i < 30; i++) {
    // Apply wind force
    particles[i][2] += windX * 0.1;
    particles[i][3] += windY * 0.1;
    
    // Move particles
    particles[i][0] += particles[i][2];
    particles[i][1] += particles[i][3];
    
    // Wrap around screen
    if (particles[i][0] < 0) particles[i][0] = gfx->width();
    if (particles[i][0] >= gfx->width()) particles[i][0] = 0;
    if (particles[i][1] < 0) particles[i][1] = gfx->height();
    if (particles[i][1] >= gfx->height()) particles[i][1] = 0;
    
    // Draw particle with tail
    uint16_t color = gfx->color565(100 + i * 4, 200, 150 + i * 2);
    gfx->fillCircle(particles[i][0], particles[i][1], 2, color);
    gfx->drawLine(particles[i][0], particles[i][1], 
                  particles[i][0] - particles[i][2], 
                  particles[i][1] - particles[i][3], color);
  }
}

void drawAnsiTTY() {
  // ANSI terminal with colored text
  gfx->fillScreen(BLACK);
  
  int lineHeight = 12;
  int numLines = gfx->height() / lineHeight;
  int scroll = (int)(animationTime * 5) % (numLines * 2);
  
  uint16_t colors[] = {RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, WHITE};
  
  for (int i = 0; i < numLines; i++) {
    int y = i * lineHeight + 2;
    int colorIndex = (i + scroll) % 7;
    gfx->setTextColor(colors[colorIndex]);
    gfx->setTextSize(1);
    gfx->setCursor(2, y);
    
    switch ((i + scroll) % 8) {
      case 0: gfx->printf("[32mSYSTEM[0m: Boot OK"); break;
      case 1: gfx->printf("[31mERROR[0m: File not found"); break;
      case 2: gfx->printf("[33mWARN[0m: Low memory"); break;
      case 3: gfx->printf("[36mINFO[0m: Process started"); break;
      case 4: gfx->printf("[35mDEBUG[0m: Trace enabled"); break;
      case 5: gfx->printf("[37m$ ls -la /home/user"); break;
      case 6: gfx->printf("[34mLOADING[0m..."); break;
      case 7: gfx->printf("[31m>>[0m Ready"); break;
    }
  }
}

void drawAnt() {
  // Ant colony simulation
  static int ants[20][3]; // x, y, direction for each ant
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 20; i++) {
      ants[i][0] = gfx->width() / 2;
      ants[i][1] = gfx->height() / 2;
      ants[i][2] = random(0, 8); // 8 directions
    }
    initialized = true;
  }
  
  gfx->fillScreen(gfx->color565(139, 69, 19)); // Brown background
  
  // Draw nest
  int nestX = gfx->width() / 2;
  int nestY = gfx->height() / 2;
  gfx->fillCircle(nestX, nestY, 15, gfx->color565(101, 67, 33));
  
  // Update and draw ants
  for (int i = 0; i < 20; i++) {
    // Random walk with bias toward center
    if (random(0, 10) < 3) {
      ants[i][2] = random(0, 8);
    }
    
    // Move ant
    int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    
    ants[i][0] += dx[ants[i][2]];
    ants[i][1] += dy[ants[i][2]];
    
    // Keep ants on screen
    ants[i][0] = constrain(ants[i][0], 1, gfx->width() - 2);
    ants[i][1] = constrain(ants[i][1], 1, gfx->height() - 2);
    
    // Draw ant
    gfx->fillCircle(ants[i][0], ants[i][1], 1, BLACK);
    
    // Draw trail back to nest occasionally
    if (random(0, 50) == 0) {
      gfx->drawLine(ants[i][0], ants[i][1], nestX, nestY, 
                    gfx->color565(160, 82, 45));
    }
  }
}

void drawAntInspect() {
  // Ant inspection - magnified view of ant behavior
  gfx->fillScreen(gfx->color565(50, 50, 50)); // Dark gray
  
  // Draw magnifying glass circle
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int radius = 80;
  
  gfx->drawCircle(centerX, centerY, radius, WHITE);
  gfx->drawCircle(centerX, centerY, radius + 1, WHITE);
  
  // Draw magnified ant
  int antX = centerX + sin(animationTime * 2) * 30;
  int antY = centerY + cos(animationTime * 1.5) * 25;
  
  // Ant body (magnified)
  gfx->fillEllipse(antX, antY, 8, 12, BLACK);
  gfx->fillEllipse(antX - 6, antY, 4, 6, BLACK); // head
  gfx->fillEllipse(antX + 8, antY, 5, 8, BLACK); // abdomen
  
  // Ant legs
  for (int i = 0; i < 6; i++) {
    float legAngle = (i * PI / 3) + sin(animationTime * 8 + i) * 0.5;
    int legX = antX + cos(legAngle) * 15;
    int legY = antY + sin(legAngle) * 10;
    gfx->drawLine(antX, antY, legX, legY, BLACK);
  }
  
  // Magnifying glass handle
  gfx->drawLine(centerX + radius - 10, centerY + radius - 10,
                centerX + radius + 20, centerY + radius + 20, WHITE);
}

void drawAntMaze() {
  // Ant navigating through a maze
  gfx->fillScreen(gfx->color565(139, 69, 19)); // Brown
  
  // Draw maze walls
  int wallColor = gfx->color565(101, 67, 33);
  
  // Horizontal walls
  for (int y = 40; y < gfx->height(); y += 40) {
    for (int x = 0; x < gfx->width(); x += 80) {
      gfx->fillRect(x, y, 60, 8, wallColor);
    }
  }
  
  // Vertical walls
  for (int x = 40; x < gfx->width(); x += 80) {
    for (int y = 0; y < gfx->height(); y += 40) {
      gfx->fillRect(x, y, 8, 30, wallColor);
    }
  }
  
  // Ant navigating maze
  static int antX = 20, antY = 20;
  static int direction = 0;
  static int stepCount = 0;
  
  stepCount++;
  if (stepCount > 20) {
    stepCount = 0;
    direction = random(0, 4);
  }
  
  // Move ant
  int dx[] = {2, 0, -2, 0};
  int dy[] = {0, 2, 0, -2};
  
  int newX = antX + dx[direction];
  int newY = antY + dy[direction];
  
  // Check bounds and walls (simplified)
  if (newX > 5 && newX < gfx->width() - 5 && 
      newY > 5 && newY < gfx->height() - 5) {
    antX = newX;
    antY = newY;
  } else {
    direction = random(0, 4);
  }
  
  // Draw ant
  gfx->fillCircle(antX, antY, 3, BLACK);
  
  // Draw ant trail
  static int trailX[50], trailY[50];
  static int trailIndex = 0;
  
  trailX[trailIndex] = antX;
  trailY[trailIndex] = antY;
  trailIndex = (trailIndex + 1) % 50;
  
  for (int i = 0; i < 50; i++) {
    if (trailX[i] != 0 || trailY[i] != 0) {
      gfx->drawPixel(trailX[i], trailY[i], RED);
    }
  }
}

void drawAntSpotlight() {
  // Spotlight following ants
  gfx->fillScreen(BLACK);
  
  // Draw multiple ants
  static int ants[10][2];
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 10; i++) {
      ants[i][0] = random(gfx->width());
      ants[i][1] = random(gfx->height());
    }
    initialized = true;
  }
  
  // Move ants randomly
  for (int i = 0; i < 10; i++) {
    ants[i][0] += random(-2, 3);
    ants[i][1] += random(-2, 3);
    
    // Keep on screen
    ants[i][0] = constrain(ants[i][0], 10, gfx->width() - 10);
    ants[i][1] = constrain(ants[i][1], 10, gfx->height() - 10);
  }
  
  // Spotlight follows first ant
  int spotX = ants[0][0];
  int spotY = ants[0][1];
  int spotRadius = 50 + sin(animationTime * 3) * 10;
  
  // Draw spotlight effect (bright circle)
  for (int r = 0; r < spotRadius; r += 3) {
    int brightness = map(r, 0, spotRadius, 255, 0);
    uint16_t color = gfx->color565(brightness/4, brightness/4, brightness/8);
    gfx->drawCircle(spotX, spotY, r, color);
  }
  
  // Draw ants (only visible in spotlight)
  for (int i = 0; i < 10; i++) {
    int distance = sqrt(pow(ants[i][0] - spotX, 2) + pow(ants[i][1] - spotY, 2));
    if (distance < spotRadius) {
      gfx->fillCircle(ants[i][0], ants[i][1], 2, RED);
    }
  }
}

void drawApollonian() {
  // Apollonian gasket fractal
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  float scale = sin(animationTime * 0.5) * 0.3 + 1.0;
  
  // Draw nested circles
  for (int level = 0; level < 6; level++) {
    float radius = (80 - level * 12) * scale;
    if (radius < 5) continue;
    
    uint16_t color = gfx->color565(255 - level * 40, level * 40, 128 + level * 20);
    
    // Main circle
    gfx->drawCircle(centerX, centerY, radius, color);
    
    // Tangent circles
    for (int i = 0; i < level + 3; i++) {
      float angle = (i * 2 * PI / (level + 3)) + animationTime * (level + 1) * 0.2;
      float subRadius = radius / (level + 2);
      int x = centerX + cos(angle) * (radius - subRadius);
      int y = centerY + sin(angle) * (radius - subRadius);
      gfx->drawCircle(x, y, subRadius, color);
    }
  }
}

void drawApple2() {
  // Apple II computer screen simulation
  gfx->fillScreen(BLACK);
  
  // Green phosphor color
  uint16_t greenColor = gfx->color565(0, 255, 0);
  uint16_t dimGreen = gfx->color565(0, 128, 0);
  
  gfx->setTextColor(greenColor);
  gfx->setTextSize(1);
  
  // Apple II startup text
  int lineHeight = 10;
  int currentLine = 0;
  
  gfx->setCursor(5, currentLine * lineHeight);
  gfx->printf("APPLE ][");
  currentLine += 2;
  
  gfx->setCursor(5, currentLine * lineHeight);
  gfx->printf("COPYRIGHT 1977,1978,1979");
  currentLine++;
  
  gfx->setCursor(5, currentLine * lineHeight);
  gfx->printf("APPLE COMPUTER INC.");
  currentLine += 2;
  
  gfx->setCursor(5, currentLine * lineHeight);
  gfx->printf("ALL RIGHTS RESERVED");
  currentLine += 2;
  
  // Scrolling code lines
  int scroll = (int)(animationTime * 3) % 8;
  for (int i = 0; i < 12; i++) {
    gfx->setCursor(5, (currentLine + i) * lineHeight);
    switch ((i + scroll) % 8) {
      case 0: gfx->printf("10 PRINT \"HELLO WORLD\""); break;
      case 1: gfx->printf("20 FOR I=1 TO 10"); break;
      case 2: gfx->printf("30 PRINT I"); break;
      case 3: gfx->printf("40 NEXT I"); break;
      case 4: gfx->printf("50 END"); break;
      case 5: gfx->printf("RUN"); break;
      case 6: gfx->printf("HELLO WORLD"); break;
      case 7: gfx->printf("READY"); break;
    }
  }
  
  // Blinking cursor
  if ((int)(animationTime * 4) % 2) {
    gfx->fillRect(5 + 6*6, (currentLine + 12) * lineHeight, 6, 8, greenColor);
  }
}

void drawApple2Main() {
  // Apple II main program demonstration
  gfx->fillScreen(BLACK);
  
  uint16_t greenColor = gfx->color565(0, 255, 0);
  gfx->setTextColor(greenColor);
  gfx->setTextSize(1);
  
  // Draw Apple II hi-res graphics simulation
  int phase = (int)(animationTime * 2) % 4;
  
  switch(phase) {
    case 0:
      // Text mode
      gfx->setCursor(5, 5);
      gfx->printf("*** APPLE ][ DEMO ***");
      gfx->setCursor(5, 25);
      gfx->printf("LOADING GRAPHICS...");
      break;
      
    case 1:
      // Lo-res graphics blocks
      for (int y = 0; y < gfx->height(); y += 8) {
        for (int x = 0; x < gfx->width(); x += 14) {
          uint16_t color = gfx->color565(
            random(0, 256), 
            random(0, 256), 
            random(0, 256)
          );
          gfx->fillRect(x, y, 14, 8, color);
        }
      }
      break;
      
    case 2:
      // Hi-res line drawing
      for (int i = 0; i < 20; i++) {
        float angle = (i * PI / 10) + animationTime * 2;
        int x1 = gfx->width() / 2;
        int y1 = gfx->height() / 2;
        int x2 = x1 + cos(angle) * 100;
        int y2 = y1 + sin(angle) * 80;
        uint16_t color = gfx->color565(0, 255 - i*10, i*10);
        gfx->drawLine(x1, y1, x2, y2, color);
      }
      break;
      
    case 3:
      // Apple II paddle/joystick demo
      gfx->setCursor(5, 5);
      gfx->printf("PADDLE DEMO");
      
      int paddleX = gfx->width()/2 + sin(animationTime * 3) * 100;
      int paddleY = gfx->height()/2 + cos(animationTime * 2) * 60;
      
      gfx->fillCircle(paddleX, paddleY, 8, greenColor);
      gfx->drawLine(gfx->width()/2, gfx->height()/2, paddleX, paddleY, greenColor);
      break;
  }
}

void loop() {
  // Handle auto scroll
  static unsigned long lastAutoScroll = 0;
  if (autoScroll && (millis() - lastAutoScroll > autoScrollSeconds * 1000)) {
    currentMode = (AnimationMode)((currentMode + 1) % totalEffects);
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
      currentMode = (AnimationMode)((currentMode + 1) % totalEffects);
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
      currentMode = (AnimationMode)((currentMode + 1) % totalEffects);
      Serial.printf("Touch: Switched to mode %d\n", currentMode);
      lastPress = millis();
      lastAutoScroll = millis(); // Reset auto scroll timer
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
    case ANALOGTV:
      drawAnalogTV();
      break;
    case ANALOGTV_CLI:
      drawAnalogTVCLI();
      break;
    case ANEMONE:
      drawAnemone();
      break;
    case ANEMOTAXIS:
      drawAnemotaxis();
      break;
    case ANSI_TTY:
      drawAnsiTTY();
      break;
    case ANT:
      drawAnt();
      break;
    case ANTINSPECT:
      drawAntInspect();
      break;
    case ANTMAZE:
      drawAntMaze();
      break;
    case ANTSPOTLIGHT:
      drawAntSpotlight();
      break;
    case APOLLONIAN:
      drawApollonian();
      break;
    case APPLE2:
      drawApple2();
      break;
    case APPLE2_MAIN:
      drawApple2Main();
      break;
  }
  
  animationTime += 0.05;
  delay(50);
}