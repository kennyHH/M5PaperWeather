#pragma once
#if __has_include(<M5Unified.h>)
#include <M5Unified.h>
#include <M5GFX.h>
#define USE_M5UNIFIED
using M5Display = M5GFX;

class M5EPD_Canvas : public M5Canvas {
public:
    enum grayscale_t {
        G0 = 0, G1, G2, G3, G4, G5, G6, G7,
        G8, G9, G10, G11, G12, G13, G14, G15
    };
    M5EPD_Canvas(M5Display* dp = &M5.Display) : M5Canvas(dp) {}
};

#define M5_EPD M5.Display
#else
#include <M5EPD.h>
#define M5_EPD M5.EPD
#endif
