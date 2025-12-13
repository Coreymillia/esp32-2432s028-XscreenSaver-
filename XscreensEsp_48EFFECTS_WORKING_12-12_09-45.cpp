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

// Animation modes - 48 EFFECTS
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
  COW_FACE = 27,
  COW_HIDE = 28,
  COW_HOOFS = 29,
  COW_HORNS = 30,
  COW_TAIL = 31,
  COW_UDDER = 32,
  CRACKBERG = 33,
  CRITICAL = 34,
  CRUMBLER = 35,
  CRYSTAL = 36,
  CUBE21 = 37,
  CUBENETIC = 38,
  CUBESTACK = 39,
  CUBESTORM = 40,
  CUBETWIST = 41,
  CUBICGRID = 42,
  CUBOCTEVERSION = 43,
  CWAVES = 44,
  CYNOSURE = 45,
  DANGERBALL = 46,
  DECAYSCREEN = 47,
  MODE_COUNT = 48
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

void drawCowFace() {
  // Cow face effect - animated cow face
  gfx->fillScreen(gfx->color565(135, 206, 235)); // Sky blue background
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Head
  int headSize = 60 + sin(animationTime * 2) * 5;
  gfx->fillCircle(centerX, centerY, headSize, WHITE);
  gfx->drawCircle(centerX, centerY, headSize, BLACK);
  
  // Cow spots on head
  for (int i = 0; i < 6; i++) {
    float spotAngle = i * PI / 3 + animationTime * 0.3;
    int spotX = centerX + cos(spotAngle) * (headSize - 20);
    int spotY = centerY + sin(spotAngle) * (headSize - 25);
    int spotSize = 8 + sin(animationTime * 4 + i) * 3;
    gfx->fillCircle(spotX, spotY, spotSize, BLACK);
  }
  
  // Eyes
  int eyeOffset = 18;
  int eyeY = centerY - 15;
  
  // Eye whites
  gfx->fillCircle(centerX - eyeOffset, eyeY, 12, WHITE);
  gfx->fillCircle(centerX + eyeOffset, eyeY, 12, WHITE);
  gfx->drawCircle(centerX - eyeOffset, eyeY, 12, BLACK);
  gfx->drawCircle(centerX + eyeOffset, eyeY, 12, BLACK);
  
  // Pupils (follow a pattern)
  int pupilOffsetX = (int)(sin(animationTime) * 4);
  int pupilOffsetY = (int)(cos(animationTime * 1.3) * 3);
  gfx->fillCircle(centerX - eyeOffset + pupilOffsetX, eyeY + pupilOffsetY, 6, BLACK);
  gfx->fillCircle(centerX + eyeOffset + pupilOffsetX, eyeY + pupilOffsetY, 6, BLACK);
  
  // Eye shine
  gfx->fillCircle(centerX - eyeOffset + pupilOffsetX - 2, eyeY + pupilOffsetY - 2, 2, WHITE);
  gfx->fillCircle(centerX + eyeOffset + pupilOffsetX - 2, eyeY + pupilOffsetY - 2, 2, WHITE);
  
  // Snout
  int snoutY = centerY + 10;
  gfx->fillRect(centerX - 20, snoutY, 40, 25, gfx->color565(255, 192, 203)); // Pink
  gfx->drawRect(centerX - 20, snoutY, 40, 25, BLACK);
  
  // Nostrils
  gfx->fillCircle(centerX - 8, snoutY + 12, 3, BLACK);
  gfx->fillCircle(centerX + 8, snoutY + 12, 3, BLACK);
  
  // Mouth
  gfx->drawLine(centerX, snoutY + 25, centerX - 15, snoutY + 35, BLACK);
  gfx->drawLine(centerX, snoutY + 25, centerX + 15, snoutY + 35, BLACK);
  
  // Ears
  gfx->fillCircle(centerX - 45, centerY - 35, 15, WHITE);
  gfx->fillCircle(centerX + 45, centerY - 35, 15, WHITE);
  gfx->drawCircle(centerX - 45, centerY - 35, 15, BLACK);
  gfx->drawCircle(centerX + 45, centerY - 35, 15, BLACK);
  
  // Inner ears
  gfx->fillCircle(centerX - 45, centerY - 35, 8, gfx->color565(255, 192, 203));
  gfx->fillCircle(centerX + 45, centerY - 35, 8, gfx->color565(255, 192, 203));
}

void drawCowHide() {
  // Cow hide pattern effect
  gfx->fillScreen(WHITE);
  
  static bool spots[50][4]; // x, y, size, initialized
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 50; i++) {
      spots[i][0] = random(0, gfx->width());
      spots[i][1] = random(0, gfx->height());
      spots[i][2] = random(10, 30);
      spots[i][3] = 1;
    }
    initialized = true;
  }
  
  // Draw cow spots
  for (int i = 0; i < 50; i++) {
    int x = spots[i][0];
    int y = spots[i][1];
    int baseSize = spots[i][2];
    
    // Animated size
    int size = baseSize + sin(animationTime * 2 + i * 0.3) * 5;
    
    // Main spot
    gfx->fillCircle(x, y, size, BLACK);
    
    // Irregular cow spot shape
    for (int j = 0; j < 6; j++) {
      float angle = j * PI / 3 + animationTime * 0.1;
      int blobX = x + cos(angle) * (size * 0.7);
      int blobY = y + sin(angle) * (size * 0.5);
      int blobSize = size / 3 + sin(animationTime * 3 + i + j) * 3;
      gfx->fillCircle(blobX, blobY, blobSize, BLACK);
    }
  }
  
  // Moving spots
  for (int i = 0; i < 10; i++) {
    float moveTime = animationTime * 0.5 + i * 0.8;
    int moveX = (int)(sin(moveTime) * 100 + gfx->width() / 2);
    int moveY = (int)(cos(moveTime * 1.3) * 80 + gfx->height() / 2);
    int moveSize = 15 + sin(animationTime * 4 + i) * 8;
    
    gfx->fillCircle(moveX, moveY, moveSize, gfx->color565(50, 50, 50));
  }
}

void drawCowHoofs() {
  // Cow hoofs walking effect
  gfx->fillScreen(gfx->color565(34, 139, 34)); // Forest green ground
  
  // Draw ground line
  int groundY = gfx->height() - 40;
  gfx->drawFastHLine(0, groundY, gfx->width(), gfx->color565(139, 69, 19));
  
  // Four cow legs walking
  for (int leg = 0; leg < 4; leg++) {
    float legPhase = animationTime * 3 + leg * PI / 2;
    
    // Leg position - walking cycle
    int legX = 60 + leg * 60;
    int legBaseY = groundY - 10;
    
    // Walking motion - lift legs alternately
    int legLift = 0;
    if (leg % 2 == 0) {
      legLift = (int)(sin(legPhase) * 15);
      if (legLift < 0) legLift = 0;
    } else {
      legLift = (int)(sin(legPhase + PI) * 15);
      if (legLift < 0) legLift = 0;
    }
    
    int legY = legBaseY - legLift;
    
    // Draw leg
    gfx->drawLine(legX, legY - 40, legX, legY, BLACK);
    gfx->drawLine(legX - 2, legY - 40, legX - 2, legY, BLACK);
    gfx->drawLine(legX + 2, legY - 40, legX + 2, legY, BLACK);
    
    // Draw hoof
    gfx->fillRect(legX - 8, legY, 16, 12, BLACK);
    gfx->drawRect(legX - 8, legY, 16, 12, gfx->color565(64, 64, 64));
    
    // Hoof split
    gfx->drawFastVLine(legX, legY, 12, gfx->color565(32, 32, 32));
    
    // Dust clouds when hoof hits ground
    if (legLift < 3) {
      for (int dust = 0; dust < 5; dust++) {
        int dustX = legX - 15 + dust * 6 + random(-3, 4);
        int dustY = legY + 10 + random(-2, 5);
        int dustSize = 2 + random(0, 3);
        gfx->fillCircle(dustX, dustY, dustSize, gfx->color565(160, 160, 120));
      }
    }
  }
  
  // Cow body silhouette above
  int bodyY = groundY - 80;
  gfx->fillRect(40, bodyY, 200, 40, WHITE);
  gfx->drawRect(40, bodyY, 200, 40, BLACK);
  
  // Cow spots on body
  for (int i = 0; i < 8; i++) {
    int spotX = 50 + i * 22 + sin(animationTime + i) * 5;
    int spotY = bodyY + 5 + sin(animationTime * 2 + i) * 8;
    int spotSize = 6 + sin(animationTime * 3 + i) * 3;
    gfx->fillCircle(spotX, spotY, spotSize, BLACK);
  }
  
  // Tail swishing
  float tailAngle = sin(animationTime * 4) * 0.5;
  int tailX = 240 + cos(tailAngle) * 20;
  int tailY = bodyY + 20 + sin(tailAngle) * 10;
  gfx->drawLine(240, bodyY + 20, tailX, tailY, BLACK);
  gfx->fillCircle(tailX, tailY, 4, BLACK); // Tail tuft
}

