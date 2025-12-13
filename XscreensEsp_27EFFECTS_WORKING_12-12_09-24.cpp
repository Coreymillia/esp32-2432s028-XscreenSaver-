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

// Animation modes - 27 EFFECTS
enum AnimationMode {
  BOXED = 0,
  BOXFIT = 1, 
  BRAID = 2,
  BSOD = 3,
  B_SPHERE = 4,
  BUBBLE3D = 5,
  BUILDLWO = 6,
  BUMPS = 7,
  CAGE = 8,
  CAROUSEL = 9,
  CCURVE = 10,
  CELTIC = 11,
  CHESSMODELS = 12,
  CHOMPYTOWER = 13,
  CIRCUIT = 14,
  CITYFLOW = 15,
  CLIENTMSG = 16,
  CLOUDLIFE = 17,
  COLORBARS = 18,
  COLORS = 19,
  COMPANION = 20,
  COMPANION_DISC = 21,
  COMPANION_HEART = 22,
  COMPANION_QUAD = 23,
  COMPASS = 24,
  CORAL = 25,
  COVID19 = 26,
  MODE_COUNT = 27
};

// Global variables
AnimationMode currentMode = BOXED;
float animationTime = 0;
bool bootPressed = false;
unsigned long lastPress = 0;
bool autoScroll = false;
unsigned long autoScrollTimer = 0;
unsigned long bootHoldStart = 0;
const unsigned long HOLD_TIME = 1000; // 1 second hold to toggle auto-scroll
const unsigned long AUTO_SCROLL_INTERVAL = 30000; // 30 seconds per effect

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

void drawBoxed() {
  // Boxed effect - rotating 3D-like boxes
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 8; i++) {
    int x = (int)(sin(animationTime * 2 + i * 0.8) * 120 + gfx->width() / 2);
    int y = (int)(cos(animationTime * 1.5 + i * 0.6) * 100 + gfx->height() / 2);
    int size = (int)(sin(animationTime * 3 + i) * 8 + 15);
    
    float phase = animationTime + i * 0.7;
    int red = (int)(sin(phase) * 127 + 128);
    int green = (int)(cos(phase + 1) * 127 + 128);  
    int blue = (int)(sin(phase + 2) * 127 + 128);
    
    if (x > size && y > size && x < gfx->width()-size && y < gfx->height()-size) {
      gfx->fillRect(x-size, y-size, size*2, size*2, gfx->color565(red, green, blue));
      gfx->drawRect(x-size, y-size, size*2, size*2, WHITE);
    }
  }
}

void drawBoxfit() {
  // Boxfit effect - fitting rectangles
  gfx->fillScreen(BLACK);
  for (int i = 0; i < 29; i++) {
    int x = (int)(sin(animationTime + i * 0.5) * 100 + gfx->width() / 2);
    int y = (int)(cos(animationTime * 1.3 + i * 0.3) * 80 + gfx->height() / 2);
    int size = (int)(sin(animationTime * 2 + i) * 15 + 20);
    
    float phase = animationTime + i * 0.2;
    int r = (int)(sin(phase) * 127 + 128);
    int g = (int)(sin(phase + 2) * 127 + 128);
    int b = (int)(sin(phase + 4) * 127 + 128);
    
    if (x > 0 && y > 0 && x < gfx->width() && y < gfx->height() && size > 0) {
      gfx->fillRect(x, y, size, size, gfx->color565(r, g, b));
    }
  }
}

void drawBraid() {
  // Braid effect - interwoven lines
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 50; j++) {
      float t = animationTime + j * 0.1;
      float angle1 = t + i * PI / 3;
      float angle2 = t + i * PI / 3 + PI;
      
      int radius = 60 + sin(t + i) * 20;
      int x1 = centerX + cos(angle1) * radius;
      int y1 = centerY + sin(angle1) * radius * 0.5;
      int x2 = centerX + cos(angle2) * radius;
      int y2 = centerY + sin(angle2) * radius * 0.5;
      
      uint16_t color = gfx->color565(128 + sin(t + i) * 127, 
                                     128 + cos(t + i + 1) * 127,
                                     128 + sin(t + i + 2) * 127);
      gfx->drawLine(x1, y1, x2, y2, color);
    }
  }
}

void drawBSOD() {
  // Blue Screen of Death effect
  gfx->fillScreen(gfx->color565(0, 0, 170)); // Blue background
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  
  // Fake BSOD text
  gfx->setCursor(10, 20);
  gfx->printf("A problem has been detected");
  gfx->setCursor(10, 40);
  gfx->printf("SYSTEM_THREAD_EXCEPTION");
  gfx->setCursor(10, 60);
  gfx->printf("*** STOP: 0x000000");
  
  // Flickering effect
  if ((int)(animationTime * 10) % 20 < 10) {
    gfx->setCursor(10, 100);
    gfx->printf("Press any key to continue");
  }
  
  // Error bars
  for (int i = 0; i < 5; i++) {
    int y = 140 + i * 20;
    int width = (int)(sin(animationTime + i) * 100 + 150);
    gfx->drawFastHLine(10, y, width, WHITE);
  }
}

void drawBSphere() {
  // B_Sphere effect - bouncing spheres
  gfx->fillScreen(gfx->color565(0, 0, 50)); // Dark blue background
  
  for (int i = 0; i < 12; i++) {
    float t = animationTime + i * 0.3;
    int x = (int)(sin(t * 1.5) * (gfx->width()/2 - 30) + gfx->width()/2);
    int y = (int)(cos(t * 1.2) * (gfx->height()/2 - 30) + gfx->height()/2);
    int radius = (int)(sin(t * 2) * 8 + 12);
    
    // Color based on position and time
    int red = (int)(sin(t) * 127 + 128);
    int green = (int)(cos(t + 1) * 127 + 128);
    int blue = (int)(sin(t + 2) * 127 + 128);
    
    gfx->fillCircle(x, y, radius, gfx->color565(red, green, blue));
    gfx->drawCircle(x, y, radius + 2, WHITE);
  }
}

