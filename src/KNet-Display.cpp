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
#include <driver/rtc_io.h>
#include <driver/adc.h>

unsigned long SleepTime = 60000;

#define LED GPIO_NUM_2
#define DISPLAY_POWERPIN GPIO_NUM_17
// #define BATTERY_PIN GPIO_NUM_2 
#define BATTERY_PIN A4


DisplayClass Display;
float bat;

void setup()
{
	unsigned long start = millis();

	//btStop();

	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);

	Serial.begin(115200);
	Serial.println("Booting");

	bat = analogRead(BATTERY_PIN) / 4096.0 * 7.445;
	
	WiFi_Setup();
	
	bool minut_10_tick = Display.Tid[Display.Tid.length() - 1] == '0';

	if (minut_10_tick)
		SendBattery();

	WiFi.disconnect(true);

	pinMode(DISPLAY_POWERPIN, OUTPUT);
	digitalWrite(DISPLAY_POWERPIN, HIGH);

	minut_10_tick = true;  // Force full update everytime
	
	Display.setup(minut_10_tick);  // Run full update every 10th min.
	Display.UpdateDisplayUdeTemperatur();
	Display.UpdateDisplayTid();
	Display.UpdateDisplayBeskeder();
	Display.UpdateDisplayBattery(bat);
	
	int runtime = start - millis();

	unsigned long t = runtime + SleepTime;

	if (t < 100 || t > SleepTime)
	{
		Serial.println("Something wrong with sleeptimer - restarting ESP");
		ESP.restart();
	}

	Serial.print("Sleeping : ");
	Serial.println(t);

	Display.Sleep();

	digitalWrite(DISPLAY_POWERPIN, LOW);
	gpio_reset_pin(DISPLAY_POWERPIN);
	gpio_hold_en(LED);
	gpio_reset_pin(DISPLAY_POWERPIN);
	rtc_gpio_isolate(GPIO_NUM_12);
 	gpio_deep_sleep_hold_en();

	esp_deep_sleep(t * 1000);

	Serial.println("Should never end up here ......");
}

void loop()
{
}
