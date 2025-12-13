// Psychedelic Clock for ESP32-2432S028 Display
// Converted from Python screensaver programs
// Use board "ESP32 Dev Module"

#include <Arduino_GFX_Library.h>
#include <RTClib.h>
#include <math.h>
#include <WiFi.h>
#include <WebServer.h>

// Display pins
#define BL_PIN 21
#define BOOT_PIN 0
#define BOOT_BUTTON_DEBOUNCE_TIME 400

// RGB LED pin
#define LED_PIN 4

// Display speed
#define DISPLAY_SPI_SPEED 80000000L
#define CLOCK_STRIP_HEIGHT 30  // Reserved bottom strip height

// Animation area helpers (reduced height when clock enabled)
#define getAnimationWidth() (gfx->width())
#define getAnimationHeight() (showClock ? (gfx->height() - CLOCK_STRIP_HEIGHT) : gfx->height())

// RTC module (DS3231 or DS1307)
RTC_DS3231 rtc;

// Display setup
Arduino_DataBus *bus = new Arduino_HWSPI(2 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, 12 /* MISO */);
Arduino_GFX *gfx = new Arduino_ILI9341(bus);

// WiFi AP setup
const char* ssid = "PsyClock";
const char* password = "clockset123";
WebServer server(80);
bool showClock = false;

// Software clock variables
bool useManualTime = false;
unsigned long manualTimeSet = 0;  // millis() when time was manually set
int manualHour = 12;
int manualMinute = 0;

// Animation modes
enum AnimationMode {
  PLASMA_FIELD = 0,
  KALEIDOSCOPE = 1,
  MANDELBROT = 2,
  MATRIX_RAIN = 3,
  FIRE = 4,
  BOUNCING_BALLS = 5,
  SPIRALS = 6,
  WAVES = 7,
  SIERPINSKI = 8,
  STARFIELD = 9,
  DNA_HELIX = 10,
  NEON_RAIN = 11,
  HEAVY_RAIN = 12,
  MICRO_DOTS = 13,
  RAINDROPS = 14,
  DRAGON_CURVE = 15,
  TUNNEL = 16,
  LISSAJOUS = 17,
  FIREWORKS = 18,
  LIGHTNING = 19,
  HYPNO_VORTEX = 20,
  VORONOI_LAVA = 21,
  AURORA = 22,
  MOIRE_MANDALA = 23,
  INTERFERENCE_RINGS = 24,
  CRT_VECTOR = 25,
  LASER_SHOW = 26,
  SMOKE_TRAILS = 27,
  GLITCH_ART = 28,
  WATERCOLOR = 29,
  QUASICRYSTAL = 30,
  HYPERBOLIC_GRID = 31,
  STRANGE_ATTRACTOR = 32,
  CELLULAR_AUTOMATA = 33,
  CRYSTAL_GROWTH = 34,
  QUANTUM_TUNNELS = 35,
  FRACTAL_SPARKS = 36,
  VORONOI_BLOOM = 37,
  PERLIN_NEBULA = 38,
  FRACTAL_MOSAIC = 39,
  SNOWFALL = 40,
  METEOR_SHOWER = 41,
  HEXAGON_GRID = 42,
  SINE_WAVES_GRID = 43,
  RGB_SHIFT = 44,
  TV_STATIC = 45,
  PARTICLE_FOUNTAIN = 46,
  COLOR_CYCLE_BARS = 47,
  AURORA_FIRE = 48,
  METEOR_PLASMA = 49,
  SNOW_MATRIX = 50,
  JULIA_FIRE = 51,
  MANDELBROT_LIGHTNING = 52,
  DRAGON_DNA = 53,
  VORONOI_STARFIELD = 54,
  FIREWORK_TUNNEL = 55,
  SMOKE_LAVA = 56,
  TETRIS_RAIN = 57,
  JULIA_STANDALONE = 58,
  RETRO_GEOMETRY = 59,
  MODE_COUNT = 60
};

// Global variables
volatile bool skipRequested = false;
volatile uint32_t isrTick = 0;
uint32_t lastPress = 0;
AnimationMode currentMode = PLASMA_FIELD;
float animationTime = 0;
int frameCount = 0;
bool rtcAvailable = false;

// Plasma field variables
uint8_t plasmaBuffer[240 * 320]; // Full screen buffer for plasma values
uint16_t plasmaPalette[256];

// Clock display variables
unsigned long lastClockUpdate = 0;
const unsigned long clockUpdateInterval = 1000; // Update clock every second

// Forward declarations
void initializePlasmaPalette();
void drawPlasmaField();
void drawKaleidoscope();
void drawMandelbrot();
void drawMatrixRain();
void drawFire();
void drawBouncingBalls();
void drawSpirals();
void drawWaves();
void drawSierpinski();
void drawStarfield();
void drawDNAHelix();
void drawNeonRain();
void drawHeavyRain();
void drawMicroDots();
void drawRaindrops();
void drawDragonCurve();
void drawTunnel();
void drawLissajous();
void drawFireworks();
void drawLightning();
void drawHypnoVortex();
void drawVoronoiLava();
void drawAurora();
void drawMoireMandala();
void drawInterferenceRings();
void drawCRTVector();
void drawLaserShow();
void drawSmokeTrails();
void drawGlitchArt();
void drawWatercolor();
void drawQuasicrystal();
void drawCellularAutomata();
void drawCrystalGrowth();
void drawQuantumTunnels();
void drawFractalSparks();
void drawVoronoiBloom();
void drawPerlinNebula();
void drawFractalMosaic();
void drawSnowfall();
void drawMeteorShower();
void drawHexagonGrid();
void drawSineWavesGrid();
void drawRGBShift();
void drawTVStatic();
void drawParticleFountain();
void drawColorCycleBars();
void drawAuroraFire();
void drawMeteorPlasma();
void drawSnowMatrix();
void drawJuliaFire();
void drawMandelbrotLightning();
void drawDragonDNA();
void drawVoronoiStarfield();
void drawFireworkTunnel();
void drawSmokeLava();
void drawTetrisRain();
void drawJuliaStandalone();
void drawRetroGeometry();
void drawHyperbolicGrid();
void drawStrangeAttractor();
void drawClock();
void drawClockOverlay();
void fillScreenExceptClock(uint16_t color);
void setRTCTime();
void handleRoot();
void handleToggleClock();
void handleSetTime();

void updateLED() {
  // Simple LED sync based on animation time
  float intensity = (sin(animationTime * 2.0) + 1.0) * 0.5; // 0.0 to 1.0
  int brightness = (int)(intensity * 255);
  analogWrite(LED_PIN, brightness);
}

void IRAM_ATTR onButtonPress() {
  skipRequested = true;
  isrTick = xTaskGetTickCountFromISR();
}

void setup() {
  Serial.begin(115200);
  
  // Display backlight - MAXIMUM brightness override
  pinMode(BL_PIN, OUTPUT);
  digitalWrite(BL_PIN, HIGH);
  
  // Try PWM control for maximum backlight
  ledcSetup(0, 5000, 8);
  ledcAttachPin(BL_PIN, 0);
  ledcWrite(0, 255); // Maximum PWM (100% duty cycle)
  
  Serial.println("Backlight set to MAXIMUM");
  
  // RGB LED setup
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Start with LED off
  Serial.println("RGB LED initialized");
  
  // Display initialization
  Serial.println("Initializing Psychedelic Clock...");
  if (!gfx->begin(DISPLAY_SPI_SPEED)) {
    Serial.println("Display initialization failed!");
    while (true) delay(1000);
  }
  
  gfx->setRotation(1);           // Counter-clockwise one position from 0
  fillScreenExceptClock(RGB565_BLACK); // Fill black first
  gfx->invertDisplay(false);     // Try FALSE for this hardware version
  Serial.println("Display inversion disabled - testing for correct black!");
  
  Serial.printf("Display size: %dx%d\n", gfx->width(), gfx->height());
  
  // Clock initialization - try RTC first, fallback to internal clock
  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(10, 100);
  gfx->print("Init Clock...");
  
  rtcAvailable = rtc.begin();
  if (!rtcAvailable) {
    Serial.println("No RTC found - using internal clock");
    fillScreenExceptClock(RGB565_BLACK);
    gfx->setCursor(10, 100);
    gfx->print("Internal Clock");
    delay(1000);
  } else {
    Serial.println("RTC found and initialized!");
    // Check if RTC lost power and if so, set the time
    if (rtc.lostPower()) {
      Serial.println("RTC lost power, setting time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }
  
  // Initialize palettes
  initializePlasmaPalette();
  
  // Button setup
  pinMode(BOOT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BOOT_PIN), onButtonPress, FALLING);
  
  // WiFi AP setup
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", handleRoot);
  server.on("/toggle", handleToggleClock);
  server.on("/settime", handleSetTime);
  server.begin();
  
  fillScreenExceptClock(RGB565_BLACK);
  Serial.println("Psychedelic Clock ready!");
}

void loop() {
  // Handle web server
  server.handleClient();
  
  // Handle mode switching
  if (skipRequested) {
    uint32_t now = millis();
    if (now - lastPress >= BOOT_BUTTON_DEBOUNCE_TIME) {
      currentMode = (AnimationMode)((currentMode + 1) % MODE_COUNT);
      animationTime = 0; // Reset animation
      Serial.printf("Switched to mode %d\n", currentMode);
      lastPress = now;
    }
    skipRequested = false;
  }
  
  // Draw current animation mode
  switch (currentMode) {
    case PLASMA_FIELD:
      drawPlasmaField();
      break;
    case KALEIDOSCOPE:
      drawKaleidoscope();
      break;
    case MANDELBROT:
      drawMandelbrot();
      break;
    case MATRIX_RAIN:
      drawMatrixRain();
      break;
    case FIRE:
      drawFire();
      break;
    case BOUNCING_BALLS:
      drawBouncingBalls();
      break;
    case SPIRALS:
      drawSpirals();
      break;
    case WAVES:
      drawWaves();
      break;
    case SIERPINSKI:
      drawSierpinski();
      break;
    case STARFIELD:
      drawStarfield();
      break;
    case DNA_HELIX:
      drawDNAHelix();
      break;
    case NEON_RAIN:
      drawNeonRain();
      break;
    case HEAVY_RAIN:
      drawHeavyRain();
      break;
    case MICRO_DOTS:
      drawMicroDots();
      break;
    case RAINDROPS:
      drawRaindrops();
      break;
    case DRAGON_CURVE:
      drawDragonCurve();
      break;
    case TUNNEL:
      drawTunnel();
      break;
    case LISSAJOUS:
      drawLissajous();
      break;
    case FIREWORKS:
      drawFireworks();
      break;
    case LIGHTNING:
      drawLightning();
      break;
    case HYPNO_VORTEX:
      drawHypnoVortex();
      break;
    case VORONOI_LAVA:
      drawVoronoiLava();
      break;
    case AURORA:
      drawAurora();
      break;
    case MOIRE_MANDALA:
      drawMoireMandala();
      break;
    case INTERFERENCE_RINGS:
      drawInterferenceRings();
      break;
    case CRT_VECTOR:
      drawCRTVector();
      break;
    case LASER_SHOW:
      drawLaserShow();
      break;
    case SMOKE_TRAILS:
      drawSmokeTrails();
      break;
    case GLITCH_ART:
      drawGlitchArt();
      break;
    case WATERCOLOR:
      drawWatercolor();
      break;
    case QUASICRYSTAL:
      drawQuasicrystal();
      break;
    case HYPERBOLIC_GRID:
      drawHyperbolicGrid();
      break;
    case STRANGE_ATTRACTOR:
      drawStrangeAttractor();
      break;
    case CELLULAR_AUTOMATA:
      drawCellularAutomata();
      break;
    case CRYSTAL_GROWTH:
      drawCrystalGrowth();
      break;
    case QUANTUM_TUNNELS:
      drawQuantumTunnels();
      break;
    case FRACTAL_SPARKS:
      drawFractalSparks();
      break;
    case VORONOI_BLOOM:
      drawVoronoiBloom();
      break;
    case PERLIN_NEBULA:
      drawPerlinNebula();
      break;
    case FRACTAL_MOSAIC:
      drawFractalMosaic();
      break;
    case SNOWFALL:
      drawSnowfall();
      break;
    case METEOR_SHOWER:
      drawMeteorShower();
      break;
    case HEXAGON_GRID:
      drawHexagonGrid();
      break;
    case SINE_WAVES_GRID:
      drawSineWavesGrid();
      break;
    case RGB_SHIFT:
      drawRGBShift();
      break;
    case TV_STATIC:
      drawTVStatic();
      break;
    case PARTICLE_FOUNTAIN:
      drawParticleFountain();
      break;
    case COLOR_CYCLE_BARS:
      drawColorCycleBars();
      break;
    case AURORA_FIRE:
      drawAuroraFire();
      break;
    case METEOR_PLASMA:
      drawMeteorPlasma();
      break;
    case SNOW_MATRIX:
      drawSnowMatrix();
      break;
    case JULIA_FIRE:
      drawJuliaFire();
      break;
    case MANDELBROT_LIGHTNING:
      drawMandelbrotLightning();
      break;
    case DRAGON_DNA:
      drawDragonDNA();
      break;
    case VORONOI_STARFIELD:
      drawVoronoiStarfield();
      break;
    case FIREWORK_TUNNEL:
      drawFireworkTunnel();
      break;
    case SMOKE_LAVA:
      drawSmokeLava();
      break;
    case TETRIS_RAIN:
      drawTetrisRain();
      break;
    case JULIA_STANDALONE:
      drawJuliaStandalone();
      break;
    case RETRO_GEOMETRY:
      drawRetroGeometry();
      break;
  }
  
  // FORCE clock to draw last (sprite-like behavior)
  if (showClock) {
    drawClockOverlay();  // New function that can't be overwritten
  }
  
  // Force maximum brightness EVERY FRAME (aggressive override)
  digitalWrite(BL_PIN, HIGH);               // GPIO21 variant
  digitalWrite(27, HIGH);                   // GPIO27 variant  
  ledcWrite(0, 255);                        // Maximum PWM
  
  // Ensure pins are configured as outputs
  pinMode(BL_PIN, OUTPUT);
  pinMode(27, OUTPUT);
  
  // Update animation time
  animationTime += 0.05f;
  frameCount++;
  
  // Update RGB LED to match display
  updateLED();
  
  delay(50); // ~20 FPS
}

void initializePlasmaPalette() {
  for (int i = 0; i < 256; i++) {
    float t = i / 255.0f;
    
    // Classic plasma colors with mathematical waves
    int r = (int)(128 + 127 * sin(t * M_PI * 2));
    int g = (int)(128 + 127 * sin(t * M_PI * 2 + M_PI / 3));
    int b = (int)(128 + 127 * sin(t * M_PI * 2 + 2 * M_PI / 3));
    
    // Boost psychedelic colors
    if (t > 0.2f && t < 0.4f) { // Enhance magentas
      r = min(255, (int)(r * 1.3f));
      b = min(255, (int)(b * 1.2f));
    } else if (t > 0.6f && t < 0.8f) { // Enhance cyans
      g = min(255, (int)(g * 1.3f));
      b = min(255, (int)(b * 1.3f));
    }
    
    plasmaPalette[i] = gfx->color565(r, g, b);
  }
}

void drawPlasmaField() {
  int width = gfx->width();
  int height = getAnimationHeight();
  int centerX = width / 2;
  int centerY = height / 2;
  
  // Calculate plasma field
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      float value = 0;
      
      // Multiple sine waves for classic plasma effect
      value += sin((x + animationTime * 30) / 16.0f);
      value += sin((y + animationTime * 20) / 8.0f);
      value += sin((x + y + animationTime * 25) / 16.0f);
      
      // Circular patterns
      float dist = sqrt((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));
      value += sin(dist / 8.0f + animationTime * 15);
      
      // Rotating pattern
      float angle = atan2(y - centerY, x - centerX);
      value += sin(angle * 3 + animationTime * 10);
      
      // Normalize to 0-255
      int plasmaValue = (int)((value + 5) / 10 * 255) % 256;
      
      // Draw pixel
      gfx->drawPixel(x, y, plasmaPalette[plasmaValue]);
    }
  }
}

void drawKaleidoscope() {
  int width = gfx->width();
  int height = getAnimationHeight();
  int centerX = width / 2;
  int centerY = height / 2;
  int radius = min(width, height) / 2 - 10;
  
  fillScreenExceptClock(RGB565_BLACK);
  
  // Kaleidoscope parameters
  int segments = 6;
  float segmentAngle = 2 * M_PI / segments;
  
  // Draw kaleidoscope pattern
  for (int seg = 0; seg < segments; seg++) {
    float baseAngle = animationTime * 0.5f + seg * segmentAngle;
    
    // Draw multiple elements per segment
    for (int elem = 0; elem < 8; elem++) {
      float elemAngle = baseAngle + elem * M_PI / 4;
      float distance = 20 + 60 * sin(animationTime + elem);
      
      int x = centerX + distance * cos(elemAngle);
      int y = centerY + distance * sin(elemAngle);
      
      // Psychedelic colors
      int colorIndex = (elem + (int)(animationTime * 10)) % 256;
      uint16_t color = plasmaPalette[colorIndex];
      
      // Draw shape with pulsing size
      int size = 3 + 2 * sin(animationTime * 2 + elem);
      gfx->fillCircle(x, y, size, color);
      
      // Draw mirror reflection
      float mirrorAngle = baseAngle - (elemAngle - baseAngle);
      int mx = centerX + distance * cos(mirrorAngle);
      int my = centerY + distance * sin(mirrorAngle);
      gfx->fillCircle(mx, my, size, color);
    }
  }
}

void drawMandelbrot() {
  int width = gfx->width();
  int height = getAnimationHeight();
  
  // Mandelbrot parameters with animation
  float zoom = 0.5f + 0.3f * sin(animationTime * 0.1f);
  float offsetX = -0.7f + 0.2f * cos(animationTime * 0.05f);
  float offsetY = 0.0f + 0.1f * sin(animationTime * 0.03f);
  
  for (int py = 0; py < height; py++) {
    for (int px = 0; px < width; px++) {
      // Convert pixel to complex plane
      float x0 = (px - width/2.0f) / (width/4.0f) * zoom + offsetX;
      float y0 = (py - height/2.0f) / (height/4.0f) * zoom + offsetY;
      
      float x = 0, y = 0;
      int iteration = 0;
      int maxIter = 50;
      
      // Mandelbrot calculation
      while (x*x + y*y <= 4 && iteration < maxIter) {
        float xtemp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtemp;
        iteration++;
      }
      
      // Color based on iteration count
      if (iteration == maxIter) {
        gfx->drawPixel(px, py, RGB565_BLACK);
      } else {
        int colorIndex = (iteration * 8 + (int)(animationTime * 20)) % 256;
        gfx->drawPixel(px, py, plasmaPalette[colorIndex]);
      }
    }
  }
}

void drawClock() {
  if (!showClock || millis() - lastClockUpdate < clockUpdateInterval) {
    return;
  }
  
  lastClockUpdate = millis();
}