void drawBubble3D() {
  // Bubble3D effect - 3D floating bubbles
  static float bubbles[20][4]; // x, y, z, size for each bubble
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 20; i++) {
      bubbles[i][0] = random(-100, 100); // x
      bubbles[i][1] = random(-100, 100); // y  
      bubbles[i][2] = random(50, 200);   // z
      bubbles[i][3] = random(5, 15);     // size
    }
    initialized = true;
  }
  
  gfx->fillScreen(gfx->color565(0, 20, 60)); // Deep water background
  
  for (int i = 0; i < 20; i++) {
    // Animate bubbles
    bubbles[i][1] -= 0.5; // Rise up
    bubbles[i][0] += sin(animationTime + i) * 0.3; // Drift
    
    if (bubbles[i][1] < -100) {
      bubbles[i][1] = 100;
      bubbles[i][0] = random(-100, 100);
    }
    
    // Project 3D to 2D
    float perspective = 150.0 / bubbles[i][2];
    int screenX = (int)(bubbles[i][0] * perspective + gfx->width() / 2);
    int screenY = (int)(bubbles[i][1] * perspective + gfx->height() / 2);
    int screenSize = (int)(bubbles[i][3] * perspective);
    
    if (screenX > 0 && screenX < gfx->width() && screenY > 0 && screenY < gfx->height()) {
      // Draw bubble with highlight
      gfx->fillCircle(screenX, screenY, screenSize, gfx->color565(100, 150, 255));
      gfx->fillCircle(screenX - screenSize/3, screenY - screenSize/3, screenSize/3, WHITE);
    }
  }
}

void drawBuildlwo() {
  // Buildlwo effect - 3D object construction
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw wireframe-style 3D construction
  for (int i = 0; i < 8; i++) {
    float t = animationTime + i * PI / 4;
    int radius = 60 + sin(animationTime + i) * 20;
    
    // Draw rotating wireframe cube
    for (int j = 0; j < 4; j++) {
      float angle1 = t + j * PI / 2;
      float angle2 = t + (j + 1) * PI / 2;
      
      int x1 = centerX + cos(angle1) * radius;
      int y1 = centerY + sin(angle1) * radius;
      int x2 = centerX + cos(angle2) * radius;
      int y2 = centerY + sin(angle2) * radius;
      
      // Inner and outer connections
      uint16_t color = gfx->color565(128 + sin(t) * 127, 255, 128 + cos(t) * 127);
      gfx->drawLine(x1, y1, x2, y2, color);
      gfx->drawLine(x1, y1, centerX, centerY, color);
    }
  }
}

void drawBumps() {
  // Bumps effect - bump mapping simulation
  gfx->fillScreen(BLACK);
  
  // Create bump map effect with light source
  int lightX = (int)(sin(animationTime * 0.5) * 100 + gfx->width() / 2);
  int lightY = (int)(cos(animationTime * 0.3) * 80 + gfx->height() / 2);
  
  for (int y = 0; y < gfx->height(); y += 4) {
    for (int x = 0; x < gfx->width(); x += 4) {
      // Calculate distance from light source
      float dx = x - lightX;
      float dy = y - lightY;
      float distance = sqrt(dx*dx + dy*dy);
      
      // Height map based on sine waves
      float height = sin(x * 0.1 + animationTime) * sin(y * 0.1 + animationTime * 0.7);
      
      // Lighting calculation
      float intensity = 1.0 / (1.0 + distance * 0.01) * (height + 1.0) * 0.5;
      intensity = constrain(intensity, 0, 1);
      
      int brightness = (int)(intensity * 255);
      uint16_t color = gfx->color565(brightness, brightness/2, brightness);
      gfx->fillRect(x, y, 4, 4, color);
    }
  }
  
  // Draw light source
  gfx->fillCircle(lightX, lightY, 8, YELLOW);
}

void drawCage() {
  // Cage effect - Escher-style impossible cage
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw impossible cage structure
  for (int layer = 0; layer < 3; layer++) {
    float layerOffset = layer * PI / 3 + animationTime * 0.2;
    int radius = 40 + layer * 25;
    
    for (int i = 0; i < 6; i++) {
      float angle1 = layerOffset + i * PI / 3;
      float angle2 = layerOffset + (i + 1) * PI / 3;
      
      // Outer hexagon
      int x1 = centerX + cos(angle1) * radius;
      int y1 = centerY + sin(angle1) * radius;
      int x2 = centerX + cos(angle2) * radius;
      int y2 = centerY + sin(angle2) * radius;
      
      uint16_t color = gfx->color565(255 - layer * 50, 200 - layer * 30, 255);
      gfx->drawLine(x1, y1, x2, y2, color);
      
      // Connect to inner layers for impossible effect
      if (layer > 0) {
        int innerRadius = radius - 25;
        int ix = centerX + cos(angle1 + PI) * innerRadius;
        int iy = centerY + sin(angle1 + PI) * innerRadius;
        gfx->drawLine(x1, y1, ix, iy, color);
      }
    }
  }
}

void drawCarousel() {
  // Carousel effect - rotating objects in 3D
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < 8; i++) {
    float angle = (animationTime + i * PI / 4) * 0.5;
    float z = sin(angle * 2) * 50 + 100; // Z depth
    float scale = 100.0 / z; // Perspective scaling
    
    int x = centerX + cos(angle) * 80 * scale;
    int y = centerY + sin(angle + animationTime) * 20 * scale;
    int size = (int)(15 * scale);
    
    if (size > 2) {
      uint16_t color = gfx->color565(
        (int)(sin(angle + animationTime) * 127 + 128),
        (int)(cos(angle + animationTime + 1) * 127 + 128),
        (int)(sin(angle + animationTime + 2) * 127 + 128)
      );
      gfx->fillRect(x - size/2, y - size/2, size, size, color);
    }
  }
}