void drawCowHorns() {
  // Cow horns effect - bull with dramatic horns
  gfx->fillScreen(gfx->color565(160, 82, 45)); // Brown background
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Bull head
  int headSize = 55;
  gfx->fillCircle(centerX, centerY, headSize, gfx->color565(139, 69, 19));
  gfx->drawCircle(centerX, centerY, headSize, BLACK);
  
  // Horns - animated length and angle
  float hornAngle = sin(animationTime * 0.5) * 0.3;
  int hornLength = 40 + sin(animationTime * 2) * 8;
  
  // Left horn
  float leftAngle = -2.0 + hornAngle;
  int horn1X1 = centerX - 25;
  int horn1Y1 = centerY - 35;
  int horn1X2 = horn1X1 + cos(leftAngle) * hornLength;
  int horn1Y2 = horn1Y1 + sin(leftAngle) * hornLength;
  
  // Draw horn with thickness
  for (int thickness = 0; thickness < 5; thickness++) {
    gfx->drawLine(horn1X1 + thickness - 2, horn1Y1, 
                  horn1X2 + thickness - 2, horn1Y2, 
                  gfx->color565(240, 240, 200)); // Bone white
  }
  gfx->drawLine(horn1X1, horn1Y1, horn1X2, horn1Y2, BLACK); // Outline
  
  // Right horn
  float rightAngle = -1.14 - hornAngle;
  int horn2X1 = centerX + 25;
  int horn2Y1 = centerY - 35;
  int horn2X2 = horn2X1 + cos(rightAngle) * hornLength;
  int horn2Y2 = horn2Y1 + sin(rightAngle) * hornLength;
  
  // Draw horn with thickness
  for (int thickness = 0; thickness < 5; thickness++) {
    gfx->drawLine(horn2X1 + thickness - 2, horn2Y1, 
                  horn2X2 + thickness - 2, horn2Y2, 
                  gfx->color565(240, 240, 200));
  }
  gfx->drawLine(horn2X1, horn2Y1, horn2X2, horn2Y2, BLACK);
  
  // Horn tips (sharp points)
  gfx->fillCircle(horn1X2, horn1Y2, 3, BLACK);
  gfx->fillCircle(horn2X2, horn2Y2, 3, BLACK);
  
  // Angry eyes
  int eyeY = centerY - 10;
  gfx->fillCircle(centerX - 15, eyeY, 8, RED);
  gfx->fillCircle(centerX + 15, eyeY, 8, RED);
  gfx->fillCircle(centerX - 15, eyeY, 4, BLACK);
  gfx->fillCircle(centerX + 15, eyeY, 4, BLACK);
  
  // Angry eyebrows
  gfx->drawLine(centerX - 25, eyeY - 15, centerX - 8, eyeY - 8, BLACK);
  gfx->drawLine(centerX + 8, eyeY - 8, centerX + 25, eyeY - 15, BLACK);
  
  // Snorting nostrils
  gfx->fillRect(centerX - 20, centerY + 15, 40, 20, BLACK);
  gfx->fillCircle(centerX - 8, centerY + 25, 4, BLACK);
  gfx->fillCircle(centerX + 8, centerY + 25, 4, BLACK);
  
  // Steam from nostrils
  for (int i = 0; i < 6; i++) {
    float steamTime = animationTime * 4 + i * 0.5;
    int steamX = centerX + (i < 3 ? -8 : 8) + sin(steamTime) * 3;
    int steamY = centerY + 25 - i * 8;
    int steamSize = 3 - i / 2;
    if (steamSize > 0) {
      gfx->fillCircle(steamX, steamY, steamSize, gfx->color565(200, 200, 200));
    }
  }
  
  // Horn rings (age indicators)
  for (int ring = 0; ring < 3; ring++) {
    int ringPos = hornLength * (0.3 + ring * 0.2);
    int ring1X = horn1X1 + cos(leftAngle) * ringPos;
    int ring1Y = horn1Y1 + sin(leftAngle) * ringPos;
    int ring2X = horn2X1 + cos(rightAngle) * ringPos;
    int ring2Y = horn2Y1 + sin(rightAngle) * ringPos;
    
    gfx->drawCircle(ring1X, ring1Y, 2, gfx->color565(139, 69, 19));
    gfx->drawCircle(ring2X, ring2Y, 2, gfx->color565(139, 69, 19));
  }
}

void drawCowTail() {
  // Cow tail effect - swishing and swatting flies
  gfx->fillScreen(gfx->color565(34, 139, 34)); // Green pasture
  
  // Cow body (rear view)
  int bodyX = gfx->width() / 2;
  int bodyY = gfx->height() / 2;
  
  gfx->fillRect(bodyX - 60, bodyY - 40, 120, 80, WHITE);
  gfx->drawRect(bodyX - 60, bodyY - 40, 120, 80, BLACK);
  
  // Cow spots
  for (int i = 0; i < 10; i++) {
    int spotX = bodyX - 50 + (i % 4) * 25 + sin(animationTime + i) * 5;
    int spotY = bodyY - 30 + (i / 4) * 20;
    int spotSize = 8 + sin(animationTime * 2 + i) * 3;
    gfx->fillCircle(spotX, spotY, spotSize, BLACK);
  }
  
  // Tail base
  int tailBaseX = bodyX + 60;
  int tailBaseY = bodyY;
  
  // Animated tail swishing
  float tailSwing = sin(animationTime * 3) * 1.2;
  float tailCurve = sin(animationTime * 2) * 0.8;
  
  // Tail segments for curved motion
  int segments = 8;
  int prevX = tailBaseX;
  int prevY = tailBaseY;
  
  for (int seg = 1; seg <= segments; seg++) {
    float segmentRatio = (float)seg / segments;
    float angle = tailSwing * segmentRatio + tailCurve * sin(segmentRatio * PI);
    
    int segLength = 15;
    int segX = prevX + cos(angle) * segLength;
    int segY = prevY + sin(angle) * segLength;
    
    // Tail thickness decreases toward tip
    int thickness = 6 - seg;
    if (thickness < 1) thickness = 1;
    
    for (int t = 0; t < thickness; t++) {
      gfx->drawLine(prevX + t - thickness/2, prevY, 
                    segX + t - thickness/2, segY, 
                    gfx->color565(139, 69, 19));
    }
    
    prevX = segX;
    prevY = segY;
  }
  
  // Tail tuft at the end
  int tuftSize = 8 + sin(animationTime * 4) * 3;
  gfx->fillCircle(prevX, prevY, tuftSize, BLACK);
  
  // Flies being swatted
  for (int fly = 0; fly < 6; fly++) {
    float flyTime = animationTime * 6 + fly * 1.0;
    
    // Flies try to land on cow, tail swats them away
    float flyDistance = 80 + sin(flyTime) * 40;
    float flyAngle = fly * PI / 3 + sin(flyTime * 0.7) * 0.5;
    
    int flyX = bodyX + cos(flyAngle) * flyDistance;
    int flyY = bodyY + sin(flyAngle) * flyDistance;
    
    // Check if tail is near fly (swatting)
    float distToTail = sqrt((flyX - prevX) * (flyX - prevX) + (flyY - prevY) * (flyY - prevY));
    if (distToTail < 20) {
      // Fly gets swatted - show motion lines
      for (int line = 0; line < 3; line++) {
        int lineX = flyX + cos(tailSwing + line * 0.5) * (10 + line * 5);
        int lineY = flyY + sin(tailSwing + line * 0.5) * (10 + line * 5);
        gfx->drawLine(flyX, flyY, lineX, lineY, YELLOW);
      }
    } else {
      // Normal fly
      gfx->fillCircle(flyX, flyY, 2, BLACK);
      // Fly wings
      gfx->drawLine(flyX - 3, flyY - 1, flyX - 1, flyY - 1, gfx->color565(200, 200, 255));
      gfx->drawLine(flyX + 1, flyY - 1, flyX + 3, flyY - 1, gfx->color565(200, 200, 255));
    }
  }
}

