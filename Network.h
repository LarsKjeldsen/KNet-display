// Network.h

#ifndef _NETWORK_h
#define _NETWORK_h

#include "Arduino.h"
#include <BLEDevice.h>
#include <ssl_client.h>
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
#include <time.h>
#include <PubSubClient.h>
#include "Display.h"
#include <string.h>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <cJSON.h>

void WiFi_Setup();
void SendBattery();
#endif

