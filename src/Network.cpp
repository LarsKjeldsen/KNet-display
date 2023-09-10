#include "Network.h"

char ssid[] = "Kjeldsen";
char password[] = "donnafrida";

IPAddress ip(192, 168, 1, 216);
IPAddress gw(192, 168, 1, 1);
IPAddress mask(255, 255, 255, 0);
IPAddress mqtt(192, 168, 1, 22);

WiFiClient ethClient;
HTTPClient httpClient;
PubSubClient client(ethClient);

extern DisplayClass Display;
extern float bat;

void WiFi_Setup()
{
	char s[10];
	int retry = 0;
	WiFi.mode(WIFI_STA);

	WiFi.config(ip, gw, mask);

	WiFi.setSleep(true); // <-- WITHOUT THIS ESP CONNECTS ONLY AFTER FIRST FEW RESTARTS OR DOESN'T CONNECT AT ALL

	WiFi.begin(ssid, password);
	WiFi.setHostname("KNet-Display-1");

	retry = 0;
	while (int ret = WiFi.waitForConnectResult() != WL_CONNECTED)
	{
		Serial.print("Connection Failed! Retrying... ErrorCode = ");
		Serial.println(ret);
		delay(200);
		if (retry++ >= 5)
			ESP.restart();
		WiFi.begin(ssid, password);
	}

	retry = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(50);
		if (retry++ >= 100)
			ESP.restart();
	}

	//	Serial.println("");
	//	Serial.print("WiFi connected IP address: ");
	//	Serial.println(WiFi.localIP());

	httpClient.begin("http://192.168.1.22:8123/api/states/sensor.besked");
	httpClient.addHeader("Authorization", "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJiNzAwMTZhMWE5ODg0Mjc5ODdiOTA4MjYzOGYzYzM0MyIsImlhdCI6MTY2NjM1MjgyMSwiZXhwIjoxOTgxNzEyODIxfQ.bFjE97PlJEqvcgNdxfnfbnqmfXk3UrGi6hRejPrNFo4");

	httpClient.addHeader("Content-Type", "application/json");
	int httpCode = httpClient.GET();

	if (httpCode == -11 || httpCode == -1) // Try again.
	{
		httpCode = httpClient.GET();
		Serial.print("*************** Retrying HTTP request httpcode = ");
		Serial.println(httpCode);
	}

	if ((httpCode == 200) | (httpCode == 201))
	{
		String payload = httpClient.getString();
		Serial.println(payload);
		cJSON *root = cJSON_Parse(payload.c_str());
		std::string besked_raw = (cJSON_GetObjectItem(root, "state")->valuestring);
		Serial.println(("besked = " + besked_raw).c_str());

		int pos = 0;
		std::string token;
		pos = besked_raw.find("|");
		if (pos == std::string::npos)
		{
			Display.Besked = std::string("Error in string (1).....\n ") + std::string(payload.c_str());
			return;
		}
		token = besked_raw.substr(0, pos);
		Display.Tid = token;
		Serial.println(("Tid = " + token).c_str());
		besked_raw.erase(0, pos + 1);

		pos = besked_raw.find("|");
		if (pos == std::string::npos)
		{
			Display.Besked = std::string("Error in string (2).....\n ") + std::string(payload.c_str());
			return;
		}
		token = besked_raw.substr(0, pos);
		Display.Ude_Temp = token + (char)186;
		Serial.println(("Ude_temp = " + token).c_str());
		besked_raw.erase(0, pos + 1);

		for (int i = 0; i < besked_raw.length(); i++)
			if (besked_raw[i] == '|')
				besked_raw[i] = '\n';

		Display.Besked = besked_raw;
	}
	else
	{
		Serial.print("Error code is : ");
		Serial.println(httpCode);
		char S1[10]; char S2[10];
		Display.Besked = std::string("Home assistant is down..... ") + itoa(httpCode, S1, 10) + std::string("\nWiFi status : ") + itoa((int)WiFi.status(), S2, 10);
		+" : " + WiFi.status();
	}

	httpClient.end();
}

void SendBattery()
{
	client.setServer(mqtt, 1883);
	client.connect("KNet-Display");
	char s[10];
	boolean ret = client.publish("KNet/Display/1/Battery", dtostrf(bat, 4, 2, s), false);
	client.disconnect();
	delay(200);
}

void SendError(String str)
{
	client.setServer(mqtt, 1883);
	client.connect("KNet-Display");
	boolean ret = client.publish("KNet/Debug/Display1", str.c_str(), false);
	client.disconnect();
	delay(200);
}
