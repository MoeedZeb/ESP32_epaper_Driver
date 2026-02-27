#pragma once
#include <Arduino.h>
#include <SPI.h>

// Driver that mirrors Waveshare Python epd2in13_V3.py behavior (B/W 250x122)
// Resolution in Python: width=122, height=250 (portrait)
// Buffer layout: bytes per row = ceil(width/8) = 16, total bytes = 16*250=4000

class EPD_2in13_V3_BW {
public:
  static constexpr uint16_t WIDTH  = 122;
  static constexpr uint16_t HEIGHT = 250;
  static constexpr uint16_t BYTES_PER_LINE = (WIDTH + 7) / 8; // 16
  static constexpr uint32_t BUFFER_SIZE = BYTES_PER_LINE * HEIGHT; // 4000
    // Landscape view (what you want to draw in your application)
  static constexpr uint16_t LANDSCAPE_W = 250;
  static constexpr uint16_t LANDSCAPE_H = 122;
  static constexpr uint16_t LANDSCAPE_BYTES_PER_LINE = (LANDSCAPE_W + 7) / 8; // 32
  static constexpr uint32_t LANDSCAPE_BUFFER_SIZE = LANDSCAPE_BYTES_PER_LINE * LANDSCAPE_H; // 3904

  EPD_2in13_V3_BW(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t busy,
                 SPIClass& spi = SPI, uint32_t spi_hz = 4000000);

   // NEW: accept 250x122 buffers directly (driver rotates internally)
  void displayLandscape(const uint8_t* image250x122);        // full update
  void displayPartialLandscape(const uint8_t* image250x122); // partial update
  bool begin();                 // init() equivalent
  void clear(uint8_t color = 0xFF);
  void display(const uint8_t* image);        // full update
  void displayPartial(const uint8_t* image); // partial update sequence like python
  void sleep();

  // helper: set a pixel in a buffer (MSB is leftmost pixel in each byte)
  static inline void setPixel(uint8_t* buf, int x, int y, bool black)
{
  if (!buf) return;
  if (x < 0 || y < 0 || x >= (int)WIDTH || y >= (int)HEIGHT) return;

  const uint32_t index = (uint32_t)y * BYTES_PER_LINE + (uint32_t)(x >> 3);
  const uint8_t  mask  = 0x80 >> (x & 7); // MSB-leftmost

  if (black) {
    buf[index] &= ~mask; // 0 = black
  } else {
    buf[index] |= mask;  // 1 = white
  }
}

private:
  SPIClass& _spi;
  uint32_t _spiHz;
  uint8_t _cs, _dc, _rst, _busy;

  void resetHW();
  void waitBusy(const char* tag);
  void sendCommand(uint8_t cmd);
  void sendData(uint8_t data);
  void sendDataBulk(const uint8_t* data, size_t len);

  void setWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
  void setCursor(uint16_t x, uint16_t y);

  void lutWrite(const uint8_t* lut); // writes 153 bytes at 0x32
  void setLut(const uint8_t* lut);   // mirrors python SetLut()

  void turnOnDisplay();      // python TurnOnDisplay()
  void turnOnDisplayPart();  // python TurnOnDisplayPart()

  // LUT arrays copied from your Python file (length 159 bytes: 153 + 6 config bytes)
  static const uint8_t LUT_FULL[159];
  static const uint8_t LUT_PARTIAL[159];
};