void drawCCurve() {
  // C Curve fractal
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  float scale = 60 + sin(animationTime * 0.5) * 20;
  
  // Draw C-curve iterations
  for (int depth = 1; depth <= 6; depth++) {
    int segments = 1 << depth; // 2^depth segments
    
    for (int i = 0; i < segments; i++) {
      float t = (float)i / segments;
      float angle = 0;
      
      // Calculate C-curve angle
      int temp = i;
      for (int j = 0; j < depth; j++) {
        if (temp & 1) angle += PI / 4;
        else angle -= PI / 4;
        temp >>= 1;
      }
      
      angle += animationTime * 0.2;
      
      int x = centerX + cos(angle) * scale * t;
      int y = centerY + sin(angle) * scale * t;
      
      uint16_t color = gfx->color565(255 - depth * 30, 100 + depth * 20, 255);
      gfx->fillCircle(x, y, 2, color);
    }
  }
}

void drawCeltic() {
  // Celtic knot pattern
  gfx->fillScreen(BLACK);
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw interwoven Celtic pattern
  for (int ring = 0; ring < 3; ring++) {
    float radius = 30 + ring * 25;
    int segments = 8 + ring * 4;
    
    for (int i = 0; i < segments; i++) {
      float angle1 = (float)i / segments * 2 * PI + animationTime * (0.1 + ring * 0.05);
      float angle2 = (float)(i + 1) / segments * 2 * PI + animationTime * (0.1 + ring * 0.05);
      
      // Create interwoven effect
      float wave = sin(angle1 * 4 + animationTime) * 10;
      
      int x1 = centerX + cos(angle1) * (radius + wave);
      int y1 = centerY + sin(angle1) * (radius + wave);
      int x2 = centerX + cos(angle2) * (radius + wave);
      int y2 = centerY + sin(angle2) * (radius + wave);
      
      uint16_t color = gfx->color565(
        100 + ring * 50,
        200 - ring * 30,
        150 + ring * 35
      );
      
      gfx->drawLine(x1, y1, x2, y2, color);
      
      // Draw crossing effect
      if (i % 2 == ring % 2) {
        gfx->fillCircle(x1, y1, 3, WHITE);
      }
    }
  }
}

void drawChessModels() {
  // Chess models effect - 3D chess pieces
  gfx->fillScreen(gfx->color565(40, 20, 0)); // Dark brown board
  
  // Draw chess board pattern
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      int squareX = x * (gfx->width() / 8);
      int squareY = y * (gfx->height() / 8);
      int squareW = gfx->width() / 8;
      int squareH = gfx->height() / 8;
      
      if ((x + y) % 2 == 0) {
        gfx->fillRect(squareX, squareY, squareW, squareH, gfx->color565(200, 180, 120));
      }
    }
  }
  
  // Animate chess pieces
  for (int i = 0; i < 6; i++) {
    float angle = animationTime * 0.3 + i * PI / 3;
    int pieceX = (int)(sin(angle) * 120 + gfx->width() / 2);
    int pieceY = (int)(cos(angle) * 80 + gfx->height() / 2);
    
    // Draw different piece types
    uint16_t color = (i < 3) ? WHITE : BLACK;
    switch (i % 3) {
      case 0: // Pawn
        gfx->fillCircle(pieceX, pieceY, 8, color);
        gfx->fillRect(pieceX - 3, pieceY - 12, 6, 8, color);
        break;
      case 1: // Rook
        gfx->fillRect(pieceX - 8, pieceY - 8, 16, 16, color);
        gfx->fillRect(pieceX - 10, pieceY - 12, 4, 8, color);
        gfx->fillRect(pieceX + 6, pieceY - 12, 4, 8, color);
        break;
      case 2: // King
        gfx->fillCircle(pieceX, pieceY, 10, color);
        gfx->drawLine(pieceX - 6, pieceY - 15, pieceX + 6, pieceY - 15, color);
        gfx->drawLine(pieceX, pieceY - 20, pieceX, pieceY - 10, color);
        break;
    }
  }
}

void drawChompyTower() {
  // Chompy tower effect - stacking animated creatures
  gfx->fillScreen(gfx->color565(20, 40, 80)); // Night sky
  
  int baseY = gfx->height() - 20;
  int towerHeight = 8;
  
  for (int level = 0; level < towerHeight; level++) {
    int y = baseY - level * 25;
    int x = gfx->width() / 2 + sin(animationTime + level * 0.5) * (20 - level * 2);
    
    // Body
    int bodySize = 20 - level * 2;
    uint16_t bodyColor = gfx->color565(
      100 + level * 20, 
      150 - level * 10, 
      200 + level * 5
    );
    gfx->fillCircle(x, y, bodySize, bodyColor);
    
    // Eyes
    int eyeOffset = bodySize / 3;
    gfx->fillCircle(x - eyeOffset, y - eyeOffset/2, 3, WHITE);
    gfx->fillCircle(x + eyeOffset, y - eyeOffset/2, 3, WHITE);
    gfx->fillCircle(x - eyeOffset, y - eyeOffset/2, 1, BLACK);
    gfx->fillCircle(x + eyeOffset, y - eyeOffset/2, 1, BLACK);
    
    // Mouth (chompy!)
    float mouthOpen = sin(animationTime * 8 + level) * 0.5 + 0.5;
    int mouthHeight = (int)(mouthOpen * 8 + 2);
    gfx->fillRect(x - 8, y + 2, 16, mouthHeight, BLACK);
    
    // Teeth
    if (mouthHeight > 4) {
      for (int tooth = 0; tooth < 3; tooth++) {
        int toothX = x - 6 + tooth * 6;
        gfx->fillTriangle(toothX, y + 2, toothX + 2, y + 2, toothX + 1, y + 6, WHITE);
      }
    }
  }
}