// Sprite-like clock overlay - ALWAYS draws on top
void drawClockOverlay() {
  static unsigned long lastOverlayUpdate = 0;
  
  // Update every frame to ensure it's always on top
  if (millis() - lastOverlayUpdate < 100) {  // 10 FPS update rate
    return;
  }
  
  lastOverlayUpdate = millis();
  
  // Format time and date for overlay
  char overlayTimeStr[16];
  char overlayDateStr[32];
  
  if (rtcAvailable) {
    DateTime now = rtc.now();
    sprintf(overlayTimeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  } else if (useManualTime) {
    unsigned long secondsSinceSet = (millis() - manualTimeSet) / 1000;
    int totalMinutes = manualHour * 60 + manualMinute + (secondsSinceSet / 60);
    int hours = (totalMinutes / 60) % 24;
    int minutes = totalMinutes % 60;
    int seconds = secondsSinceSet % 60;
    sprintf(overlayTimeStr, "%02d:%02d:%02d", hours, minutes, seconds);
  } else {
    unsigned long totalSeconds = millis() / 1000;
    int hours = (totalSeconds / 3600) % 24;
    int minutes = (totalSeconds / 60) % 60;
    int seconds = totalSeconds % 60;
    sprintf(overlayTimeStr, "%02d:%02d:%02d", hours, minutes, seconds);
  }
  
  // Clock positioned in protected bottom strip
  int overlayClockY = gfx->height() - CLOCK_STRIP_HEIGHT + 5;
  int overlayClockX = 10;
  
  // FORCE clear and redraw (sprite-like behavior)
  gfx->fillRect(overlayClockX, overlayClockY, 120, 16, RGB565_BLACK);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(overlayClockX, overlayClockY);
  gfx->print(overlayTimeStr);
  
  // Effect number on right side
  char overlayEffectStr[8];
  sprintf(overlayEffectStr, "#%d", (int)currentMode);
  int overlayEffectX = gfx->width() - 60;
  gfx->fillRect(overlayEffectX, overlayClockY, 50, 16, RGB565_BLACK);
  gfx->setCursor(overlayEffectX, overlayClockY);
  gfx->print(overlayEffectStr);
  
  // Format time and date
  char timeStr[16];
  char dateStr[32];
  
  if (rtcAvailable) {
    // Use RTC time
    DateTime now = rtc.now();
    sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    
    const char* daysOfWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    sprintf(dateStr, "%s %s %02d", 
            daysOfWeek[now.dayOfTheWeek()], 
            months[now.month() - 1], 
            now.day());
  } else if (useManualTime) {
    // Use manually set time
    unsigned long secondsSinceSet = (millis() - manualTimeSet) / 1000;
    int totalMinutes = manualHour * 60 + manualMinute + (secondsSinceSet / 60);
    int hours = (totalMinutes / 60) % 24;
    int minutes = totalMinutes % 60;
    int seconds = secondsSinceSet % 60;
    
    sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);
    sprintf(dateStr, "");
  } else {
    // Use internal millis-based clock (starts from boot time)
    unsigned long totalSeconds = millis() / 1000;
    int hours = (totalSeconds / 3600) % 24;
    int minutes = (totalSeconds / 60) % 60;
    int seconds = totalSeconds % 60;
    
    sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);
    sprintf(dateStr, "");
  }
  
  // Clock now lives in protected bottom strip
  int clockY = gfx->height() - CLOCK_STRIP_HEIGHT + 5;  // 5px from bottom edge
  int clockX = 10;  // Left side of strip
  
  // Clear clock area first (prevent text overlap)
  gfx->fillRect(clockX, clockY, 120, 16, RGB565_BLACK);  // Clear area for size 2 text
  
  // Draw time in protected strip
  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(2);  // Bigger text in dedicated area
  gfx->setCursor(clockX, clockY);
  gfx->print(timeStr);
  
  // Show effect number on right side of strip
  char effectStr[8];
  sprintf(effectStr, "#%d", (int)currentMode);
  int effectX = gfx->width() - 60;  // Right side of strip
  gfx->fillRect(effectX, clockY, 50, 16, RGB565_BLACK);  // Clear effect number area
  gfx->setCursor(effectX, clockY);
  gfx->print(effectStr);
}

// Helper function to clear screen with protected clock strip  
void fillScreenExceptClock(uint16_t color) {
  if (!showClock) {
    gfx->fillScreen(color);
    return;
  }
  
  // Fill everything EXCEPT the bottom 30 pixels (clock strip)
  int protectedHeight = gfx->height() - CLOCK_STRIP_HEIGHT;
  gfx->fillRect(0, 0, gfx->width(), protectedHeight, color);
  
  // Fill clock strip with black background
  gfx->fillRect(0, protectedHeight, gfx->width(), CLOCK_STRIP_HEIGHT, RGB565_BLACK);
}

// Helper function to set RTC time manually via Serial
void setRTCTime() {
  Serial.println("Enter time in format: YYYY MM DD HH MM SS");
  Serial.println("Example: 2024 11 22 18 30 00");
  
  // You can call this function to set time manually
  // For now, it sets to a default time - modify as needed
  DateTime newTime(2024, 11, 22, 18, 30, 0); // Nov 22, 2024, 6:30:00 PM
  rtc.adjust(newTime);
  Serial.println("RTC time set!");
}

// ============ MATRIX RAIN EFFECT ============
#define MAX_DROPS 20
struct MatrixDrop {
  int x, y, speed, length;
  bool active;
};
MatrixDrop drops[MAX_DROPS];

void drawMatrixRain() {
  fillScreenExceptClock(RGB565_BLACK);
  
  for (int i = 0; i < MAX_DROPS; i++) {
    if (!drops[i].active && random(100) < 5) {
      drops[i].x = random(gfx->width() / 8) * 8;
      drops[i].y = 0;
      drops[i].speed = random(2, 6);
      drops[i].length = random(10, 25);
      drops[i].active = true;
    }
    
    if (drops[i].active) {
      // Draw trail
      for (int j = 0; j < drops[i].length; j++) {
        int y = drops[i].y - j * 8;
        if (y >= 0 && y < getAnimationHeight()) {
          uint8_t brightness = 255 - (j * 255 / drops[i].length);
          uint16_t color = gfx->color565(0, brightness, 0);
          gfx->setTextColor(color);
          gfx->setCursor(drops[i].x, y);
          gfx->print(char(random(33, 126)));
        }
      }
      
      drops[i].y += drops[i].speed;
      if (drops[i].y > getAnimationHeight() + drops[i].length * 8) {
        drops[i].active = false;
      }
    }
  }
}

// ============ FIRE EFFECT ============
#define FIRE_WIDTH 40
#define FIRE_HEIGHT 30
uint8_t fire[FIRE_HEIGHT][FIRE_WIDTH];

void drawFire() {
  // Add heat at bottom
  for (int x = 0; x < FIRE_WIDTH; x++) {
    fire[FIRE_HEIGHT - 1][x] = random(200, 255);
  }
  
  // Propagate heat upward with cooling
  for (int y = 0; y < FIRE_HEIGHT - 1; y++) {
    for (int x = 0; x < FIRE_WIDTH; x++) {
      int cooling = random(0, 5);
      int nextX = (x + random(3) - 1 + FIRE_WIDTH) % FIRE_WIDTH;
      
      fire[y][x] = max(0, (fire[y + 1][nextX] + fire[y + 1][x]) / 2 - cooling);
    }
  }
  
  // Draw fire
  for (int y = 0; y < FIRE_HEIGHT; y++) {
    for (int x = 0; x < FIRE_WIDTH; x++) {
      uint8_t heat = fire[y][x];
      uint16_t color;
      
      if (heat < 85) {
        color = gfx->color565(heat * 3, 0, 0);
      } else if (heat < 170) {
        color = gfx->color565(255, (heat - 85) * 3, 0);
      } else {
        color = gfx->color565(255, 255, (heat - 170) * 3);
      }
      
      int screenX = x * gfx->width() / FIRE_WIDTH;
      int screenY = y * gfx->height() / FIRE_HEIGHT;
      gfx->fillRect(screenX, screenY, gfx->width() / FIRE_WIDTH + 1, gfx->height() / FIRE_HEIGHT + 1, color);
    }
  }
}

// ============ BOUNCING BALLS EFFECT ============
#define NUM_BALLS 15
struct Ball {
  float x, y, vx, vy;
  uint16_t color;
  int radius;
};
Ball balls[NUM_BALLS];
bool ballsInitialized = false;

void drawBouncingBalls() {
  fillScreenExceptClock(RGB565_BLACK);
  
  if (!ballsInitialized) {
    for (int i = 0; i < NUM_BALLS; i++) {
      balls[i].x = random(20, gfx->width() - 20);
      balls[i].y = random(20, gfx->height() - 20);
      balls[i].vx = random(-3, 3);
      balls[i].vy = random(-3, 3);
      balls[i].radius = random(8, 15);
      balls[i].color = gfx->color565(random(200, 255), random(200, 255), random(200, 255));
    }
    ballsInitialized = true;
  }
  
  for (int i = 0; i < NUM_BALLS; i++) {
    // Update position
    balls[i].x += balls[i].vx;
    balls[i].y += balls[i].vy;
    
    // Bounce off walls
    if (balls[i].x - balls[i].radius <= 0 || balls[i].x + balls[i].radius >= gfx->width()) {
      balls[i].vx *= -1;
      balls[i].x = constrain(balls[i].x, balls[i].radius, gfx->width() - balls[i].radius);
    }
    if (balls[i].y - balls[i].radius <= 0 || balls[i].y + balls[i].radius >= gfx->height()) {
      balls[i].vy *= -1;
      balls[i].y = constrain(balls[i].y, balls[i].radius, gfx->height() - balls[i].radius);
    }
    
    // Draw ball
    gfx->fillCircle(balls[i].x, balls[i].y, balls[i].radius, balls[i].color);
  }
}