void drawCowUdder() {
  // Cow udder effect - dairy cow with milk production
  gfx->fillScreen(gfx->color565(135, 206, 235)); // Sky blue
  
  // Draw barn in background
  gfx->fillRect(10, 50, gfx->width() - 20, gfx->height() - 100, gfx->color565(139, 69, 19));
  gfx->fillTriangle(10, 50, gfx->width()/2, 20, gfx->width() - 10, 50, RED);
  
  // Cow body (side view)
  int cowX = gfx->width() / 2;
  int cowY = gfx->height() - 80;
  
  // Body
  gfx->fillRect(cowX - 50, cowY - 40, 100, 40, WHITE);
  gfx->drawRect(cowX - 50, cowY - 40, 100, 40, BLACK);
  
  // Head
  gfx->fillCircle(cowX - 60, cowY - 25, 20, WHITE);
  gfx->drawCircle(cowX - 60, cowY - 25, 20, BLACK);
  
  // Eye
  gfx->fillCircle(cowX - 65, cowY - 30, 4, BLACK);
  gfx->fillCircle(cowX - 66, cowY - 32, 1, WHITE);
  
  // Legs
  for (int leg = 0; leg < 4; leg++) {
    int legX = cowX - 35 + leg * 20;
    gfx->drawLine(legX, cowY, legX, cowY + 25, BLACK);
    gfx->fillRect(legX - 3, cowY + 20, 6, 8, BLACK);
  }
  
  // Udder (main feature)
  int udderX = cowX - 10;
  int udderY = cowY - 5;
  
  // Main udder sac
  gfx->fillCircle(udderX, udderY, 25, gfx->color565(255, 192, 203)); // Pink
  gfx->drawCircle(udderX, udderY, 25, BLACK);
  
  // Four teats
  for (int teat = 0; teat < 4; teat++) {
    int teatX = udderX - 15 + teat * 10;
    int teatY = udderY + 20;
    
    // Teat
    gfx->fillRect(teatX - 2, teatY, 4, 12, gfx->color565(255, 182, 193));
    gfx->drawRect(teatX - 2, teatY, 4, 12, BLACK);
    
    // Milk drops (animated)
    if (sin(animationTime * 4 + teat) > 0.5) {
      for (int drop = 0; drop < 3; drop++) {
        int dropY = teatY + 15 + drop * 8 + (int)(animationTime * 30 + teat * 10) % 20;
        if (dropY < gfx->height() - 30) {
          gfx->fillCircle(teatX, dropY, 2, WHITE);
        }
      }
    }
  }
  
  // Milk bucket
  int bucketX = udderX;
  int bucketY = gfx->height() - 25;
  gfx->fillRect(bucketX - 15, bucketY, 30, 20, gfx->color565(150, 150, 150));
  gfx->drawRect(bucketX - 15, bucketY, 30, 20, BLACK);
  
  // Milk level in bucket (rises over time)
  int milkLevel = (int)(sin(animationTime * 0.5) * 8 + 10);
  gfx->fillRect(bucketX - 13, bucketY + 18 - milkLevel, 26, milkLevel, WHITE);
  
  // Bucket handle
  gfx->drawCircle(bucketX + 20, bucketY + 10, 8, BLACK);
  
  // Farmer's hand occasionally
  if (sin(animationTime * 0.3) > 0.7) {
    gfx->fillCircle(bucketX - 25, bucketY - 10, 8, gfx->color565(255, 219, 172));
    gfx->drawCircle(bucketX - 25, bucketY - 10, 8, BLACK);
    // Milking motion lines
    gfx->drawLine(bucketX - 25, bucketY - 10, udderX, udderY + 15, gfx->color565(200, 200, 200));
  }
  
  // Cow spots
  for (int spot = 0; spot < 6; spot++) {
    int spotX = cowX - 40 + spot * 15;
    int spotY = cowY - 35 + sin(animationTime + spot) * 8;
    gfx->fillCircle(spotX, spotY, 5 + spot % 3, BLACK);
  }
  
  // Tail
  float tailAngle = sin(animationTime * 2) * 0.3;
  int tailX = cowX + 50 + cos(tailAngle) * 15;
  int tailY = cowY - 20 + sin(tailAngle) * 8;
  gfx->drawLine(cowX + 50, cowY - 20, tailX, tailY, BLACK);
  gfx->fillCircle(tailX, tailY, 3, BLACK);
}

void drawCrackberg() {
  // Crackberg effect - cracked surface with growing fractures
  gfx->fillScreen(gfx->color565(80, 80, 80)); // Gray surface
  
  static bool cracks[60][40];
  static bool initialized = false;
  static int generation = 0;
  
  if (!initialized || generation > 200) {
    // Initialize with some initial cracks
    for (int x = 0; x < 60; x++) {
      for (int y = 0; y < 40; y++) {
        cracks[x][y] = false;
      }
    }
    // Create initial crack seeds
    for (int i = 0; i < 5; i++) {
      int x = random(10, 50);
      int y = random(10, 30);
      cracks[x][y] = true;
    }
    initialized = true;
    generation = 0;
  }
  
  // Crack propagation (every 30 frames)
  if ((int)(animationTime * 10) % 30 == 0) {
    bool newCracks[60][40];
    for (int x = 0; x < 60; x++) {
      for (int y = 0; y < 40; y++) {
        newCracks[x][y] = cracks[x][y];
      }
    }
    
    for (int x = 1; x < 59; x++) {
      for (int y = 1; y < 39; y++) {
        if (!cracks[x][y]) {
          // Check for adjacent cracks
          int adjacent = 0;
          if (cracks[x-1][y] || cracks[x+1][y] || cracks[x][y-1] || cracks[x][y+1]) adjacent++;
          
          // Propagate crack based on stress
          if (adjacent > 0 && random(0, 8) == 0) {
            newCracks[x][y] = true;
          }
        }
      }
    }
    
    for (int x = 0; x < 60; x++) {
      for (int y = 0; y < 40; y++) {
        cracks[x][y] = newCracks[x][y];
      }
    }
    generation++;
  }
  
  // Draw cracks
  for (int x = 0; x < 60; x++) {
    for (int y = 0; y < 40; y++) {
      if (cracks[x][y]) {
        int screenX = x * (gfx->width() / 60);
        int screenY = y * (gfx->height() / 40);
        
        // Crack depth based on age
        uint16_t crackColor = BLACK;
        gfx->fillRect(screenX, screenY, 
                      gfx->width() / 60 + 1, gfx->height() / 40 + 1, 
                      crackColor);
        
        // Add some red glow for deep cracks
        if (random(0, 4) == 0) {
          gfx->fillRect(screenX, screenY, 
                        gfx->width() / 60 + 1, gfx->height() / 40 + 1, 
                        gfx->color565(100, 0, 0));
        }
      }
    }
  }
  
  // Stress lines radiating from crack centers
  for (int i = 0; i < 8; i++) {
    float stressAngle = i * PI / 4 + animationTime * 0.5;
    int centerX = gfx->width() / 2;
    int centerY = gfx->height() / 2;
    
    int x1 = centerX + cos(stressAngle) * 20;
    int y1 = centerY + sin(stressAngle) * 20;
    int x2 = centerX + cos(stressAngle) * 80;
    int y2 = centerY + sin(stressAngle) * 80;
    
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(60, 60, 60));
  }
}

void drawCritical() {
  // Critical system warning effect
  gfx->fillScreen(BLACK);
  
  // Flashing red background for urgency
  bool flash = (int)(animationTime * 8) % 2 == 0;
  if (flash) {
    gfx->fillScreen(gfx->color565(100, 0, 0));
  }
  
  // Warning text
  gfx->setTextColor(flash ? WHITE : RED);
  gfx->setTextSize(2);
  
  int textY = 20;
  gfx->setCursor(20, textY);
  gfx->printf("CRITICAL");
  
  textY += 25;
  gfx->setTextSize(1);
  gfx->setCursor(10, textY);
  gfx->printf("SYSTEM FAILURE IMMINENT");
  
  textY += 15;
  gfx->setCursor(10, textY);
  gfx->printf("Temperature: %d°C", 150 + (int)(sin(animationTime * 4) * 50));
  
  textY += 15;
  gfx->setCursor(10, textY);
  gfx->printf("Memory: %d%% FULL", 95 + (int)(sin(animationTime * 2) * 5));
  
  textY += 15;
  gfx->setCursor(10, textY);
  gfx->printf("CPU Load: %d%%", 98 + (int)(sin(animationTime * 6) * 2));
  
  // Warning symbols
  for (int i = 0; i < 6; i++) {
    float symbolTime = animationTime * 4 + i * 1.0;
    int symbolX = 20 + i * 40;
    int symbolY = gfx->height() - 40;
    
    if (sin(symbolTime) > 0) {
      // Warning triangle
      gfx->fillTriangle(symbolX, symbolY - 15, 
                        symbolX - 10, symbolY + 5,
                        symbolX + 10, symbolY + 5, 
                        YELLOW);
      gfx->drawTriangle(symbolX, symbolY - 15, 
                        symbolX - 10, symbolY + 5,
                        symbolX + 10, symbolY + 5, 
                        BLACK);
      
      // Exclamation mark
      gfx->drawFastVLine(symbolX, symbolY - 10, 8, BLACK);
      gfx->fillCircle(symbolX, symbolY + 2, 1, BLACK);
    }
  }
  
  // System graphs showing critical levels
  int graphX = gfx->width() - 80;
  int graphY = 80;
  int graphW = 60;
  int graphH = 40;
  
  gfx->drawRect(graphX, graphY, graphW, graphH, WHITE);
  
  // Critical level line
  int criticalY = graphY + graphH - 35;
  gfx->drawFastHLine(graphX, criticalY, graphW, RED);
  
  // Animated graph data showing critical values
  for (int x = 0; x < graphW - 5; x += 3) {
    float dataPoint = sin(animationTime * 2 + x * 0.1) * 0.3 + 0.9; // Mostly above critical
    int barHeight = (int)(dataPoint * graphH);
    int barY = graphY + graphH - barHeight;
    
    uint16_t barColor = (barHeight > 35) ? RED : YELLOW;
    gfx->drawFastVLine(graphX + x + 2, barY, barHeight, barColor);
  }
  
  // Error counter
  gfx->setCursor(10, gfx->height() - 15);
  gfx->setTextColor(RED);
  gfx->printf("Errors: %d", (int)(animationTime * 3) % 9999);
}