void drawCircuit() {
  // Circuit board effect
  gfx->fillScreen(gfx->color565(0, 40, 0)); // Dark green PCB
  
  // Draw circuit traces
  for (int i = 0; i < 20; i++) {
    float traceTime = animationTime * 2 + i * 0.3;
    int startX = (int)(sin(traceTime) * 100 + gfx->width() / 2);
    int startY = (int)(cos(traceTime) * 80 + gfx->height() / 2);
    
    // Horizontal traces
    int traceLength = 40 + sin(traceTime) * 20;
    uint16_t traceColor = gfx->color565(0, 200, 100);
    gfx->drawFastHLine(startX, startY, traceLength, traceColor);
    
    // Vertical connections
    if (i % 3 == 0) {
      gfx->drawFastVLine(startX + traceLength, startY - 10, 20, traceColor);
    }
    
    // Components (resistors, capacitors)
    if (i % 4 == 0) {
      // Resistor
      gfx->fillRect(startX + 10, startY - 3, 15, 6, gfx->color565(139, 69, 19));
      // Color bands
      gfx->drawFastVLine(startX + 13, startY - 3, 6, RED);
      gfx->drawFastVLine(startX + 16, startY - 3, 6, YELLOW);
      gfx->drawFastVLine(startX + 19, startY - 3, 6, BLUE);
    } else if (i % 4 == 1) {
      // Capacitor
      gfx->fillRect(startX + 15, startY - 4, 3, 8, gfx->color565(100, 100, 100));
      gfx->fillRect(startX + 19, startY - 4, 3, 8, gfx->color565(100, 100, 100));
    }
    
    // IC chips
    if (i % 6 == 0) {
      gfx->fillRect(startX + 5, startY - 8, 20, 16, BLACK);
      // Pins
      for (int pin = 0; pin < 4; pin++) {
        gfx->drawFastVLine(startX + 7 + pin * 4, startY - 10, 4, gfx->color565(200, 200, 200));
        gfx->drawFastVLine(startX + 7 + pin * 4, startY + 8, 4, gfx->color565(200, 200, 200));
      }
    }
  }
  
  // Power flowing through traces
  for (int i = 0; i < 5; i++) {
    int pulseX = (int)(animationTime * 100 + i * 60) % gfx->width();
    int pulseY = 40 + i * 20;
    gfx->fillCircle(pulseX, pulseY, 3, CYAN);
  }
}

void drawCityFlow() {
  // City flow effect - traffic and data streams
  gfx->fillScreen(BLACK);
  
  // Draw city grid
  for (int i = 0; i < 8; i++) {
    int x = i * (gfx->width() / 7);
    gfx->drawFastVLine(x, 0, gfx->height(), gfx->color565(40, 40, 40));
  }
  for (int i = 0; i < 6; i++) {
    int y = i * (gfx->height() / 5);
    gfx->drawFastHLine(0, y, gfx->width(), gfx->color565(40, 40, 40));
  }
  
  // Moving traffic lights
  for (int i = 0; i < 15; i++) {
    float flowTime = animationTime * 3 + i * 0.4;
    int x = (int)(flowTime * 20) % gfx->width();
    int y = 20 + (i % 4) * 60;
    
    // Traffic stream
    uint16_t color = (i % 3 == 0) ? RED : (i % 3 == 1) ? GREEN : BLUE;
    gfx->fillRect(x, y, 8, 4, color);
    
    // Data packets
    if (i % 2 == 0) {
      int dataX = (int)(flowTime * 15 + 50) % gfx->width();
      gfx->fillCircle(dataX, y + 20, 2, YELLOW);
    }
  }
}

void drawClientMsg() {
  // Client message system - scrolling text and status
  gfx->fillScreen(gfx->color565(0, 0, 30)); // Dark blue terminal
  
  gfx->setTextColor(GREEN);
  gfx->setTextSize(1);
  
  // Scrolling message system
  int scroll = (int)(animationTime * 30) % 300;
  
  const char* messages[] = {
    "CLIENT> Connection established",
    "SERVER> Authentication required", 
    "CLIENT> Sending credentials...",
    "SERVER> Access granted",
    "CLIENT> Requesting data stream",
    "SERVER> Stream active",
    "CLIENT> Processing packets",
    "SERVER> Transfer complete"
  };
  
  for (int i = 0; i < 8; i++) {
    int y = 10 + i * 15 - scroll;
    if (y > -10 && y < gfx->height()) {
      gfx->setCursor(5, y);
      
      // Blinking cursor effect
      if (i == 7 && (int)(animationTime * 4) % 2 == 0) {
        gfx->printf("%s_", messages[i]);
      } else {
        gfx->printf("%s", messages[i]);
      }
      
      // Status indicators
      if (i % 2 == 0) {
        gfx->fillCircle(gfx->width() - 15, y + 4, 3, (i < 6) ? GREEN : RED);
      }
    }
  }
  
  // Network activity indicator
  int activity = (int)(sin(animationTime * 8) * 50 + 50);
  gfx->drawFastHLine(5, gfx->height() - 10, activity, CYAN);
}

void drawCloudLife() {
  // Cloud life - Conway's Game of Life in the clouds
  static bool grid[40][30];
  static bool newGrid[40][30];
  static bool initialized = false;
  static int generation = 0;
  
  if (!initialized || generation > 100) {
    // Initialize random pattern
    for (int x = 0; x < 40; x++) {
      for (int y = 0; y < 30; y++) {
        grid[x][y] = random(0, 4) == 0; // 25% alive
      }
    }
    initialized = true;
    generation = 0;
  }
  
  // Game of Life rules (every 10 frames)
  if ((int)(animationTime * 10) % 10 == 0) {
    for (int x = 0; x < 40; x++) {
      for (int y = 0; y < 30; y++) {
        int neighbors = 0;
        
        // Count neighbors
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + 40) % 40;
            int ny = (y + dy + 30) % 30;
            if (grid[nx][ny]) neighbors++;
          }
        }
        
        // Apply rules
        if (grid[x][y]) {
          newGrid[x][y] = (neighbors == 2 || neighbors == 3);
        } else {
          newGrid[x][y] = (neighbors == 3);
        }
      }
    }
    
    // Copy back
    for (int x = 0; x < 40; x++) {
      for (int y = 0; y < 30; y++) {
        grid[x][y] = newGrid[x][y];
      }
    }
    generation++;
  }
  
  // Draw cloud background
  gfx->fillScreen(gfx->color565(135, 206, 235)); // Sky blue
  
  // Draw living cells as cloud particles
  for (int x = 0; x < 40; x++) {
    for (int y = 0; y < 30; y++) {
      if (grid[x][y]) {
        int screenX = x * (gfx->width() / 40);
        int screenY = y * (gfx->height() / 30);
        
        // Cloud-like appearance
        int size = 3 + sin(animationTime + x + y) * 2;
        uint16_t cloudColor = gfx->color565(255, 255, 255 - (x + y) % 50);
        gfx->fillCircle(screenX, screenY, size, cloudColor);
      }
    }
  }
}

