/*
 Name:		Gang-Display.ino
 Created:	7/16/2019 6:56:00 PM
 Author:	lars S. Kjeldsen
*/

// the setup function runs once when you press reset or power the board

#include <arduino.h>
#include <PubSubClient.h>
#include <gfxfont.h>
#include <SPI.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "Display.h"
#include <WiFiUdp.h>
#include <WiFiType.h>
#include <WiFiServer.h>
#include <WiFiScan.h>
#include <WiFiMulti.h>
#include <WiFiGeneric.h>
#include <WiFiClient.h>
#include <WiFiSTA.h>
#include <WiFiAP.h>
#include <WiFi.h>
#include <ETH.h>
#include <Adafruit_GFX.h>
#include <GxEPD2_BW.h>
#include "Network.h"


unsigned long SleepTime = 60000;

#define LED 22
#define DISPLAY_POWERPIN 17 // 5

DisplayClass Display;
float bat;

void setup() {

	//	pinMode(LED, OUTPUT);
	//	digitalWrite(LED, HIGH);
	unsigned long start = millis();

	Serial.begin(115200);
	Serial.println("Booting");

	pinMode(DISPLAY_POWERPIN, OUTPUT);
	digitalWrite(DISPLAY_POWERPIN, HIGH);

	bat = analogRead(A7) / 4096.0 * 7.445;

	WiFi_Setup();
	bool minut_10_tick = Display.Tid[Display.Tid.length() - 1] == '0';

	if (minut_10_tick)
		SendBattery();

	WiFi.disconnect();

	// pinMode(DISPLAY_POWERPIN, OUTPUT);
	// digitalWrite(DISPLAY_POWERPIN, HIGH);

	int hour = 25;

	if (Display.Tid.length() == 5 && Display.Tid[0] != 'x')
	{
		hour = atoi(Display.Tid.substr(0, 2).c_str());

		Serial.print("Hour = ");  Serial.println(hour);

		if (hour > 6)
			SleepTime = 60000;  // 1 min
		else
		{
			SleepTime = 600000;   // 10 min
			Display.Tid[4] = 'x';
		}
	}

	minut_10_tick = true;  // Force full update everytime

	Display.setup(minut_10_tick);  // Run full update every 10th min.

	Display.UpdateDisplayUdeTemperatur();

	Display.UpdateDisplayTid();

	Display.UpdateDisplayBeskeder();
	Display.UpdateDisplayBattery(bat);

	int runtime = start - millis();

	unsigned long t = runtime + SleepTime;

	if (t < 100 || t > SleepTime) {
		Serial.println("Something wrong with sleeptimer - restarting ESP");
		ESP.restart();
	}

	Serial.print("Sleeping : "); Serial.println(t);

	Display.Sleep();
	digitalWrite(DISPLAY_POWERPIN, LOW);

	esp_deep_sleep(t * 1000);

	Serial.println("Should never end up here ......");
}


void loop()
{
}
