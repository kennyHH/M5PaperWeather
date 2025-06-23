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
  * @file Time.h
  * 
  * Helper function to set the internal RTC date and time.
  */
#pragma once
#include "Data.hpp"
#include "M5Compatibility.hpp"

/* Set the internal RTC clock with the weather timestamp */
bool SetRTCDateTime(MyData &myData)
{
   time_t time = myData.weather.currentTime;

   if (time > 0) {
      rtc_time_t RTCtime;
      rtc_date_t RTCDate;
   
      Serial.println("Epochtime: " + String(time));
      
      RTCDate.year = year(time);
      RTCDate.RTC_FIELD_MON  = month(time);
      RTCDate.RTC_FIELD_DAY  = day(time);
      M5_RTC.setDate(&RTCDate);
   
      RTCtime.RTC_FIELD_HOUR = hour(time);
      RTCtime.RTC_FIELD_MIN  = minute(time);
      RTCtime.RTC_FIELD_SEC  = second(time);
      M5_RTC.setTime(&RTCtime);
      return true;
   } 
   return false;
}