void drawColorBars() {
  // Color bars effect - TV test pattern
  gfx->fillScreen(BLACK);
  
  int barWidth = gfx->width() / 8;
  uint16_t colors[] = {
    WHITE, YELLOW, CYAN, GREEN, MAGENTA, RED, BLUE, BLACK
  };
  
  // Main color bars
  for (int i = 0; i < 8; i++) {
    gfx->fillRect(i * barWidth, 0, barWidth, gfx->height() * 2 / 3, colors[i]);
  }
  
  // Bottom test pattern
  int bottomY = gfx->height() * 2 / 3;
  int bottomHeight = gfx->height() / 3;
  
  // Pluge pattern
  for (int i = 0; i < 4; i++) {
    int x = i * (gfx->width() / 4);
    int brightness = i * 85; // 0, 85, 170, 255
    gfx->fillRect(x, bottomY, gfx->width() / 4, bottomHeight, 
                  gfx->color565(brightness, brightness, brightness));
  }
  
  // Animated sweep
  int sweepX = (int)(sin(animationTime) * gfx->width() / 2 + gfx->width() / 2);
  gfx->drawFastVLine(sweepX, 0, gfx->height(), WHITE);
}

void drawColors() {
  // Colors effect - cycling through color space
  gfx->fillScreen(BLACK);
  
  // HSV color wheel effect
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int radius = min(gfx->width(), gfx->height()) / 3;
  
  for (int angle = 0; angle < 360; angle += 5) {
    float rad = (angle + animationTime * 50) * PI / 180.0;
    int x1 = centerX;
    int y1 = centerY;
    int x2 = centerX + cos(rad) * radius;
    int y2 = centerY + sin(rad) * radius;
    
    // Convert HSV to RGB approximation
    float hue = (angle + animationTime * 100) / 360.0;
    while (hue > 1.0) hue -= 1.0;
    
    int r, g, b;
    if (hue < 0.167) {
      r = 255; g = hue * 6 * 255; b = 0;
    } else if (hue < 0.333) {
      r = (0.333 - hue) * 6 * 255; g = 255; b = 0;
    } else if (hue < 0.5) {
      r = 0; g = 255; b = (hue - 0.333) * 6 * 255;
    } else if (hue < 0.667) {
      r = 0; g = (0.667 - hue) * 6 * 255; b = 255;
    } else if (hue < 0.833) {
      r = (hue - 0.667) * 6 * 255; g = 0; b = 255;
    } else {
      r = 255; g = 0; b = (1.0 - hue) * 6 * 255;
    }
    
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(r, g, b));
  }
  
  // Color gradient bands
  for (int y = 0; y < gfx->height(); y += 4) {
    int colorShift = (int)(animationTime * 50 + y) % 768;
    int r = 0, g = 0, b = 0;
    
    if (colorShift < 256) {
      r = colorShift; g = 255 - colorShift; b = 0;
    } else if (colorShift < 512) {
      r = 0; g = colorShift - 256; b = 255 - (colorShift - 256);
    } else {
      r = colorShift - 512; g = 0; b = 255 - (colorShift - 512);
    }
    
    gfx->drawFastHLine(0, y, gfx->width(), gfx->color565(r, g, b));
  }
}

void drawCompanion() {
  // Companion effect - following companion cube/creature
  gfx->fillScreen(gfx->color565(20, 20, 40)); // Dark space
  
  // Main companion
  int companionX = (int)(sin(animationTime * 0.8) * 100 + gfx->width() / 2);
  int companionY = (int)(cos(animationTime * 0.6) * 60 + gfx->height() / 2);
  
  // Draw companion body
  int bodySize = 20 + sin(animationTime * 3) * 3;
  gfx->fillRect(companionX - bodySize/2, companionY - bodySize/2, 
                bodySize, bodySize, gfx->color565(200, 200, 200));
  
  // Companion markings (heart symbol)
  uint16_t pink = gfx->color565(255, 192, 203);
  gfx->fillCircle(companionX - 4, companionY - 2, 3, pink);
  gfx->fillCircle(companionX + 4, companionY - 2, 3, pink);
  gfx->fillTriangle(companionX - 7, companionY + 1, 
                    companionX + 7, companionY + 1,
                    companionX, companionY + 8, pink);
  
  // Eyes
  gfx->fillCircle(companionX - 5, companionY - 5, 2, BLUE);
  gfx->fillCircle(companionX + 5, companionY - 5, 2, BLUE);
  
  // Following trail
  for (int i = 0; i < 10; i++) {
    float trailTime = animationTime - i * 0.1;
    int trailX = (int)(sin(trailTime * 0.8) * 100 + gfx->width() / 2);
    int trailY = (int)(cos(trailTime * 0.6) * 60 + gfx->height() / 2);
    
    int trailSize = (10 - i) * 2;
    uint16_t trailColor = gfx->color565(100 - i * 8, 100 - i * 8, 150);
    gfx->fillCircle(trailX, trailY, trailSize, trailColor);
  }
  
  // Floating particles around companion
  for (int i = 0; i < 8; i++) {
    float particleAngle = animationTime * 2 + i * PI / 4;
    int particleRadius = 35 + sin(animationTime * 4 + i) * 10;
    int particleX = companionX + cos(particleAngle) * particleRadius;
    int particleY = companionY + sin(particleAngle) * particleRadius;
    
    gfx->fillCircle(particleX, particleY, 2, YELLOW);
  }
}