// ============ SPIRALS EFFECT ============
void drawSpirals() {
  static float angle = 0;
  angle += 0.05;
  
  fillScreenExceptClock(RGB565_BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw multiple spirals
  for (int spiral = 0; spiral < 3; spiral++) {
    float spiralAngle = angle + spiral * (TWO_PI / 3);
    
    for (float t = 0; t < 4 * PI; t += 0.1) {
      float r = t * 8;
      int x = centerX + cos(t + spiralAngle) * r;
      int y = centerY + sin(t + spiralAngle) * r;
      
      if (x >= 0 && x < gfx->width() && y >= 0 && y < getAnimationHeight()) {
        uint8_t colorVal = (uint8_t)(t * 20 + angle * 10);
        uint16_t color = gfx->color565(
          128 + 127 * sin(colorVal * 0.1),
          128 + 127 * sin(colorVal * 0.1 + 2),
          128 + 127 * sin(colorVal * 0.1 + 4)
        );
        gfx->drawPixel(x, y, color);
      }
    }
  }
}

// ============ WAVES EFFECT ============
void drawWaves() {
  static float phase = 0;
  phase += 0.08;
  
  fillScreenExceptClock(RGB565_BLACK);
  
  for (int y = 0; y < getAnimationHeight(); y += 3) {
    for (int x = 0; x < gfx->width(); x += 3) {
      float wave1 = sin(x * 0.05 + phase) * 20;
      float wave2 = sin(y * 0.05 + phase * 1.3) * 20;
      float combined = wave1 + wave2;
      
      if (abs(y - gfx->height() / 2 - combined) < 3) {
        uint8_t colorVal = (uint8_t)(x + y + phase * 20);
        uint16_t color = gfx->color565(
          128 + 127 * sin(colorVal * 0.05),
          128 + 127 * sin(colorVal * 0.05 + 1.5),
          128 + 127 * sin(colorVal * 0.05 + 3)
        );
        gfx->fillRect(x, y, 3, 3, color);
      }
    }
  }
}

// ============ SIERPINSKI TRIANGLE ============
struct SierpinskiPoint {
  float x, y;
};

SierpinskiPoint sierpinskiVertices[3] = {
  {160, 20},           // Top center
  {20, 300},           // Bottom left  
  {300, 300}           // Bottom right
};

void drawSierpinski() {
  static SierpinskiPoint current = {160, 160};
  static float colorPhase = 0;
  colorPhase += 0.02;
  
  fillScreenExceptClock(RGB565_BLACK);
  
  // Chaos game algorithm - draw many points per frame
  for (int i = 0; i < 500; i++) {
    // Pick random vertex
    int vertexIdx = random(3);
    SierpinskiPoint& vertex = sierpinskiVertices[vertexIdx];
    
    // Move halfway to that vertex
    current.x = (current.x + vertex.x) / 2;
    current.y = (current.y + vertex.y) / 2;
    
    // Color based on which vertex was chosen
    uint16_t color;
    if (vertexIdx == 0) color = gfx->color565(255, 0, 0);      // Red
    else if (vertexIdx == 1) color = gfx->color565(0, 255, 0); // Green  
    else color = gfx->color565(0, 0, 255);                     // Blue
    
    gfx->drawPixel(current.x, current.y, color);
  }
}

// ============ STARFIELD EFFECT ============
#define NUM_STARS 50
struct Star {
  float x, y, z, speed;
};
Star stars[NUM_STARS];
bool starfieldInitialized = false;

void drawStarfield() {
  if (!starfieldInitialized) {
    for (int i = 0; i < NUM_STARS; i++) {
      stars[i].x = random(-gfx->width(), gfx->width());
      stars[i].y = random(-gfx->height(), gfx->height());
      stars[i].z = random(1, 100);
      stars[i].speed = random(1, 4);
    }
    starfieldInitialized = true;
  }
  
  fillScreenExceptClock(RGB565_BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int i = 0; i < NUM_STARS; i++) {
    // Move star forward
    stars[i].z -= stars[i].speed;
    
    // Reset star if it goes past screen
    if (stars[i].z <= 0) {
      stars[i].x = random(-gfx->width(), gfx->width());
      stars[i].y = random(-gfx->height(), gfx->height());
      stars[i].z = 100;
    }
    
    // Project 3D to 2D
    float k = 128.0 / stars[i].z;
    int px = centerX + stars[i].x * k;
    int py = centerY + stars[i].y * k;
    
    // Draw star if on screen
    if (px >= 0 && px < gfx->width() && py >= 0 && py < getAnimationHeight()) {
      // Size and brightness based on distance
      int size = (100 - stars[i].z) / 25 + 1;
      uint8_t brightness = map(stars[i].z, 1, 100, 255, 50);
      uint16_t color = gfx->color565(brightness, brightness, brightness);
      
      if (size > 1) {
        gfx->fillCircle(px, py, size, color);
      } else {
        gfx->drawPixel(px, py, color);
      }
      
      // Draw motion trail
      if (stars[i].z < 50) {
        float prevK = 128.0 / (stars[i].z + stars[i].speed);
        int prevPx = centerX + stars[i].x * prevK;
        int prevPy = centerY + stars[i].y * prevK;
        gfx->drawLine(px, py, prevPx, prevPy, color);
      }
    }
  }
}

// ============ DNA HELIX EFFECT ============
void drawDNAHelix() {
  static float rotation = 0;
  rotation += 0.05;
  
  fillScreenExceptClock(RGB565_BLACK);
  
  int centerX = gfx->width() / 2;
  int baseY = 20;
  
  // Draw DNA strands
  for (float y = 0; y < getAnimationHeight() - 40; y += 3) {
    float angle = y * 0.1 + rotation;
    
    // Calculate helix positions
    int x1 = centerX + sin(angle) * 40;
    int x2 = centerX - sin(angle) * 40;
    int py = baseY + y;
    
    // Color based on position
    uint8_t colorVal = (uint8_t)(y + rotation * 10);
    uint16_t color1 = gfx->color565(
      128 + 127 * sin(colorVal * 0.1),
      128 + 127 * sin(colorVal * 0.1 + 2),
      128 + 127 * sin(colorVal * 0.1 + 4)
    );
    uint16_t color2 = gfx->color565(
      128 + 127 * sin(colorVal * 0.1 + 3),
      128 + 127 * sin(colorVal * 0.1 + 5),
      128 + 127 * sin(colorVal * 0.1 + 1)
    );
    
    // Draw backbone
    gfx->fillCircle(x1, py, 3, color1);
    gfx->fillCircle(x2, py, 3, color2);
    
    // Draw connecting base pairs every 10 pixels
    if ((int)y % 10 == 0) {
      gfx->drawLine(x1, py, x2, py, gfx->color565(100, 100, 100));
      // Draw base pair nodes
      int midX = (x1 + x2) / 2;
      uint16_t baseColor = gfx->color565(
        128 + 127 * sin(colorVal * 0.1 + 6),
        128 + 127 * sin(colorVal * 0.1 + 7),
        128 + 127 * sin(colorVal * 0.1 + 8)
      );
      gfx->fillCircle(midX, py, 2, baseColor);
    }
    
    // Connect to previous point (smooth strand)
    if (y > 0) {
      float prevAngle = (y - 3) * 0.1 + rotation;
      int prevX1 = centerX + sin(prevAngle) * 40;
      int prevX2 = centerX - sin(prevAngle) * 40;
      int prevY = baseY + y - 3;
      
      gfx->drawLine(x1, py, prevX1, prevY, color1);
      gfx->drawLine(x2, py, prevX2, prevY, color2);
    }
  }
}

// ============ NEON RAIN EFFECT ============
#define NEON_DROPS 60
struct NeonDrop {
  int x, y, speed;
  uint8_t brightness;
  bool isBlue;
  bool active;
};
NeonDrop neonDrops[NEON_DROPS];
bool neonRainInitialized = false;

void drawNeonRain() {
  if (!neonRainInitialized) {
    for (int i = 0; i < NEON_DROPS; i++) {
      neonDrops[i].active = false;
    }
    neonRainInitialized = true;
  }
  
  // Fade background slightly for trail effect
  gfx->fillRect(0, 0, gfx->width(), gfx->height(), gfx->color565(0, 5, 10));
  
  for (int i = 0; i < NEON_DROPS; i++) {
    if (!neonDrops[i].active && random(100) < 8) {
      neonDrops[i].x = random(gfx->width());
      neonDrops[i].y = 0;
      neonDrops[i].speed = random(3, 8);
      neonDrops[i].brightness = random(150, 255);
      neonDrops[i].isBlue = random(10) < 3; // 30% blue, 70% green
      neonDrops[i].active = true;
    }
    
    if (neonDrops[i].active) {
      // Draw neon glow effect
      uint16_t color;
      if (neonDrops[i].isBlue) {
        color = gfx->color565(0, neonDrops[i].brightness / 2, neonDrops[i].brightness);
      } else {
        color = gfx->color565(0, neonDrops[i].brightness, neonDrops[i].brightness / 3);
      }
      
      // Draw with glow
      gfx->drawPixel(neonDrops[i].x, neonDrops[i].y, color);
      gfx->drawPixel(neonDrops[i].x, neonDrops[i].y + 1, color);
      if (neonDrops[i].brightness > 200) {
        gfx->drawPixel(neonDrops[i].x - 1, neonDrops[i].y, color);
        gfx->drawPixel(neonDrops[i].x + 1, neonDrops[i].y, color);
      }
      
      neonDrops[i].y += neonDrops[i].speed;
      
      if (neonDrops[i].y >= gfx->height()) {
        // Splash effect at bottom
        for (int s = 0; s < 3; s++) {
          int splashX = neonDrops[i].x + random(-5, 6);
          if (splashX >= 0 && splashX < gfx->width()) {
            gfx->drawPixel(splashX, gfx->height() - 2, color);
          }
        }
        neonDrops[i].active = false;
      }
    }
  }
}

// ============ HEAVY RAIN EFFECT ============
#define HEAVY_DROPS 100
struct HeavyDrop {
  float x, y, speed, wind;
  uint8_t length;
  bool active;
};
HeavyDrop heavyDrops[HEAVY_DROPS];
bool heavyRainInitialized = false;

void drawHeavyRain() {
  if (!heavyRainInitialized) {
    for (int i = 0; i < HEAVY_DROPS; i++) {
      heavyDrops[i].active = false;
    }
    heavyRainInitialized = true;
  }
  
  // Dark stormy background
  fillScreenExceptClock(gfx->color565(10, 10, 20));
  
  for (int i = 0; i < HEAVY_DROPS; i++) {
    if (!heavyDrops[i].active && random(100) < 15) {
      heavyDrops[i].x = random(gfx->width());
      heavyDrops[i].y = -10;
      heavyDrops[i].speed = random(8, 15);
      heavyDrops[i].wind = random(-2, 3);
      heavyDrops[i].length = random(8, 18);
      heavyDrops[i].active = true;
    }
    
    if (heavyDrops[i].active) {
      // Draw heavy rain streak
      uint16_t rainColor = gfx->color565(160, 200, 220);
      
      int startY = heavyDrops[i].y;
      int endY = heavyDrops[i].y + heavyDrops[i].length;
      int startX = heavyDrops[i].x;
      int endX = heavyDrops[i].x + heavyDrops[i].wind;
      
      gfx->drawLine(startX, startY, endX, endY, rainColor);
      
      // Move drop
      heavyDrops[i].y += heavyDrops[i].speed;
      heavyDrops[i].x += heavyDrops[i].wind * 0.3;
      
      if (heavyDrops[i].y >= gfx->height()) {
        // Heavy splash
        for (int s = 0; s < 5; s++) {
          int splashX = heavyDrops[i].x + random(-8, 9);
          int splashY = gfx->height() - random(3, 8);
          if (splashX >= 0 && splashX < gfx->width()) {
            gfx->drawPixel(splashX, splashY, gfx->color565(200, 220, 240));
          }
        }
        heavyDrops[i].active = false;
      }
    }
  }
}

// ============ MICRO DOTS EFFECT ============
void drawMicroDots() {
  static float phase = 0;
  static uint8_t dotGrid[60][80];
  phase += 0.1;
  
  fillScreenExceptClock(RGB565_BLACK);
  
  // Update dot grid with wave patterns
  for (int y = 0; y < 60; y++) {
    for (int x = 0; x < 80; x++) {
      float dist = sqrt(pow(x - 40, 2) + pow(y - 30, 2));
      float wave = sin(dist * 0.3 - phase) * 127 + 128;
      dotGrid[y][x] = (uint8_t)wave;
    }
  }
  
  // Draw micro dots (4x4 pixels each)
  for (int y = 0; y < 60; y++) {
    for (int x = 0; x < 80; x++) {
      uint8_t brightness = dotGrid[y][x];
      
      if (brightness > 100) {
        uint16_t color = gfx->color565(
          128 + 127 * sin(brightness * 0.1),
          128 + 127 * sin(brightness * 0.1 + 2),
          128 + 127 * sin(brightness * 0.1 + 4)
        );
        gfx->fillRect(x * 4, y * 4, 3, 3, color);
      }
    }
  }
}

// ============ RAINDROPS EFFECT ============
#define NUM_RAINDROPS 30
struct Raindrop {
  int x, y, speed, length;
  bool active;
};
Raindrop raindrops[NUM_RAINDROPS];
bool raindropsInitialized = false;

void drawRaindrops() {
  if (!raindropsInitialized) {
    for (int i = 0; i < NUM_RAINDROPS; i++) {
      raindrops[i].active = false;
    }
    raindropsInitialized = true;
  }
  
  // Gentle fade for trails
  gfx->fillRect(0, 0, gfx->width(), gfx->height(), gfx->color565(20, 30, 50));
  
  for (int i = 0; i < NUM_RAINDROPS; i++) {
    if (!raindrops[i].active && random(100) < 3) {
      raindrops[i].x = random(gfx->width());
      raindrops[i].y = 0;
      raindrops[i].speed = random(2, 5);
      raindrops[i].length = random(5, 12);
      raindrops[i].active = true;
    }
    
    if (raindrops[i].active) {
      // Draw soft blue-white raindrop
      uint16_t dropColor = gfx->color565(150, 180, 220);
      
      for (int j = 0; j < raindrops[i].length; j++) {
        int py = raindrops[i].y - j;
        if (py >= 0 && py < getAnimationHeight()) {
          uint8_t alpha = 255 - (j * 255 / raindrops[i].length);
          uint16_t fadeColor = gfx->color565(alpha * 150 / 255, alpha * 180 / 255, alpha * 220 / 255);
          gfx->drawPixel(raindrops[i].x, py, fadeColor);
        }
      }
      
      raindrops[i].y += raindrops[i].speed;
      
      if (raindrops[i].y >= gfx->height()) {
        // Gentle ripple effect
        gfx->drawCircle(raindrops[i].x, gfx->height() - 2, 3, gfx->color565(100, 150, 200));
        gfx->drawCircle(raindrops[i].x, gfx->height() - 2, 5, gfx->color565(80, 120, 160));
        raindrops[i].active = false;
      }
    }
  }
}

void drawDragonCurve() {
  static int iteration = 0;
  static float angle = 0;
  angle += 0.02;
  
  fillScreenExceptClock(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Generate dragon curve using L-system
  String sequence = "F";
  int depth = 11; // Dragon curve iterations
  
  for (int i = 0; i < depth; i++) {
    String nextSeq = "";
    for (unsigned int j = 0; j < sequence.length(); j++) {
      if (sequence[j] == 'F') {
        nextSeq += "F+G";
      } else if (sequence[j] == 'G') {
        nextSeq += "F-G";
      } else {
        nextSeq += sequence[j];
      }
    }
    sequence = nextSeq;
  }
  
  // Draw the dragon curve
  float x = centerX - 60;
  float y = centerY;
  float currentAngle = angle;
  float stepSize = 2.0;
  
  for (unsigned int i = 0; i < sequence.length() && i < 2000; i++) {
    if (sequence[i] == 'F' || sequence[i] == 'G') {
      float newX = x + cos(currentAngle) * stepSize;
      float newY = y + sin(currentAngle) * stepSize;
      
      uint8_t colorVal = (i * 2 + frameCount) % 256;
      uint16_t color = plasmaPalette[colorVal];
      
      if (newX >= 0 && newX < gfx->width() && newY >= 0 && newY < getAnimationHeight()) {
        gfx->drawLine(x, y, newX, newY, color);
      }
      
      x = newX;
      y = newY;
    } else if (sequence[i] == '+') {
      currentAngle += PI / 2;
    } else if (sequence[i] == '-') {
      currentAngle -= PI / 2;
    }
  }
  
  iteration++;
}

void drawTunnel() {
  static float time = 0;
  time += 0.05;
  
  fillScreenExceptClock(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw tunnel with perspective
  for (int y = 0; y < getAnimationHeight(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      int dx = x - centerX;
      int dy = y - centerY;
      
      float distance = sqrt(dx * dx + dy * dy);
      float angle = atan2(dy, dx);
      
      if (distance > 0) {
        // Tunnel texture coordinates
        float u = angle / PI + time;
        float v = 32.0 / distance + time * 2;
        
        // Create checkerboard tunnel pattern
        int checkX = (int)(u * 8) % 2;
        int checkY = (int)(v * 8) % 2;
        
        if (checkX == checkY) {
          // Color based on depth
          uint8_t colorVal = (uint8_t)(v * 50 + time * 30) % 256;
          uint16_t color = plasmaPalette[colorVal];
          gfx->fillRect(x, y, 2, 2, color);
        }
      }
    }
  }
}

void drawLissajous() {
  static float phase = 0;
  static float aFreq = 3;
  static float bFreq = 2;
  phase += 0.02;
  
  fillScreenExceptClock(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int radius = 100;
  
  // Draw multiple Lissajous curves
  for (int curve = 0; curve < 3; curve++) {
    float phaseOffset = curve * TWO_PI / 3;
    float prevX = 0, prevY = 0;
    
    for (float t = 0; t < TWO_PI; t += 0.05) {
      float x = centerX + sin(aFreq * t + phase + phaseOffset) * radius;
      float y = centerY + sin(bFreq * t + phase * 0.7) * radius * 0.6;
      
      if (t > 0) {
        uint8_t colorVal = (uint8_t)(t * 40 + phase * 20 + curve * 85) % 256;
        uint16_t color = plasmaPalette[colorVal];
        gfx->drawLine(prevX, prevY, x, y, color);
      }
      
      prevX = x;
      prevY = y;
    }
  }
  
  // Change frequencies over time
  if (frameCount % 300 == 0) {
    aFreq = random(2, 6);
    bFreq = random(2, 6);
  }
}

struct Firework {
  float x, y, vy;
  bool active, exploded;
  uint16_t color;
  struct { float x, y, vx, vy; uint8_t life; bool active; uint16_t color; } particles[12];
};

void drawFireworks() {
  static Firework fireworks[3];
  
  // Fade background
  gfx->fillRect(0, 0, gfx->width(), gfx->height(), gfx->color565(0, 0, 5));
  
  for (int i = 0; i < 3; i++) {
    // Launch new firework
    if (!fireworks[i].active && random(100) < 2) {
      fireworks[i].x = random(60, gfx->width() - 60);
      fireworks[i].y = gfx->height();
      fireworks[i].vy = -random(8, 14);
      fireworks[i].exploded = false;
      fireworks[i].active = true;
      fireworks[i].color = plasmaPalette[random(256)];
    }
    
    if (fireworks[i].active && !fireworks[i].exploded) {
      // Draw rising firework
      gfx->fillCircle(fireworks[i].x, fireworks[i].y, 2, fireworks[i].color);
      
      fireworks[i].y += fireworks[i].vy;
      fireworks[i].vy += 0.3; // gravity
      
      // Explode when velocity reverses
      if (fireworks[i].vy > 0 || fireworks[i].y < 60) {
        fireworks[i].exploded = true;
        
        // Create explosion particles
        for (int p = 0; p < 12; p++) {
          float angle = (TWO_PI / 12) * p;
          float speed = random(2, 6);
          fireworks[i].particles[p].x = fireworks[i].x;
          fireworks[i].particles[p].y = fireworks[i].y;
          fireworks[i].particles[p].vx = cos(angle) * speed;
          fireworks[i].particles[p].vy = sin(angle) * speed;
          fireworks[i].particles[p].life = 255;
          fireworks[i].particles[p].color = fireworks[i].color;
          fireworks[i].particles[p].active = true;
        }
      }
    }
    
    if (fireworks[i].exploded) {
      bool anyActive = false;
      
      for (int p = 0; p < 12; p++) {
        if (fireworks[i].particles[p].active) {
          auto* part = &fireworks[i].particles[p];
          
          // Draw particle
          if (part->life > 50) {
            gfx->drawPixel(part->x, part->y, part->color);
          }
          
          part->x += part->vx;
          part->y += part->vy;
          part->vy += 0.1; // gravity
          part->life -= 5;
          
          if (part->life <= 0) {
            part->active = false;
          } else {
            anyActive = true;
          }
        }
      }
      
      if (!anyActive) {
        fireworks[i].active = false;
      }
    }
  }
}

void drawLightning() {
  static unsigned long lastStrike = 0;
  static bool striking = false;
  static int strikeFrame = 0;
  
  unsigned long now = millis();
  
  // Random lightning strikes
  if (!striking && random(100) < 3) {
    striking = true;
    strikeFrame = 0;
    lastStrike = now;
  }
  
  if (striking) {
    // Flash effect
    if (strikeFrame < 2) {
      fillScreenExceptClock(gfx->color565(200, 200, 255));
    } else {
      fillScreenExceptClock(gfx->color565(20, 20, 40));
    }
    
    // Draw lightning bolts
    for (int bolt = 0; bolt < 2; bolt++) {
      int x = random(60, gfx->width() - 60);
      int y = 0;
      
      // Draw bolt in segments
      for (int segment = 0; segment < 15 && y < getAnimationHeight() - 10; segment++) {
        int nextX = x + random(-15, 16);
        int nextY = y + random(12, 22);
        
        nextX = constrain(nextX, 5, gfx->width() - 5);
        nextY = constrain(nextY, y + 10, gfx->height());
        
        // Main bolt
        gfx->drawLine(x, y, nextX, nextY, WHITE);
        
        // Glow effect
        if (nextX > 1 && nextX < gfx->width() - 2) {
          gfx->drawLine(x - 1, y, nextX - 1, nextY, gfx->color565(150, 150, 255));
          gfx->drawLine(x + 1, y, nextX + 1, nextY, gfx->color565(150, 150, 255));
        }
        
        x = nextX;
        y = nextY;
      }
    }
    
    strikeFrame++;
    if (strikeFrame > 5) {
      striking = false;
    }
  } else {
    fillScreenExceptClock(gfx->color565(20, 20, 40));
  }
}

void drawHypnoVortex() {
  static float time = 0;
  static float rotation = 0;
  time += 0.05;
  rotation += 0.02;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  for (int y = 0; y < getAnimationHeight(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      int dx = x - centerX;
      int dy = y - centerY;
      
      float radius = sqrt(dx * dx + dy * dy);
      float angle = atan2(dy, dx);
      
      // Polar perlin-like noise
      float noise = sin(radius * 0.1 + time) * 
                    sin(angle * 3 + rotation) * 
                    cos(angle * 5 - time * 0.7);
      
      // Vortex distortion
      float vortexAngle = angle + radius * 0.01 + time + noise * 0.5;
      
      uint8_t colorVal = (uint8_t)((sin(vortexAngle * 2 + time) + 1) * 127) % 256;
      uint16_t color = plasmaPalette[colorVal];
      
      gfx->fillRect(x, y, 2, 2, color);
    }
  }
}

struct VoronoiPoint {
  float x, y, vx, vy;
  uint16_t color;
};

void drawVoronoiLava() {
  static VoronoiPoint voronoiPoints[8];
  static bool initialized = false;
  static float pulsePhase = 0;
  pulsePhase += 0.03;
  
  // Initialize points
  if (!initialized) {
    for (int i = 0; i < 8; i++) {
      voronoiPoints[i].x = random(gfx->width());
      voronoiPoints[i].y = random(getAnimationHeight());
      voronoiPoints[i].vx = random(-2, 3) * 0.5;
      voronoiPoints[i].vy = random(-2, 3) * 0.5;
      voronoiPoints[i].color = plasmaPalette[random(256)];
    }
    initialized = true;
  }
  
  // Update points
  for (int i = 0; i < 8; i++) {
    voronoiPoints[i].x += voronoiPoints[i].vx;
    voronoiPoints[i].y += voronoiPoints[i].vy;
    
    if (voronoiPoints[i].x < 0 || voronoiPoints[i].x >= gfx->width()) {
      voronoiPoints[i].vx *= -1;
      voronoiPoints[i].x = constrain(voronoiPoints[i].x, 0, gfx->width() - 1);
    }
    if (voronoiPoints[i].y < 0 || voronoiPoints[i].y >= gfx->height()) {
      voronoiPoints[i].vy *= -1;
      voronoiPoints[i].y = constrain(voronoiPoints[i].y, 0, gfx->height() - 1);
    }
  }
  
  // Draw voronoi cells
  for (int y = 0; y < getAnimationHeight(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      float minDist = 10000;
      int closestPoint = 0;
      
      for (int i = 0; i < 8; i++) {
        float dx = x - voronoiPoints[i].x;
        float dy = y - voronoiPoints[i].y;
        float dist = dx * dx + dy * dy;
        
        if (dist < minDist) {
          minDist = dist;
          closestPoint = i;
        }
      }
      
      // Lava-like pulsing
      float pulse = sin(sqrt(minDist) * 0.1 + pulsePhase) * 0.3 + 0.7;
      
      uint16_t baseColor = voronoiPoints[closestPoint].color;
      uint8_t r = ((baseColor >> 11) & 0x1F) * pulse;
      uint8_t g = ((baseColor >> 5) & 0x3F) * pulse;
      uint8_t b = (baseColor & 0x1F) * pulse;
      
      uint16_t pulsedColor = ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
      gfx->fillRect(x, y, 2, 2, pulsedColor);
    }
  }
}

void drawAurora() {
  static float time = 0;
  time += 0.03;
  
  fillScreenExceptClock(gfx->color565(0, 0, 20));
  
  // Draw multiple aurora curtains
  for (int curtain = 0; curtain < 5; curtain++) {
    float curtainPhase = curtain * 0.7;
    
    for (int x = 0; x < gfx->width(); x += 2) {
      // Sine wave curtain position
      float wave1 = sin(x * 0.05 + time + curtainPhase) * 30;
      float wave2 = sin(x * 0.03 - time * 0.7 + curtainPhase) * 20;
      float baseY = 80 + curtain * 25 + wave1 + wave2;
      
      // Vertical spread with gradient
      for (int spread = -15; spread < 15; spread++) {
        int y = baseY + spread;
        if (y >= 0 && y < getAnimationHeight()) {
          float intensity = 1.0 - abs(spread) / 15.0;
          intensity *= (sin(x * 0.1 + time + curtain) * 0.3 + 0.7);
          
          uint8_t colorVal = (uint8_t)(curtain * 50 + time * 20 + x * 0.5) % 256;
          uint16_t auroraColor = plasmaPalette[colorVal];
          
          uint8_t r = ((auroraColor >> 11) & 0x1F) * intensity;
          uint8_t g = ((auroraColor >> 5) & 0x3F) * intensity;
          uint8_t b = (auroraColor & 0x1F) * intensity;
          
          uint16_t fadedColor = ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
          gfx->drawPixel(x, y, fadedColor);
        }
      }
    }
  }
}

void drawMoireMandala() {
  static float angle1 = 0;
  static float angle2 = 0;
  angle1 += 0.02;
  angle2 -= 0.015;
  
  fillScreenExceptClock(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw first mandala layer
  for (int i = 0; i < 24; i++) {
    float a = angle1 + i * TWO_PI / 24;
    for (int r = 20; r < 120; r += 8) {
      int x1 = centerX + cos(a) * r;
      int y1 = centerY + sin(a) * r;
      int x2 = centerX + cos(a) * (r + 6);
      int y2 = centerY + sin(a) * (r + 6);
      
      uint16_t color = plasmaPalette[(uint8_t)(i * 10 + r) % 256];
      gfx->drawLine(x1, y1, x2, y2, color);
    }
  }
  
  // Draw second mandala layer (creates Moiré)
  for (int i = 0; i < 24; i++) {
    float a = angle2 + i * TWO_PI / 24 + PI / 24;
    for (int r = 20; r < 120; r += 8) {
      int x1 = centerX + cos(a) * r;
      int y1 = centerY + sin(a) * r;
      int x2 = centerX + cos(a) * (r + 6);
      int y2 = centerY + sin(a) * (r + 6);
      
      uint16_t color = plasmaPalette[(uint8_t)(i * 10 + r + 128) % 256];
      gfx->drawLine(x1, y1, x2, y2, color);
    }
  }
}

struct RingSource {
  int x, y;
  float phase, speed;
};

void drawInterferenceRings() {
  static RingSource ringSources[3];
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 3; i++) {
      ringSources[i].x = random(60, gfx->width() - 60);
      ringSources[i].y = random(60, gfx->height() - 60);
      ringSources[i].phase = 0;
      ringSources[i].speed = random(5, 15) * 0.1;
    }
    initialized = true;
  }
  
  fillScreenExceptClock(BLACK);
  
  // Update phases
  for (int i = 0; i < 3; i++) {
    ringSources[i].phase += ringSources[i].speed;
  }
  
  // Draw interference pattern
  for (int y = 0; y < getAnimationHeight(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      float totalWave = 0;
      
      // Sum waves from all sources
      for (int i = 0; i < 3; i++) {
        float dx = x - ringSources[i].x;
        float dy = y - ringSources[i].y;
        float distance = sqrt(dx * dx + dy * dy);
        
        totalWave += sin(distance * 0.2 - ringSources[i].phase);
      }
      
      // Normalize and colorize
      totalWave = (totalWave + 3) / 6;
      uint8_t colorVal = (uint8_t)(totalWave * 255) % 256;
      
      if (totalWave > 0.3) {
        gfx->fillRect(x, y, 2, 2, plasmaPalette[colorVal]);
      }
    }
  }
}

void drawCRTVector() {
  static float angle = 0;
  static float scale = 1.0;
  static bool growing = true;
  angle += 0.03;
  
  // Fade background for trails
  gfx->fillRect(0, 0, gfx->width(), gfx->height(), gfx->color565(0, 2, 0));
  
  scale += growing ? 0.02 : -0.02;
  if (scale > 1.5) growing = false;
  if (scale < 0.5) growing = true;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw vector star with glow
  for (int i = 0; i < 8; i++) {
    float a1 = angle + i * TWO_PI / 8;
    float a2 = angle + (i + 1) * TWO_PI / 8;
    
    int x1 = centerX + cos(a1) * 80 * scale;
    int y1 = centerY + sin(a1) * 80 * scale;
    int x2 = centerX + cos(a2) * 80 * scale;
    int y2 = centerY + sin(a2) * 80 * scale;
    
    // Bright core
    gfx->drawLine(x1, y1, x2, y2, gfx->color565(0, 255, 0));
    
    // Glow effect
    gfx->drawLine(x1 + 1, y1, x2 + 1, y2, gfx->color565(0, 180, 0));
    gfx->drawLine(x1 - 1, y1, x2 - 1, y2, gfx->color565(0, 180, 0));
    gfx->drawLine(x1, y1 + 1, x2, y2 + 1, gfx->color565(0, 180, 0));
    gfx->drawLine(x1, y1 - 1, x2, y2 - 1, gfx->color565(0, 180, 0));
  }
  
  // Draw inner spirograph
  float prevX = 0, prevY = 0;
  for (float t = 0; t < TWO_PI; t += 0.2) {
    float r = 40 + sin(t * 3 + angle) * 10;
    float x = centerX + cos(t) * r * scale;
    float y = centerY + sin(t) * r * scale;
    
    if (t > 0) {
      gfx->drawLine(prevX, prevY, x, y, gfx->color565(0, 120, 255));
    }
    prevX = x;
    prevY = y;
  }
}

void drawLaserShow() {
  static float phase1 = 0, phase2 = 0;
  phase1 += 0.07;
  phase2 += 0.05;
  
  // Dark background with fade
  gfx->fillRect(0, 0, gfx->width(), gfx->height(), gfx->color565(0, 0, 5));
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // Draw laser lissajous
  float prevX = 0, prevY = 0;
  for (float t = 0; t < TWO_PI; t += 0.08) {
    float x = centerX + sin(3 * t + phase1) * 90;
    float y = centerY + sin(2 * t + phase2) * 70;
    
    if (t > 0) {
      uint8_t colorVal = (uint8_t)(t * 40 + phase1 * 20) % 256;
      uint16_t beamColor = plasmaPalette[colorVal];
      
      // Laser beam with bloom
      gfx->drawLine(prevX, prevY, x, y, beamColor);
      gfx->drawLine(prevX + 1, prevY, x + 1, y, beamColor);
      gfx->drawLine(prevX, prevY + 1, x, y + 1, beamColor);
    }
    prevX = x;
    prevY = y;
  }
  
  // Add laser dots at key points
  for (int i = 0; i < 6; i++) {
    float a = phase1 + i * TWO_PI / 6;
    int x = centerX + cos(a) * 80;
    int y = centerY + sin(a) * 60;
    gfx->fillCircle(x, y, 3, WHITE);
    gfx->drawCircle(x, y, 5, plasmaPalette[(uint8_t)(i * 40) % 256]);
  }
}

struct SmokeParticle {
  float x, y, vx, vy;
  uint8_t life;
  bool active;
};

void drawSmokeTrails() {
  static SmokeParticle smokeParticles[20];
  static bool initialized = false;
  
  if (!initialized) {
    for (int i = 0; i < 20; i++) {
      smokeParticles[i].active = false;
    }
    initialized = true;
  }
  
  // Fade background
  gfx->fillRect(0, 0, gfx->width(), gfx->height(), gfx->color565(5, 5, 10));
  
  // Spawn new particles
  for (int i = 0; i < 20; i++) {
    if (!smokeParticles[i].active && random(100) < 10) {
      smokeParticles[i].x = gfx->width() / 2 + random(-20, 21);
      smokeParticles[i].y = gfx->height() - 10;
      smokeParticles[i].vx = random(-10, 11) / 10.0;
      smokeParticles[i].vy = -random(5, 15) / 10.0;
      smokeParticles[i].life = 255;
      smokeParticles[i].active = true;
    }
  }
  
  // Update and draw particles
  for (int i = 0; i < 20; i++) {
    if (smokeParticles[i].active) {
      // Drift and rise
      smokeParticles[i].x += smokeParticles[i].vx + sin(smokeParticles[i].y * 0.1) * 0.5;
      smokeParticles[i].y += smokeParticles[i].vy;
      smokeParticles[i].vx *= 0.98; // Damping
      smokeParticles[i].vy *= 0.99;
      smokeParticles[i].life -= 3;
      
      if (smokeParticles[i].life > 0 && smokeParticles[i].y > 0) {
        uint8_t alpha = smokeParticles[i].life;
        uint16_t smokeColor = gfx->color565(alpha, alpha, alpha + 20);
        
        // Draw wispy smoke
        gfx->drawPixel(smokeParticles[i].x, smokeParticles[i].y, smokeColor);
        gfx->drawPixel(smokeParticles[i].x + 1, smokeParticles[i].y, smokeColor);
        gfx->drawPixel(smokeParticles[i].x, smokeParticles[i].y + 1, smokeColor);
      } else {
        smokeParticles[i].active = false;
      }
    }
  }
}

void drawGlitchArt() {
  static int glitchTimer = 0;
  glitchTimer++;
  
  if (glitchTimer % 3 == 0) {
    // Base pattern
    for (int y = 0; y < getAnimationHeight(); y += 4) {
      for (int x = 0; x < gfx->width(); x += 4) {
        uint8_t val = (x + y + glitchTimer) % 256;
        gfx->fillRect(x, y, 4, 4, plasmaPalette[val]);
      }
    }
  }
  
  // Random glitches
  if (random(100) < 30) {
    int glitchType = random(4);
    
    switch (glitchType) {
      case 0: // Scanline shift
        {
          int line = random(getAnimationHeight());
          int shift = random(-20, 21);
          for (int x = 0; x < gfx->width(); x += 8) {
            uint8_t val = random(256);
            gfx->fillRect(x + shift, line, 8, 2, plasmaPalette[val]);
          }
        }
        break;
        
      case 1: // RGB channel separation
        {
          int x = random(gfx->width() - 40);
          int y = random(gfx->height() - 40);
          gfx->fillRect(x, y, 40, 40, gfx->color565(255, 0, 0));
          gfx->fillRect(x + 3, y, 40, 40, gfx->color565(0, 255, 0));
          gfx->fillRect(x + 6, y, 40, 40, gfx->color565(0, 0, 255));
        }
        break;
        
      case 2: // Random blocks
        {
          for (int i = 0; i < 5; i++) {
            int bx = random(gfx->width() - 20);
            int by = random(gfx->height() - 20);
            int bw = random(10, 30);
            int bh = random(5, 20);
            gfx->fillRect(bx, by, bw, bh, plasmaPalette[random(256)]);
          }
        }
        break;
        
      case 3: // Static noise
        {
          for (int i = 0; i < 50; i++) {
            int x = random(gfx->width());
            int y = random(getAnimationHeight());
            gfx->drawPixel(x, y, random(65536));
          }
        }
        break;
    }
  }
}

void drawWatercolor() {
  static float time = 0;
  static uint8_t canvas[40][60]; // Reduced size for ESP32
  static bool initialized = false;
  time += 0.02;
  
  if (!initialized) {
    memset(canvas, 0, sizeof(canvas));
    initialized = true;
  }
  
  // Add new "paint drops"
  if (random(100) < 5) {
    int cx = random(5, 55);
    int cy = random(5, 35);
    uint8_t color = random(256);
    
    for (int r = 0; r < 6; r++) {
      for (int a = 0; a < 8; a++) {
        float angle = a * TWO_PI / 8;
        int x = cx + cos(angle) * r;
        int y = cy + sin(angle) * r;
        
        if (x >= 0 && x < 60 && y >= 0 && y < 40) {
          canvas[y][x] = color;
        }
      }
    }
  }
  
  // Diffuse (watercolor spread)
  uint8_t newCanvas[40][60];
  for (int y = 1; y < 39; y++) {
    for (int x = 1; x < 59; x++) {
      int sum = canvas[y][x] * 4 + 
                canvas[y-1][x] + canvas[y+1][x] + 
                canvas[y][x-1] + canvas[y][x+1];
      newCanvas[y][x] = sum / 8;
    }
  }
  memcpy(canvas, newCanvas, sizeof(canvas));
  
  // Draw canvas
  fillScreenExceptClock(gfx->color565(240, 240, 230)); // Paper color
  for (int y = 0; y < 40; y++) {
    for (int x = 0; x < 60; x++) {
      if (canvas[y][x] > 10) {
        uint16_t color = plasmaPalette[canvas[y][x]];
        gfx->fillRect(x * 5, y * 8, 5, 8, color);
      }
    }
  }
}

void drawQuasicrystal() {
  static float phase = 0;
  phase += 0.02;
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  
  // 7-fold symmetry quasicrystal
  for (int y = 0; y < getAnimationHeight(); y += 2) {
    for (int x = 0; x < gfx->width(); x += 2) {
      int dx = x - centerX;
      int dy = y - centerY;
      
      float sum = 0;
      // Interference of 7 waves
      for (int k = 0; k < 7; k++) {
        float angle = k * TWO_PI / 7 + phase;
        float wave = cos(dx * cos(angle) * 0.05 + dy * sin(angle) * 0.05 + phase);
        sum += wave;
      }
      
      uint8_t colorVal = (uint8_t)((sum + 7) * 18) % 256;
      gfx->fillRect(x, y, 2, 2, plasmaPalette[colorVal]);
    }
  }
}

void drawHyperbolicGrid() {
  static float rotation = 0;
  rotation += 0.01;
  
  fillScreenExceptClock(BLACK);
  
  int centerX = gfx->width() / 2;
  int centerY = gfx->height() / 2;
  int radius = 110;
  
  // Poincaré disk model - draw hyperbolic lines
  for (int i = 0; i < 12; i++) {
    float angle = rotation + i * TWO_PI / 12;
    
    // Draw geodesics (circles orthogonal to boundary)
    for (float t = -0.9; t < 0.9; t += 0.1) {
      float x1 = centerX + cos(angle) * t * radius;
      float y1 = centerY + sin(angle) * t * radius;
      float x2 = centerX + cos(angle) * (t + 0.1) * radius;
      float y2 = centerY + sin(angle) * (t + 0.1) * radius;
      
      gfx->drawLine(x1, y1, x2, y2, plasmaPalette[(uint8_t)(i * 20) % 256]);
    }
  }
  
  // Draw radial circles
  for (int r = 20; r < radius; r += 20) {
    gfx->drawCircle(centerX, centerY, r, plasmaPalette[(uint8_t)(r * 2 + rotation * 50) % 256]);
  }
}

void drawStrangeAttractor() {
  static float x = 0.1, y = 0, z = 0;
  static float time = 0;
  time += 0.01;
  
  gfx->fillRect(0, 0, gfx->width(), gfx->height(), gfx->color565(0, 0, 2));
  
  // Lorenz attractor parameters
  float sigma = 10.0;
  float rho = 28.0;
  float beta = 8.0 / 3.0;
  float dt = 0.01;
  
  // Draw multiple steps per frame
  for (int step = 0; step < 50; step++) {
    float dx = sigma * (y - x);
    float dy = x * (rho - z) - y;
    float dz = x * y - beta * z;
    
    x += dx * dt;
    y += dy * dt;
    z += dz * dt;
    
    // Project to 2D
    int px = gfx->width() / 2 + x * 4;
    int py = gfx->height() / 2 - y * 4;
    
    if (px >= 0 && px < gfx->width() && py >= 0 && py < getAnimationHeight()) {
      uint8_t colorVal = (uint8_t)(z * 3 + time * 20) % 256;
      gfx->drawPixel(px, py, plasmaPalette[colorVal]);
      gfx->drawPixel(px + 1, py, plasmaPalette[colorVal]);
    }
  }
}

void drawCellularAutomata() {
  static uint8_t cells[320];
  static bool initialized = false;
  static int generation = 0;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  // Reset when mode changes
  if (currentMode != lastMode) {
    initialized = false;
    generation = 0;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < gfx->width(); i++) {
      cells[i] = random(2);
    }
    cells[gfx->width()/2] = 1;
    initialized = true;
  }
  
  // Draw multiple generations per frame for full screen coverage
  for (int gen = 0; gen < 4; gen++) {
    int y = generation % gfx->height();
    for (int x = 0; x < gfx->width(); x++) {
      uint16_t color = cells[x] ? 
        gfx->color565(255, 50 + sin(animationTime + x * 0.1) * 50, 150) :
        gfx->color565(10, 10, 20);
      gfx->drawPixel(x, y, color);
    }
    
    uint8_t newCells[320]; // Max width we support
    for (int i = 0; i < gfx->width(); i++) {
      int left = cells[(i - 1 + gfx->width()) % gfx->width()];
      int center = cells[i];
      int right = cells[(i + 1) % gfx->width()];
      newCells[i] = left ^ (center | right);
    }
    
    memcpy(cells, newCells, gfx->width());
    generation++;
    
    if (generation >= gfx->height()) {
      initialized = false;
      generation = 0;
      break;
    }
  }
}

void drawCrystalGrowth() {
  static struct Crystal {
    float x, y, size;
    uint16_t color;
    bool active;
  } crystals[50];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  // Reset when mode changes
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 50; i++) {
      crystals[i].active = false;
    }
    initialized = true;
  }
  
  // Dark fade effect for full screen
  for (int i = 0; i < 500; i++) {
    int x = random(gfx->width());
    int y = random(getAnimationHeight());
    uint16_t color = gfx->color565(2, 2, 5);
    gfx->drawPixel(x, y, color);
  }
  
  // Spawn more crystals for better coverage
  if (random(100) < 15) {
    for (int i = 0; i < 50; i++) {
      if (!crystals[i].active) {
        crystals[i].x = random(gfx->width());
        crystals[i].y = random(getAnimationHeight());
        crystals[i].size = 1;
        crystals[i].color = gfx->color565(100 + random(155), 50 + random(100), 150 + random(105));
        crystals[i].active = true;
        break;
      }
    }
  }
  
  for (int i = 0; i < 50; i++) {
    if (crystals[i].active) {
      crystals[i].size += 0.2;
      
      int points = 6;
      for (int p = 0; p < points; p++) {
        float angle = (TWO_PI / points) * p + animationTime * 0.1;
        int x1 = crystals[i].x + cos(angle) * crystals[i].size;
        int y1 = crystals[i].y + sin(angle) * crystals[i].size;
        int x2 = crystals[i].x + cos(angle + 0.5) * crystals[i].size * 0.7;
        int y2 = crystals[i].y + sin(angle + 0.5) * crystals[i].size * 0.7;
        
        if (x1 >= 0 && x1 < gfx->width() && y1 >= 0 && y1 < getAnimationHeight()) {
          gfx->drawPixel(x1, y1, crystals[i].color);
        }
        if (x2 >= 0 && x2 < gfx->width() && y2 >= 0 && y2 < getAnimationHeight()) {
          gfx->drawPixel(x2, y2, crystals[i].color);
        }
      }
      
      if (crystals[i].size > 25) {
        crystals[i].active = false;
      }
    }
  }
}

