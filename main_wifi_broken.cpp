// Minimal ESP32 Screensaver Framework - First 3 Effects
// abstractile.c, aligned_malloc.c, alpha.c

#include <Arduino_GFX_Library.h>
#include <XPT2046_Touchscreen.h>
#include <WiFi.h>
#include <WebServer.h>
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

// WiFi AP setup
const char* ssid = "XScreens1";
const char* password = "xxx123";
WebServer server(80);

// Global variables
AnimationMode currentMode = ABSTRACTILE;
float animationTime = 0;
bool bootPressed = false;
unsigned long lastPress = 0;

// Settings
bool autoScroll = false;
int autoScrollSeconds = 5;
bool showStatusBox = true;

// Function declarations
String getEffectName(AnimationMode mode);

// Web server handlers
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>XScreens Control</title></head><body>";
  html += "<h1>XScreens Control Panel</h1>";
  html += "<p>Current Effect: " + String(currentMode) + " (" + getEffectName(currentMode) + ")</p>";
  html += "<p>Auto Scroll: " + String(autoScroll ? "ON" : "OFF") + "</p>";
  html += "<p>Auto Scroll Seconds: " + String(autoScrollSeconds) + "</p>";
  html += "<p>Status Box: " + String(showStatusBox ? "ON" : "OFF") + "</p>";
  html += "<br>";
  html += "<a href='/toggle-autoscroll'><button>Toggle Auto Scroll</button></a><br><br>";
  html += "<form action='/set-seconds' method='get'>";
  html += "Auto Scroll Seconds: <input type='number' name='seconds' value='" + String(autoScrollSeconds) + "' min='1' max='60'>";
  html += "<input type='submit' value='Set'></form><br>";
  html += "<a href='/toggle-status'><button>Toggle Status Box</button></a><br><br>";
  html += "<a href='/'><button>Refresh</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleToggleAutoScroll() {
  autoScroll = !autoScroll;
  Serial.printf("Auto scroll: %s\n", autoScroll ? "ON" : "OFF");
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSetSeconds() {
  if (server.hasArg("seconds")) {
    autoScrollSeconds = server.arg("seconds").toInt();
    if (autoScrollSeconds < 1) autoScrollSeconds = 1;
    if (autoScrollSeconds > 60) autoScrollSeconds = 60;
    Serial.printf("Auto scroll seconds set to: %d\n", autoScrollSeconds);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleToggleStatus() {
  showStatusBox = !showStatusBox;
  Serial.printf("Status box: %s\n", showStatusBox ? "ON" : "OFF");
  server.sendHeader("Location", "/");
  server.send(303);
}

String getEffectName(AnimationMode mode) {
  switch(mode) {
    case ABSTRACTILE: return "Abstractile";
    case ALIGNED_MALLOC: return "Aligned Malloc";
    case ALPHA: return "Alpha";
    default: return "Unknown";
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Minimal 3-Effect Screensaver");
  
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
  
  // Setup WiFi AP
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.printf("WiFi AP Started: %s\n", ssid);
  Serial.printf("AP IP address: %s\n", IP.toString().c_str());
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/toggle-autoscroll", handleToggleAutoScroll);
  server.on("/set-seconds", handleSetSeconds);
  server.on("/toggle-status", handleToggleStatus);
  server.begin();
  Serial.println("Web server started");
  
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
  // Handle web server
  server.handleClient();
  
  // Auto scroll check
  static unsigned long lastAutoScroll = 0;
  if (autoScroll && (millis() - lastAutoScroll > autoScrollSeconds * 1000)) {
    currentMode = (AnimationMode)((currentMode + 1) % MODE_COUNT);
    Serial.printf("Auto-switched to mode %d\n", currentMode);
    lastAutoScroll = millis();
    gfx->fillScreen(BLACK);
  }
  
  // Check boot button for mode switch
  if (digitalRead(BOOT_PIN) == LOW) {
    if (!bootPressed && (millis() - lastPress > 300)) {
      currentMode = (AnimationMode)((currentMode + 1) % MODE_COUNT);
      Serial.printf("Switched to mode %d\n", currentMode);
      lastPress = millis();
      lastAutoScroll = millis(); // Reset auto scroll timer
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
  }
  
  // Draw status box if enabled
  if (showStatusBox) {
    int boxHeight = 25;
    int y = gfx->height() - boxHeight;
    gfx->fillRect(0, y, gfx->width(), boxHeight, BLACK);
    gfx->drawRect(0, y, gfx->width(), boxHeight, WHITE);
    
    gfx->setTextColor(WHITE);
    gfx->setTextSize(1);
    gfx->setCursor(5, y + 5);
    gfx->printf("Effect %d: %s", currentMode + 1, getEffectName(currentMode).c_str());
    
    gfx->setCursor(5, y + 15);
    gfx->printf("AutoScroll: %s (%ds)", autoScroll ? "ON" : "OFF", autoScrollSeconds);
    
    // Show IP on right side
    gfx->setCursor(gfx->width() - 80, y + 10);
    gfx->printf("IP: %s", WiFi.softAPIP().toString().c_str());
  }
  
  animationTime += 0.05;
  delay(50);
}