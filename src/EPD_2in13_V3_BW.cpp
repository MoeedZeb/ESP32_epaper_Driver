#include "EPD_2in13_V3_BW.h"
#include <cstring>

// ---- LUTs copied from your Python driver exactly ----
const uint8_t EPD_2in13_V3_BW::LUT_PARTIAL[159] = {
  0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x14,0x0,0x0,0x0,0x0,0x0,0x0,
  0x1,0x0,0x0,0x0,0x0,0x0,0x0,
  0x1,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,
  0x22,0x17,0x41,0x00,0x32,0x36
};

const uint8_t EPD_2in13_V3_BW::LUT_FULL[159] = {
  0x80,0x4A,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x40,0x4A,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x80,0x4A,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x40,0x4A,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0xF,0x0,0x0,0x0,0x0,0x0,0x0,
  0xF,0x0,0x0,0xF,0x0,0x0,0x2,
  0xF,0x0,0x0,0x0,0x0,0x0,0x0,
  0x1,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,
  0x22,0x17,0x41,0x0,0x32,0x36
};
// Rotate a 250x122 buffer (landscape) into 122x250 buffer (portrait RAM order)
// This matches your current working portrait driver mapping.
static void rotate90CW_250x122_to_122x250(const uint8_t* src, uint8_t* dst)
{
  const int SW = 250, SH = 122;
  const int DW = 122, DH = 250;

  const int srcBPL = (SW + 7) / 8; // 32
  const int dstBPL = (DW + 7) / 8; // 16

  // start white
  memset(dst, 0xFF, dstBPL * DH);

  auto srcBlack = [&](int x, int y) -> bool {
    int idx = y * srcBPL + (x >> 3);
    uint8_t mask = 0x80 >> (x & 7);
    return (src[idx] & mask) == 0; // 0=black
  };

  auto dstSet = [&](int x, int y, bool black) {
    int idx = y * dstBPL + (x >> 3);
    uint8_t mask = 0x80 >> (x & 7);
    if (black) dst[idx] &= ~mask; else dst[idx] |= mask;
  };

  // CW rotation:
  // dst_x = y
  // dst_y = (SW - 1 - x)
  for (int y = 0; y < SH; y++) {
    for (int x = 0; x < SW; x++) {
      bool black = srcBlack(x, y);
      int dx = y;
      int dy = (SW - 1 - x);
      if (dx >= 0 && dx < DW && dy >= 0 && dy < DH) {
        dstSet(dx, dy, black);
      }
    }
  }
}
// ---- Class implementation ----

EPD_2in13_V3_BW::EPD_2in13_V3_BW(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t busy,
                                 SPIClass& spi, uint32_t spi_hz)
: _spi(spi), _spiHz(spi_hz), _cs(cs), _dc(dc), _rst(rst), _busy(busy)
{}

bool EPD_2in13_V3_BW::begin()
{
  pinMode(_cs, OUTPUT);
  pinMode(_dc, OUTPUT);
  pinMode(_rst, OUTPUT);
  pinMode(_busy, INPUT);

  digitalWrite(_cs, HIGH);
  digitalWrite(_dc, LOW);
  digitalWrite(_rst, HIGH);

  // Match Pi epdconfig:
  // SPI.max_speed_hz = 4000000, SPI.mode = 0b00
  _spi.begin(); // uses default ESP32 pins unless you call SPI.begin(sck,miso,mosi,ss)

  resetHW();

  waitBusy("after reset");
  sendCommand(0x12); // SWRESET
  waitBusy("after swreset");

  // Driver output control
  sendCommand(0x01);
  sendData(0xF9);
  sendData(0x00);
  sendData(0x00);

  // Data entry mode
  sendCommand(0x11);
  sendData(0x03);

  setWindow(0, 0, WIDTH - 1, HEIGHT - 1);
  setCursor(0, 0);

  // Border waveform
  sendCommand(0x3C);
  sendData(0x05);

  // Display update control
  sendCommand(0x21);
  sendData(0x00);
  sendData(0x80);

  sendCommand(0x18);
  sendData(0x80);

  waitBusy("before setLUT");
  setLut(LUT_FULL);
  return true;
}

void EPD_2in13_V3_BW::resetHW()
{
  // Python reset():
  // RST=1 20ms, RST=0 2ms, RST=1 20ms
  digitalWrite(_rst, HIGH);
  delay(20);
  digitalWrite(_rst, LOW);
  delay(2);
  digitalWrite(_rst, HIGH);
  delay(20);
}

void EPD_2in13_V3_BW::waitBusy(const char* tag)
{
  (void)tag;
  // Python ReadBusy():
  // while busy == 1: delay 10ms
  while (digitalRead(_busy) == HIGH) {
    delay(10);
  }
}

void EPD_2in13_V3_BW::sendCommand(uint8_t cmd)
{
  SPISettings s(_spiHz, MSBFIRST, SPI_MODE0);
  _spi.beginTransaction(s);
  digitalWrite(_dc, LOW);
  digitalWrite(_cs, LOW);
  _spi.transfer(cmd);
  digitalWrite(_cs, HIGH);
  _spi.endTransaction();
}