void drawCrumbler() {
  // Crumbler effect - structures crumbling and falling apart
  gfx->fillScreen(gfx->color565(135, 206, 235)); // Sky blue
  
  static int blocks[40][4]; // x, y, vy, type
  static bool initialized = false;
  
  if (!initialized) {
    // Initialize building blocks
    for (int i = 0; i < 40; i++) {
      blocks[i][0] = random(0, gfx->width());
      blocks[i][1] = random(50, gfx->height() - 100);
      blocks[i][2] = 0; // velocity
      blocks[i][3] = random(0, 3); // block type
    }
    initialized = true;
  }
  
  // Draw ground
  gfx->fillRect(0, gfx->height() - 20, gfx->width(), 20, gfx->color565(34, 139, 34));
  
  // Update and draw falling blocks
  for (int i = 0; i < 40; i++) {
    // Apply gravity to some blocks
    if (sin(animationTime * 0.5 + i * 0.1) > 0.3) {
      blocks[i][2] += 1; // gravity
      blocks[i][1] += blocks[i][2];
    }
    
    // Reset if block hits ground
    if (blocks[i][1] > gfx->height() - 30) {
      blocks[i][1] = random(20, 60);
      blocks[i][0] = random(0, gfx->width());
      blocks[i][2] = 0;
      
      // Dust cloud when hitting ground
      for (int dust = 0; dust < 5; dust++) {
        int dustX = blocks[i][0] + random(-10, 11);
        int dustY = gfx->height() - 25 + random(-5, 6);
        gfx->fillCircle(dustX, dustY, 2, gfx->color565(160, 160, 120));
      }
    }
    
    // Draw block based on type
    int blockX = blocks[i][0];
    int blockY = blocks[i][1];
    
    switch (blocks[i][3]) {
      case 0: // Brick
        gfx->fillRect(blockX, blockY, 20, 10, gfx->color565(139, 69, 19));
        gfx->drawRect(blockX, blockY, 20, 10, BLACK);
        break;
      case 1: // Concrete
        gfx->fillRect(blockX, blockY, 15, 15, gfx->color565(128, 128, 128));
        gfx->drawRect(blockX, blockY, 15, 15, BLACK);
        break;
      case 2: // Glass
        gfx->fillRect(blockX, blockY, 12, 18, gfx->color565(173, 216, 230));
        gfx->drawRect(blockX, blockY, 12, 18, BLACK);
        // Glass cracks
        gfx->drawLine(blockX + 2, blockY + 2, blockX + 10, blockY + 16, BLACK);
        gfx->drawLine(blockX + 8, blockY + 3, blockX + 4, blockY + 15, BLACK);
        break;
    }
    
    // Add motion blur for fast falling blocks
    if (blocks[i][2] > 5) {
      for (int trail = 1; trail < blocks[i][2] && trail < 8; trail++) {
        int trailY = blockY - trail * 3;
        if (trailY > 0) {
          uint16_t trailColor = gfx->color565(100 - trail * 10, 100 - trail * 10, 100 - trail * 10);
          gfx->fillRect(blockX, trailY, 
                        (blocks[i][3] == 0) ? 20 : (blocks[i][3] == 1) ? 15 : 12, 
                        2, trailColor);
        }
      }
    }
  }
  
  // Structural stress lines
  for (int line = 0; line < 6; line++) {
    float lineTime = animationTime * 2 + line * 0.8;
    int x1 = (int)(sin(lineTime) * 50 + gfx->width() / 2);
    int y1 = 60;
    int x2 = x1 + sin(lineTime * 1.3) * 30;
    int y2 = gfx->height() - 30;
    
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(200, 100, 100));
  }
  
  // Explosion effects occasionally
  if (sin(animationTime * 0.7) > 0.8) {
    int explodeX = random(50, gfx->width() - 50);
    int explodeY = random(60, gfx->height() - 60);
    
    for (int spark = 0; spark < 8; spark++) {
      float sparkAngle = spark * PI / 4 + animationTime * 10;
      int sparkX = explodeX + cos(sparkAngle) * 15;
      int sparkY = explodeY + sin(sparkAngle) * 15;
      gfx->fillCircle(sparkX, sparkY, 3, YELLOW);
    }
    gfx->fillCircle(explodeX, explodeY, 8, RED);
  }
}

void drawCrystal() {
  // Crystal effect - growing crystalline structures
  gfx->fillScreen(gfx->color565(0, 0, 30)); // Dark background
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Main crystal formation
  for (int layer = 0; layer < 6; layer++) {
    float layerAngle = animationTime * 0.3 + layer * PI / 3;
    int layerRadius = 20 + layer * 15;
    
    for (int crystal = 0; crystal < 6; crystal++) {
      float angle = layerAngle + crystal * PI / 3;
      int x = centerX + cos(angle) * layerRadius;
      int y = centerY + sin(angle) * layerRadius;
      
      // Crystal size varies with time
      int crystalSize = 8 + sin(animationTime * 2 + layer + crystal) * 4;
      
      // Crystal color - prismatic effect
      float hue = (layer + crystal + animationTime * 0.5) / 6.0;
      int r = (int)(sin(hue * 2 * PI) * 127 + 128);
      int g = (int)(sin(hue * 2 * PI + 2 * PI / 3) * 127 + 128);
      int b = (int)(sin(hue * 2 * PI + 4 * PI / 3) * 127 + 128);
      
      uint16_t crystalColor = gfx->color565(r, g, b);
      
      // Draw hexagonal crystal
      for (int side = 0; side < 6; side++) {
        float sideAngle = angle + side * PI / 3;
        int x1 = x + cos(sideAngle) * crystalSize;
        int y1 = y + sin(sideAngle) * crystalSize;
        int x2 = x + cos(sideAngle + PI / 3) * crystalSize;
        int y2 = y + sin(sideAngle + PI / 3) * crystalSize;
        
        gfx->drawLine(x1, y1, x2, y2, crystalColor);
        gfx->drawLine(x, y, x1, y1, crystalColor);
      }
      
      // Crystal center
      gfx->fillCircle(x, y, 2, WHITE);
    }
  }
  
  // Floating crystal particles
  for (int i = 0; i < 15; i++) {
    float particleTime = animationTime * 1.5 + i * 0.4;
    int pX = centerX + sin(particleTime) * 80 + cos(particleTime * 0.7) * 40;
    int pY = centerY + cos(particleTime) * 60 + sin(particleTime * 1.3) * 30;
    
    int sparkle = (int)(sin(particleTime * 8) * 3 + 4);
    uint16_t sparkleColor = gfx->color565(200 + sparkle * 10, 150 + sparkle * 15, 255);
    gfx->fillCircle(pX, pY, sparkle, sparkleColor);
  }
}

void drawCube21() {
  // Cube21 effect - 3D rotating cubes in formation
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw 21 cubes in a pattern
  for (int cube = 0; cube < 21; cube++) {
    float cubeAngle = cube * 2 * PI / 21 + animationTime;
    float radius = 60 + sin(animationTime * 2 + cube * 0.3) * 20;
    
    int cubeX = centerX + cos(cubeAngle) * radius;
    int cubeY = centerY + sin(cubeAngle) * radius;
    
    // 3D cube rotation
    float rotX = animationTime * 1.5 + cube * 0.2;
    float rotY = animationTime * 1.2 + cube * 0.15;
    
    // Cube size varies
    int cubeSize = 8 + sin(animationTime * 3 + cube) * 4;
    
    // Cube color based on position
    uint16_t cubeColor = gfx->color565(
      128 + sin(cubeAngle) * 127,
      128 + cos(cubeAngle + 1) * 127,
      128 + sin(cubeAngle + 2) * 127
    );
    
    // Draw 3D cube wireframe
    // Front face
    gfx->drawRect(cubeX - cubeSize, cubeY - cubeSize, cubeSize * 2, cubeSize * 2, cubeColor);
    
    // Back face offset for 3D effect
    int offsetX = (int)(cos(rotX) * 6);
    int offsetY = (int)(sin(rotY) * 6);
    gfx->drawRect(cubeX - cubeSize + offsetX, cubeY - cubeSize + offsetY, 
                  cubeSize * 2, cubeSize * 2, cubeColor);
    
    // Connect front and back faces
    gfx->drawLine(cubeX - cubeSize, cubeY - cubeSize, 
                  cubeX - cubeSize + offsetX, cubeY - cubeSize + offsetY, cubeColor);
    gfx->drawLine(cubeX + cubeSize, cubeY - cubeSize, 
                  cubeX + cubeSize + offsetX, cubeY - cubeSize + offsetY, cubeColor);
    gfx->drawLine(cubeX - cubeSize, cubeY + cubeSize, 
                  cubeX - cubeSize + offsetX, cubeY + cubeSize + offsetY, cubeColor);
    gfx->drawLine(cubeX + cubeSize, cubeY + cubeSize, 
                  cubeX + cubeSize + offsetX, cubeY + cubeSize + offsetY, cubeColor);
  }
  
  // Central connecting lines
  for (int line = 0; line < 21; line++) {
    float lineAngle = line * 2 * PI / 21 + animationTime;
    float radius = 60 + sin(animationTime * 2 + line * 0.3) * 20;
    
    int lineX = centerX + cos(lineAngle) * radius;
    int lineY = centerY + sin(lineAngle) * radius;
    
    if (line % 3 == 0) {
      gfx->drawLine(centerX, centerY, lineX, lineY, 
                    gfx->color565(100, 100, 200));
    }
  }
}

