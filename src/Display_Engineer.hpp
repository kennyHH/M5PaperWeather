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
  * @file Display.h
  *
  * Main class for drawing the content to the e-paper display.
  */
#pragma once
#include "Data.hpp"
#include "Icons.hpp"
#include "M5Compatibility.hpp"


M5EPD_Canvas canvas(&M5_EPD); // Main canvas of the e-paper

/* Main class for drawing the content to the e-paper display. */
class WeatherDisplay
{
protected:
    MyData &myData; //!< Reference to the global data
    int     maxX;   //!< Max width of the e-paper
    int     maxY;   //!< Max height of the e-paper
    String  text_font            = "/FutureXtraBoldItaCdn.ttf";
    int     text_size_small      = 14;
    int     text_size_medium     = 20;
    int     text_size_large      = 28;
    bool    interface_guide      = false;
    bool    interface_stroke     = false;
    bool    interface_header     = false;
    bool    interface_status_bar = false;

protected:
    void DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom = 0, int32_t degTo = 360);

    void DrawIcon(int x, int y, const uint16_t *icon, int dx = 64, int dy = 64, bool highContrast = false);

    void DrawHead();
    void DrawRSSI(int x, int y);
    void DrawBattery();

    void DrawGuide(int divider, M5EPD_Canvas::grayscale_t color);

    void DrawWeatherInfo(int x, int y, int dx, int dy);
    void DrawSunInfo(int x, int y, int dx, int dy);
    void DrawInfo(int x, int y, int dx, int dy);
    void DrawM5PaperInfo(int x, int y, int dx, int dy);

    void DrawDaily(int x, int y, int dx, int dy, Weather &weather, int index);

    void DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], float values2[], float nightZone1 = 0, float nightZone2 = 0);
    void DrawDualGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], int offsetB, int yMinB, int yMaxB, float valuesB[], float nightZone1 = 0, float nightZone2 = 0);

    int map(int x, int in_min, int in_max, int out_min, int out_max);

public:
   WeatherDisplay(MyData &md, int x = 960, int y = 540)
      : myData(md)
      , maxX(x)
      , maxY(y)
   {
   }

   void Show();

   void ShowM5PaperInfo();
};

/* Draw a circle with optional start and end point */
void WeatherDisplay::DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom /* = 0 */, int32_t degTo /* = 360 */)
{
   for (int i = degFrom; i < degTo; i++) {
      double radians = i * PI / 180;
      double px      = x + r * cos(radians);
      double py      = y + r * sin(radians);

      canvas.drawPixel(px, py, color);
   }
}

/* Draw a the rssi value as circle parts */
void WeatherDisplay::DrawRSSI(int x, int y)
{
   int iQuality = WifiGetRssiAsQualityInt(myData.wifiRSSI);

   if (iQuality >= 80) DrawCircle(x + 12, y, 16, M5EPD_Canvas::G15, 225, 315);
   if (iQuality >= 40) DrawCircle(x + 12, y, 12, M5EPD_Canvas::G15, 225, 315);
   if (iQuality >= 20) DrawCircle(x + 12, y,  8, M5EPD_Canvas::G15, 225, 315);
   if (iQuality >= 10) DrawCircle(x + 12, y,  4, M5EPD_Canvas::G15, 225, 315);
   DrawCircle(x + 12, y, 2, M5EPD_Canvas::G15, 225, 315);
}

/* Draw a the battery icon */
void WeatherDisplay::DrawBattery()
{


    canvas.fillRect(0, 0, map(myData.batteryCapacity, 0, 100, 0, 960), 7, M5EPD_Canvas::G15);
    //canvas.fillRect(960, 535, map(myData.batteryCapacity, 0, 100, 0, -960), 5, M5EPD_Canvas::G15);

}

/* Draw a the head with version, city, rssi and battery */
void WeatherDisplay::DrawHead()
{
   canvas.drawString(VERSION, 20, 10);
   canvas.drawCentreString(CITY_NAME, maxX / 2, 10, 1);
   canvas.drawString(WifiGetRssiAsQuality(myData.wifiRSSI) + "%", maxX - 200, 10);
   DrawRSSI(maxX - 155, 25);
   canvas.drawString(String(myData.batteryCapacity) + "%", maxX - 110, 10);
   //DrawBattery(maxX - 65, 10);
}