void drawCompanionDisc() {
  // Companion disc - spinning disc with companion markings
  gfx->fillScreen(gfx->color565(30, 30, 50));
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int discRadius = 60 + sin(animationTime * 2) * 10;
  
  // Main disc
  gfx->fillCircle(centerX, centerY, discRadius, gfx->color565(180, 180, 180));
  gfx->drawCircle(centerX, centerY, discRadius, WHITE);
  
  // Spinning segments
  for (int i = 0; i < 8; i++) {
    float angle = animationTime * 3 + i * PI / 4;
    int x1 = centerX + cos(angle) * discRadius;
    int y1 = centerY + sin(angle) * discRadius;
    int x2 = centerX + cos(angle) * (discRadius - 20);
    int y2 = centerY + sin(angle) * (discRadius - 20);
    
    uint16_t segmentColor = (i % 2 == 0) ? RED : BLUE;
    gfx->drawLine(centerX, centerY, x1, y1, segmentColor);
    gfx->fillCircle(x2, y2, 5, segmentColor);
  }
  
  // Center companion symbol
  uint16_t pink = gfx->color565(255, 192, 203);
  gfx->fillCircle(centerX - 8, centerY - 5, 6, pink);
  gfx->fillCircle(centerX + 8, centerY - 5, 6, pink);
  gfx->fillTriangle(centerX - 12, centerY + 2, 
                    centerX + 12, centerY + 2,
                    centerX, centerY + 15, pink);
  
  // Orbital particles
  for (int i = 0; i < 6; i++) {
    float orbitAngle = animationTime * 1.5 + i * PI / 3;
    int orbitRadius = discRadius + 20 + sin(animationTime * 4 + i) * 8;
    int orbX = centerX + cos(orbitAngle) * orbitRadius;
    int orbY = centerY + sin(orbitAngle) * orbitRadius;
    gfx->fillCircle(orbX, orbY, 3, YELLOW);
  }
}

void drawCompanionHeart() {
  // Companion heart - pulsing heart with companion features
  gfx->fillScreen(gfx->color565(40, 20, 40));
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Pulsing heart effect
  float pulse = sin(animationTime * 4) * 0.3 + 0.7;
  int heartSize = (int)(30 * pulse);
  uint16_t heartColor = gfx->color565((int)(255 * pulse), 100, 150);
  
  // Draw large heart shape
  gfx->fillCircle(centerX - heartSize/2, centerY - heartSize/4, heartSize/2, heartColor);
  gfx->fillCircle(centerX + heartSize/2, centerY - heartSize/4, heartSize/2, heartColor);
  gfx->fillTriangle(centerX - heartSize, centerY, 
                    centerX + heartSize, centerY,
                    centerX, centerY + heartSize, heartColor);
  
  // Companion eyes on the heart
  gfx->fillCircle(centerX - heartSize/3, centerY - heartSize/6, 4, BLUE);
  gfx->fillCircle(centerX + heartSize/3, centerY - heartSize/6, 4, BLUE);
  gfx->fillCircle(centerX - heartSize/3, centerY - heartSize/6, 2, WHITE);
  gfx->fillCircle(centerX + heartSize/3, centerY - heartSize/6, 2, WHITE);
  
  // Heart particles emanating outward
  for (int i = 0; i < 12; i++) {
    float particleAngle = i * PI / 6 + animationTime * 2;
    float distance = 80 + sin(animationTime * 3 + i) * 20;
    int partX = centerX + cos(particleAngle) * distance;
    int partY = centerY + sin(particleAngle) * distance;
    
    // Small hearts
    int smallSize = 4 + sin(animationTime * 5 + i) * 2;
    uint16_t pink = gfx->color565(255, 150 + i * 8, 200);
    gfx->fillCircle(partX - smallSize/2, partY, smallSize/2, pink);
    gfx->fillCircle(partX + smallSize/2, partY, smallSize/2, pink);
    gfx->fillTriangle(partX - smallSize, partY + smallSize/2,
                      partX + smallSize, partY + smallSize/2,
                      partX, partY + smallSize, pink);
  }
}

void drawCompanionQuad() {
  // Companion quad - four companion cubes in formation
  gfx->fillScreen(gfx->color565(25, 25, 35));
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Four companion positions in a square formation
  int positions[4][2] = {
    {centerX - 40, centerY - 40}, // Top left
    {centerX + 40, centerY - 40}, // Top right  
    {centerX - 40, centerY + 40}, // Bottom left
    {centerX + 40, centerY + 40}  // Bottom right
  };
  
  for (int i = 0; i < 4; i++) {
    float offset = animationTime * 2 + i * PI / 2;
    int compX = positions[i][0] + sin(offset) * 15;
    int compY = positions[i][1] + cos(offset * 0.7) * 10;
    
    // Companion body
    int size = 18 + sin(animationTime * 3 + i) * 3;
    gfx->fillRect(compX - size/2, compY - size/2, size, size, 
                  gfx->color565(200 - i * 20, 200 - i * 20, 200 - i * 20));
    
    // Different colored heart for each companion
    uint16_t heartColors[4] = {
      gfx->color565(255, 100, 150), // Pink
      gfx->color565(100, 255, 150), // Green
      gfx->color565(150, 100, 255), // Purple
      gfx->color565(255, 200, 100)  // Orange
    };
    
    // Heart symbol
    gfx->fillCircle(compX - 4, compY - 2, 3, heartColors[i]);
    gfx->fillCircle(compX + 4, compY - 2, 3, heartColors[i]);
    gfx->fillTriangle(compX - 6, compY + 1,
                      compX + 6, compY + 1,
                      compX, compY + 7, heartColors[i]);
    
    // Eyes
    gfx->fillCircle(compX - 5, compY - 6, 2, CYAN);
    gfx->fillCircle(compX + 5, compY - 6, 2, CYAN);
  }
  
  // Connection lines between companions
  for (int i = 0; i < 4; i++) {
    int next = (i + 1) % 4;
    float alpha = sin(animationTime * 4 + i) * 0.5 + 0.5;
    if (alpha > 0.3) {
      gfx->drawLine(positions[i][0], positions[i][1],
                    positions[next][0], positions[next][1], 
                    gfx->color565((int)(100 * alpha), (int)(200 * alpha), 255));
    }
  }
  
  // Center energy core
  int coreSize = (int)(8 + sin(animationTime * 6) * 4);
  gfx->fillCircle(centerX, centerY, coreSize, WHITE);
}