void drawCubenetic() {
  // Cubenetic effect - genetic algorithm visualization with cubes
  gfx->fillScreen(gfx->color565(20, 40, 20)); // Dark green background
  
  static int population[16][4]; // x, y, fitness, generation
  static bool initialized = false;
  static int generation = 0;
  
  if (!initialized) {
    // Initialize random population
    for (int i = 0; i < 16; i++) {
      population[i][0] = random(20, gfx->width() - 20);
      population[i][1] = random(20, gfx->height() - 20);
      population[i][2] = random(0, 100); // fitness
      population[i][3] = 0; // generation
    }
    initialized = true;
  }
  
  // Evolution step every 60 frames
  if ((int)(animationTime * 10) % 60 == 0) {
    // Selection and mutation
    for (int i = 0; i < 16; i++) {
      // Calculate fitness based on position (closer to center = higher fitness)
      int dx = population[i][0] - gfx->width() / 2;
      int dy = population[i][1] - gfx->height() / 2;
      population[i][2] = 100 - (int)(sqrt(dx*dx + dy*dy) * 0.3);
      
      // Mutate position towards center for high fitness individuals
      if (population[i][2] > 50) {
        if (population[i][0] > gfx->width() / 2) population[i][0] -= random(1, 4);
        else population[i][0] += random(1, 4);
        
        if (population[i][1] > gfx->height() / 2) population[i][1] -= random(1, 4);
        else population[i][1] += random(1, 4);
      } else {
        // Random mutation for low fitness
        population[i][0] += random(-5, 6);
        population[i][1] += random(-5, 6);
        
        // Keep in bounds
        population[i][0] = constrain(population[i][0], 10, gfx->width() - 10);
        population[i][1] = constrain(population[i][1], 10, gfx->height() - 10);
      }
      
      population[i][3] = generation; // Update generation
    }
    generation++;
  }
  
  // Draw population as evolving cubes
  for (int i = 0; i < 16; i++) {
    int cubeX = population[i][0];
    int cubeY = population[i][1];
    int fitness = population[i][2];
    
    // Cube size based on fitness
    int cubeSize = 4 + fitness / 10;
    
    // Color based on fitness (green = good, red = bad)
    uint16_t cubeColor;
    if (fitness > 70) {
      cubeColor = gfx->color565(0, 255, 0); // Green - high fitness
    } else if (fitness > 40) {
      cubeColor = gfx->color565(255, 255, 0); // Yellow - medium fitness
    } else {
      cubeColor = gfx->color565(255, 0, 0); // Red - low fitness
    }
    
    // Draw cube
    gfx->fillRect(cubeX - cubeSize, cubeY - cubeSize, 
                  cubeSize * 2, cubeSize * 2, cubeColor);
    gfx->drawRect(cubeX - cubeSize, cubeY - cubeSize, 
                  cubeSize * 2, cubeSize * 2, WHITE);
    
    // Fitness indicator
    gfx->fillCircle(cubeX, cubeY, fitness / 20, cubeColor);
    
    // DNA connections to nearby individuals
    for (int j = i + 1; j < 16; j++) {
      int dx = abs(population[i][0] - population[j][0]);
      int dy = abs(population[i][1] - population[j][1]);
      int distance = dx + dy;
      
      if (distance < 50) {
        uint16_t connectionColor = gfx->color565(0, 100, 255);
        gfx->drawLine(population[i][0], population[i][1],
                      population[j][0], population[j][1], connectionColor);
      }
    }
  }
  
  // Target area (evolution goal)
  int targetX = gfx->width() / 2;
  int targetY = gfx->height() / 2;
  gfx->drawCircle(targetX, targetY, 30, WHITE);
  gfx->drawCircle(targetX, targetY, 25, WHITE);
  
  // Generation counter
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(5, 5);
  gfx->printf("Gen: %d", generation);
  
  // Best fitness
  int bestFitness = 0;
  for (int i = 0; i < 16; i++) {
    if (population[i][2] > bestFitness) bestFitness = population[i][2];
  }
  gfx->setCursor(5, 15);
  gfx->printf("Best: %d", bestFitness);
}

void drawCubeStack() {
  // Cubestack effect - stacking cubes building upward
  gfx->fillScreen(gfx->color565(40, 40, 80)); // Dark blue background
  
  static int stacks[8][10]; // 8 stacks, up to 10 cubes each
  static bool initialized = false;
  
  if (!initialized) {
    for (int stack = 0; stack < 8; stack++) {
      for (int cube = 0; cube < 10; cube++) {
        stacks[stack][cube] = 0; // 0 = empty, 1 = cube present
      }
    }
    initialized = true;
  }
  
  // Add cubes to stacks over time
  if ((int)(animationTime * 5) % 20 == 0) {
    int stackIndex = random(0, 8);
    // Find topmost empty position
    for (int cube = 0; cube < 10; cube++) {
      if (stacks[stackIndex][cube] == 0) {
        stacks[stackIndex][cube] = 1;
        break;
      }
    }
    
    // Reset if all stacks are full
    bool allFull = true;
    for (int stack = 0; stack < 8; stack++) {
      if (stacks[stack][9] == 0) allFull = false;
    }
    if (allFull) {
      for (int stack = 0; stack < 8; stack++) {
        for (int cube = 0; cube < 10; cube++) {
          stacks[stack][cube] = 0;
        }
      }
    }
  }
  
  // Draw the stacks
  int baseY = gfx->height() - 20;
  int stackWidth = gfx->width() / 8;
  
  for (int stack = 0; stack < 8; stack++) {
    int stackX = stack * stackWidth + stackWidth / 2;
    
    for (int cube = 0; cube < 10; cube++) {
      if (stacks[stack][cube] == 1) {
        int cubeY = baseY - cube * 18 - 10;
        int cubeSize = 15;
        
        // Cube color based on height and stack
        uint16_t cubeColor = gfx->color565(
          100 + cube * 15 + stack * 10,
          150 - cube * 8,
          200 + stack * 6
        );
        
        // Draw 3D cube
        gfx->fillRect(stackX - cubeSize/2, cubeY - cubeSize/2, 
                      cubeSize, cubeSize, cubeColor);
        gfx->drawRect(stackX - cubeSize/2, cubeY - cubeSize/2, 
                      cubeSize, cubeSize, WHITE);
        
        // 3D effect - top and right faces
        gfx->fillTriangle(stackX - cubeSize/2, cubeY - cubeSize/2,
                          stackX + cubeSize/2, cubeY - cubeSize/2,
                          stackX + cubeSize/2 - 4, cubeY - cubeSize/2 - 4, 
                          gfx->color565((100 + cube * 15 + stack * 10) + 30, 
                                        (150 - cube * 8) + 30, 
                                        (200 + stack * 6) + 30));
        
        gfx->fillTriangle(stackX + cubeSize/2, cubeY - cubeSize/2,
                          stackX + cubeSize/2, cubeY + cubeSize/2,
                          stackX + cubeSize/2 - 4, cubeY + cubeSize/2 - 4,
                          gfx->color565((100 + cube * 15 + stack * 10) - 30, 
                                        (150 - cube * 8) - 30, 
                                        (200 + stack * 6) - 30));
      }
    }
  }
  
  // Draw base platform
  gfx->fillRect(0, baseY, gfx->width(), 20, gfx->color565(100, 100, 100));
  gfx->drawRect(0, baseY, gfx->width(), 20, WHITE);
}