/* Draw one icon from the binary data */
void WeatherDisplay::DrawIcon(int x, int y, const uint16_t *icon, int dx /*= 64*/, int dy /*= 64*/, bool highContrast /*= false*/)
{
   for (int yi = 0; yi < dy; yi++) {
      for (int xi = 0; xi < dx; xi++) {
         uint16_t pixel = icon[yi * dx + xi];

         if (highContrast) {
            if (15 - (pixel / 4096) > 0) canvas.drawPixel(x + xi, y + yi, M5EPD_Canvas::G15);
         } else {
            canvas.drawPixel(x + xi, y + yi, 15 - (pixel / 4096));
         }
      }
   }
}

/* Draw the sun information with sunrise and sunset */
void WeatherDisplay::DrawSunInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(text_size_large);
   DrawIcon(x + 25, y + 85, (uint16_t *) SUNRISE64x64);
   canvas.drawString(getHourMinString(myData.weather.sunrise), x + 105, y + 110, 1);

   DrawIcon(x + 25, y + 150, (uint16_t *) SUNSET64x64);
   canvas.drawString(getHourMinString(myData.weather.sunset), x + 105, y + 180, 1);
}

/* Draw current weather information */
void WeatherDisplay::DrawWeatherInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(text_size_large);

   String icon = myData.weather.hourlyIcon[0];
   int iconX = x + dx / 2 - 66;
   int iconY = y + 80;

        if (icon == "01d") DrawIcon(iconX, iconY, (uint16_t *) image_data_01d, 64, 64, true);
   else if (icon == "01n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "02d") DrawIcon(iconX, iconY, (uint16_t *) image_data_02d, 64, 64, true);
   else if (icon == "02n") DrawIcon(iconX, iconY, (uint16_t *) image_data_02n, 64, 64, true);
   else if (icon == "03d") DrawIcon(iconX, iconY, (uint16_t *) image_data_03d, 64, 64, true);
   else if (icon == "03n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "04d") DrawIcon(iconX, iconY, (uint16_t *) image_data_04d, 64, 64, true);
   else if (icon == "04n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "09d") DrawIcon(iconX, iconY, (uint16_t *) image_data_09d, 64, 64, true);
   else if (icon == "09n") DrawIcon(iconX, iconY, (uint16_t *) image_data_09n, 64, 64, true);
   else if (icon == "10d") DrawIcon(iconX, iconY, (uint16_t *) image_data_10d, 64, 64, true);
   else if (icon == "10n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "11d") DrawIcon(iconX, iconY, (uint16_t *) image_data_11d, 64, 64, true);
   else if (icon == "11n") DrawIcon(iconX, iconY, (uint16_t *) image_data_11n, 64, 64, true);
   else if (icon == "13d") DrawIcon(iconX, iconY, (uint16_t *) image_data_13d, 64, 64, true);
   else if (icon == "13n") DrawIcon(iconX, iconY, (uint16_t *) image_data_13n, 64, 64, true);
   else if (icon == "50d") DrawIcon(iconX, iconY, (uint16_t *) image_data_50d, 64, 64, true);
   else if (icon == "50n") DrawIcon(iconX, iconY, (uint16_t *) image_data_50n, 64, 64, true);
   else DrawIcon(iconX, iconY, (uint16_t *) image_data_unknown, 64, 64, true);

   canvas.drawString(myData.weather.hourlyMain[0], x + dx / 2 + 2, y + 110, 1);

   //getFloatString(myData.weather.hourlyMaxTemp[0], " °C") + " " +

   canvas.drawCentreString(getFloatString(myData.weather.feels_like,  "°C"), x + dx / 2, y + 145, 1);
   String humidity = String(myData.weather.humidity) + "%";
   if(myData.weather.hourlyRain[0] > 0) humidity += "/" + getFloatString(myData.weather.hourlyRain[0], "mm");
   canvas.drawCentreString(humidity, x + dx / 2, y + 180, 1);
}

/* Draw the wind information part */
void WeatherDisplay::DrawInfo(int x, int y, int dx, int dy)
{
    //110, 150, 180
   canvas.setTextSize(text_size_large);

   canvas.drawCentreString(String(myData.weather.uvi) + " UV", x + dx / 2, y + 110, 1);
   canvas.drawCentreString(String(myData.weather.pressure) + " KPA", x + dx / 2, y + 145, 1);
   canvas.drawCentreString(getFloatString(myData.weather.dew_point, "°Dew"), x + dx / 2, y + 180, 1);
}

/* Draw the M5Paper environment and RTC information */
void WeatherDisplay::DrawM5PaperInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(24);

   canvas.setTextColor(M5EPD_Canvas::G3, BLACK);
   canvas.setTextSize(text_size_medium);
   canvas.drawCentreString("updated " + getRTCTimeString(), x + dx / 2, y + 115, 1);
   //canvas.drawCentreString("updated", x + dx / 2, y + 95, 1);

   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextSize(text_size_large);
   canvas.drawCentreString(getRTCDateString(), x + dx / 2, y + 145, 1);

   canvas.setTextColor(M5EPD_Canvas::G3, BLACK);
   canvas.drawCentreString(String(myData.sht30Temperatur) + "°C/" + String(myData.sht30Humidity) + "%", x + dx / 2, y + 180, 1);

   canvas.setTextColor(WHITE, BLACK);
}

/* Draw one daily weather information */
void WeatherDisplay::DrawDaily(int x, int y, int dx, int dy, Weather &weather, int index)
{
   time_t time = weather.forecastTime[index];
   int    tMin = weather.forecastMinTemp[index];
   int    tMax = weather.forecastMaxTemp[index];
   int    pop  = weather.forecastPop[index];
   int    rain = weather.forecastRain[index];
   String icon = weather.forecastIcon[index];

   canvas.setTextSize(text_size_medium);
   if (weekday(time) == 1 || weekday(time) == 7) {
      canvas.setTextColor(WHITE, M5EPD_Canvas::G1);
      canvas.fillRect(x + 1, y - 4, dx - 1, 26, M5EPD_Canvas::G1);
   }
   canvas.drawCentreString(index == 0 ? "Today" : getShortDayOfWeekString(time), x + dx / 2, y, 1);
   canvas.setTextColor(WHITE, BLACK);

   int iconX = x + dx / 2 - 32;
   int iconY = y + 20;

        if (icon == "01d") DrawIcon(iconX, iconY, (uint16_t *) image_data_01d, 64, 64, true);
   else if (icon == "01n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "02d") DrawIcon(iconX, iconY, (uint16_t *) image_data_02d, 64, 64, true);
   else if (icon == "02n") DrawIcon(iconX, iconY, (uint16_t *) image_data_02n, 64, 64, true);
   else if (icon == "03d") DrawIcon(iconX, iconY, (uint16_t *) image_data_03d, 64, 64, true);
   else if (icon == "03n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "04d") DrawIcon(iconX, iconY, (uint16_t *) image_data_04d, 64, 64, true);
   else if (icon == "04n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "09d") DrawIcon(iconX, iconY, (uint16_t *) image_data_09d, 64, 64, true);
   else if (icon == "09n") DrawIcon(iconX, iconY, (uint16_t *) image_data_09n, 64, 64, true);
   else if (icon == "10d") DrawIcon(iconX, iconY, (uint16_t *) image_data_10d, 64, 64, true);
   else if (icon == "10n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "11d") DrawIcon(iconX, iconY, (uint16_t *) image_data_11d, 64, 64, true);
   else if (icon == "11n") DrawIcon(iconX, iconY, (uint16_t *) image_data_11n, 64, 64, true);
   else if (icon == "13d") DrawIcon(iconX, iconY, (uint16_t *) image_data_13d, 64, 64, true);
   else if (icon == "13n") DrawIcon(iconX, iconY, (uint16_t *) image_data_13n, 64, 64, true);
   else if (icon == "50d") DrawIcon(iconX, iconY, (uint16_t *) image_data_50d, 64, 64, true);
   else if (icon == "50n") DrawIcon(iconX, iconY, (uint16_t *) image_data_50n, 64, 64, true);
   else DrawIcon(iconX, iconY, (uint16_t *) image_data_unknown, 64, 64, true);

   canvas.setTextSize(text_size_medium);
   canvas.drawCentreString(String(tMax)+"/"+String(tMin), x + dx / 2, y + 87, 1);
   if(pop > 0) {
    canvas.setTextColor(M5EPD_Canvas::G7, BLACK);
   	canvas.drawCentreString(String(pop)+"%", x + dx / 2, y + 110, 1);
    if (rain > 0) canvas.drawCentreString(String(rain)+"mm", x + dx / 2, y + 130, 1);
    canvas.setTextColor(WHITE, BLACK);
   }
}

/* Draw a graph with x- and y-axis and values */
void WeatherDisplay::DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], float values2[], float nightZone1, float nightZone2)
{
   String yMinString = String(yMin);
   String yMaxString = String(yMax);
   int    textWidth  = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
   int    graphX     = x + 5 + textWidth + 5;
   int    graphY     = y + 35;
   int    graphDX    = dx - textWidth - 20;
   int    graphDY    = dy - 35 - 20;
   float  xStep      = graphDX / xMax;
   float  yStep      = graphDY / (yMax - yMin);
   int    iOldX      = 0;
   int    iOldY      = 0;

   canvas.setTextSize(text_size_medium);
   canvas.drawCentreString(title, x + dx / 2, y + 10, 1);
   canvas.setTextSize(text_size_small);

   canvas.setTextColor(M5EPD_Canvas::G7, BLACK);
   canvas.drawString(yMaxString, x + 5, graphY - 5);
   //canvas.drawString(yMinString, x + 5, graphY + graphDY - 3);
   for (int i = 0; i <= xMax; i++) {
      canvas.drawCentreString(String(i), graphX + i * xStep, graphY + graphDY + 5, 1);
   }
   canvas.setTextColor(WHITE, BLACK);
   if (nightZone1 > 0) {
      canvas.fillRect(graphX, graphY, nightZone1 * (float)graphDX / 12.F, graphDY, M5EPD_Canvas::G1);
   }
   if (nightZone2 > 0) {
      canvas.fillRect(max(graphX + graphDX - nightZone2 * (float)graphDX / 12.F, (float)graphX), graphY, min(nightZone2 * (float)graphDX / 12.F, (float)graphDX), graphDY, M5EPD_Canvas::G1);
   }

   if(interface_stroke)  canvas.drawRect(graphX, graphY, graphDX, graphDY, M5EPD_Canvas::G15);

   if (yMin < 0 && yMax > 0) { // null line?
      float yValueDX = (float) graphDY / (yMax - yMin);
      int   yPos     = graphY + graphDY - (0.0 - yMin) * yValueDX;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.drawString("0", graphX - 20, yPos);
      for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10) {
         canvas.drawLine(xDash, yPos, xDash + 5, yPos, M5EPD_Canvas::G15);
      }
   }
   for (int i = xMin; i <= xMax; i++) {
      float yValue   = values[i];
      float yValueDY = (float) graphDY / (yMax - yMin);
      int   xPos     = graphX + graphDX / xMax * i;
      int   yPos     = graphY + graphDY - (yValue - yMin) * yValueDY;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.fillCircle(xPos, yPos, 2, M5EPD_Canvas::G15);
      if (i > xMin) {
         canvas.drawLine(iOldX, iOldY, xPos, yPos, M5EPD_Canvas::G15);
      }
      iOldX = xPos;
      iOldY = yPos;
   }
   if (values2 != NULL) {
      for (int i = xMin; i <= xMax; i++) {
         float yValue   = values2[i];
         float yValueDY = (float) graphDY / (yMax - yMin);
         int   xPos     = graphX + graphDX / xMax * i;
         int   yPos     = graphY + graphDY - (yValue - yMin) * yValueDY;

         if (yPos > graphY + graphDY) yPos = graphY + graphDY;
         if (yPos < graphY)           yPos = graphY;

         if (i > xMin) {
            canvas.drawLine(iOldX, iOldY, xPos, yPos, M5EPD_Canvas::G15);
            canvas.drawLine(iOldX, iOldY+1, xPos, yPos+1, M5EPD_Canvas::G0);
         }
         canvas.fillCircle(xPos, yPos, 3, M5EPD_Canvas::G15);
         canvas.fillCircle(xPos, yPos, 2, M5EPD_Canvas::G0);
         iOldX = xPos;
         iOldY = yPos;
      }
   }
}

/* Draw a dual graph */
void WeatherDisplay::DrawDualGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], int offset, int yMinB, int yMaxB, float valuesB[], float nightZone1, float nightZone2)
{
   String yMinString = String(yMinB);
   String yMaxString = String(yMaxB);
   int    textWidth  = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
   int    graphX     = x + 5 + textWidth + 5;
   int    graphY     = y + 35;
   int    graphDX    = dx - textWidth - 20;
   int    graphDY    = dy - 35 - 20;
   float  xStep      = graphDX / xMax;
   float  yStep      = graphDY / (yMax - yMin);
   int    iOldX      = 0;
   int    iOldY      = 0;

   canvas.setTextSize(text_size_medium);
   canvas.drawCentreString(title, x + dx / 2, y + 10, 1);
   canvas.setTextSize(text_size_small);

   canvas.setTextColor(M5EPD_Canvas::G7, BLACK);
   canvas.drawString(yMaxString, x + 5, graphY - 5);
   //canvas.drawString(yMinString, x + 5, graphY + graphDY - 3);


   for (int i = 0; i <= xMax; i++) {
      canvas.drawCentreString(String(i), graphX + i * xStep, graphY + graphDY + 5, 1);
   }
   canvas.setTextColor(WHITE, BLACK);

   if (nightZone1 > 0) {
      canvas.fillRect(graphX, graphY, nightZone1 * (float)graphDX / 12.F, graphDY, M5EPD_Canvas::G1);
   }
   if (nightZone2 > 0) {
      canvas.fillRect(max(graphX + graphDX - nightZone2 * (float)graphDX / 12.F, (float)graphX), graphY, min(nightZone2 * (float)graphDX / 12.F, (float)graphDX), graphDY, M5EPD_Canvas::G1);
   }
   if(interface_stroke)  canvas.drawRect(graphX, graphY, graphDX, graphDY, M5EPD_Canvas::G15);
   if (yMin < 0 && yMax > 0) { // null line?
      float yValueDX = (float) graphDY / (yMax - yMin);
      int   yPos     = graphY + graphDY - (0.0 - yMin) * yValueDX;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.drawString("0", graphX - 20, yPos);
      for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10) {
         canvas.drawLine(xDash, yPos, xDash + 5, yPos, M5EPD_Canvas::G15);
      }
   }
   for (int i = xMin; i <= xMax; i++) {
      float yValue   = valuesB[i - xMin];
      float yValueDY = (float) graphDY / (float)(yMaxB - yMinB);
      int   xPos     = graphX + graphDX / (xMax - xMin) * i;
      int   yPos     = graphY + graphDY - ((yValue - (float)yMinB) * yValueDY);

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      int width = graphDX / (xMax - xMin) - 4;
      int height = (graphY + graphDY) - yPos;
      if (i == xMin) {
         width /= 2;
      } else {
         xPos -= width / 2;
      }
      if (i == xMax) {
         width = width / 2 + 2;
      }
      if (height > 0) {
         canvas.fillRect(xPos, yPos, width, height, M5EPD_Canvas::G3);
      }
   }
   for (int i = xMin + offset; i <= xMax; i++) {
      float yValue   = values[i];
      float yValueDY = (float) graphDY / (yMax - yMin);
      int   xPos     = graphX + graphDX / xMax * i;
      int   yPos     = graphY + graphDY - (yValue - yMin) * yValueDY;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      if (i > xMin + offset) {
         canvas.drawLine(iOldX, iOldY,   xPos, yPos,   M5EPD_Canvas::G15);
         canvas.drawLine(iOldX, iOldY+1, xPos, yPos+1, M5EPD_Canvas::G0);
      }
      canvas.fillCircle(xPos, yPos, 3, M5EPD_Canvas::G0);
      canvas.fillCircle(xPos, yPos, 2, M5EPD_Canvas::G15);
      iOldX = xPos;
      iOldY = yPos;
   }
}