void drawQuantumTunnels() {
  static struct Tunnel {
    float x, y, z, dx, dy, dz;
    uint16_t color;
    int life;
  } tunnels[30];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  // Reset when mode changes
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 30; i++) {
      tunnels[i].x = 120;
      tunnels[i].y = 160;
      tunnels[i].z = random(100) + 50;
      tunnels[i].dx = (random(200) - 100) / 100.0;
      tunnels[i].dy = (random(200) - 100) / 100.0;
      tunnels[i].dz = (random(100) + 50) / 100.0;
      tunnels[i].color = gfx->color565(100 + random(155), 50 + random(155), 100 + random(155));
      tunnels[i].life = random(200) + 100;
    }
    initialized = true;
  }
  
  // Full screen quantum field background
  for (int i = 0; i < 1000; i++) {
    int x = random(240);
    int y = random(320);
    float quantum = sin(x * 0.05 + animationTime) * cos(y * 0.05 + animationTime * 0.7);
    uint16_t color = gfx->color565(
      abs(quantum) * 40,
      abs(quantum) * 25,
      abs(quantum) * 60
    );
    gfx->drawPixel(x, y, color);
  }
  
  for (int i = 0; i < 30; i++) {
    tunnels[i].dx += (random(20) - 10) / 1000.0;
    tunnels[i].dy += (random(20) - 10) / 1000.0;
    tunnels[i].x += tunnels[i].dx;
    tunnels[i].y += tunnels[i].dy;
    tunnels[i].z += tunnels[i].dz;
    
    if (tunnels[i].z > 0) {
      int screenX = 120 + (tunnels[i].x - 120) * (200.0 / tunnels[i].z);
      int screenY = 160 + (tunnels[i].y - 160) * (200.0 / tunnels[i].z);
      
      if (screenX >= 0 && screenX < 320 && screenY >= 0 && screenY < 240) {
        int size = max(2, (int)(15.0 / tunnels[i].z * 10));
        gfx->fillCircle(screenX, screenY, size, tunnels[i].color);
        // Add glow effect
        gfx->drawCircle(screenX, screenY, size + 1, tunnels[i].color);
      }
    }
    
    tunnels[i].life--;
    if (tunnels[i].life <= 0 || tunnels[i].z <= 0 || tunnels[i].z > 200) {
      tunnels[i].x = 120 + (random(200) - 100);
      tunnels[i].y = 160 + (random(200) - 100);
      tunnels[i].z = random(50) + 150;
      tunnels[i].dx = (random(200) - 100) / 100.0;
      tunnels[i].dy = (random(200) - 100) / 100.0;
      tunnels[i].dz = -(random(100) + 50) / 100.0;
      tunnels[i].life = random(200) + 100;
      tunnels[i].color = gfx->color565(100 + random(155), 50 + random(155), 100 + random(155));
    }
  }
}

