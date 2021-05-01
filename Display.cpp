#include "Display.h"

#include "BodoniMT8.h"
#include "BodoniMT12.h"
#include "BodoniMT24.h"
#include "BodoniMT30.h"


GxEPD2_BW<GxEPD2_270, GxEPD2_270::HEIGHT> display(GxEPD2_270(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25));
extern DisplayClass Display;

void DisplayClass::setup(bool FullUpdate)
{
	Serial.print("FullUpdate = "); Serial.println(FullUpdate);
	display.init(0, FullUpdate, true);

//	SPI.end(); // release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
	//SPI: void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
	//SPI.begin(13, 12, 14, 15); // map and init SPI pins SCK(13), MISO(12), MOSI(14), SS(15)
	SPI.begin();
	// *** end of special handling for Waveshare ESP32 Driver board *** //
	display.setRotation(1);
	display.setTextColor(GxEPD_BLACK);

	h = display.height();
	h2 = display.height() / 2;
	w = display.width();
	w2 = display.width() / 2;

	temp_x1 = 0;
	temp_y1 = 22; // Room for header tex
	temp_x2 = w2 - 15;
	temp_y2 = h2;

	tid_x1 = temp_x2 + LineWidth;
	tid_y1 = temp_y1;
	tid_x2 = w;
	tid_y2 = h2;

	text_x1 = 0;
	text_y1 = temp_y2 + LineWidth;
	text_x2 = w; 
	text_y2 = h;

	display.setPartialWindow(0, 0, w, h);

//	display.setFont(&FreeSerifBold9pt7b);
	display.setFont(&LSK8pt8b);

	static char* char_temp = "Ude temp";
	static char* char_ur = "Ur";

	int16_t tbx, tby; uint16_t x1, tbw, tbh, tbx_temp, tbx_ur;

	display.getTextBounds(char_temp, 0, 0, &tbx, &tby, &tbw, &tbh);
	x1 = (((temp_x2 - temp_x1) - tbw) / 2) - tbx;
	tbx_temp = x1;

	display.getTextBounds(char_ur, 0, 0, &tbx, &tby, &tbw, &tbh);
	x1 = (((tid_x2 - tid_x1) - tbw) / 2) - tbx;
	tbx_ur = x1 + tid_x1;

	display.firstPage();
	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.fillRect(temp_x2, temp_x1, LineWidth, temp_y2,   GxEPD_BLACK);
		display.fillRect(temp_x1, temp_y2, w,         LineWidth, GxEPD_BLACK);
		display.setCursor(tbx_temp, 11);
		display.print(char_temp);
		display.setCursor(tbx_ur, 11);
		display.print(char_ur);
	} while (display.nextPage());
}


void DisplayClass::UpdateDisplayUdeTemperatur()
{
	display.setPartialWindow(temp_x1, temp_y1, temp_x2 - temp_x1, temp_y2 - temp_y1);
	display.setTextColor(GxEPD_BLACK);
	display.setFont(&LSK30pt8b);

	display.firstPage();
	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(temp_x1 + 5, temp_y2 - 20);
		display.print(Ude_Temp.c_str());
	} while (display.nextPage());
}

void DisplayClass::UpdateDisplayTid()
{
	int16_t tbx, tby; 
	uint16_t x1, tbw, tbh, tbx_temp, tbx_ur;

//	display.setFont(&FreeSerifBold24pt7b);
	display.setFont(&LSK30pt8b);
	display.getTextBounds(Tid.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
	x1 = (((tid_x2 - tid_x1) - tbw) / 2) - tbx;
	tbx_ur = x1 + tid_x1;

	display.setPartialWindow(tid_x1, tid_y1, tid_x2 - tid_x1, tid_y2 - tid_y1);

	display.setTextColor(GxEPD_BLACK);

	display.firstPage();
	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(tbx_ur, tid_y2 - 20);
		display.print(Tid.c_str());
	} while (display.nextPage());
}



void DisplayClass::UpdateDisplayBeskeder()
{
	Besked = StringConv(Besked);

	display.setFont(&LSK12pt8b);
	display.setTextColor(GxEPD_BLACK);

	display.setPartialWindow(text_x1, text_y1+LineWidth, text_x2 - text_x1, text_y2 - text_y1);
	Serial.println(Besked.c_str());
	display.firstPage();
	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(0, text_y1 +  25);
		display.println(Besked.c_str());

	} while (display.nextPage());
}



void DisplayClass::UpdateDisplayBattery(double bat)
{
	display.setPartialWindow(w-23, h-10, 23, 10);

	display.setTextColor(GxEPD_BLACK);
	display.setFont(&LSK8pt8b);

	display.firstPage();
	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(w-23, h-2);
		display.print(bat, 2);
	} while (display.nextPage());
}



void DisplayClass::Sleep()
{
	display.hibernate();
	display.powerOff();
	SPI.end();
}


std::string DisplayClass::StringConv(std::string s)
{
	std::string r = "";

	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] == 134)
			s[i] = 198;
		else if (s[i] == 152)
			s[i] = 216;
		else if (s[i] == 133)
			s[i] = 197;
		else if (s[i] == 166)
			s[i] = 230;
		else if (s[i] == 184)
			s[i] = 248;
		else if (s[i] == 165)
			s[i] = 229;

		if (s[i] != 195)
			r += s[i];

	}
	return r;
	
	/*
	186;  // Grad
	197;  // Å
	198;  // Æ
	216;  // Ø
	229;  // å
	230;  // æ
	248;  // ø
*/
}