void drawCompass() {
  // Compass effect - navigational compass with moving needle
  gfx->fillScreen(gfx->color565(40, 60, 80)); // Dark blue background
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int compassRadius = 70;
  
  // Compass outer ring
  gfx->drawCircle(centerX, centerY, compassRadius, WHITE);
  gfx->drawCircle(centerX, centerY, compassRadius - 2, WHITE);
  
  // Cardinal directions
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(centerX - 3, centerY - compassRadius + 15);
  gfx->printf("N");
  gfx->setCursor(centerX + compassRadius - 15, centerY - 3);
  gfx->printf("E");
  gfx->setCursor(centerX - 3, centerY + compassRadius - 25);
  gfx->printf("S");
  gfx->setCursor(centerX - compassRadius + 8, centerY - 3);
  gfx->printf("W");
  
  // Degree markings
  for (int i = 0; i < 36; i++) {
    float angle = i * PI / 18;
    int x1 = centerX + cos(angle) * (compassRadius - 5);
    int y1 = centerY + sin(angle) * (compassRadius - 5);
    int x2 = centerX + cos(angle) * (compassRadius - 10);
    int y2 = centerY + sin(angle) * (compassRadius - 10);
    
    if (i % 9 == 0) {
      gfx->drawLine(x1, y1, x2 - 5, y2 - 5, WHITE);
    } else {
      gfx->drawLine(x1, y1, x2, y2, gfx->color565(150, 150, 150));
    }
  }
  
  // Rotating needle
  float needleAngle = animationTime * 0.5 + sin(animationTime * 0.3) * 2;
  int needleLength = compassRadius - 20;
  
  // North needle (red)
  int nX = centerX + cos(needleAngle - PI/2) * needleLength;
  int nY = centerY + sin(needleAngle - PI/2) * needleLength;
  gfx->drawLine(centerX, centerY, nX, nY, RED);
  gfx->fillCircle(nX, nY, 3, RED);
  
  // South needle (white)
  int sX = centerX + cos(needleAngle + PI/2) * needleLength;
  int sY = centerY + sin(needleAngle + PI/2) * needleLength;
  gfx->drawLine(centerX, centerY, sX, sY, WHITE);
  gfx->fillCircle(sX, sY, 3, WHITE);
  
  // Center pivot
  gfx->fillCircle(centerX, centerY, 5, YELLOW);
}

void drawCoral() {
  // Coral effect - growing coral reef
  gfx->fillScreen(gfx->color565(0, 20, 60)); // Deep ocean blue
  
  static bool coralGrid[40][30];
  static bool initialized = false;
  static int generation = 0;
  
  if (!initialized || generation > 150) {
    // Initialize coral seeds
    for (int x = 0; x < 40; x++) {
      for (int y = 0; y < 30; y++) {
        coralGrid[x][y] = false;
      }
    }
    // Plant initial coral seeds at bottom
    for (int x = 5; x < 35; x++) {
      if (random(0, 3) == 0) {
        coralGrid[x][28] = true;
      }
    }
    initialized = true;
    generation = 0;
  }
  
  // Coral growth simulation (every 20 frames)
  if ((int)(animationTime * 10) % 20 == 0) {
    bool newGrid[40][30];
    for (int x = 0; x < 40; x++) {
      for (int y = 0; y < 30; y++) {
        newGrid[x][y] = coralGrid[x][y];
      }
    }
    
    for (int x = 1; x < 39; x++) {
      for (int y = 1; y < 29; y++) {
        if (!coralGrid[x][y]) {
          // Check for adjacent coral (growth)
          int neighbors = 0;
          for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
              if (dx == 0 && dy == 0) continue;
              if (coralGrid[x + dx][y + dy]) neighbors++;
            }
          }
          // Grow if 1-3 neighbors, prefer upward growth
          if (neighbors >= 1 && neighbors <= 3) {
            if (coralGrid[x][y + 1] && random(0, 4) == 0) {
              newGrid[x][y] = true;
            }
          }
        }
      }
    }
    
    for (int x = 0; x < 40; x++) {
      for (int y = 0; y < 30; y++) {
        coralGrid[x][y] = newGrid[x][y];
      }
    }
    generation++;
  }
  
  // Draw coral
  for (int x = 0; x < 40; x++) {
    for (int y = 0; y < 30; y++) {
      if (coralGrid[x][y]) {
        int screenX = x * (gfx->width() / 40);
        int screenY = y * (gfx->height() / 30);
        
        // Coral colors based on depth and position
        int depth = 30 - y;
        uint16_t coralColor = gfx->color565(
          255 - depth * 4,
          100 + depth * 2 + (x % 5) * 10,
          150 + sin(animationTime + x + y) * 30
        );
        
        gfx->fillRect(screenX, screenY, 
                      gfx->width() / 40 + 1, gfx->height() / 30 + 1, 
                      coralColor);
      }
    }
  }
  
  // Swimming fish
  for (int i = 0; i < 5; i++) {
    float fishTime = animationTime * 2 + i * 1.2;
    int fishX = (int)(sin(fishTime) * 120 + gfx->width() / 2);
    int fishY = 30 + i * 40 + sin(fishTime * 2) * 15;
    
    uint16_t fishColor = gfx->color565(255, 200 - i * 30, 100 + i * 20);
    gfx->fillCircle(fishX, fishY, 4, fishColor);
    gfx->fillTriangle(fishX - 6, fishY, fishX - 10, fishY - 3, fishX - 10, fishY + 3, fishColor);
  }
}

