/*
 Name:		Bryggers-Display.ino
 Created:	7/16/2019 6:56:00 PM
 Author:	lars S. Kjeldsen
*/

// the setup function runs once when you press reset or power the board

#include <BLEDevice.h>
#include <ssl_client.h>
#include <PubSubClient.h>
#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <GxEPD2_GFX.h>
#include <GxEPD2_EPD.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2.h>
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
#include "Network.h"
#include <Arduino.h>

#define SLEEPTIME 60000
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

	minut_10_tick = true;  // Force full update everytime

	Display.setup(minut_10_tick);  // Run full update every 10th min.

	Display.UpdateDisplayUdeTemperatur();

	Display.UpdateDisplayTid();

	Display.UpdateDisplayBeskeder();
	Display.UpdateDisplayBattery(bat);

	int runtime = start - millis();

	unsigned long t = runtime + SLEEPTIME;

		if (t < 100 || t > SLEEPTIME) {
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
