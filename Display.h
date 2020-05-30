// Display.h

#ifndef _DISPLAY_h
#define _DISPLAY_h

#include "arduino.h"
#include <spi.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <GxEPD2_GFX.h>
#include <GxEPD2_EPD.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2.h>
#include "Network.h"



class DisplayClass
{
 protected:
	 uint16_t h;
	 uint16_t h2;
	 uint16_t w;
	 uint16_t w2;

	 uint16_t temp_x1, temp_y1, temp_x2, temp_y2;
	 uint16_t tid_x1, tid_y1, tid_x2, tid_y2;
	 uint16_t text_x1, text_y1, text_x2, text_y2;

	 #define LineWidth 5

public:
	std::string Ude_Temp;
	std::string Tid;
	std::string Besked;

	void setup(bool FullUpdate = false);
	void Sleep();
	void UpdateDisplayUdeTemperatur();
	void UpdateDisplayTid();
	void UpdateDisplayBeskeder();
	void UpdateDisplayBattery(double);
private:
	std::string StringConv(std::string s);
};

extern DisplayClass Display;

#endif