void drawCubeStorm() {
  // Cubestorm effect - cubes flying in storm pattern
  gfx->fillScreen(gfx->color565(20, 20, 40)); // Dark stormy background
  
  static float cubes[25][6]; // x, y, vx, vy, rotation, size
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 25; i++) {
      cubes[i][0] = random(0, gfx->width());   // x
      cubes[i][1] = random(0, gfx->height());  // y
      cubes[i][2] = random(-3, 4);             // vx
      cubes[i][3] = random(-3, 4);             // vy
      cubes[i][4] = 0;                         // rotation
      cubes[i][5] = random(8, 16);             // size
    }
    initialized = true;
  }
  
  // Storm wind force
  float windX = sin(animationTime * 0.5) * 2;
  float windY = cos(animationTime * 0.3) * 1.5;
  
  // Update cube positions and draw them
  for (int i = 0; i < 25; i++) {
    // Apply wind force
    cubes[i][2] += windX * 0.1;
    cubes[i][3] += windY * 0.1;
    
    // Apply turbulence
    cubes[i][2] += sin(animationTime * 4 + i) * 0.3;
    cubes[i][3] += cos(animationTime * 3 + i) * 0.3;
    
    // Update position
    cubes[i][0] += cubes[i][2];
    cubes[i][1] += cubes[i][3];
    
    // Update rotation
    cubes[i][4] += 0.1;
    
    // Wrap around screen
    if (cubes[i][0] < -20) cubes[i][0] = gfx->width() + 20;
    if (cubes[i][0] > gfx->width() + 20) cubes[i][0] = -20;
    if (cubes[i][1] < -20) cubes[i][1] = gfx->height() + 20;
    if (cubes[i][1] > gfx->height() + 20) cubes[i][1] = -20;
    
    // Draw rotating cube
    int cubeX = (int)cubes[i][0];
    int cubeY = (int)cubes[i][1];
    int cubeSize = (int)cubes[i][5];
    float rotation = cubes[i][4];
    
    // Cube color based on speed
    float speed = sqrt(cubes[i][2]*cubes[i][2] + cubes[i][3]*cubes[i][3]);
    uint16_t cubeColor = gfx->color565(
      (int)(100 + speed * 20),
      (int)(150 + sin(rotation) * 50),
      (int)(200 + cos(rotation) * 30)
    );
    
    // Draw rotated cube (simplified)
    int offset = (int)(sin(rotation) * 5);
    
    // Main cube
    gfx->fillRect(cubeX - cubeSize/2, cubeY - cubeSize/2, 
                  cubeSize, cubeSize, cubeColor);
    gfx->drawRect(cubeX - cubeSize/2, cubeY - cubeSize/2, 
                  cubeSize, cubeSize, WHITE);
    
    // Rotation effect - offset back face
    gfx->drawRect(cubeX - cubeSize/2 + offset, cubeY - cubeSize/2 + offset, 
                  cubeSize, cubeSize, gfx->color565(150, 150, 150));
    
    // Connect faces for 3D effect
    gfx->drawLine(cubeX - cubeSize/2, cubeY - cubeSize/2,
                  cubeX - cubeSize/2 + offset, cubeY - cubeSize/2 + offset, WHITE);
    gfx->drawLine(cubeX + cubeSize/2, cubeY - cubeSize/2,
                  cubeX + cubeSize/2 + offset, cubeY - cubeSize/2 + offset, WHITE);
    
    // Motion blur for fast moving cubes
    if (speed > 4) {
      for (int trail = 1; trail < 4; trail++) {
        int trailX = cubeX - (int)(cubes[i][2] * trail * 2);
        int trailY = cubeY - (int)(cubes[i][3] * trail * 2);
        uint16_t trailColor = gfx->color565(50 - trail * 10, 50 - trail * 10, 100 - trail * 20);
        gfx->fillRect(trailX - cubeSize/4, trailY - cubeSize/4, 
                      cubeSize/2, cubeSize/2, trailColor);
      }
    }
  }
  
  // Lightning effects occasionally
  if (random(0, 100) < 2) {
    int lightningX = random(0, gfx->width());
    for (int i = 0; i < gfx->height(); i += 8) {
      lightningX += random(-5, 6);
      lightningX = constrain(lightningX, 0, gfx->width()-1);
      gfx->drawFastVLine(lightningX, i, 6, WHITE);
    }
  }
}

void drawCubeTwist() {
  // Cubetwist effect - cubes twisting in spiral pattern
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw twisted cube formation
  for (int ring = 0; ring < 5; ring++) {
    float ringRadius = 20 + ring * 20;
    int cubesInRing = 6 + ring * 2;
    
    for (int cube = 0; cube < cubesInRing; cube++) {
      // Base angle for this cube
      float baseAngle = (cube * 2 * PI) / cubesInRing;
      
      // Twist effect - inner rings rotate faster
      float twistRate = (5 - ring) * 0.3;
      float twistAngle = animationTime * twistRate + baseAngle;
      
      // Spiral twist - each ring offset vertically
      float spiralOffset = sin(animationTime * 2 + ring * 0.5) * 15;
      
      // Calculate position
      int cubeX = centerX + cos(twistAngle) * ringRadius;
      int cubeY = centerY + sin(twistAngle) * ringRadius + spiralOffset;
      
      // Cube size decreases with distance from center
      int cubeSize = 12 - ring * 2;
      if (cubeSize < 4) cubeSize = 4;
      
      // Color based on twist angle and ring
      uint16_t cubeColor = gfx->color565(
        (int)(sin(twistAngle) * 127 + 128),
        (int)(cos(twistAngle + ring) * 127 + 128),
        255 - ring * 40
      );
      
      // 3D rotation based on twist
      float rotX = twistAngle * 2;
      float rotY = animationTime + ring * 0.3;
      
      // Draw 3D cube with perspective
      int depth = (int)(cos(rotY) * 8);
      
      // Front face
      gfx->fillRect(cubeX - cubeSize/2, cubeY - cubeSize/2, 
                    cubeSize, cubeSize, cubeColor);
      gfx->drawRect(cubeX - cubeSize/2, cubeY - cubeSize/2, 
                    cubeSize, cubeSize, WHITE);
      
      // Back face (offset for 3D effect)
      int backX = cubeX + depth;
      int backY = cubeY + depth;
      gfx->drawRect(backX - cubeSize/2, backY - cubeSize/2, 
                    cubeSize, cubeSize, 
                    gfx->color565(100, 100, 150));
      
      // Connect corners for 3D wireframe
      gfx->drawLine(cubeX - cubeSize/2, cubeY - cubeSize/2,
                    backX - cubeSize/2, backY - cubeSize/2, WHITE);
      gfx->drawLine(cubeX + cubeSize/2, cubeY - cubeSize/2,
                    backX + cubeSize/2, backY - cubeSize/2, WHITE);
      gfx->drawLine(cubeX - cubeSize/2, cubeY + cubeSize/2,
                    backX - cubeSize/2, backY + cubeSize/2, WHITE);
      gfx->drawLine(cubeX + cubeSize/2, cubeY + cubeSize/2,
                    backX + cubeSize/2, backY + cubeSize/2, WHITE);
    }
  }
  
  // Central twist vortex
  for (int i = 0; i < 8; i++) {
    float vortexAngle = animationTime * 4 + i * PI / 4;
    int vortexRadius = (int)(sin(animationTime * 3 + i) * 15 + 25);
    
    int vortexX = centerX + cos(vortexAngle) * vortexRadius;
    int vortexY = centerY + sin(vortexAngle) * vortexRadius;
    
    gfx->drawLine(centerX, centerY, vortexX, vortexY, 
                  gfx->color565(255, 200 - i * 20, 100 + i * 15));
  }
  
  // Center point
  gfx->fillCircle(centerX, centerY, 5, WHITE);
}

void drawCubicGrid() {
  // Cubic grid effect - 3D grid of rotating cubes
  gfx->fillScreen(gfx->color565(10, 10, 30)); // Dark background
  
  int gridSize = 6;
  int spacing = gfx->width() / (gridSize + 1);
  
  for (int x = 0; x < gridSize; x++) {
    for (int y = 0; y < gridSize; y++) {
      int cubeX = (x + 1) * spacing;
      int cubeY = (y + 1) * spacing;
      
      // Individual rotation for each cube
      float rotationOffset = (x + y) * 0.5;
      float rotation = animationTime * 2 + rotationOffset;
      
      // Cube size varies with rotation
      int cubeSize = 12 + sin(rotation) * 4;
      
      // Color based on position and rotation
      uint16_t cubeColor = gfx->color565(
        (int)(sin(rotation + x * 0.5) * 127 + 128),
        (int)(cos(rotation + y * 0.5) * 127 + 128),
        (int)(sin(rotation + x + y) * 127 + 128)
      );
      
      // 3D cube with rotation effect
      int depth = (int)(cos(rotation * 1.5) * 6);
      
      // Front face
      gfx->fillRect(cubeX - cubeSize/2, cubeY - cubeSize/2, 
                    cubeSize, cubeSize, cubeColor);
      gfx->drawRect(cubeX - cubeSize/2, cubeY - cubeSize/2, 
                    cubeSize, cubeSize, WHITE);
      
      // Back face
      int backX = cubeX + depth;
      int backY = cubeY + depth;
      gfx->drawRect(backX - cubeSize/2, backY - cubeSize/2, 
                    cubeSize, cubeSize, 
                    gfx->color565(100, 100, 150));
      
      // Connecting lines for 3D effect
      if (abs(depth) > 2) {
        gfx->drawLine(cubeX - cubeSize/2, cubeY - cubeSize/2,
                      backX - cubeSize/2, backY - cubeSize/2, WHITE);
        gfx->drawLine(cubeX + cubeSize/2, cubeY - cubeSize/2,
                      backX + cubeSize/2, backY - cubeSize/2, WHITE);
      }
      
      // Grid connections
      if (x < gridSize - 1) {
        int nextCubeX = (x + 2) * spacing;
        gfx->drawLine(cubeX + cubeSize/2, cubeY, 
                      nextCubeX - cubeSize/2, cubeY, 
                      gfx->color565(50, 50, 100));
      }
      if (y < gridSize - 1) {
        int nextCubeY = (y + 2) * spacing;
        gfx->drawLine(cubeX, cubeY + cubeSize/2, 
                      cubeX, nextCubeY - cubeSize/2, 
                      gfx->color565(50, 50, 100));
      }
    }
  }
}

