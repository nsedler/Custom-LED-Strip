#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <LittleFS.h>

#include "credentials.h"

#define t 32
#define p 750

#define ledPin D3
#define numLeds p
#define ledType WS2812B
#define colorOrder GRB
#define framesPerSecond 120

CRGB leds[numLeds];
CRGB currentColor(95, 255, 255);

ESP8266WebServer server(80);

bool rainbowWave = 0;
bool rainbowColors = 0;
bool blink = 0;
bool sinelon = 0;

uint8_t brightness = 55;
uint16_t interval = 50;
bool power = 1;

void handleRoot();
void handleJSFile();
void handleLedPower();

void handleSetColor();
void handleBrightness();
void handleMode();
void handleInterval();

void toggleBrightness();
void rainbowWaveEffect();
void rainbowColorsEffect();
void sinelonEffect();

void setup() {
  delay(3000);

  Serial.begin(115200);

  IPAddress staticIP(192, 168, 4, 48);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 252, 0);

  // Connect to Wi-Fi with hardcoded values
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, gateway,
              subnet); // Set the static IP, gateway, and subnet
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F(""));
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Define RESTful routes
  // GETS
  server.on("/", HTTP_GET, handleRoot);
  server.on("/data/index.js", HTTP_GET, handleJSFile);
  // POSTS
  server.on("/api/led/power", HTTP_POST, handleLedPower);
  server.on("/api/led/color", HTTP_POST, handleSetColor);
  server.on("/api/led/brightness", HTTP_POST, handleBrightness);
  server.on("/api/led/mode", HTTP_POST, handleMode);
  server.on("/api/led/interval", HTTP_POST, handleInterval);

  // Start the server
  server.begin();

  FastLED.addLeds<ledType, ledPin, colorOrder>(leds, numLeds)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  fill_solid(leds, numLeds, currentColor);
  FastLED.show();

  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS filesystem");
  }
  LittleFS.begin();
}

unsigned long previousMillis = 0;

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("Wifi Not Connected!"));
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  // Handle incoming client requests
  server.handleClient();

  unsigned long currentMillis = millis();

  // Check and perform the selected effect based on elapsed time
  if (rainbowWave == 1 && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Save the last execution time
    rainbowWaveEffect();
  }

  if (rainbowColors == 1 && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Save the last execution time
    rainbowColorsEffect();
  }

  if (blink == 1 && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Save the last execution time
    toggleBrightness();
  }

  if (sinelon == 1) {
    sinelonEffect();
  }
  FastLED.delay(1000 / 250);
}

void toggleBrightness() {
  if (FastLED.getBrightness() > 0) {
    FastLED.setBrightness(0);
  } else {
    FastLED.setBrightness(brightness);
  }
  FastLED.show();
}

void rainbowWaveEffect() {
  static uint8_t hue = 0;

  for (int i = 0; i < numLeds; i++) {
    leds[i] = CHSV(hue + (i * 256 / numLeds), 255, 255);
  }

  FastLED.show();

  hue += 1;
  if (hue >= 255) {
    hue = 0;
  }
}

void rainbowColorsEffect() {
  static uint8_t hue = 0;

  fill_solid(leds, numLeds, CHSV(hue, 255, 255));

  FastLED.show();

  hue += 1;
  if (hue >= 255) {
    hue = 0;
  }
}

void sinelonEffect() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, numLeds, 20);
  int pos = beatsin16(10, 0, numLeds - 1);
  leds[pos] += currentColor;
}

void handleRoot() {
  Serial.println(F("Root loaded"));
  char htmlFilename[] = "/index.html"; // Specify the path to your HTML file
  char htmlContentType[] = "text/html";

  File file = LittleFS.open(htmlFilename, "r");
  if (file) {
    server.streamFile(file, htmlContentType);
    file.close();
    return;
  }

  server.send(404, "text/plain", "File not found");
}

void handleJSFile() {
  char jsFilename[] = "/index.js"; // Specify the path to your JavaScript file
  char jsContentType[] = "application/javascript";

  File file = LittleFS.open(jsFilename, "r");
  if (file) {
    server.streamFile(file, jsContentType);
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
  }
}

void handleLedPower() {
  // Turn off the LEDs (you can customize this logic)
  Serial.print(F("Led power set to: "));
  Serial.println(power);
  toggleBrightness();
  power = !power;
  server.send(200, "application/json", "{\"status\":\"LEDs turned off\"}");
}

void handleSetColor() {
  rainbowWave = 0;
  String color =
      server.arg("color"); // Get the "color" parameter from the request body
  String r = server.arg("r");
  String g = server.arg("g");
  String b = server.arg("b");

  // Convert the color value to CRGB format
  CRGB newColor(r.toInt(), g.toInt(), b.toInt());
  
  Serial.println("Color set to: " + r + "," + g + "," + b);
  // Set the LEDs to the new color
  fill_solid(leds, numLeds, newColor);
  FastLED.show();
  currentColor = newColor;
  server.send(200, "application/json", "{\"status\":\"Color set\"}");
}

void handleBrightness() {
  String brightString = server.arg("brightness");
  int brightInt = brightString.toInt();
  brightness = brightInt;
  Serial.print(F("Brightness set to "));
  Serial.println(brightness);
  FastLED.setBrightness(brightInt);
  FastLED.show();

  server.send(200, "application/json", "{\"status\":\"Brightness set\"}");
}

void handleMode() {
  String ledMode = server.arg("mode");
  if (ledMode == "rainbowWave") {
    rainbowWave = 1;
    blink = 0;
    sinelon = 0;
    rainbowColors = 0;
  } else if (ledMode == "rainbowColors") {
    rainbowWave = 0;
    blink = 0;
    sinelon = 0;
    rainbowColors = 1;
  } else if (ledMode == "blink") {
    blink = 1;
    rainbowWave = 0;
    sinelon = 0;
    rainbowColors = 0;
  } else if (ledMode == "sinelon") {
    sinelon = 1;
    rainbowWave = 0;
    blink = 0;
    rainbowColors = 0;
  } else if (ledMode == "solid") {
    sinelon = 0;
    rainbowWave = 0;
    blink = 0;
    rainbowColors = 0;

    fill_solid(leds, numLeds, currentColor);
    FastLED.show();
  }
  server.send(200, "application/json", "{\"status\":\"Mode set\"}");
}

void handleInterval() {
  String interStr = server.arg("interval");
  interval = interStr.toInt();
  server.send(200, "application/json", "{\"status\":\"Mode set\"}");
}