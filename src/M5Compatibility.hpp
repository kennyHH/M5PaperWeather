#pragma once

#if __has_include(<M5Unified.h>)

#include <M5Unified.h>
#include <M5GFX.h>

#define USE_M5UNIFIED
using M5Display = M5GFX;
using rtc_time_t = m5::rtc_time_t;
using rtc_date_t = m5::rtc_date_t;

inline uint32_t getBatteryVoltage() { return M5.Power.getBatteryVoltage(); }

class M5EPD_Canvas : public M5Canvas {
public:
    using grayscale_t = uint8_t;
    M5EPD_Canvas(M5Display* dp = &M5.Display) : M5Canvas(dp) {}
    void createCanvas(int w, int h) { createSprite(w, h); }
    void createRender(int, int) {}
    void pushCanvas(int x, int y, uint32_t mode = 0) { pushSprite(x, y); }
};

#define M5_EPD  M5.Display
#define M5_RTC  M5.Rtc
#define UPDATE_MODE_GC16 0
#define UPDATE_MODE_DU4  0

#define RTC_FIELD_MON  month
#define RTC_FIELD_DAY  date
#define RTC_FIELD_HOUR hours
#define RTC_FIELD_MIN  minutes
#define RTC_FIELD_SEC  seconds

#else

#include <M5EPD.h>

using rtc_time_t = ::rtc_time_t;
using rtc_date_t = ::rtc_date_t;

inline uint32_t getBatteryVoltage() { return M5.getBatteryVoltage(); }

#define M5_EPD  M5.EPD
#define M5_RTC  M5.RTC
#define RTC_FIELD_MON  mon
#define RTC_FIELD_DAY  day
#define RTC_FIELD_HOUR hour
#define RTC_FIELD_MIN  min
#define RTC_FIELD_SEC  sec

#endif

