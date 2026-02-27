# ESP32_epaper_Driver
Optimized ESP32 driver for the Waveshare 2.13" V3 tri-color E-Paper display, operating in Black &amp; White mode to reduce flicker and enable smooth partial and full refresh.

# ESP32 E-Paper 2.13" V3 – Optimized Black & White Driver for Tri-Color Panels


## Overview

This repository contains a lightweight, optimized C++ driver for the **Waveshare 2.13-inch V3 E-Paper Display**, designed specifically for ESP32 systems.

Although the hardware module is a **tri-color electrophoretic display (Black / White / Red or Yellow)**, this driver intentionally operates it in **Black & White mode only** to achieve:

* Reduced flicker
* Faster refresh
* Smooth partial updates
* Lower power consumption
* Cleaner UI transitions

This makes the display behave like a high-performance monochrome E-Paper module while running on ESP32.

---

## Why This Driver Exists

### The Problem with Tri-Color Panels on ESP32

The Waveshare 2.13" V3 module is physically a **tri-color display**.
Tri-color E-Paper panels require complex waveform sequences to properly position three types of charged pigment particles.

Most generic ESP32 E-Paper libraries:

* Apply full tri-color waveform cycles
* Execute multiple inversion passes
* Perform extended flashing sequences
* Cause strong flickering during refresh
* Increase refresh time significantly
* Consume more power

Even when only black-and-white content is displayed, these libraries still execute the full tri-color waveform.

---

### Our Solution

This driver treats the tri-color panel as a **monochrome display**.

Instead of driving all three pigment layers, it:

* Uses simplified black/white waveform logic
* Skips unnecessary red/yellow activation cycles
* Reduces inversion stages
* Optimizes partial update handling
* Maintains framebuffer-based control

This dramatically improves refresh smoothness on ESP32.

---

## Benefits

Using this optimized Black & White driver:

* ✅ Significantly reduced flicker
* ✅ Faster partial refresh
* ✅ Stable UI transitions
* ✅ Lower power usage
* ✅ Better suitability for dynamic data

Ideal for:

* IoT dashboards
* Indoor Air Quality monitors
* Smoke detectors
* Battery-powered systems
* Robotics status displays
* Embedded research projects

---

## Display Specifications

* Display Size: 2.13 inch
* Resolution: 250 × 122 pixels
* Physical Type: Tri-color electrophoretic display
* Operating Mode (this driver): Black & White
* Interface: SPI
* Controller: UC8151 / SSD1680 (V3 variant)
* Refresh: Full & Partial
* Ultra-low power (consumes power only during refresh)

---

## How E-Paper Refresh Works (Engineering Insight)

E-Paper displays use microcapsules filled with charged pigment particles.

During refresh:

1. Black inversion
2. White inversion
3. Final image stabilization

Tri-color panels require additional voltage phases for red/yellow particles, increasing flicker and refresh time.

This driver avoids unnecessary tri-color waveforms, resulting in smoother updates.

---

## Features

### Framebuffer-Based Rendering

The driver maintains a full framebuffer in RAM:

```
WIDTH  = 122 pixels
HEIGHT = 250 pixels
Buffer size ≈ 4000 bytes
```

Enables:

* Off-screen drawing
* Custom graphics rendering
* Pixel manipulation
* UI composition
* Controlled refresh timing

---

### Pixel-Level Control

```cpp
static void setPixel(uint8_t* buf, int x, int y, bool black);
```

Supports:

* Custom fonts
* Graph plotting
* Shape rendering
* UI widgets
* Drawing primitives

---

### Full Refresh

* Clears ghosting
* Applies complete waveform
* Produces visible flash (normal behavior)
* Recommended after multiple partial updates

---

### Partial Refresh

* Updates selected region only
* Minimal flicker
* Faster execution
* Ideal for dynamic sensor data

---

## Library Structure (Arduino 1.5+ Standard)

```
ESP32_epaper2inch3_BW/
│
├── library.properties
├── LICENSE
├── README.md
│
├── src/
│   ├── EPD_2in13_V3_BW.h
│   └── EPD_2in13_V3_BW.cpp
│
└── examples/
    └── example_epd_2in13_v3_bw/
        └── example_epd_2in13_v3_bw.ino
```

Fully compatible with:

* Arduino IDE
* arduino-cli
* ESP32 Arduino Core

---

## Hardware Connections (ESP32 Example)

| E-Paper | ESP32  |
| ------- | ------ |
| VCC     | 3.3V   |
| GND     | GND    |
| DIN     | MOSI   |
| CLK     | SCK    |
| CS      | GPIO5  |
| DC      | GPIO17 |
| RST     | GPIO16 |
| BUSY    | GPIO4  |

---

## Memory Usage

* Framebuffer: ~4 KB
* No dynamic heap allocation
* Minimal stack usage
* FreeRTOS compatible

Optimized for ESP32 embedded environments.

---

## Power Characteristics

* Zero power consumption when image is static
* Power used only during refresh
* Suitable for battery-powered IoT applications

---

## Important Note

Since this driver operates the tri-color panel in Black & White mode:

* The third color (Red/Yellow) is not used
* Monochrome rendering only
* Tradeoff: improved smoothness and reduced flicker

If tri-color rendering is required, a full tri-color waveform driver should be used instead.

---

## Example Applications

* Environmental monitoring systems
* IAQ dashboards
* Low-power smart home displays
* Robotics control panels
* Academic embedded systems research

---

# License

MIT License

```
MIT License

Copyright (c) 2026

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...

```

---

# library.properties

Create this file in the root:

```
name=ESP32_epaper2inch3_BW
version=1.0.0
author=Your Name
maintainer=Your Name
sentence=Optimized Black & White driver for Waveshare 2.13" V3 tri-color E-Paper on ESP32.
paragraph=Reduces flicker by operating tri-color panel in monochrome mode with optimized waveform control.
category=Display
architectures=esp32
includes=EPD_2in13_V3_BW.h
```