void drawCuboctaversion() {
  // Cuboctaversion effect - morphing between cube and octahedron
  gfx->fillScreen(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Morphing parameter between 0 and 1
  float morphParam = sin(animationTime * 0.8) * 0.5 + 0.5;
  
  // Draw multiple cuboctahedra at different scales
  for (int scale = 1; scale <= 3; scale++) {
    float radius = 30 * scale + sin(animationTime * 1.5 + scale) * 10;
    
    // 8 vertices of cuboctahedron
    float vertices[8][2];
    
    for (int i = 0; i < 8; i++) {
      float angle = i * PI / 4 + animationTime * (0.5 + scale * 0.2);
      
      // Interpolate between octagon (cube projection) and diamond (octahedron)
      float cubeRadius = radius;
      float octRadius = radius * 0.7;
      float currentRadius = cubeRadius * (1 - morphParam) + octRadius * morphParam;
      
      // Vertex distortion based on morph parameter
      float distortion = 1 + sin(angle * 4 + animationTime * 3) * morphParam * 0.3;
      currentRadius *= distortion;
      
      vertices[i][0] = centerX + cos(angle) * currentRadius;
      vertices[i][1] = centerY + sin(angle) * currentRadius;
    }
    
    // Color based on scale and morph
    uint16_t shapeColor = gfx->color565(
      (int)(100 + morphParam * 155),
      (int)(150 - morphParam * 50 + scale * 20),
      (int)(200 - morphParam * 100 + scale * 15)
    );
    
    // Draw edges
    for (int i = 0; i < 8; i++) {
      int next = (i + 1) % 8;
      gfx->drawLine((int)vertices[i][0], (int)vertices[i][1],
                    (int)vertices[next][0], (int)vertices[next][1], 
                    shapeColor);
      
      // Cross connections for cuboctahedral structure
      if (i % 2 == 0) {
        int cross = (i + 4) % 8;
        gfx->drawLine((int)vertices[i][0], (int)vertices[i][1],
                      (int)vertices[cross][0], (int)vertices[cross][1], 
                      gfx->color565(50 + scale * 30, 50 + scale * 30, 100 + scale * 30));
      }
    }
    
    // Draw vertices as points
    for (int i = 0; i < 8; i++) {
      gfx->fillCircle((int)vertices[i][0], (int)vertices[i][1], 
                      2 + morphParam * 2, WHITE);
    }
    
    // Central morph indicator
    int centerSize = (int)(morphParam * 8 + 2);
    gfx->fillCircle(centerX, centerY, centerSize, 
                    gfx->color565((int)(morphParam * 255), 
                                  (int)((1 - morphParam) * 255), 100));
  }
  
  // Rotation trails
  for (int trail = 0; trail < 12; trail++) {
    float trailAngle = animationTime * 2 + trail * PI / 6;
    float trailRadius = 80 + sin(animationTime * 3 + trail) * 20;
    
    int trailX = centerX + cos(trailAngle) * trailRadius;
    int trailY = centerY + sin(trailAngle) * trailRadius;
    
    gfx->fillCircle(trailX, trailY, 1 + trail % 3, 
                    gfx->color565(trail * 20, 100, 200 - trail * 10));
  }
}

void drawCWaves() {
  // C-waves effect - sine waves forming C shapes
  gfx->fillScreen(gfx->color565(0, 0, 20)); // Dark blue background
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw multiple C-wave patterns
  for (int wave = 0; wave < 5; wave++) {
    float waveOffset = wave * PI / 2.5 + animationTime * 0.3;
    float waveAmplitude = 20 + wave * 8;
    float waveRadius = 40 + wave * 15;
    
    // C-wave color
    uint16_t waveColor = gfx->color565(
      (int)(sin(waveOffset) * 127 + 128),
      (int)(cos(waveOffset + 1) * 127 + 128),
      255 - wave * 30
    );
    
    // Draw C-shaped wave
    for (int angle = 30; angle <= 330; angle += 3) { // C shape - not full circle
      float radians = angle * PI / 180.0 + waveOffset;
      
      // Wave modulation along the C
      float waveHeight = sin(radians * 3 + animationTime * 4) * waveAmplitude * 0.3;
      float currentRadius = waveRadius + waveHeight;
      
      // Additional wave component
      currentRadius += sin(radians * 2 + animationTime * 2) * waveAmplitude * 0.2;
      
      int x = centerX + cos(radians) * currentRadius;
      int y = centerY + sin(radians) * currentRadius;
      
      // Draw wave point with size based on wave height
      int pointSize = 2 + abs((int)waveHeight) / 8;
      gfx->fillCircle(x, y, pointSize, waveColor);
      
      // Connect to previous point for smooth wave
      if (angle > 30) {
        float prevRadians = (angle - 3) * PI / 180.0 + waveOffset;
        float prevWaveHeight = sin(prevRadians * 3 + animationTime * 4) * waveAmplitude * 0.3;
        float prevRadius = waveRadius + prevWaveHeight + sin(prevRadians * 2 + animationTime * 2) * waveAmplitude * 0.2;
        
        int prevX = centerX + cos(prevRadians) * prevRadius;
        int prevY = centerY + sin(prevRadians) * prevRadius;
        
        gfx->drawLine(prevX, prevY, x, y, waveColor);
      }
    }
    
    // C-wave endpoints (open ends of C)
    float startAngle = 30 * PI / 180.0 + waveOffset;
    float endAngle = 330 * PI / 180.0 + waveOffset;
    
    int startX = centerX + cos(startAngle) * waveRadius;
    int startY = centerY + sin(startAngle) * waveRadius;
    int endX = centerX + cos(endAngle) * waveRadius;
    int endY = centerY + sin(endAngle) * waveRadius;
    
    gfx->fillCircle(startX, startY, 4, WHITE);
    gfx->fillCircle(endX, endY, 4, WHITE);
  }
  
  // Interference patterns between C-waves
  for (int x = 0; x < gfx->width(); x += 8) {
    for (int y = 0; y < gfx->height(); y += 8) {
      float dx = x - centerX;
      float dy = y - centerY;
      float distance = sqrt(dx*dx + dy*dy);
      
      // Multiple wave interference
      float interference = 0;
      for (int w = 0; w < 3; w++) {
        interference += sin(distance * 0.1 + animationTime * (2 + w) + w * PI / 3);
      }
      
      if (interference > 1.5) {
        int intensity = (int)((interference - 1.5) * 100);
        intensity = constrain(intensity, 0, 255);
        gfx->fillCircle(x, y, 1, gfx->color565(0, intensity, intensity));
      }
    }
  }
  
  // Central wave source
  int pulseSize = (int)(sin(animationTime * 6) * 5 + 8);
  gfx->fillCircle(centerX, centerY, pulseSize, CYAN);
  gfx->drawCircle(centerX, centerY, pulseSize + 3, WHITE);
}

void drawCynosure() {
  // Cynosure effect - center of attraction with orbiting elements
  gfx->fillScreen(gfx->color565(5, 5, 20)); // Very dark background
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Central cynosure - bright attracting center
  int centerPulse = (int)(sin(animationTime * 4) * 8 + 15);
  gfx->fillCircle(centerX, centerY, centerPulse, WHITE);
  gfx->fillCircle(centerX, centerY, centerPulse - 3, YELLOW);
  gfx->fillCircle(centerX, centerY, centerPulse - 6, gfx->color565(255, 150, 0));
  
  // Attraction rings
  for (int ring = 1; ring <= 4; ring++) {
    int ringRadius = centerPulse + ring * 25;
    uint16_t ringColor = gfx->color565(50 - ring * 8, 50 - ring * 8, 100 - ring * 15);
    
    // Pulsing ring
    if ((int)(animationTime * 8 + ring) % 3 == 0) {
      gfx->drawCircle(centerX, centerY, ringRadius, ringColor);
    }
  }
  
  // Orbiting objects attracted to center
  for (int orbit = 0; orbit < 12; orbit++) {
    float orbitAngle = animationTime * (1.0 + orbit * 0.1) + orbit * PI / 6;
    float orbitRadius = 80 + sin(animationTime * 2 + orbit) * 30;
    
    // Spiral inward attraction
    orbitRadius -= sin(animationTime * 0.5 + orbit * 0.3) * 20;
    
    int objX = centerX + cos(orbitAngle) * orbitRadius;
    int objY = centerY + sin(orbitAngle) * orbitRadius;
    
    // Object size decreases as it gets closer to center
    int objSize = (int)(orbitRadius * 0.08 + 3);
    
    // Color based on distance from center
    float distanceRatio = orbitRadius / 120.0;
    uint16_t objColor = gfx->color565(
      (int)(255 * (1 - distanceRatio)),
      (int)(150 * distanceRatio),
      (int)(200 * distanceRatio)
    );
    
    gfx->fillCircle(objX, objY, objSize, objColor);
    
    // Attraction trail
    gfx->drawLine(objX, objY, 
                  centerX + cos(orbitAngle) * (orbitRadius * 0.7), 
                  centerY + sin(orbitAngle) * (orbitRadius * 0.7), 
                  gfx->color565(30, 30, 60));
  }
  
  // Magnetic field lines
  for (int field = 0; field < 8; field++) {
    float fieldAngle = field * PI / 4 + animationTime * 0.3;
    
    for (int point = 1; point <= 6; point++) {
      float fieldRadius = point * 20;
      int fieldX = centerX + cos(fieldAngle) * fieldRadius;
      int fieldY = centerY + sin(fieldAngle) * fieldRadius;
      
      // Curved field lines
      fieldX += (int)(sin(animationTime * 3 + point) * 5);
      fieldY += (int)(cos(animationTime * 2 + point) * 5);
      
      gfx->fillCircle(fieldX, fieldY, 1, gfx->color565(20, 100, 200));
    }
  }
}

void drawDangerball() {
  // Dangerball effect - hazardous bouncing ball with warnings
  gfx->fillScreen(BLACK);
  
  static float ballX = 120;
  static float ballY = 120;
  static float ballVX = 3;
  static float ballVY = 2.5;
  static bool initialized = false;
  
  if (!initialized) {
    ballX = gfx->width() / 2;
    ballY = gfx->height() / 2;
    ballVX = random(-4, 5);
    ballVY = random(-4, 5);
    initialized = true;
  }
  
  // Update ball position
  ballX += ballVX;
  ballY += ballVY;
  
  int ballRadius = 12 + sin(animationTime * 6) * 3;
  
  // Bounce off walls with danger effects
  bool bounced = false;
  if (ballX - ballRadius < 0 || ballX + ballRadius > gfx->width()) {
    ballVX = -ballVX;
    bounced = true;
  }
  if (ballY - ballRadius < 0 || ballY + ballRadius > gfx->height()) {
    ballVY = -ballVY;
    bounced = true;
  }
  
  // Keep ball in bounds
  ballX = constrain(ballX, ballRadius, gfx->width() - ballRadius);
  ballY = constrain(ballY, ballRadius, gfx->height() - ballRadius);
  
  // Draw danger ball
  uint16_t dangerColor = gfx->color565(255, (int)(sin(animationTime * 8) * 127 + 128), 0);
  gfx->fillCircle((int)ballX, (int)ballY, ballRadius, dangerColor);
  
  // Hazard symbols on ball
  gfx->fillTriangle((int)ballX, (int)ballY - 6,
                    (int)ballX - 4, (int)ballY + 2,
                    (int)ballX + 4, (int)ballY + 2, BLACK);
  gfx->fillCircle((int)ballX, (int)ballY + 4, 1, BLACK);
  gfx->drawFastVLine((int)ballX, (int)ballY - 3, 4, BLACK);
  
  // Danger aura
  for (int aura = 1; aura <= 3; aura++) {
    int auraRadius = ballRadius + aura * 8;
    uint16_t auraColor = gfx->color565(200 - aura * 50, 50 - aura * 15, 0);
    
    if ((int)(animationTime * 10 + aura) % 4 == 0) {
      gfx->drawCircle((int)ballX, (int)ballY, auraRadius, auraColor);
    }
  }
  
  // Bounce explosion effect
  if (bounced) {
    for (int spark = 0; spark < 8; spark++) {
      float sparkAngle = spark * PI / 4;
      int sparkX = (int)ballX + cos(sparkAngle) * 20;
      int sparkY = (int)ballY + sin(sparkAngle) * 20;
      gfx->fillCircle(sparkX, sparkY, 3, YELLOW);
    }
  }
  
  // Warning indicators at screen edges
  if (ballX < 60) {
    gfx->fillTriangle(5, (int)ballY, 15, (int)ballY - 8, 15, (int)ballY + 8, RED);
  }
  if (ballX > gfx->width() - 60) {
    gfx->fillTriangle(gfx->width() - 5, (int)ballY, 
                      gfx->width() - 15, (int)ballY - 8, 
                      gfx->width() - 15, (int)ballY + 8, RED);
  }
  if (ballY < 60) {
    gfx->fillTriangle((int)ballX, 5, (int)ballX - 8, 15, (int)ballX + 8, 15, RED);
  }
  if (ballY > gfx->height() - 60) {
    gfx->fillTriangle((int)ballX, gfx->height() - 5, 
                      (int)ballX - 8, gfx->height() - 15, 
                      (int)ballX + 8, gfx->height() - 15, RED);
  }
  
  // Speed indicator
  float speed = sqrt(ballVX*ballVX + ballVY*ballVY);
  int speedBars = (int)(speed);
  
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(5, 5);
  gfx->printf("DANGER");
  
  for (int bar = 0; bar < speedBars && bar < 8; bar++) {
    uint16_t barColor = (bar < 4) ? YELLOW : RED;
    gfx->fillRect(5 + bar * 8, 15, 6, 8, barColor);
  }
}

void drawDecayScreen() {
  // Decay screen effect - screen deterioration and static
  static uint16_t pixels[320*240/16]; // Reduced resolution for performance
  static bool initialized = false;
  static int decayLevel = 0;
  
  if (!initialized) {
    // Initialize with some pattern
    for (int i = 0; i < 320*240/16; i++) {
      pixels[i] = gfx->color565(random(100, 255), random(100, 255), random(100, 255));
    }
    initialized = true;
  }
  
  // Progressive decay
  if ((int)(animationTime * 20) % 10 == 0) {
    decayLevel++;
    
    // Add noise and corruption
    for (int i = 0; i < 50; i++) {
      int index = random(0, 320*240/16);
      
      if (decayLevel < 50) {
        // Early decay - color corruption
        uint8_t r = (pixels[index] >> 11) & 0x1F;
        uint8_t g = (pixels[index] >> 5) & 0x3F;
        uint8_t b = pixels[index] & 0x1F;
        
        r = max(0, (int)(r - random(0, 3)));
        g = max(0, (int)(g - random(0, 5)));
        b = max(0, (int)(b - random(0, 3)));
        
        pixels[index] = (r << 11) | (g << 5) | b;
      } else if (decayLevel < 100) {
        // Mid decay - dead pixels
        if (random(0, 3) == 0) {
          pixels[index] = BLACK;
        } else {
          pixels[index] = gfx->color565(random(0, 50), random(0, 50), random(0, 50));
        }
      } else {
        // Severe decay - mostly static
        pixels[index] = gfx->color565(random(0, 255), random(0, 255), random(0, 255));
      }
    }
  }
  
  // Draw decayed pixels
  int pixelSize = 4;
  for (int y = 0; y < gfx->height(); y += pixelSize) {
    for (int x = 0; x < gfx->width(); x += pixelSize) {
      int index = (y / pixelSize) * (gfx->width() / pixelSize) + (x / pixelSize);
      if (index < 320*240/16) {
        gfx->fillRect(x, y, pixelSize, pixelSize, pixels[index]);
      }
    }
  }
  
  // Additional decay effects
  if (decayLevel > 30) {
    // Scan lines
    for (int y = 0; y < gfx->height(); y += 4) {
      if (random(0, 4) == 0) {
        gfx->drawFastHLine(0, y, gfx->width(), BLACK);
      }
    }
  }
  
  if (decayLevel > 60) {
    // Vertical noise bars
    for (int x = 0; x < gfx->width(); x += 8) {
      if (random(0, 8) == 0) {
        uint16_t noiseColor = gfx->color565(random(0, 100), random(0, 100), random(0, 100));
        gfx->drawFastVLine(x, 0, gfx->height(), noiseColor);
      }
    }
  }
  
  if (decayLevel > 90) {
    // Screen flicker
    if (random(0, 10) == 0) {
      gfx->fillScreen(gfx->color565(random(0, 50), random(0, 50), random(0, 50)));
    }
  }
  
  // Reset after complete decay
  if (decayLevel > 150) {
    decayLevel = 0;
    // Reinitialize with new pattern
    for (int i = 0; i < 320*240/16; i++) {
      pixels[i] = gfx->color565(random(100, 255), random(100, 255), random(100, 255));
    }
  }
  
  // Decay level indicator
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(5, 5);
  gfx->printf("Decay: %d%%", (decayLevel * 100) / 150);
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
    case COW_FACE:
      drawCowFace();
      break;
    case COW_HIDE:
      drawCowHide();
      break;
    case COW_HOOFS:
      drawCowHoofs();
      break;
    case COW_HORNS:
      drawCowHorns();
      break;
    case COW_TAIL:
      drawCowTail();
      break;
    case COW_UDDER:
      drawCowUdder();
      break;
    case CRACKBERG:
      drawCrackberg();
      break;
    case CRITICAL:
      drawCritical();
      break;
    case CRUMBLER:
      drawCrumbler();
      break;
    case CRYSTAL:
      drawCrystal();
      break;
    case CUBE21:
      drawCube21();
      break;
    case CUBENETIC:
      drawCubenetic();
      break;
    case CUBESTACK:
      drawCubeStack();
      break;
    case CUBESTORM:
      drawCubeStorm();
      break;
    case CUBETWIST:
      drawCubeTwist();
      break;
    case CUBICGRID:
      drawCubicGrid();
      break;
    case CUBOCTEVERSION:
      drawCuboctaversion();
      break;
    case CWAVES:
      drawCWaves();
      break;
    case CYNOSURE:
      drawCynosure();
      break;
    case DANGERBALL:
      drawDangerball();
      break;
    case DECAYSCREEN:
      drawDecayScreen();
      break;
  }
  
  animationTime += 0.05;
  delay(50);
}