/**The MIT License (MIT)
 
 Copyright (c) 2018 by ThingPulse Ltd., https://thingpulse.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <WiFiClientSecure.h>
#include <FS.h>
#include <ESP8266mDNS.h>  
#include "SpotifyClient.h"
#include "settings.h"

 // Statements like:
 // #pragma message(Reminder "Fix this problem!")
 // Which will cause messages like:
 // C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
 // to show up during compiles. Note that you can NOT use the
 // words "error" or "warning" in your reminders, since it will
 // make the IDE think it should abort execution. You can double
 // click on these messages and jump to the line in question.
 //
 // see https://stackoverflow.com/questions/5966594/how-can-i-use-pragma-message-so-that-the-message-points-to-the-filelineno
 //
#define Stringize( L )     #L 
#define MakeString( M, L ) M(L)
#define $Line MakeString( Stringize, __LINE__ )
#define Reminder __FILE__ "(" $Line ") : Reminder: "
#define DEBUG 0

const char* host = "api.spotify.com";
const int httpsPort = 443;

SpotifyClient client(clientId, clientSecret, redirectUri);
SpotifyData data;
SpotifyAuth auth;

String currentImageUrl = "";
uint32_t lastDrawingUpdate = 0;
uint16_t counter = 0;
long lastUpdate = 0;

String formatTime(uint32_t time);
void saveRefreshToken(String refreshToken);
String loadRefreshToken();

void drawSongInfo();
DrawingCallback drawSongInfoCallback = &drawSongInfo;

SoftwareSerial espSerial;

char buf[128];
char prevbuf[128];

void setup(){
	Serial.begin(115200);
 
  // 8 bits, no parity, 1 stopbit
  // RX = D7, TX = D8
  espSerial.begin(9600, SWSERIAL_8N1, 13, 15, false, 256);
 
	int i = 0;

	pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);    // LOW to Turn on;
	delay(2000);
	boolean mounted = SPIFFS.begin();
 
	if (!mounted) {
//		Serial.println("FS not formatted. Doing that now\r");
		SPIFFS.format();
//		Serial.println("FS formatted...\r");
		mounted = SPIFFS.begin();
	}
  
//	Serial.println();
//	Serial.print("connecting to ");
//	Serial.print(ssid);
//	Serial.println("\r");
//	Serial.print("CONN:Connecting to \nWiFi...\r\n");
//	PIC_send("INIT:Connecting to \nWiFi...\r\n");

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
//		Serial.print(".");
	}
//	Serial.println("SUCCESS:Connected.\r\n");
	//PIC_send("INIT:Connected.\r\n");
//	Serial.println("WiFi connected");
//	Serial.println("IP address: ");
//	Serial.println(WiFi.localIP());


	if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
//		Serial.println("Error setting up MDNS responder!");
	}
//	Serial.println("mDNS responder started");


	String code = "";
	String grantType = "";
	String refreshToken = loadRefreshToken();
	if (refreshToken == "") {
//		Serial.println("No refresh token found. Requesting through browser");
//		Serial.print("TOKEN:Open browser at http://esp8266.local\r\n");
		//PIC_send("INIT:Open browser at http://esp8266.local\r\n");
//		Serial.println ( "Open browser at http://" + espotifierNodeName + ".local" );
//		Serial.println ( "Open browser at " + redirectUri );

		code = client.startConfigPortal();
		grantType = "authorization_code";
	} 
	else {
		//PIC_send("INIT:Using refresh\ntoken in FS.");
		code = refreshToken;
		grantType = "refresh_token";
	}
	client.getToken(&auth, grantType, code);
	//Serial.printf("Refresh token: %s\nAccess Token: %s\n", auth.refreshToken.c_str(), auth.accessToken.c_str());
	if (auth.refreshToken != "") {
		saveRefreshToken(auth.refreshToken);
	}
	client.setDrawingCallback(&drawSongInfoCallback);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);    // LOW to Turn on;
  Serial.print("Connected to\nSpotify.\r\n");
	PIC_send("Connected to\nSpotify.\r\n");
	delay(5000);
}

void loop() {
	int i = 0;

	if (millis() - lastUpdate > 2000) {
		uint16_t responseCode = client.update(&data, &auth);
    memset(prevbuf, 0, sizeof(prevbuf));
    sprintf(prevbuf, "%s", buf);
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s\n%s\r\n", data.artistName.c_str(), data.title.c_str());
    
		if(strcmp(buf, prevbuf)){
      Serial.print(buf);
      PIC_send(buf); 
		}
   
//		Serial.printf("\n\n%s - %s\n", data.artistName.c_str(), data.title.c_str());
		lastUpdate = millis();
//		Serial.printf("--------Response Code: %d\n", responseCode);
//		Serial.printf("--------Free mem: %d\n", ESP.getFreeHeap());

		if (responseCode == 401) {
			client.getToken(&auth, "refresh_token", auth.refreshToken);
			if (auth.refreshToken != "") {
				saveRefreshToken(auth.refreshToken);
			}
		}
		if (responseCode == 200) {
//			Serial.printf("ERR:responsecode 200\n");
		}
		if (responseCode == 400) {
			//PIC_send("ERRR:Please define clientId and clientSecret");
		}
	}

	// The part where commands are sent
	unsigned long start = millis();
	String command = "";
	String method = "";
	char volume[64];
  char rxcommand[64];
	i = 0;
	memset(rxcommand, 0, sizeof(rxcommand));
	if(espSerial.available()){
		while( (millis() - start) < 100 ){
			if(espSerial.available()){
				rxcommand[i] = espSerial.read();
        Serial.print(rxcommand[i]);
				i++;
				start = millis();
				if(rxcommand[i] != '\n')
					continue;
			}

			if(strstr(rxcommand, "NEXT")){
				method = "POST";
				command = "next";
				break;
			}
			else if(strstr(rxcommand, "PREV")){
				method = "POST";
				command = "previous";
				break;
			}
			else if(strstr(rxcommand, "PLAYPAUSE")){
				method = "PUT";
				command = "play";
				if(data.isPlaying)
					command = "pause";
				data.isPlaying = !data.isPlaying;
				break;
			}
//			else if(strstr(rxcommand, "VOL\r\n")){
//				method  = "PUT";
//				command = "volume";
//				memset(volume, 0, sizeof(volume));
//				for(i = 0; i < 64; i++){
//					if(rxcommand[i] == 'V')
//						break;
//					else
//						volume[i] = rxcommand[i];
//				}
//				uint16_t responseCode = client.volumeset(&auth, method, volume);
//				Serial.print("playerCommand response =");
//				Serial.println(responseCode);
//				break;
//			}
		}
		if(command != "volume"){
			uint16_t responseCode = client.playerCommand(&auth, method, command);
//			Serial.print("playerCommand response =");
//			Serial.println(responseCode);
		}
	}
}

void drawSongInfo() {
	if (millis() - lastDrawingUpdate < 333) {
		return;   
	}
	lastDrawingUpdate = millis();
	long timeSinceUpdate = 0;
	if (data.isPlaying) {
		timeSinceUpdate = millis() - lastUpdate;
	}
	//drawProgress(min(data.progressMs + timeSinceUpdate, data.durationMs), data.durationMs, data.title, data.artistName, data.isPlaying, data.isPlayerActive);
}

String formatTime(uint32_t time) {
	char time_str[10];
	uint8_t minutes = time / (1000 * 60);
	uint8_t seconds = (time / 1000) % 60;
	sprintf(time_str, "%2d:%02d", minutes, seconds);
	return String(time_str);
}

void saveRefreshToken(String refreshToken){
	File f = SPIFFS.open("/refreshToken.txt", "w+");
	if (!f) {
		//Serial.println("ERR:Failed to open config file");
		return;
	}
	f.println(refreshToken);
	f.close();
}

String loadRefreshToken() {
	//Serial.println("Loading config");
	File f = SPIFFS.open("/refreshToken.txt", "r");
	if (!f) {
		//Serial.println("Failed to open config file");
		return "";
	}
	while(f.available()) {
		//Lets read line by line from the file
		String token = f.readStringUntil('\r');
		//Serial.printf("Refresh Token: %s\n", token.c_str());
		f.close();
		return token;
	}
	return "";
}

void PIC_send(char *data){
	int i = 0;
	for(i = 0; i < strlen(data); i++){
		espSerial.write(data[i]); 
//		if((i%4) == 0){
//			delay(1);
//		}
	}
}