void drawFractalSparks() {
  static struct Spark {
    float x, y, dx, dy;
    uint16_t color;
    int life, maxLife;
    bool active;
  } sparks[60];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 60; i++) {
      sparks[i].active = false;
    }
    initialized = true;
  }
  
  // Dark fade
  for (int i = 0; i < 300; i++) {
    int x = random(240);
    int y = random(320);
    gfx->drawPixel(x, y, gfx->color565(1, 1, 3));
  }
  
  // Spawn sparks from fractal points
  if (random(100) < 20) {
    for (int i = 0; i < 60; i++) {
      if (!sparks[i].active) {
        // Mandelbrot-based spawn points
        float cx = -0.7 + sin(animationTime * 0.1) * 0.3;
        float cy = cos(animationTime * 0.07) * 0.3;
        
        sparks[i].x = 120 + cx * 80;
        sparks[i].y = 160 + cy * 80;
        sparks[i].dx = (random(200) - 100) / 50.0;
        sparks[i].dy = (random(200) - 100) / 50.0;
        sparks[i].life = sparks[i].maxLife = random(100) + 50;
        sparks[i].color = gfx->color565(
          150 + random(105),
          100 + random(100),
          50 + random(155)
        );
        sparks[i].active = true;
        break;
      }
    }
  }
  
  // Update and draw sparks
  for (int i = 0; i < 60; i++) {
    if (sparks[i].active) {
      sparks[i].x += sparks[i].dx;
      sparks[i].y += sparks[i].dy;
      sparks[i].dx *= 0.98;
      sparks[i].dy *= 0.98;
      sparks[i].life--;
      
      // Draw spark with fractal trail
      if (sparks[i].x >= 0 && sparks[i].x < 320 && sparks[i].y >= 0 && sparks[i].y < 240) {
        float intensity = (float)sparks[i].life / sparks[i].maxLife;
        uint8_t r = ((sparks[i].color >> 11) & 0x1F) * intensity;
        uint8_t g = ((sparks[i].color >> 5) & 0x3F) * intensity;
        uint8_t b = (sparks[i].color & 0x1F) * intensity;
        
        gfx->drawPixel(sparks[i].x, sparks[i].y, gfx->color565(r, g, b));
        
        // Fractal branching
        if (random(100) < 10 && sparks[i].life > 20) {
          for (int j = 0; j < 60; j++) {
            if (!sparks[j].active) {
              sparks[j].x = sparks[i].x;
              sparks[j].y = sparks[i].y;
              sparks[j].dx = sparks[i].dx * 0.7 + (random(60) - 30) / 30.0;
              sparks[j].dy = sparks[i].dy * 0.7 + (random(60) - 30) / 30.0;
              sparks[j].life = sparks[j].maxLife = sparks[i].life / 2;
              sparks[j].color = sparks[i].color;
              sparks[j].active = true;
              break;
            }
          }
        }
      }
      
      if (sparks[i].life <= 0) {
        sparks[i].active = false;
      }
    }
  }
}

void drawVoronoiBloom() {
  static struct Seed {
    float x, y;
    uint16_t color;
    float pulse;
  } seeds[12];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 12; i++) {
      seeds[i].x = random(320);
      seeds[i].y = random(240);
      seeds[i].color = gfx->color565(
        50 + random(205),
        30 + random(150),
        80 + random(175)
      );
      seeds[i].pulse = random(100) / 10.0;
    }
    initialized = true;
  }
  
  // Update seed positions with organic movement
  for (int i = 0; i < 12; i++) {
    seeds[i].x += sin(animationTime * 0.1 + i) * 0.5;
    seeds[i].y += cos(animationTime * 0.07 + i * 1.3) * 0.4;
    seeds[i].pulse += 0.1;
    
    // Wrap around screen
    if (seeds[i].x < 0) seeds[i].x = 320;
    if (seeds[i].x >= 320) seeds[i].x = 0;
    if (seeds[i].y < 0) seeds[i].y = 240;
    if (seeds[i].y >= 240) seeds[i].y = 0;
  }
  
  // Draw Voronoi diagram with blooming effect
  for (int y = 0; y < 240; y += 2) {
    for (int x = 0; x < 320; x += 2) {
      float minDist = 999999;
      int closestSeed = 0;
      
      // Find closest seed
      for (int i = 0; i < 12; i++) {
        float dx = x - seeds[i].x;
        float dy = y - seeds[i].y;
        float dist = dx * dx + dy * dy;
        if (dist < minDist) {
          minDist = dist;
          closestSeed = i;
        }
      }
      
      // Calculate bloom intensity based on distance and pulse
      float dist = sqrt(minDist);
      float bloom = sin(seeds[closestSeed].pulse + dist * 0.05) * 0.5 + 0.5;
      bloom *= max(0.0f, 1.0f - dist / 100.0f);
      
      uint8_t r = ((seeds[closestSeed].color >> 11) & 0x1F) * bloom;
      uint8_t g = ((seeds[closestSeed].color >> 5) & 0x3F) * bloom;
      uint8_t b = (seeds[closestSeed].color & 0x1F) * bloom;
      
      gfx->drawPixel(x, y, gfx->color565(r, g, b));
      gfx->drawPixel(x + 1, y, gfx->color565(r, g, b));
      gfx->drawPixel(x, y + 1, gfx->color565(r, g, b));
      gfx->drawPixel(x + 1, y + 1, gfx->color565(r, g, b));
    }
  }
}

void drawPerlinNebula() {
  static float timeOffset = 0;
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  timeOffset += 0.02;
  
  // Simplified Perlin-like noise using sin waves
  for (int y = 0; y < 240; y += 3) {
    for (int x = 0; x < 320; x += 3) {
      // Multi-octave noise simulation
      float noise = 0;
      float amplitude = 1.0;
      float frequency = 0.01;
      
      for (int octave = 0; octave < 4; octave++) {
        noise += sin((x + timeOffset * 10) * frequency) * 
                cos((y + timeOffset * 7) * frequency) * amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
      }
      
      // Add swirling motion
      float swirl = sin(x * 0.02 + timeOffset * 2) + cos(y * 0.03 + timeOffset * 1.5);
      noise += swirl * 0.3;
      
      // Normalize and create nebula colors
      noise = (noise + 2.0) / 4.0;
      noise = max(0.0f, min(1.0f, noise));
      
      // Nebula color palette (purple, blue, pink)
      uint8_t r, g, b;
      if (noise < 0.3) {
        r = 10 + noise * 80;
        g = 5 + noise * 30;
        b = 30 + noise * 150;
      } else if (noise < 0.6) {
        r = 50 + (noise - 0.3) * 200;
        g = 20 + (noise - 0.3) * 100;
        b = 80 + (noise - 0.3) * 120;
      } else {
        r = 150 + (noise - 0.6) * 100;
        g = 50 + (noise - 0.6) * 150;
        b = 100 + (noise - 0.6) * 100;
      }
      
      uint16_t color = gfx->color565(r, g, b);
      
      // Draw 3x3 blocks for performance
      for (int dy = 0; dy < 3 && (y + dy) < 240; dy++) {
        for (int dx = 0; dx < 3 && (x + dx) < 320; dx++) {
          gfx->drawPixel(x + dx, y + dy, color);
        }
      }
    }
  }
}

void drawFractalMosaic() {
  static struct Tile {
    float x, y, size, rotation;
    uint16_t color;
    bool active;
  } tiles[40];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 40; i++) {
      tiles[i].active = false;
    }
    initialized = true;
  }
  
  // Dark background fade
  for (int i = 0; i < 200; i++) {
    int x = random(320);
    int y = random(240);
    gfx->drawPixel(x, y, gfx->color565(2, 2, 5));
  }
  
  // Spawn new tiles
  if (random(100) < 10) {
    for (int i = 0; i < 40; i++) {
      if (!tiles[i].active) {
        tiles[i].x = random(320);
        tiles[i].y = random(240);
        tiles[i].size = random(20) + 10;
        tiles[i].rotation = random(360);
        tiles[i].color = gfx->color565(
          random(200) + 55,
          random(150) + 50,
          random(200) + 55
        );
        tiles[i].active = true;
        break;
      }
    }
  }
  
  // Update and draw fractal tiles
  for (int i = 0; i < 40; i++) {
    if (tiles[i].active) {
      tiles[i].rotation += 2;
      tiles[i].size += 0.5;
      
      // Draw fractal tile pattern
      float rad = tiles[i].rotation * PI / 180.0;
      for (int side = 0; side < 4; side++) {
        float sideAngle = rad + (side * PI / 2);
        
        // Main square
        for (int p = 0; p < 8; p++) {
          float px = tiles[i].x + cos(sideAngle) * (tiles[i].size * p / 8);
          float py = tiles[i].y + sin(sideAngle) * (tiles[i].size * p / 8);
          
          if (px >= 0 && px < 320 && py >= 0 && py < 240) {
            gfx->drawPixel(px, py, tiles[i].color);
          }
          
          // Fractal subdivisions
          if (p % 2 == 0 && tiles[i].size > 15) {
            float fx = px + cos(sideAngle + PI/4) * 5;
            float fy = py + sin(sideAngle + PI/4) * 5;
            if (fx >= 0 && fx < 320 && fy >= 0 && fy < 240) {
              gfx->drawPixel(fx, fy, tiles[i].color);
            }
          }
        }
      }
      
      if (tiles[i].size > 50) {
        tiles[i].active = false;
      }
    }
  }
}

void drawSnowfall() {
  static struct Snowflake {
    float x, y, vx, vy;
    uint16_t color;
    int size;
    bool active;
  } flakes[80];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(5, 10, 20));
    for (int i = 0; i < 80; i++) {
      flakes[i].active = false;
    }
    initialized = true;
  }
  
  // Winter sky background
  for (int i = 0; i < 100; i++) {
    int x = random(320);
    int y = random(240);
    gfx->drawPixel(x, y, gfx->color565(5, 10, 20));
  }
  
  // Spawn new snowflakes
  if (random(100) < 30) {
    for (int i = 0; i < 80; i++) {
      if (!flakes[i].active) {
        flakes[i].x = random(320);
        flakes[i].y = -10;
        flakes[i].vx = (random(40) - 20) / 20.0;
        flakes[i].vy = random(20) / 10.0 + 1.0;
        flakes[i].size = random(3) + 1;
        flakes[i].color = gfx->color565(
          200 + random(55),
          200 + random(55),
          220 + random(35)
        );
        flakes[i].active = true;
        break;
      }
    }
  }
  
  // Update and draw snowflakes
  for (int i = 0; i < 80; i++) {
    if (flakes[i].active) {
      flakes[i].x += flakes[i].vx + sin(animationTime + i) * 0.5;
      flakes[i].y += flakes[i].vy;
      
      // Draw snowflake with crystalline pattern
      if (flakes[i].x >= 0 && flakes[i].x < 320 && 
          flakes[i].y >= 0 && flakes[i].y < 240) {
        
        // Center pixel
        gfx->drawPixel(flakes[i].x, flakes[i].y, flakes[i].color);
        
        // Snowflake arms (6-pointed)
        if (flakes[i].size > 1) {
          for (int arm = 0; arm < 6; arm++) {
            float angle = (arm * PI / 3) + animationTime * 0.1;
            int dx = cos(angle) * flakes[i].size;
            int dy = sin(angle) * flakes[i].size;
            int px = flakes[i].x + dx;
            int py = flakes[i].y + dy;
            
            if (px >= 0 && px < 320 && py >= 0 && py < 240) {
              gfx->drawPixel(px, py, flakes[i].color);
            }
          }
        }
      }
      
      // Remove if off screen
      if (flakes[i].y > 240 || flakes[i].x < -10 || flakes[i].x > 330) {
        flakes[i].active = false;
      }
    }
  }
}

void drawMeteorShower() {
  static struct Meteor {
    float x, y, vx, vy;
    uint16_t color;
    int life, maxLife;
    bool active;
  } meteors[25];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 10));
    for (int i = 0; i < 25; i++) {
      meteors[i].active = false;
    }
    initialized = true;
  }
  
  // Deep space background with stars
  for (int i = 0; i < 50; i++) {
    int x = random(320);
    int y = random(240);
    if (random(100) < 5) {
      gfx->drawPixel(x, y, gfx->color565(200, 200, 255));
    } else {
      gfx->drawPixel(x, y, gfx->color565(0, 0, 10));
    }
  }
  
  // Spawn meteors from upper corners
  if (random(100) < 8) {
    for (int i = 0; i < 25; i++) {
      if (!meteors[i].active) {
        meteors[i].x = random(100) + 220; // Upper right area
        meteors[i].y = random(50);
        meteors[i].vx = -(random(100) + 50) / 20.0; // Move left
        meteors[i].vy = (random(80) + 40) / 20.0;   // Move down
        meteors[i].life = meteors[i].maxLife = random(50) + 30;
        meteors[i].color = gfx->color565(
          200 + random(55),
          150 + random(80),
          50 + random(100)
        );
        meteors[i].active = true;
        break;
      }
    }
  }
  
  // Update and draw meteors with trails
  for (int i = 0; i < 25; i++) {
    if (meteors[i].active) {
      meteors[i].x += meteors[i].vx;
      meteors[i].y += meteors[i].vy;
      meteors[i].life--;
      
      // Draw meteor trail
      float intensity = (float)meteors[i].life / meteors[i].maxLife;
      
      for (int trail = 0; trail < 8; trail++) {
        float tx = meteors[i].x - meteors[i].vx * trail * 0.5;
        float ty = meteors[i].y - meteors[i].vy * trail * 0.5;
        float trailIntensity = intensity * (1.0 - trail / 8.0);
        
        if (tx >= 0 && tx < 320 && ty >= 0 && ty < 240 && trailIntensity > 0) {
          uint8_t r = ((meteors[i].color >> 11) & 0x1F) * trailIntensity;
          uint8_t g = ((meteors[i].color >> 5) & 0x3F) * trailIntensity;
          uint8_t b = (meteors[i].color & 0x1F) * trailIntensity;
          
          gfx->drawPixel(tx, ty, gfx->color565(r, g, b));
          
          // Bright center for meteor head
          if (trail == 0) {
            gfx->drawPixel(tx + 1, ty, meteors[i].color);
            gfx->drawPixel(tx, ty + 1, meteors[i].color);
          }
        }
      }
      
      if (meteors[i].life <= 0 || meteors[i].x < -50 || meteors[i].y > 250) {
        meteors[i].active = false;
      }
    }
  }
}

void drawHexagonGrid() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float hexPhase = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  hexPhase += 0.05;
  
  // Hexagon grid parameters
  int hexSize = 20;
  int hexSpacing = hexSize * 1.5;
  
  for (int row = -2; row < 15; row++) {
    for (int col = -2; col < 20; col++) {
      // Hexagonal grid offset
      float x = col * hexSpacing + (row % 2) * (hexSpacing / 2);
      float y = row * hexSize * 0.866; // sqrt(3)/2
      
      if (x >= -hexSize && x < 320 + hexSize && y >= -hexSize && y < 240 + hexSize) {
        // Color based on position and time
        float colorPhase = hexPhase + (x * 0.01) + (y * 0.01);
        uint16_t color = gfx->color565(
          128 + sin(colorPhase) * 127,
          128 + sin(colorPhase + PI/3) * 127,
          128 + sin(colorPhase + 2*PI/3) * 127
        );
        
        // Draw hexagon
        for (int side = 0; side < 6; side++) {
          float angle1 = side * PI / 3;
          float angle2 = (side + 1) * PI / 3;
          
          int x1 = x + cos(angle1) * hexSize;
          int y1 = y + sin(angle1) * hexSize;
          int x2 = x + cos(angle2) * hexSize;
          int y2 = y + sin(angle2) * hexSize;
          
          // Draw line between hex vertices
          int steps = max(abs(x2 - x1), abs(y2 - y1));
          if (steps > 0) {
            for (int i = 0; i <= steps; i++) {
              int px = x1 + (x2 - x1) * i / steps;
              int py = y1 + (y2 - y1) * i / steps;
              
              if (px >= 0 && px < 320 && py >= 0 && py < 240) {
                gfx->drawPixel(px, py, color);
              }
            }
          }
        }
        
        // Center dot
        if (x >= 0 && x < 320 && y >= 0 && y < 240) {
          gfx->drawPixel(x, y, gfx->color565(255, 255, 255));
        }
      }
    }
  }
}

void drawSineWavesGrid() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float wavePhase = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  wavePhase += 0.1;
  
  // Clear with dark background
  for (int i = 0; i < 200; i++) {
    int x = random(320);
    int y = random(240);
    gfx->drawPixel(x, y, gfx->color565(5, 5, 15));
  }
  
  // Horizontal sine waves
  for (int waveNum = 0; waveNum < 8; waveNum++) {
    float baseY = (waveNum + 1) * 240 / 9;
    float frequency = 0.02 + waveNum * 0.005;
    float amplitude = 15 + waveNum * 3;
    
    uint16_t waveColor = gfx->color565(
      100 + sin(wavePhase + waveNum) * 100,
      150 + cos(wavePhase + waveNum * 1.3) * 100,
      200 + sin(wavePhase + waveNum * 0.7) * 55
    );
    
    for (int x = 0; x < 320; x++) {
      float y = baseY + sin(x * frequency + wavePhase + waveNum * 0.5) * amplitude;
      
      if (y >= 0 && y < 240) {
        gfx->drawPixel(x, (int)y, waveColor);
        // Add thickness
        if (y + 1 < 240) gfx->drawPixel(x, (int)y + 1, waveColor);
        if (y - 1 >= 0) gfx->drawPixel(x, (int)y - 1, waveColor);
      }
    }
  }
  
  // Vertical sine waves (intersecting)
  for (int waveNum = 0; waveNum < 6; waveNum++) {
    float baseX = (waveNum + 1) * 320 / 7;
    float frequency = 0.03 + waveNum * 0.003;
    float amplitude = 20 + waveNum * 2;
    
    uint16_t waveColor = gfx->color565(
      200 + cos(wavePhase + waveNum * 1.1) * 55,
      100 + sin(wavePhase + waveNum * 0.9) * 100,
      150 + cos(wavePhase + waveNum * 1.5) * 100
    );
    
    for (int y = 0; y < 240; y++) {
      float x = baseX + cos(y * frequency + wavePhase + waveNum * 0.7) * amplitude;
      
      if (x >= 0 && x < 320) {
        gfx->drawPixel((int)x, y, waveColor);
        // Add thickness
        if (x + 1 < 320) gfx->drawPixel((int)x + 1, y, waveColor);
        if (x - 1 >= 0) gfx->drawPixel((int)x - 1, y, waveColor);
      }
    }
  }
}

