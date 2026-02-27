#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include "EPD_2in13_V3_BW.h"

static const uint8_t PIN_CS   = 5;
static const uint8_t PIN_DC   = 17;
static const uint8_t PIN_RST  = 16;
static const uint8_t PIN_BUSY = 4;

static const uint8_t PIN_SCK  = 18;
static const uint8_t PIN_MISO = 19;
static const uint8_t PIN_MOSI = 23;

EPD_2in13_V3_BW epd(PIN_CS, PIN_DC, PIN_RST, PIN_BUSY, SPI, 4000000);

// LANDSCAPE canvas
GFXcanvas1 canvas(EPD_2in13_V3_BW::LANDSCAPE_W, EPD_2in13_V3_BW::LANDSCAPE_H);

void setup() {
  Serial.begin(115200);
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);
  epd.begin();

  canvas.fillScreen(1);
  canvas.setTextColor(0);
  canvas.setFont(&FreeMonoBold12pt7b);
  canvas.setCursor(20, 50);
  canvas.print("Warm Welcome by");
  canvas.setCursor(20, 70);
  canvas.print("");
  canvas.setCursor(60, 80);
  canvas.print("Moeed Zeb");
  epd.displayLandscape(canvas.getBuffer()); // full update
  delay(2000);
}

void loop() {
  static int n = 0;

  canvas.fillScreen(1);
  canvas.setTextColor(0);
  canvas.setFont(&FreeMonoBold18pt7b);
  canvas.setCursor(10, 50);
  canvas.print("Count: ");
  canvas.print(n);

  epd.displayPartialLandscape(canvas.getBuffer()); // partial update
  n = (n + 1) % 10;

  delay(2000);
}