void EPD_2in13_V3_BW::sendData(uint8_t data)
{
  SPISettings s(_spiHz, MSBFIRST, SPI_MODE0);
  _spi.beginTransaction(s);
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, LOW);
  _spi.transfer(data);
  digitalWrite(_cs, HIGH);
  _spi.endTransaction();
}

void EPD_2in13_V3_BW::sendDataBulk(const uint8_t* data, size_t len)
{
  SPISettings s(_spiHz, MSBFIRST, SPI_MODE0);
  _spi.beginTransaction(s);
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, LOW);
  while (len--) {
    _spi.transfer(*data++);
  }
  digitalWrite(_cs, HIGH);
  _spi.endTransaction();
}

void EPD_2in13_V3_BW::lutWrite(const uint8_t* lut)
{
  sendCommand(0x32);
  // Python sends 153 bytes for LUT waveform
  sendDataBulk(lut, 153);
  waitBusy("after lut");
}

void EPD_2in13_V3_BW::setLut(const uint8_t* lut)
{
  // Mirrors Python SetLut():
  lutWrite(lut);

  sendCommand(0x3F);
  sendData(lut[153]);

  sendCommand(0x03); // gate voltage
  sendData(lut[154]);

  sendCommand(0x04); // source voltage
  sendData(lut[155]); // VSH
  sendData(lut[156]); // VSH2
  sendData(lut[157]); // VSL

  sendCommand(0x2C); // VCOM
  sendData(lut[158]);
}

void EPD_2in13_V3_BW::setWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
  sendCommand(0x44);
  sendData((x_start >> 3) & 0xFF);
  sendData((x_end   >> 3) & 0xFF);

  sendCommand(0x45);
  sendData(y_start & 0xFF);
  sendData((y_start >> 8) & 0xFF);
  sendData(y_end & 0xFF);
  sendData((y_end >> 8) & 0xFF);
}

void EPD_2in13_V3_BW::setCursor(uint16_t x, uint16_t y)
{
  sendCommand(0x4E);
  sendData(x & 0xFF);

  sendCommand(0x4F);
  sendData(y & 0xFF);
  sendData((y >> 8) & 0xFF);
}

void EPD_2in13_V3_BW::turnOnDisplay()
{
  sendCommand(0x22);
  sendData(0xC7);
  sendCommand(0x20);
  waitBusy("turnOnDisplay");
}

void EPD_2in13_V3_BW::turnOnDisplayPart()
{
  sendCommand(0x22);
  sendData(0x0F); // matches python "quality: 0x0f"
  sendCommand(0x20);
  waitBusy("turnOnDisplayPart");
}

void EPD_2in13_V3_BW::display(const uint8_t* image)
{
  setWindow(0, 0, WIDTH - 1, HEIGHT - 1);
  setCursor(0, 0);

  sendCommand(0x24); // WRITE_RAM
  sendDataBulk(image, BUFFER_SIZE);
  turnOnDisplay();
}

void EPD_2in13_V3_BW::displayPartial(const uint8_t* image)
{
  // Python does a tiny reset pulse:
  digitalWrite(_rst, LOW);
  delay(1);
  digitalWrite(_rst, HIGH);

  setLut(LUT_PARTIAL);

  // 0x37 block (10 bytes) exactly like python
  sendCommand(0x37);
  sendData(0x00);
  sendData(0x00);
  sendData(0x00);
  sendData(0x00);
  sendData(0x00);
  sendData(0x40);
  sendData(0x00);
  sendData(0x00);
  sendData(0x00);
  sendData(0x00);

  // BorderWavefrom
  sendCommand(0x3C);
  sendData(0x80);

  // Update control before partial write
  sendCommand(0x22);
  sendData(0xC0);
  sendCommand(0x20);
  waitBusy("pre-partial-update");

  setWindow(0, 0, WIDTH - 1, HEIGHT - 1);
  setCursor(0, 0);

  sendCommand(0x24); // WRITE_RAM
  sendDataBulk(image, BUFFER_SIZE);
  turnOnDisplayPart();
}

void EPD_2in13_V3_BW::clear(uint8_t color)
{
  static uint8_t blank[BUFFER_SIZE];
  memset(blank, color, BUFFER_SIZE);
  display(blank);
}

void EPD_2in13_V3_BW::sleep()
{
  sendCommand(0x10);
  sendData(0x01);
  delay(2000);
}

void EPD_2in13_V3_BW::displayLandscape(const uint8_t* image250x122)
{
  static uint8_t tmp[BUFFER_SIZE];
  rotate90CW_250x122_to_122x250(image250x122, tmp);
  display(tmp);
}

void EPD_2in13_V3_BW::displayPartialLandscape(const uint8_t* image250x122)
{
  static uint8_t tmp[BUFFER_SIZE];
  rotate90CW_250x122_to_122x250(image250x122, tmp);
  displayPartial(tmp);
}