void drawCovid19() {
  // COVID-19 virus simulation
  gfx->fillScreen(gfx->color565(10, 10, 20)); // Dark background
  
  static int viruses[15][4]; // x, y, vx, vy
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 15; i++) {
      viruses[i][0] = random(20, gfx->width() - 20);
      viruses[i][1] = random(20, gfx->height() - 20);
      viruses[i][2] = random(-2, 3);
      viruses[i][3] = random(-2, 3);
    }
    initialized = true;
  }
  
  // Update virus positions
  for (int i = 0; i < 15; i++) {
    viruses[i][0] += viruses[i][2];
    viruses[i][1] += viruses[i][3];
    
    // Bounce off walls
    if (viruses[i][0] < 15 || viruses[i][0] > gfx->width() - 15) {
      viruses[i][2] = -viruses[i][2];
    }
    if (viruses[i][1] < 15 || viruses[i][1] > gfx->height() - 15) {
      viruses[i][3] = -viruses[i][3];
    }
    
    // Keep in bounds
    viruses[i][0] = constrain(viruses[i][0], 15, gfx->width() - 15);
    viruses[i][1] = constrain(viruses[i][1], 15, gfx->height() - 15);
  }
  
  // Draw viruses
  for (int i = 0; i < 15; i++) {
    int vX = viruses[i][0];
    int vY = viruses[i][1];
    
    // Virus core
    int coreSize = 8 + sin(animationTime * 3 + i) * 2;
    gfx->fillCircle(vX, vY, coreSize, gfx->color565(200, 50, 50));
    
    // Spike proteins
    for (int spike = 0; spike < 12; spike++) {
      float spikeAngle = spike * PI / 6 + animationTime + i * 0.5;
      int spikeLength = coreSize + 4 + sin(animationTime * 4 + spike + i) * 2;
      
      int sX = vX + cos(spikeAngle) * spikeLength;
      int sY = vY + sin(spikeAngle) * spikeLength;
      
      // Spike protein (red)
      gfx->fillCircle(sX, sY, 2, RED);
      gfx->drawLine(vX, vY, sX, sY, gfx->color565(150, 30, 30));
    }
    
    // RNA strands inside
    for (int rna = 0; rna < 3; rna++) {
      float rnaAngle = animationTime * 2 + rna * PI * 2 / 3;
      int rnaX = vX + cos(rnaAngle) * (coreSize - 3);
      int rnaY = vY + sin(rnaAngle) * (coreSize - 3);
      gfx->fillCircle(rnaX, rnaY, 1, YELLOW);
    }
  }
  
  // Transmission particles
  for (int i = 0; i < 8; i++) {
    float pTime = animationTime * 4 + i * 0.8;
    int pX = (int)(sin(pTime) * 80 + gfx->width() / 2);
    int pY = (int)(cos(pTime * 1.3) * 60 + gfx->height() / 2);
    
    uint16_t particleColor = gfx->color565(100, 100, 255 - i * 20);
    gfx->fillCircle(pX, pY, 1 + i % 3, particleColor);
  }
}

void loop() {
  // Check boot button for mode switch and auto-scroll toggle
  if (digitalRead(BOOT_PIN) == LOW) {
    if (!bootPressed) {
      bootHoldStart = millis();
    }
    
    // Check for hold (toggle auto-scroll)
    if (millis() - bootHoldStart >= HOLD_TIME && bootPressed) {
      autoScroll = !autoScroll;
      autoScrollTimer = millis();
      Serial.printf("Auto-scroll %s\n", autoScroll ? "ON" : "OFF");
      gfx->fillScreen(BLACK);
      // Flash screen to indicate toggle
      gfx->fillScreen(autoScroll ? GREEN : RED);
      delay(200);
      gfx->fillScreen(BLACK);
      bootHoldStart = millis() + 5000; // Prevent immediate retrigger
    }
    // Check for quick press (cycle modes)
    else if (!bootPressed && (millis() - bootHoldStart < HOLD_TIME) && (millis() - lastPress > 300)) {
      autoScroll = false; // Disable auto-scroll when manually cycling
      currentMode = (AnimationMode)((currentMode + 1) % MODE_COUNT);
      Serial.printf("Manual switch to mode %d\n", currentMode);
      lastPress = millis();
      gfx->fillScreen(BLACK);
    }
    bootPressed = true;
  } else {
    bootPressed = false;
  }
  
  // Auto-scroll functionality
  if (autoScroll && (millis() - autoScrollTimer >= AUTO_SCROLL_INTERVAL)) {
    currentMode = (AnimationMode)((currentMode + 1) % MODE_COUNT);
    Serial.printf("Auto-scroll to mode %d\n", currentMode);
    autoScrollTimer = millis();
    gfx->fillScreen(BLACK);
  }
  
  // Check touch
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    if (millis() - lastPress > 300) {
      autoScroll = false; // Disable auto-scroll when manually cycling
      currentMode = (AnimationMode)((currentMode + 1) % MODE_COUNT);
      Serial.printf("Touch: Switched to mode %d\n", currentMode);
      lastPress = millis();
      gfx->fillScreen(BLACK);
    }
  }
  
  // Draw current effect
  switch (currentMode) {
    case BOXED:
      drawBoxed();
      break;
    case BOXFIT:
      drawBoxfit();
      break;
    case BRAID:
      drawBraid();
      break;
    case BSOD:
      drawBSOD();
      break;
    case B_SPHERE:
      drawBSphere();
      break;
    case BUBBLE3D:
      drawBubble3D();
      break;
    case BUILDLWO:
      drawBuildlwo();
      break;
    case BUMPS:
      drawBumps();
      break;
    case CAGE:
      drawCage();
      break;
    case CAROUSEL:
      drawCarousel();
      break;
    case CCURVE:
      drawCCurve();
      break;
    case CELTIC:
      drawCeltic();
      break;
    case CHESSMODELS:
      drawChessModels();
      break;
    case CHOMPYTOWER:
      drawChompyTower();
      break;
    case CIRCUIT:
      drawCircuit();
      break;
    case CITYFLOW:
      drawCityFlow();
      break;
    case CLIENTMSG:
      drawClientMsg();
      break;
    case CLOUDLIFE:
      drawCloudLife();
      break;
    case COLORBARS:
      drawColorBars();
      break;
    case COLORS:
      drawColors();
      break;
    case COMPANION:
      drawCompanion();
      break;
    case COMPANION_DISC:
      drawCompanionDisc();
      break;
    case COMPANION_HEART:
      drawCompanionHeart();
      break;
    case COMPANION_QUAD:
      drawCompanionQuad();
      break;
    case COMPASS:
      drawCompass();
      break;
    case CORAL:
      drawCoral();
      break;
    case COVID19:
      drawCovid19();
      break;
  }
  
  animationTime += 0.05;
  delay(50);
}