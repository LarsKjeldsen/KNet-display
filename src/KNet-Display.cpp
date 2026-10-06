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
#define DISPLAY_POWERPIN GPIO_NUM_14
// #define BATTERY_PIN GPIO_NUM_2 
#define BATTERY_PIN A4


DisplayClass Display;
float bat;

void setup()
{
	const unsigned long start = millis();
	setCpuFrequencyMhz(80);

	gpio_hold_dis(LED);
	gpio_hold_dis(DISPLAY_POWERPIN); // release the hold set before the previous deep sleep

	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);

	Serial.begin(115200);

	bat = analogRead(BATTERY_PIN) / 4096.0 * 7.445;
	Serial.print("Battery voltage = "); Serial.println(bat, 2);

	bool wifiConnected = WiFi_Setup();
	if (wifiConnected)
	{
		SendBattery();
	}

	WiFi.disconnect(true);
	WiFi.mode(WIFI_OFF);

	pinMode(DISPLAY_POWERPIN, OUTPUT);
	digitalWrite(DISPLAY_POWERPIN, HIGH);

	Display.setup(true);  // PWR is powered off during sleep, wiping the controller RAM; full refresh every wake
	Display.UpdateDisplayUdeTemperatur();
	Display.UpdateDisplayTid();
	Display.UpdateDisplayBeskeder();
	Display.UpdateDisplayBattery(bat);

	const unsigned long elapsed = millis() - start;
	unsigned long sleepForMs = SleepTime;
	if (elapsed < SleepTime)
	{
		sleepForMs = SleepTime - elapsed;
	}
	else
	{
		sleepForMs = 1000; // keep the device from restarting endlessly if setup ran too long
	}

	if (sleepForMs < 1000 || sleepForMs > SleepTime)
	{
		Serial.println("Slept too long or too short, using safe fallback");
		sleepForMs = 1000;
	}

	Serial.print("Sleeping : "); Serial.println(sleepForMs);

	Display.Sleep();

	digitalWrite(DISPLAY_POWERPIN, LOW);
	gpio_hold_en(LED);
	gpio_hold_en(DISPLAY_POWERPIN);
	rtc_gpio_isolate(GPIO_NUM_12);
 	gpio_deep_sleep_hold_en();

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,   ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,         ESP_PD_OPTION_OFF);

	esp_sleep_enable_timer_wakeup(sleepForMs * 1000ULL);
    esp_deep_sleep_start();

	Serial.println("Should never end up here ......");
}

void loop()
{
}