void drawRGBShift() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float shiftPhase = 0;
  static uint16_t basePattern[160 * 120]; // Smaller buffer
  static int patternFrame = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    
    // Generate smaller base pattern
    for (int y = 0; y < 120; y++) {
      for (int x = 0; x < 160; x++) {
        float pattern = sin(x * 0.1) * cos(y * 0.06) + 
                       sin(x * 0.04 + y * 0.08) * 0.5;
        pattern = (pattern + 2.0) / 4.0; // Normalize to 0-1
        
        basePattern[y * 160 + x] = gfx->color565(
          pattern * 255,
          pattern * 200,
          pattern * 150
        );
      }
    }
    
    initialized = true;
  }
  
  shiftPhase += 0.1;
  patternFrame++;
  
  // Update base pattern occasionally
  if (patternFrame % 60 == 0) {
    for (int i = 0; i < 50; i++) {
      int x = random(160);
      int y = random(120);
      float pattern = sin(x * 0.06 + shiftPhase) * cos(y * 0.1 + shiftPhase) + 
                     sin((x + y) * 0.04 + shiftPhase * 0.7) * 0.7;
      pattern = (pattern + 2.0) / 4.0;
      
      basePattern[y * 160 + x] = gfx->color565(
        pattern * 255,
        pattern * 180,
        pattern * 120
      );
    }
  }
  
  // RGB channel shift effect
  int redShiftX = sin(shiftPhase) * 8;
  int redShiftY = cos(shiftPhase * 1.3) * 4;
  int greenShiftX = sin(shiftPhase * 1.1) * 6;
  int greenShiftY = cos(shiftPhase * 0.9) * 6;
  int blueShiftX = sin(shiftPhase * 0.8) * 10;
  int blueShiftY = cos(shiftPhase * 1.2) * 3;
  
  // Draw with channel shifts
  for (int y = 0; y < 240; y += 2) {
    for (int x = 0; x < 320; x += 2) {
      // Sample shifted channels
      uint8_t r = 0, g = 0, b = 0;
      
      // Red channel (scale coordinates to smaller buffer)
      int rx = (x / 2) + redShiftX;
      int ry = (y / 2) + redShiftY;
      if (rx >= 0 && rx < 160 && ry >= 0 && ry < 120) {
        uint16_t color = basePattern[ry * 160 + rx];
        r = (color >> 11) & 0x1F;
        r = (r * 255) / 31;
      }
      
      // Green channel
      int gx = (x / 2) + greenShiftX;
      int gy = (y / 2) + greenShiftY;
      if (gx >= 0 && gx < 160 && gy >= 0 && gy < 120) {
        uint16_t color = basePattern[gy * 160 + gx];
        g = (color >> 5) & 0x3F;
        g = (g * 255) / 63;
      }
      
      // Blue channel
      int bx = (x / 2) + blueShiftX;
      int by = (y / 2) + blueShiftY;
      if (bx >= 0 && bx < 160 && by >= 0 && by < 120) {
        uint16_t color = basePattern[by * 160 + bx];
        b = color & 0x1F;
        b = (b * 255) / 31;
      }
      
      uint16_t finalColor = gfx->color565(r, g, b);
      
      // Draw 2x2 block for performance
      gfx->drawPixel(x, y, finalColor);
      if (x + 1 < 320) gfx->drawPixel(x + 1, y, finalColor);
      if (y + 1 < 240) gfx->drawPixel(x, y + 1, finalColor);
      if (x + 1 < 320 && y + 1 < 240) gfx->drawPixel(x + 1, y + 1, finalColor);
    }
  }
}

void drawTVStatic() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static int staticFrame = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  staticFrame++;
  
  // Full screen TV static
  for (int i = 0; i < 8000; i++) {
    int x = random(320);
    int y = random(240);
    
    // Random intensity with some bias towards gray
    int intensity = random(256);
    if (random(100) < 20) intensity = random(100) + 155; // Brighter pixels occasionally
    
    uint16_t color = gfx->color565(intensity, intensity, intensity);
    
    // Add some color tint occasionally
    if (random(100) < 5) {
      color = gfx->color565(
        intensity + random(50) - 25,
        intensity + random(30) - 15,
        intensity + random(40) - 20
      );
    }
    
    gfx->drawPixel(x, y, color);
  }
  
  // Add scan lines
  if (staticFrame % 3 == 0) {
    int scanLine = random(240);
    for (int x = 0; x < 320; x++) {
      if (random(100) < 70) {
        uint16_t darken = gfx->color565(20, 20, 20);
        gfx->drawPixel(x, scanLine, darken);
      }
    }
  }
  
  // Occasional interference patterns
  if (random(100) < 10) {
    int stripY = random(200);
    int stripHeight = random(20) + 5;
    
    for (int y = stripY; y < stripY + stripHeight && y < 240; y++) {
      for (int x = 0; x < 320; x += 2) {
        int shift = sin((x + staticFrame) * 0.1) * 5;
        if (x + shift >= 0 && x + shift < 320) {
          uint16_t bright = gfx->color565(200, 200, 200);
          gfx->drawPixel(x + shift, y, bright);
        }
      }
    }
  }
}

void drawParticleFountain() {
  static struct Particle {
    float x, y, vx, vy;
    uint16_t color;
    int life, maxLife;
    bool active;
  } particles[150];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 150; i++) {
      particles[i].active = false;
    }
    initialized = true;
  }
  
  // Dark background
  for (int i = 0; i < 300; i++) {
    int x = random(320);
    int y = random(240);
    gfx->drawPixel(x, y, gfx->color565(2, 2, 5));
  }
  
  // Spawn particles from fountain source
  for (int spawn = 0; spawn < 5; spawn++) {
    for (int i = 0; i < 150; i++) {
      if (!particles[i].active) {
        // Fountain source at bottom center
        particles[i].x = 160 + random(20) - 10;
        particles[i].y = 220;
        
        // Upward velocity with spread
        float angle = -PI/2 + (random(60) - 30) * PI/180;
        float speed = random(80) / 10.0 + 8.0;
        particles[i].vx = cos(angle) * speed;
        particles[i].vy = sin(angle) * speed;
        
        particles[i].life = particles[i].maxLife = random(100) + 50;
        
        // Rainbow colors
        int hue = (int)(animationTime * 50 + random(100)) % 360;
        if (hue < 60) {
          particles[i].color = gfx->color565(255, hue * 255/60, 0);
        } else if (hue < 120) {
          particles[i].color = gfx->color565(255 - (hue-60)*255/60, 255, 0);
        } else if (hue < 180) {
          particles[i].color = gfx->color565(0, 255, (hue-120)*255/60);
        } else if (hue < 240) {
          particles[i].color = gfx->color565(0, 255 - (hue-180)*255/60, 255);
        } else if (hue < 300) {
          particles[i].color = gfx->color565((hue-240)*255/60, 0, 255);
        } else {
          particles[i].color = gfx->color565(255, 0, 255 - (hue-300)*255/60);
        }
        
        particles[i].active = true;
        break;
      }
    }
  }
  
  // Update and draw particles
  for (int i = 0; i < 150; i++) {
    if (particles[i].active) {
      particles[i].x += particles[i].vx;
      particles[i].y += particles[i].vy;
      particles[i].vy += 0.2; // Gravity
      particles[i].vx *= 0.99; // Air resistance
      particles[i].life--;
      
      // Draw particle with fading
      if (particles[i].x >= 0 && particles[i].x < 320 && 
          particles[i].y >= 0 && particles[i].y < 240) {
        
        float intensity = (float)particles[i].life / particles[i].maxLife;
        uint8_t r = ((particles[i].color >> 11) & 0x1F) * intensity;
        uint8_t g = ((particles[i].color >> 5) & 0x3F) * intensity;
        uint8_t b = (particles[i].color & 0x1F) * intensity;
        
        uint16_t fadedColor = gfx->color565(
          (r * 255) / 31,
          (g * 255) / 63,  
          (b * 255) / 31
        );
        
        gfx->drawPixel(particles[i].x, particles[i].y, fadedColor);
        
        // Sparkle effect
        if (intensity > 0.7 && random(100) < 20) {
          gfx->drawPixel(particles[i].x + 1, particles[i].y, fadedColor);
          gfx->drawPixel(particles[i].x, particles[i].y + 1, fadedColor);
        }
      }
      
      if (particles[i].life <= 0 || particles[i].y > 240) {
        particles[i].active = false;
      }
    }
  }
}

void drawColorCycleBars() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float colorPhase = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  colorPhase += 0.05;
  
  // Vertical color bars
  int barWidth = 320 / 8;
  for (int bar = 0; bar < 8; bar++) {
    float phase = colorPhase + bar * PI / 4;
    
    uint16_t color = gfx->color565(
      128 + sin(phase) * 127,
      128 + sin(phase + PI*2/3) * 127,
      128 + sin(phase + PI*4/3) * 127
    );
    
    for (int x = bar * barWidth; x < (bar + 1) * barWidth && x < 320; x++) {
      for (int y = 0; y < 240; y++) {
        // Add some wave distortion
        float wave = sin(y * 0.05 + colorPhase * 2) * 10;
        if (x + wave >= bar * barWidth && x + wave < (bar + 1) * barWidth) {
          gfx->drawPixel(x, y, color);
        }
      }
    }
  }
  
  // Horizontal moving stripes
  for (int stripe = 0; stripe < 6; stripe++) {
    int stripeY = (stripe * 40 + (int)(animationTime * 20)) % 240;
    int stripeHeight = 8;
    
    float stripePhase = colorPhase + stripe * PI / 3;
    uint16_t stripeColor = gfx->color565(
      128 + cos(stripePhase) * 127,
      128 + cos(stripePhase + PI/2) * 127,
      128 + cos(stripePhase + PI) * 127
    );
    
    for (int y = stripeY; y < stripeY + stripeHeight && y < 240; y++) {
      for (int x = 0; x < 320; x++) {
        // Blend with existing color
        if (y >= 0) {
          gfx->drawPixel(x, y, stripeColor);
        }
      }
    }
  }
  
  // Center pulsing circle
  int centerX = 160;
  int centerY = 120;
  int radius = 30 + sin(colorPhase * 2) * 15;
  
  uint16_t centerColor = gfx->color565(
    255,
    128 + sin(colorPhase * 3) * 127,
    128 + cos(colorPhase * 3) * 127
  );
  
  for (int angle = 0; angle < 360; angle += 5) {
    float rad = angle * PI / 180;
    int x = centerX + cos(rad) * radius;
    int y = centerY + sin(rad) * radius;
    
    if (x >= 0 && x < 320 && y >= 0 && y < 240) {
      gfx->drawPixel(x, y, centerColor);
      // Thicker circle
      if (x + 1 < 320) gfx->drawPixel(x + 1, y, centerColor);
      if (y + 1 < 240) gfx->drawPixel(x, y + 1, centerColor);
    }
  }
}

void drawAuroraFire() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float auroraPhase = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  auroraPhase += 0.02;
  
  // Dark night sky background with stars
  for (int i = 0; i < 150; i++) {
    int x = random(320);
    int y = random(120); // Upper half only
    if (random(100) < 3) {
      gfx->drawPixel(x, y, gfx->color565(200, 200, 255));
    } else {
      gfx->drawPixel(x, y, gfx->color565(5, 5, 15));
    }
  }
  
  // Aurora curtains - multiple layers
  for (int layer = 0; layer < 5; layer++) {
    float layerPhase = auroraPhase + layer * 0.3;
    
    for (int x = 0; x < 320; x++) {
      // Aurora height varies with sine waves
      float baseHeight = 60 + layer * 15;
      float wave1 = sin(x * 0.02 + layerPhase * 2) * 20;
      float wave2 = sin(x * 0.05 + layerPhase * 1.5) * 10;
      float height = baseHeight + wave1 + wave2;
      
      // Aurora colors - green, blue, purple, red
      uint8_t r, g, b;
      if (layer == 0) { // Green aurora
        r = 50 + sin(layerPhase + x * 0.01) * 30;
        g = 150 + sin(layerPhase + x * 0.01) * 105;
        b = 80 + sin(layerPhase + x * 0.01) * 50;
      } else if (layer == 1) { // Blue aurora
        r = 30 + sin(layerPhase + x * 0.015) * 40;
        g = 100 + sin(layerPhase + x * 0.015) * 80;
        b = 200 + sin(layerPhase + x * 0.015) * 55;
      } else if (layer == 2) { // Purple aurora
        r = 120 + sin(layerPhase + x * 0.012) * 80;
        g = 50 + sin(layerPhase + x * 0.012) * 60;
        b = 180 + sin(layerPhase + x * 0.012) * 75;
      } else { // Fire colors mixed in
        r = 180 + sin(layerPhase + x * 0.008) * 75;
        g = 80 + sin(layerPhase + x * 0.008) * 100;
        b = 40 + sin(layerPhase + x * 0.008) * 40;
      }
      
      uint16_t auroraColor = gfx->color565(r, g, b);
      
      // Draw vertical aurora curtain
      for (int y = 0; y < height && y < 240; y++) {
        float intensity = 1.0 - (y / height);
        intensity *= (sin(y * 0.1 + layerPhase) * 0.3 + 0.7);
        
        if (intensity > 0.1) {
          uint8_t finalR = (r * intensity);
          uint8_t finalG = (g * intensity);  
          uint8_t finalB = (b * intensity);
          
          gfx->drawPixel(x, 240 - y, gfx->color565(finalR, finalG, finalB));
        }
      }
    }
  }
}

void drawMeteorPlasma() {
  static struct MeteorTrail {
    float x, y, vx, vy;
    uint16_t color;
    int life, maxLife;
    bool active;
  } meteors[30];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float plasmaTime = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 30; i++) {
      meteors[i].active = false;
    }
    initialized = true;
  }
  
  plasmaTime += 0.05;
  
  // Plasma background
  for (int y = 0; y < 240; y += 3) {
    for (int x = 0; x < 320; x += 3) {
      float plasma = sin(x * 0.02 + plasmaTime) + 
                    cos(y * 0.03 + plasmaTime * 0.7) +
                    sin((x + y) * 0.015 + plasmaTime * 1.2);
      plasma = (plasma + 3.0) / 6.0; // Normalize
      
      uint16_t plasmaColor = gfx->color565(
        plasma * 60,
        plasma * 40,
        plasma * 80
      );
      
      // Draw 3x3 blocks
      for (int dy = 0; dy < 3 && (y + dy) < 240; dy++) {
        for (int dx = 0; dx < 3 && (x + dx) < 320; dx++) {
          gfx->drawPixel(x + dx, y + dy, plasmaColor);
        }
      }
    }
  }
  
  // Spawn meteors
  if (random(100) < 8) {
    for (int i = 0; i < 30; i++) {
      if (!meteors[i].active) {
        meteors[i].x = random(100) + 220;
        meteors[i].y = random(60);
        meteors[i].vx = -(random(60) + 40) / 15.0;
        meteors[i].vy = (random(50) + 30) / 15.0;
        meteors[i].life = meteors[i].maxLife = random(80) + 40;
        meteors[i].color = gfx->color565(
          200 + random(55),
          150 + random(105),
          100 + random(100)
        );
        meteors[i].active = true;
        break;
      }
    }
  }
  
  // Update and draw meteors with plasma interaction
  for (int i = 0; i < 30; i++) {
    if (meteors[i].active) {
      meteors[i].x += meteors[i].vx;
      meteors[i].y += meteors[i].vy;
      meteors[i].life--;
      
      // Plasma-enhanced trail
      float intensity = (float)meteors[i].life / meteors[i].maxLife;
      
      for (int trail = 0; trail < 15; trail++) {
        float tx = meteors[i].x - meteors[i].vx * trail * 0.3;
        float ty = meteors[i].y - meteors[i].vy * trail * 0.3;
        float trailIntensity = intensity * (1.0 - trail / 15.0);
        
        if (tx >= 0 && tx < 320 && ty >= 0 && ty < 240 && trailIntensity > 0) {
          // Plasma enhancement
          float plasma = sin(tx * 0.05 + plasmaTime * 2) * cos(ty * 0.03 + plasmaTime);
          plasma = (plasma + 1.0) / 2.0;
          
          uint8_t r = ((meteors[i].color >> 11) & 0x1F) * trailIntensity * (1 + plasma);
          uint8_t g = ((meteors[i].color >> 5) & 0x3F) * trailIntensity * (1 + plasma * 0.7);
          uint8_t b = (meteors[i].color & 0x1F) * trailIntensity * (1 + plasma * 0.5);
          
          gfx->drawPixel(tx, ty, gfx->color565(
            min(255, r * 8),
            min(255, g * 4), 
            min(255, b * 8)
          ));
        }
      }
      
      if (meteors[i].life <= 0 || meteors[i].x < -30) {
        meteors[i].active = false;
      }
    }
  }
}