/* Draw a the head with version, city, rssi and battery */
void WeatherDisplay::DrawGuide(int divider, M5EPD_Canvas::grayscale_t color)
{
   for (int i = 1; i < divider; i++) {
       canvas.drawLine((960/divider)*i, 0, (960/divider)*i, 540, color);
       canvas.drawLine(0, (540/divider)*i, 960, (540/divider)*i, color);
   }
}

int WeatherDisplay::map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* Main function to show all the data to the e-paper */
void WeatherDisplay::Show()
{
   Serial.println("WeatherDisplay::Show");

   canvas.createCanvas(960, 540);

   int screen_width = 960;
   int screen_height = 540;

   if(interface_guide) {
       DrawGuide(8, M5EPD_Canvas::G1);
       DrawGuide(6, M5EPD_Canvas::G1);
       DrawGuide(4, M5EPD_Canvas::G2);
       DrawGuide(3, M5EPD_Canvas::G2);
   }

 	if (!SPIFFS.begin(true)) {
		log_e("SPIFFS Mount Failed");
		while(1);
   }

   canvas.loadFont(text_font, SPIFFS);
   canvas.createRender(text_size_small, 32);
   canvas.createRender(text_size_medium, 32);
   canvas.createRender(text_size_large, 32);


   canvas.setTextSize(text_size_medium);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);

   int first_row_y = -60; // used to be 35
   int secnd_row_y = 200; // used to be 286
   int third_row_y = 395; // used to be 408


    DrawBattery();
    // 15 - 264 - 484 - 710
    DrawInfo            (15, first_row_y, 232, 251);
    DrawWeatherInfo     (250, first_row_y, 232, 251);
    DrawSunInfo         (500, first_row_y, 232, 251);
    DrawM5PaperInfo     (710, first_row_y, 245, 251);


   for (int x = 15, i = 0; x <= 930; x += 116, i++) {
	  DrawDaily(x, secnd_row_y, 116, 122, myData.weather, i);
   }

   // JUST FOR THE FUCKING GRAPH!!!!!!!!!
   float hoursUntilSunlize = 0, hoursAfterSunset = 0;
   if (myData.weather.currentTime < myData.weather.sunrise) {
      hoursUntilSunlize = (myData.weather.sunrise - myData.weather.currentTime) / 3600.0;
   }
   if (myData.weather.currentTime + 12 * 60 * 60 > myData.weather.sunset) {
      hoursAfterSunset = (myData.weather.currentTime + 12 * 60 * 60 - myData.weather.sunset) / 3600.0;
   }
   DrawGraph( 15, third_row_y, 232, 122, "Temp 12h (°C)", 0, 12, myData.weather.hourlyTempRange[0], myData.weather.hourlyTempRange[1], myData.weather.hourlyMaxTemp, NULL, hoursUntilSunlize, hoursAfterSunset);
   DrawDualGraph(247, third_row_y, 232, 122, "Rain 12h (mm/%)", 0, 12,   0,  100, myData.weather.hourlyPop, 1, 0, myData.weather.hourlyMaxRain, myData.weather.hourlyRain, hoursUntilSunlize, hoursAfterSunset);
   if(interface_stroke) canvas.drawLine(480, 408, 480, 530, M5EPD_Canvas::G15);
   DrawGraph(481, third_row_y, 232, 122, "Temp 7days (°C)", 0,  7, myData.weather.forecastTempRange[0], myData.weather.forecastTempRange[1], myData.weather.forecastMinTemp, myData.weather.forecastMaxTemp);
   DrawDualGraph(713, third_row_y, 232, 122, "Rain 7days (mm/%)", 0,  7,   0,  100, myData.weather.forecastPop, 0, 0, myData.weather.forecastMaxRain, myData.weather.forecastRain);

   canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
   delay(1000);
}

/* Update only the M5Paper part of the global data */
void WeatherDisplay::ShowM5PaperInfo()
{
   Serial.println("WeatherDisplay::ShowM5PaperInfo");

   canvas.createCanvas(245, 251);
   canvas.setTextSize(text_size_medium);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);

   canvas.drawRect(0, 0, 245, 251, M5EPD_Canvas::G15);
   DrawM5PaperInfo(0, 0, 245, 251);

   canvas.pushCanvas(697, 35, UPDATE_MODE_GC16);
   delay(1000);
}
