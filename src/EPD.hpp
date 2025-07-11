/*
   Copyright (C) 2021 SFini

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  * @file EPD.h
  *
  * Helper functions for initialisizing and shutdown of the M5Paper.
  */
#pragma once
#include "M5Compatibility.hpp"

/* Initialize the M5Paper */
void InitEPD(bool clearDisplay = true)
{
#ifdef USE_M5UNIFIED
   auto cfg = M5.config();
   M5.begin(cfg);
#else
   M5.begin(false, false, true, true, false);
   M5.RTC.begin();
#endif

#ifdef USE_M5UNIFIED
   M5.Display.setRotation(180);
   if (clearDisplay) {
      M5.Display.clear();
   }
#else
   M5_EPD.SetRotation(180);
   M5.TP.SetRotation(0);
   if (clearDisplay) {
      M5_EPD.Clear(true);
   }
#endif

//   disableCore0WDT();
}

/*
 *  Shutdown the M5Paper
 *  NOTE: the M5Paper could not shutdown while on usb connection.
 *        In this case use the esp_deep_sleep_start() function.
*/
void ShutdownEPD(int sec)
{
   Serial.println("Shutdown");
#ifdef USE_M5UNIFIED
   M5.Display.sleep();
   esp_sleep_enable_timer_wakeup((uint64_t)sec * 1000000LL);
   esp_deep_sleep_start();
#else
   M5.disableEPDPower();
   M5.disableEXTPower();
   esp_sleep_enable_timer_wakeup((uint64_t)sec * 1000000LL);
   esp_sleep_enable_ext0_wakeup((gpio_num_t)M5EPD_KEY_PUSH_PIN, 0);
   gpio_hold_en((gpio_num_t)M5EPD_MAIN_PWR_PIN);
   gpio_deep_sleep_hold_en();
   esp_deep_sleep_start();
#endif
}