void drawSnowMatrix() {
  static struct MatrixFlake {
    float x, y, vy;
    char character;
    uint16_t color;
    int life;
    bool active;
  } flakes[100];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 100; i++) {
      flakes[i].active = false;
    }
    initialized = true;
  }
  
  // Dark matrix background
  for (int i = 0; i < 200; i++) {
    int x = random(320);
    int y = random(240);
    gfx->drawPixel(x, y, gfx->color565(0, 5, 0));
  }
  
  // Spawn matrix snow characters
  if (random(100) < 25) {
    for (int i = 0; i < 100; i++) {
      if (!flakes[i].active) {
        flakes[i].x = random(320);
        flakes[i].y = -8;
        flakes[i].vy = random(20) / 10.0 + 1.0;
        
        // Matrix-like characters (simplified as pixels)
        flakes[i].character = random(26) + 'A';
        
        // Matrix green with snow white mixing
        int greenIntensity = random(200) + 55;
        int whiteIntensity = random(150);
        flakes[i].color = gfx->color565(
          whiteIntensity / 4,
          max(greenIntensity, whiteIntensity),
          whiteIntensity / 4
        );
        
        flakes[i].life = random(300) + 200;
        flakes[i].active = true;
        break;
      }
    }
  }
  
  // Update and draw matrix snowflakes
  for (int i = 0; i < 100; i++) {
    if (flakes[i].active) {
      flakes[i].y += flakes[i].vy;
      flakes[i].x += sin(animationTime + i) * 0.3; // Slight drift
      flakes[i].life--;
      
      // Draw matrix character as pixel pattern
      if (flakes[i].x >= 0 && flakes[i].x < 320 && 
          flakes[i].y >= 0 && flakes[i].y < 240) {
        
        // Simple character pattern
        int px = (int)flakes[i].x;
        int py = (int)flakes[i].y;
        
        // Center pixel
        gfx->drawPixel(px, py, flakes[i].color);
        
        // Character-like pattern based on ASCII value
        int pattern = flakes[i].character % 8;
        
        if (pattern & 1 && px + 1 < 320) {
          gfx->drawPixel(px + 1, py, flakes[i].color);
        }
        if (pattern & 2 && py + 1 < 240) {
          gfx->drawPixel(px, py + 1, flakes[i].color);
        }
        if (pattern & 4 && px - 1 >= 0) {
          gfx->drawPixel(px - 1, py, flakes[i].color);
        }
        if (pattern & 8 && py - 1 >= 0) {
          gfx->drawPixel(px, py - 1, flakes[i].color);
        }
        
        // Matrix trailing effect
        if (random(100) < 30) {
          uint16_t trailColor = gfx->color565(0, 100, 0);
          if (py + 2 < 240) gfx->drawPixel(px, py + 2, trailColor);
          if (py + 3 < 240) gfx->drawPixel(px, py + 3, trailColor);
        }
      }
      
      // Remove if off screen or life expired
      if (flakes[i].y > 240 || flakes[i].life <= 0) {
        flakes[i].active = false;
      }
    }
  }
}

void drawJuliaFire() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float juliaTime = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  juliaTime += 0.02;
  
  // Julia set with fire coloring
  float cx = -0.7 + sin(juliaTime) * 0.3;
  float cy = cos(juliaTime * 0.7) * 0.3;
  
  for (int py = 0; py < 240; py += 2) {
    for (int px = 0; px < 320; px += 2) {
      // Map screen coordinates to complex plane
      float x = (px - 160) / 80.0;
      float y = (py - 120) / 80.0;
      
      // Julia set iteration
      int maxIter = 32;
      int iter = 0;
      float zx = x, zy = y;
      
      while (iter < maxIter && (zx * zx + zy * zy) < 4) {
        float temp = zx * zx - zy * zy + cx;
        zy = 2 * zx * zy + cy;
        zx = temp;
        iter++;
      }
      
      // Fire coloring based on iteration count
      uint16_t color;
      if (iter == maxIter) {
        color = gfx->color565(0, 0, 0); // Black for inside set
      } else {
        float t = (float)iter / maxIter;
        t += sin(juliaTime * 2 + px * 0.01) * 0.2; // Fire flicker
        t = max(0.0f, min(1.0f, t));
        
        if (t < 0.3) {
          // Deep red to orange
          color = gfx->color565(255 * t / 0.3, 100 * t / 0.3, 0);
        } else if (t < 0.6) {
          // Orange to yellow  
          float s = (t - 0.3) / 0.3;
          color = gfx->color565(255, 100 + 155 * s, 50 * s);
        } else {
          // Yellow to white
          float s = (t - 0.6) / 0.4;
          color = gfx->color565(255, 255, 50 + 205 * s);
        }
      }
      
      // Draw 2x2 block for performance
      gfx->drawPixel(px, py, color);
      if (px + 1 < 320) gfx->drawPixel(px + 1, py, color);
      if (py + 1 < 240) gfx->drawPixel(px, py + 1, color);
      if (px + 1 < 320 && py + 1 < 240) gfx->drawPixel(px + 1, py + 1, color);
    }
  }
}

void drawMandelbrotLightning() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float lightningTime = 0;
  static int lastStrike = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  lightningTime += 0.015;
  lastStrike++;
  
  // Mandelbrot set calculation
  float zoom = 0.5 + sin(lightningTime * 0.5) * 0.3;
  float offsetX = sin(lightningTime * 0.3) * 0.5;
  float offsetY = cos(lightningTime * 0.2) * 0.3;
  
  for (int py = 0; py < 240; py += 3) {
    for (int px = 0; px < 320; px += 3) {
      float x = (px - 160) / (100.0 * zoom) + offsetX;
      float y = (py - 120) / (100.0 * zoom) + offsetY;
      
      int maxIter = 20;
      int iter = 0;
      float zx = 0, zy = 0;
      
      while (iter < maxIter && (zx * zx + zy * zy) < 4) {
        float temp = zx * zx - zy * zy + x;
        zy = 2 * zx * zy + y;
        zx = temp;
        iter++;
      }
      
      // Lightning effect coloring
      uint16_t color;
      if (iter == maxIter) {
        color = gfx->color565(5, 5, 20); // Dark blue interior
      } else {
        float t = (float)iter / maxIter;
        
        // Electric blue to white gradient
        uint8_t r = 20 + t * 200;
        uint8_t g = 50 + t * 150;
        uint8_t b = 100 + t * 155;
        
        // Lightning flash enhancement
        if (lastStrike < 10 && random(100) < 30) {
          r = min(255, r * 2);
          g = min(255, g * 2);
          b = min(255, b * 2);
        }
        
        color = gfx->color565(r, g, b);
      }
      
      // Draw 3x3 blocks
      for (int dy = 0; dy < 3 && (py + dy) < 240; dy++) {
        for (int dx = 0; dx < 3 && (px + dx) < 320; dx++) {
          gfx->drawPixel(px + dx, py + dy, color);
        }
      }
    }
  }
  
  // Random lightning strikes
  if (random(100) < 3) {
    lastStrike = 0;
    
    // Draw lightning bolts
    int strikes = random(3) + 1;
    for (int s = 0; s < strikes; s++) {
      int x1 = random(320);
      int y1 = random(60);
      int x2 = x1 + (random(100) - 50);
      int y2 = y1 + random(180) + 60;
      
      // Jagged lightning path
      int steps = 20;
      for (int i = 0; i < steps; i++) {
        float t = (float)i / steps;
        int lx = x1 + (x2 - x1) * t + (random(40) - 20);
        int ly = y1 + (y2 - y1) * t;
        
        if (lx >= 0 && lx < 320 && ly >= 0 && ly < 240) {
          uint16_t lightningColor = gfx->color565(255, 255, 200);
          gfx->drawPixel(lx, ly, lightningColor);
          
          // Glow effect
          if (lx > 0) gfx->drawPixel(lx - 1, ly, gfx->color565(150, 150, 255));
          if (lx < 319) gfx->drawPixel(lx + 1, ly, gfx->color565(150, 150, 255));
          if (ly > 0) gfx->drawPixel(lx, ly - 1, gfx->color565(150, 150, 255));
          if (ly < 239) gfx->drawPixel(lx, ly + 1, gfx->color565(150, 150, 255));
        }
      }
    }
  }
}

void drawDragonDNA() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float dnaTime = 0;
  static float dragonPhase = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  dnaTime += 0.1;
  dragonPhase += 0.02;
  
  // Dark background with mystical glow
  for (int i = 0; i < 100; i++) {
    int x = random(320);
    int y = random(240);
    gfx->drawPixel(x, y, gfx->color565(10, 5, 15));
  }
  
  // DNA double helix
  for (int x = 0; x < 320; x += 3) {
    float t = x * 0.05;
    
    // First DNA strand
    float y1 = 120 + sin(t + dnaTime) * 40;
    float y2 = 120 + sin(t + dnaTime + PI) * 40;
    
    if (y1 >= 0 && y1 < 240) {
      uint16_t dnaColor1 = gfx->color565(
        100 + sin(dragonPhase + t) * 100,
        150 + cos(dragonPhase + t) * 50,
        200 + sin(dragonPhase + t * 2) * 55
      );
      gfx->drawPixel(x, y1, dnaColor1);
      if (y1 + 1 < 240) gfx->drawPixel(x, y1 + 1, dnaColor1);
    }
    
    if (y2 >= 0 && y2 < 240) {
      uint16_t dnaColor2 = gfx->color565(
        200 + sin(dragonPhase + t + PI/3) * 55,
        100 + cos(dragonPhase + t + PI/3) * 100,
        150 + sin(dragonPhase + t * 2 + PI/3) * 50
      );
      gfx->drawPixel(x, y2, dnaColor2);
      if (y2 + 1 < 240) gfx->drawPixel(x, y2 + 1, dnaColor2);
    }
    
    // DNA connecting rungs
    if (x % 15 == 0) {
      int steps = abs(y2 - y1);
      if (steps > 0) {
        for (int i = 0; i <= steps; i++) {
          int ry = y1 + (y2 - y1) * i / steps;
          if (ry >= 0 && ry < 240) {
            uint16_t rungColor = gfx->color565(80, 80, 150);
            gfx->drawPixel(x, ry, rungColor);
          }
        }
      }
    }
  }
  
  // Dragon curve overlay
  int dragonSize = 8;
  int centerX = 160 + sin(dragonPhase) * 60;
  int centerY = 120 + cos(dragonPhase * 0.7) * 30;
  
  // Simplified dragon curve fractal
  for (int order = 0; order < 6; order++) {
    float angle = order * PI / 3 + dragonPhase;
    float radius = dragonSize * (order + 1);
    
    for (int seg = 0; seg < 8; seg++) {
      float segAngle = angle + seg * PI / 4;
      int x1 = centerX + cos(segAngle) * radius;
      int y1 = centerY + sin(segAngle) * radius;
      int x2 = centerX + cos(segAngle + PI/8) * radius * 0.8;
      int y2 = centerY + sin(segAngle + PI/8) * radius * 0.8;
      
      // Draw dragon segment
      if (x1 >= 0 && x1 < 320 && y1 >= 0 && y1 < 240) {
        uint16_t dragonColor = gfx->color565(
          150 + sin(dragonPhase * 2 + order) * 105,
          80 + cos(dragonPhase * 1.5 + order) * 80,
          200 + sin(dragonPhase + order * 2) * 55
        );
        gfx->drawPixel(x1, y1, dragonColor);
        
        // Dragon scales effect
        if (order > 2) {
          for (int scale = 0; scale < 3; scale++) {
            int sx = x1 + (random(6) - 3);
            int sy = y1 + (random(6) - 3);
            if (sx >= 0 && sx < 320 && sy >= 0 && sy < 240) {
              uint16_t scaleColor = gfx->color565(
                (dragonColor >> 11) * 0.7,
                ((dragonColor >> 5) & 0x3F) * 0.7,
                (dragonColor & 0x1F) * 0.7
              );
              gfx->drawPixel(sx, sy, scaleColor);
            }
          }
        }
      }
    }
  }
}

void drawVoronoiStarfield() {
  static struct Star {
    float x, y, z;
    uint16_t color;
    float twinkle;
  } stars[80];
  static struct VoronoiSeed {
    float x, y;
    uint16_t color;
    float pulse;
  } seeds[8];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float starfieldTime = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    
    // Initialize stars
    for (int i = 0; i < 80; i++) {
      stars[i].x = random(320);
      stars[i].y = random(240);
      stars[i].z = random(100) / 100.0;
      stars[i].color = gfx->color565(
        200 + random(55),
        200 + random(55), 
        220 + random(35)
      );
      stars[i].twinkle = random(100) / 50.0;
    }
    
    // Initialize Voronoi seeds
    for (int i = 0; i < 8; i++) {
      seeds[i].x = random(320);
      seeds[i].y = random(240);
      seeds[i].color = gfx->color565(
        random(100) + 50,
        random(150) + 50,
        random(200) + 55
      );
      seeds[i].pulse = random(100) / 10.0;
    }
    
    initialized = true;
  }
  
  starfieldTime += 0.03;
  
  // Deep space background
  fillScreenExceptClock(gfx->color565(5, 5, 15));
  
  // Update and draw stars with twinkling
  for (int i = 0; i < 80; i++) {
    stars[i].twinkle += 0.1;
    
    float brightness = (sin(stars[i].twinkle) + 1.0) * 0.5 * stars[i].z;
    if (brightness > 0.2) {
      uint8_t r = ((stars[i].color >> 11) & 0x1F) * brightness;
      uint8_t g = ((stars[i].color >> 5) & 0x3F) * brightness;
      uint8_t b = (stars[i].color & 0x1F) * brightness;
      
      gfx->drawPixel(stars[i].x, stars[i].y, gfx->color565(
        (r * 255) / 31,
        (g * 255) / 63,
        (b * 255) / 31
      ));
    }
  }
  
  // Voronoi nebula regions
  for (int y = 0; y < 240; y += 4) {
    for (int x = 0; x < 320; x += 4) {
      float minDist = 99999;
      int closestSeed = 0;
      
      // Find closest seed
      for (int i = 0; i < 8; i++) {
        float dx = x - seeds[i].x;
        float dy = y - seeds[i].y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < minDist) {
          minDist = dist;
          closestSeed = i;
        }
      }
      
      // Nebula intensity based on distance and pulse
      float intensity = max(0.0f, 1.0f - minDist / 100.0f);
      intensity *= (sin(seeds[closestSeed].pulse + starfieldTime * 2) + 1.0) * 0.5;
      
      if (intensity > 0.1) {
        uint8_t r = ((seeds[closestSeed].color >> 11) & 0x1F) * intensity;
        uint8_t g = ((seeds[closestSeed].color >> 5) & 0x3F) * intensity;
        uint8_t b = (seeds[closestSeed].color & 0x1F) * intensity;
        
        uint16_t nebulaColor = gfx->color565(
          (r * 255) / 31,
          (g * 255) / 63,
          (b * 255) / 31
        );
        
        // Draw 4x4 blocks
        for (int dy = 0; dy < 4 && (y + dy) < 240; dy++) {
          for (int dx = 0; dx < 4 && (x + dx) < 320; dx++) {
            gfx->drawPixel(x + dx, y + dy, nebulaColor);
          }
        }
      }
    }
  }
  
  // Update seed positions
  for (int i = 0; i < 8; i++) {
    seeds[i].x += sin(starfieldTime + i) * 0.5;
    seeds[i].y += cos(starfieldTime * 0.7 + i) * 0.3;
    seeds[i].pulse += 0.05;
    
    // Wrap around
    if (seeds[i].x < 0) seeds[i].x = 320;
    if (seeds[i].x >= 320) seeds[i].x = 0;
    if (seeds[i].y < 0) seeds[i].y = 240;  
    if (seeds[i].y >= 240) seeds[i].y = 0;
  }
}

void drawFireworkTunnel() {
  static struct TunnelFirework {
    float x, y, z, vz;
    uint16_t color;
    bool exploded;
    int life;
    struct Particle {
      float x, y, z, vx, vy, vz;
      uint16_t color;
      int life;
      bool active;
    } particles[8];
  } fireworks[15];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float tunnelTime = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 15; i++) {
      fireworks[i].exploded = false;
      fireworks[i].life = 0;
      for (int p = 0; p < 8; p++) {
        fireworks[i].particles[p].active = false;
      }
    }
    initialized = true;
  }
  
  tunnelTime += 0.05;
  
  // Tunnel grid background
  fillScreenExceptClock(gfx->color565(0, 0, 5));
  
  // Draw tunnel grid
  for (int ring = 1; ring < 10; ring++) {
    float z = ring * 30 + fmod(tunnelTime * 100, 30);
    if (z > 0) {
      int radius = 200 / z * 100;
      if (radius > 5 && radius < 200) {
        uint8_t intensity = max(20.0f, 255.0f - z * 3);
        uint16_t gridColor = gfx->color565(intensity/4, intensity/4, intensity/2);
        
        // Draw tunnel ring
        for (int angle = 0; angle < 360; angle += 30) {
          float rad = angle * PI / 180;
          int x = 160 + cos(rad) * radius;
          int y = 120 + sin(rad) * radius;
          if (x >= 0 && x < 320 && y >= 0 && y < 240) {
            gfx->drawPixel(x, y, gridColor);
          }
        }
      }
    }
  }
  
  // Spawn new fireworks
  if (random(100) < 15) {
    for (int i = 0; i < 15; i++) {
      if (!fireworks[i].exploded && fireworks[i].life == 0) {
        fireworks[i].x = random(80) - 40; // Relative to center
        fireworks[i].y = random(60) - 30;
        fireworks[i].z = 500;
        fireworks[i].vz = -8;
        fireworks[i].color = gfx->color565(
          random(200) + 55,
          random(200) + 55,
          random(200) + 55
        );
        fireworks[i].life = 100;
        fireworks[i].exploded = false;
        break;
      }
    }
  }
  
  // Update and draw fireworks
  for (int i = 0; i < 15; i++) {
    if (fireworks[i].life > 0 && !fireworks[i].exploded) {
      fireworks[i].z += fireworks[i].vz;
      fireworks[i].life--;
      
      // Project to screen
      if (fireworks[i].z > 10) {
        int screenX = 160 + fireworks[i].x * 200 / fireworks[i].z;
        int screenY = 120 + fireworks[i].y * 200 / fireworks[i].z;
        
        if (screenX >= 0 && screenX < 320 && screenY >= 0 && screenY < 240) {
          int size = max(1.0f, 20.0f / fireworks[i].z * 10);
          gfx->fillCircle(screenX, screenY, size, fireworks[i].color);
        }
      }
      
      // Explode when close or life runs out
      if (fireworks[i].z < 50 || fireworks[i].life < 20) {
        fireworks[i].exploded = true;
        
        // Create explosion particles
        for (int p = 0; p < 8; p++) {
          fireworks[i].particles[p].x = fireworks[i].x;
          fireworks[i].particles[p].y = fireworks[i].y;
          fireworks[i].particles[p].z = fireworks[i].z;
          
          float angle = p * PI / 4;
          float speed = 3;
          fireworks[i].particles[p].vx = cos(angle) * speed;
          fireworks[i].particles[p].vy = sin(angle) * speed;
          fireworks[i].particles[p].vz = random(4) - 2;
          
          fireworks[i].particles[p].color = fireworks[i].color;
          fireworks[i].particles[p].life = 60;
          fireworks[i].particles[p].active = true;
        }
      }
    }
    
    // Update explosion particles
    if (fireworks[i].exploded) {
      bool anyActive = false;
      
      for (int p = 0; p < 8; p++) {
        if (fireworks[i].particles[p].active) {
          fireworks[i].particles[p].x += fireworks[i].particles[p].vx;
          fireworks[i].particles[p].y += fireworks[i].particles[p].vy;
          fireworks[i].particles[p].z += fireworks[i].particles[p].vz;
          fireworks[i].particles[p].life--;
          
          // Project particle to screen
          if (fireworks[i].particles[p].z > 5) {
            int screenX = 160 + fireworks[i].particles[p].x * 200 / fireworks[i].particles[p].z;
            int screenY = 120 + fireworks[i].particles[p].y * 200 / fireworks[i].particles[p].z;
            
            if (screenX >= 0 && screenX < 320 && screenY >= 0 && screenY < 240) {
              float intensity = (float)fireworks[i].particles[p].life / 60.0;
              uint8_t r = ((fireworks[i].particles[p].color >> 11) & 0x1F) * intensity;
              uint8_t g = ((fireworks[i].particles[p].color >> 5) & 0x3F) * intensity;
              uint8_t b = (fireworks[i].particles[p].color & 0x1F) * intensity;
              
              gfx->drawPixel(screenX, screenY, gfx->color565(
                (r * 255) / 31,
                (g * 255) / 63,
                (b * 255) / 31
              ));
            }
          }
          
          if (fireworks[i].particles[p].life > 0) {
            anyActive = true;
          } else {
            fireworks[i].particles[p].active = false;
          }
        }
      }
      
      if (!anyActive) {
        fireworks[i].life = 0;
        fireworks[i].exploded = false;
      }
    }
  }
}

