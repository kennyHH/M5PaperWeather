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


M5EPD_Canvas canvas(&M5.EPD); // Main canvas of the e-paper

/* Main class for drawing the content to the e-paper display. */
class WeatherDisplay
{
protected:
    MyData &myData;                //!< Reference to the global data
    int     screen_width;         //!< Max width of the e-paper
    int     screen_height;       //!< Max height of the e-paper
    String  text_font            = "/FuturaBT_CondExtraBlackIt.ttf";
    int     text_size            = 950;
    bool    interface_guide      = false;

protected:
    void DrawWeatherInfo(int x, int y, int dx, int dy);
    void DrawBattery();
    void DrawGuide(int divider, M5EPD_Canvas::grayscale_t color);

public:
   WeatherDisplay(MyData &md, int x = 960, int y = 540)
      : myData(md)
      , screen_width(x)
      , screen_height(y)
   {
   }
   void Show();
   void ShowM5PaperInfo();
};


/* Draw current weather information */
void WeatherDisplay::DrawWeatherInfo(int x, int y, int dx, int dy)
{
    int temperature = static_cast<int>(std::round(myData.weather.feels_like));
    String tempStr = String(temperature);
    String digit1 = tempStr.length() > 0 ? String(tempStr[0]) : "";
    String digit2 = tempStr.length() > 1 ? String(tempStr[1]) : "";

    // Calculate positions for centering
    int totalWidth = canvas.textWidth(digit1) + canvas.textWidth(digit2);
    int spacing = 350; // Adjust this value to change spacing between digits
    int startX = x + (dx - totalWidth - spacing) / 2;
    int centerY = y + dy / 2;

    canvas.setTextDatum(TR_DATUM);
    canvas.drawString("°", 1050, -200);
    canvas.setTextDatum(MC_DATUM);
    // Draw first digit
    canvas.drawString(digit1, startX, centerY);

    // Draw second digit (if exists)
    if (digit2.length() > 0) {
        startX += canvas.textWidth(digit1) + spacing;
        canvas.drawString(digit2, startX, centerY);
    }
}


/* Draw a the battery icon */
void WeatherDisplay::DrawBattery()
{
    canvas.fillRect(0, 0, map(myData.batteryCapacity, 0, 100, 0, 960), 7, M5EPD_Canvas::G15);
}


/* Draw a the head with version, city, rssi and battery */
void WeatherDisplay::DrawGuide(int divider, M5EPD_Canvas::grayscale_t color)
{
   for (int i = 1; i < divider; i++) {
       canvas.drawLine((960/divider)*i, 0, (960/divider)*i, 540, color);
       canvas.drawLine(0, (540/divider)*i, 960, (540/divider)*i, color);
   }
}

/* Main function to show all the data to the e-paper */
void WeatherDisplay::Show()
{
    if (!SPIFFS.begin(true)) {
    	log_e("SPIFFS Mount Failed");
    	while(1);
    }

    canvas.createCanvas(screen_width, screen_height);
    //canvas.setRotation(90);
    canvas.loadFont(text_font, SPIFFS);
    canvas.createRender(text_size);
    canvas.setTextSize(text_size);
    canvas.setTextColor(WHITE);
    canvas.setTextDatum(MC_DATUM);
        DrawBattery();
        DrawWeatherInfo(-75, 0, screen_width, screen_height);
    M5.EPD.SetColorReverse(true);
    canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
    delay(500);
}

/* Update only the M5Paper part of the global data */
void WeatherDisplay::ShowM5PaperInfo()
{
}