void drawSmokeLava() {
  static struct LavaBlob {
    float x, y, vx, vy, size, heat;
    uint16_t color;
    bool active;
  } blobs[25];
  static struct SmokeParticle {
    float x, y, vx, vy, life, maxLife;
    uint16_t color;
    bool active;
  } smoke[40];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float lavaTime = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 25; i++) {
      blobs[i].active = false;
    }
    for (int i = 0; i < 40; i++) {
      smoke[i].active = false;
    }
    initialized = true;
  }
  
  lavaTime += 0.03;
  
  // Dark rocky background
  for (int i = 0; i < 300; i++) {
    int x = random(320);
    int y = random(240);
    gfx->drawPixel(x, y, gfx->color565(20, 10, 5));
  }
  
  // Spawn lava blobs from bottom
  if (random(100) < 20) {
    for (int i = 0; i < 25; i++) {
      if (!blobs[i].active) {
        blobs[i].x = random(280) + 20;
        blobs[i].y = 230;
        blobs[i].vx = (random(40) - 20) / 20.0;
        blobs[i].vy = -(random(30) + 10) / 10.0;
        blobs[i].size = random(8) + 4;
        blobs[i].heat = random(100) / 100.0 + 0.5;
        
        // Lava colors - red to yellow based on heat
        if (blobs[i].heat > 0.8) {
          blobs[i].color = gfx->color565(255, 200, 100);
        } else if (blobs[i].heat > 0.6) {
          blobs[i].color = gfx->color565(255, 150, 50);
        } else {
          blobs[i].color = gfx->color565(200, 50, 20);
        }
        
        blobs[i].active = true;
        break;
      }
    }
  }
  
  // Update and draw lava blobs
  for (int i = 0; i < 25; i++) {
    if (blobs[i].active) {
      blobs[i].x += blobs[i].vx;
      blobs[i].y += blobs[i].vy;
      blobs[i].vy += 0.1; // Gravity
      blobs[i].heat -= 0.01; // Cool down
      
      // Draw lava blob
      if (blobs[i].x >= 0 && blobs[i].x < 320 && 
          blobs[i].y >= 0 && blobs[i].y < 240 && blobs[i].heat > 0) {
        
        // Heat affects color
        uint8_t r = 255 * blobs[i].heat;
        uint8_t g = 150 * blobs[i].heat;
        uint8_t b = 50 * blobs[i].heat;
        
        uint16_t heatColor = gfx->color565(r, g, b);
        
        // Draw blob with glow
        int radius = blobs[i].size * blobs[i].heat;
        for (int py = -radius; py <= radius; py++) {
          for (int px = -radius; px <= radius; px++) {
            if (px*px + py*py <= radius*radius) {
              int drawX = blobs[i].x + px;
              int drawY = blobs[i].y + py;
              if (drawX >= 0 && drawX < 320 && drawY >= 0 && drawY < 240) {
                gfx->drawPixel(drawX, drawY, heatColor);
              }
            }
          }
        }
        
        // Spawn smoke particles
        if (random(100) < 30 && blobs[i].heat > 0.3) {
          for (int s = 0; s < 40; s++) {
            if (!smoke[s].active) {
              smoke[s].x = blobs[i].x + random(10) - 5;
              smoke[s].y = blobs[i].y - 5;
              smoke[s].vx = (random(20) - 10) / 20.0;
              smoke[s].vy = -(random(20) + 10) / 10.0;
              smoke[s].life = smoke[s].maxLife = random(80) + 40;
              smoke[s].color = gfx->color565(
                60 + random(40),
                50 + random(30),
                40 + random(20)
              );
              smoke[s].active = true;
              break;
            }
          }
        }
      }
      
      // Remove cooled or off-screen blobs
      if (blobs[i].heat <= 0 || blobs[i].y > 250 || 
          blobs[i].x < -20 || blobs[i].x > 340) {
        blobs[i].active = false;
      }
    }
  }
  
  // Update and draw smoke
  for (int i = 0; i < 40; i++) {
    if (smoke[i].active) {
      smoke[i].x += smoke[i].vx + sin(lavaTime + i) * 0.3; // Turbulence
      smoke[i].y += smoke[i].vy;
      smoke[i].vy *= 0.98; // Slow rise
      smoke[i].life--;
      
      if (smoke[i].x >= 0 && smoke[i].x < 320 && 
          smoke[i].y >= 0 && smoke[i].y < 240 && smoke[i].life > 0) {
        
        float intensity = (float)smoke[i].life / smoke[i].maxLife;
        uint8_t r = ((smoke[i].color >> 11) & 0x1F) * intensity;
        uint8_t g = ((smoke[i].color >> 5) & 0x3F) * intensity;
        uint8_t b = (smoke[i].color & 0x1F) * intensity;
        
        gfx->drawPixel(smoke[i].x, smoke[i].y, gfx->color565(
          (r * 255) / 31,
          (g * 255) / 63,
          (b * 255) / 31
        ));
      }
      
      if (smoke[i].life <= 0) {
        smoke[i].active = false;
      }
    }
  }
}

void drawTetrisRain() {
  static struct TetrisBlock {
    int x, y, type, rotation;
    uint16_t color;
    bool active, landed;
    float fallSpeed;
  } blocks[20];
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float gameTime = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    for (int i = 0; i < 20; i++) {
      blocks[i].active = false;
      blocks[i].landed = false;
    }
    initialized = true;
  }
  
  gameTime += 0.1;
  
  // Dark game background with grid lines
  fillScreenExceptClock(gfx->color565(5, 5, 10));
  
  // Draw game grid
  for (int x = 0; x < 320; x += 20) {
    for (int y = 0; y < 240; y += 20) {
      gfx->drawPixel(x, y, gfx->color565(20, 20, 30));
    }
  }
  
  // Spawn new blocks
  if (random(100) < 8) {
    for (int i = 0; i < 20; i++) {
      if (!blocks[i].active) {
        blocks[i].x = (random(15) + 1) * 20;
        blocks[i].y = -20;
        blocks[i].type = random(7);
        blocks[i].rotation = random(4);
        blocks[i].fallSpeed = random(20) / 10.0 + 1.0;
        blocks[i].landed = false;
        
        // Tetris colors
        uint16_t tetrisColors[7] = {
          gfx->color565(255, 100, 100),  // I-piece red
          gfx->color565(100, 255, 100),  // O-piece green  
          gfx->color565(100, 100, 255),  // T-piece blue
          gfx->color565(255, 255, 100),  // S-piece yellow
          gfx->color565(255, 100, 255),  // Z-piece magenta
          gfx->color565(100, 255, 255),  // J-piece cyan
          gfx->color565(255, 150, 100)   // L-piece orange
        };
        blocks[i].color = tetrisColors[blocks[i].type];
        blocks[i].active = true;
        break;
      }
    }
  }
  
  // Update and draw blocks
  for (int i = 0; i < 20; i++) {
    if (blocks[i].active) {
      if (!blocks[i].landed) {
        blocks[i].y += blocks[i].fallSpeed;
        
        // Check for landing
        if (blocks[i].y >= 220) {
          blocks[i].y = 220;
          blocks[i].landed = true;
        }
      }
      
      // Draw tetris block shape
      int blockSize = 16;
      int shapes[7][4][2] = {
        {{0,0},{1,0},{2,0},{3,0}}, // I
        {{0,0},{1,0},{0,1},{1,1}}, // O  
        {{1,0},{0,1},{1,1},{2,1}}, // T
        {{1,0},{2,0},{0,1},{1,1}}, // S
        {{0,0},{1,0},{1,1},{2,1}}, // Z
        {{0,0},{0,1},{1,1},{2,1}}, // J
        {{2,0},{0,1},{1,1},{2,1}}  // L
      };
      
      // Draw the 4 squares that make up each piece
      for (int sq = 0; sq < 4; sq++) {
        int dx = shapes[blocks[i].type][sq][0] * blockSize;
        int dy = shapes[blocks[i].type][sq][1] * blockSize;
        
        // Apply rotation (simplified)
        if (blocks[i].rotation == 1) { int temp = dx; dx = -dy; dy = temp; }
        if (blocks[i].rotation == 2) { dx = -dx; dy = -dy; }
        if (blocks[i].rotation == 3) { int temp = dx; dx = dy; dy = -temp; }
        
        int drawX = blocks[i].x + dx;
        int drawY = blocks[i].y + dy;
        
        if (drawX >= 0 && drawX < 320 && drawY >= 0 && drawY < 240) {
          // Draw filled square
          for (int py = 0; py < blockSize; py++) {
            for (int px = 0; px < blockSize; px++) {
              if (drawX + px < 320 && drawY + py < 240) {
                gfx->drawPixel(drawX + px, drawY + py, blocks[i].color);
              }
            }
          }
          
          // Draw border
          gfx->drawRect(drawX, drawY, blockSize, blockSize, gfx->color565(255, 255, 255));
        }
      }
      
      // Remove blocks that have been landed for too long
      if (blocks[i].landed && random(100) < 2) {
        blocks[i].active = false;
      }
    }
  }
}

void drawJuliaStandalone() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float juliaPhase = 0;
  static float zoom = 1.0;
  static float panX = 0, panY = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  juliaPhase += 0.03;
  zoom = 0.8 + sin(juliaPhase * 0.5) * 0.6;
  panX = sin(juliaPhase * 0.3) * 0.5;
  panY = cos(juliaPhase * 0.2) * 0.3;
  
  // Animated Julia set parameters
  float cx = -0.8 + sin(juliaPhase) * 0.2;
  float cy = cos(juliaPhase * 1.3) * 0.2;
  
  // Draw Julia set
  for (int py = 0; py < 240; py += 2) {
    for (int px = 0; px < 320; px += 2) {
      // Map to complex plane with zoom and pan
      float x = (px - 160) / (80.0 * zoom) + panX;
      float y = (py - 120) / (80.0 * zoom) + panY;
      
      // Julia iteration
      int maxIter = 64;
      int iter = 0;
      float zx = x, zy = y;
      
      while (iter < maxIter && (zx * zx + zy * zy) < 4) {
        float temp = zx * zx - zy * zy + cx;
        zy = 2 * zx * zy + cy;
        zx = temp;
        iter++;
      }
      
      // Psychedelic coloring
      uint16_t color;
      if (iter == maxIter) {
        color = gfx->color565(0, 0, 0);
      } else {
        float t = (float)iter / maxIter;
        t += sin(juliaPhase * 3 + px * 0.01 + py * 0.01) * 0.3;
        t = max(0.0f, min(1.0f, t));
        
        // Rainbow spectrum
        float hue = t * 6.0 + juliaPhase;
        int segment = (int)hue % 6;
        float f = hue - segment;
        
        uint8_t r, g, b;
        switch(segment) {
          case 0: r = 255; g = f * 255; b = 0; break;
          case 1: r = (1-f) * 255; g = 255; b = 0; break;
          case 2: r = 0; g = 255; b = f * 255; break;
          case 3: r = 0; g = (1-f) * 255; b = 255; break;
          case 4: r = f * 255; g = 0; b = 255; break;
          case 5: r = 255; g = 0; b = (1-f) * 255; break;
          default: r = 0; g = 0; b = 0; break;
        }
        
        color = gfx->color565(r, g, b);
      }
      
      // Draw 2x2 blocks
      gfx->drawPixel(px, py, color);
      if (px + 1 < 320) gfx->drawPixel(px + 1, py, color);
      if (py + 1 < 240) gfx->drawPixel(px, py + 1, color);
      if (px + 1 < 320 && py + 1 < 240) gfx->drawPixel(px + 1, py + 1, color);
    }
  }
}

void drawRetroGeometry() {
  static bool initialized = false;
  static AnimationMode lastMode = PLASMA_FIELD;
  static float retroTime = 0;
  static float gridPhase = 0;
  
  if (currentMode != lastMode) {
    initialized = false;
    lastMode = currentMode;
    fillScreenExceptClock(gfx->color565(0, 0, 0));
  }
  
  if (!initialized) {
    fillScreenExceptClock(gfx->color565(0, 0, 0));
    initialized = true;
  }
  
  retroTime += 0.05;
  gridPhase += 0.1;
  
  // Retro gradient background
  for (int y = 0; y < 240; y++) {
    float gradientPos = (float)y / 240.0;
    uint8_t r = (sin(gradientPos * PI + retroTime) + 1) * 60 + 20;
    uint8_t g = (sin(gradientPos * PI + retroTime + PI/3) + 1) * 40 + 10;  
    uint8_t b = (sin(gradientPos * PI + retroTime + 2*PI/3) + 1) * 80 + 30;
    
    uint16_t bgColor = gfx->color565(r, g, b);
    for (int x = 0; x < 320; x++) {
      gfx->drawPixel(x, y, bgColor);
    }
  }
  
  // Retro perspective grid
  int vanishY = 120 + sin(retroTime) * 20;
  for (int line = 0; line < 20; line++) {
    float depth = (line + gridPhase) / 20.0;
    if (depth > 1.0) depth -= 1.0;
    
    int y = vanishY + (240 - vanishY) * depth;
    if (y >= 0 && y < 240) {
      uint8_t intensity = 100 + depth * 155;
      uint16_t gridColor = gfx->color565(intensity, 0, intensity/2);
      
      for (int x = 0; x < 320; x++) {
        gfx->drawPixel(x, y, gridColor);
      }
    }
  }
  
  // Vertical perspective lines
  for (int line = 0; line < 10; line++) {
    int baseX = 32 + line * 32;
    
    for (int y = vanishY; y < 240; y++) {
      float perspective = (float)(y - vanishY) / (240 - vanishY);
      int x = 160 + (baseX - 160) * perspective;
      
      if (x >= 0 && x < 320) {
        uint8_t intensity = 80 + perspective * 120;
        uint16_t lineColor = gfx->color565(intensity, 0, intensity/2);
        gfx->drawPixel(x, y, lineColor);
      }
    }
  }
  
  // Floating retro shapes
  for (int shape = 0; shape < 6; shape++) {
    float shapeTime = retroTime + shape * PI/3;
    int centerX = 160 + sin(shapeTime) * 80;
    int centerY = 60 + cos(shapeTime * 0.7) * 30;
    int size = 15 + sin(shapeTime * 2) * 8;
    
    uint16_t shapeColor = gfx->color565(
      200 + sin(shapeTime) * 55,
      100 + cos(shapeTime * 1.3) * 100,
      255
    );
    
    if (shape % 3 == 0) {
      // Triangle
      for (int angle = 0; angle < 3; angle++) {
        float a1 = angle * 2 * PI / 3 + shapeTime;
        float a2 = (angle + 1) * 2 * PI / 3 + shapeTime;
        
        int x1 = centerX + cos(a1) * size;
        int y1 = centerY + sin(a1) * size;
        int x2 = centerX + cos(a2) * size;
        int y2 = centerY + sin(a2) * size;
        
        // Draw triangle edge
        int steps = max(abs(x2-x1), abs(y2-y1));
        for (int i = 0; i <= steps; i++) {
          int x = x1 + (x2-x1) * i / steps;
          int y = y1 + (y2-y1) * i / steps;
          if (x >= 0 && x < 320 && y >= 0 && y < 240) {
            gfx->drawPixel(x, y, shapeColor);
          }
        }
      }
    } else if (shape % 3 == 1) {
      // Square
      for (int side = 0; side < 4; side++) {
        float angle = side * PI/2 + shapeTime;
        int x1 = centerX + cos(angle) * size;
        int y1 = centerY + sin(angle) * size;
        int x2 = centerX + cos(angle + PI/2) * size;
        int y2 = centerY + sin(angle + PI/2) * size;
        
        int steps = max(abs(x2-x1), abs(y2-y1));
        for (int i = 0; i <= steps; i++) {
          int x = x1 + (x2-x1) * i / steps;
          int y = y1 + (y2-y1) * i / steps;
          if (x >= 0 && x < 320 && y >= 0 && y < 240) {
            gfx->drawPixel(x, y, shapeColor);
          }
        }
      }
    } else {
      // Circle
      for (int angle = 0; angle < 360; angle += 5) {
        float rad = angle * PI / 180;
        int x = centerX + cos(rad) * size;
        int y = centerY + sin(rad) * size;
        if (x >= 0 && x < 320 && y >= 0 && y < 240) {
          gfx->drawPixel(x, y, shapeColor);
        }
      }
    }
  }
}

void handleRoot() {
  String html = "<html><body><h1>Psychedelic Clock Settings</h1>";
  html += "<p><strong>Clock Display:</strong> " + String(showClock ? "ON" : "OFF") + "</p>";
  
  // Show current time
  String timeStatus = "Boot Time";
  if (rtcAvailable) {
    timeStatus = "RTC Hardware";
  } else if (useManualTime) {
    timeStatus = "Manual Time";
  }
  html += "<p><strong>Time Source:</strong> " + timeStatus + "</p>";
  
  html += "<a href='/toggle'><button>Toggle Clock Display</button></a><br><br>";
  html += "<h3>Set Time</h3>";
  html += "<form action='/settime' method='get'>";
  html += "Hour: <input type='number' name='hour' min='0' max='23' value='12'><br><br>";
  html += "Minute: <input type='number' name='minute' min='0' max='59' value='0'><br><br>";
  html += "<input type='submit' value='Set Time' style='padding:10px;font-size:16px;'>";
  html += "</form>";
  html += "<p><small>Note: Time will be set and remembered until device restart</small></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleToggleClock() {
  showClock = !showClock;
  server.send(200, "text/html", "<html><body><h1>Clock Toggled!</h1><a href='/'>Back</a></body></html>");
}

void handleSetTime() {
  if (server.hasArg("hour") && server.hasArg("minute")) {
    int hour = server.arg("hour").toInt();
    int minute = server.arg("minute").toInt();
    if (rtcAvailable) {
      DateTime now = rtc.now();
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, 0));
    } else {
      // Set manual time
      useManualTime = true;
      manualHour = hour;
      manualMinute = minute;
      manualTimeSet = millis();
      Serial.printf("Manual time set to %02d:%02d\n", hour, minute);
    }
  }
  server.send(200, "text/html", "<html><body><h1>Time Set!</h1><a href='/'>Back</a></body></html>");
}
