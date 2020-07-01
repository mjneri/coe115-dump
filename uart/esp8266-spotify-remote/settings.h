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

// WiFi settings
const char* ssid = "ADD YOUR WIFI HERE";
const char* password = "ADD YOUR PASSWORD HERE";

// Spotify settings
String clientId = "b813a7acb5f545728bddbb9b7a27346e";
String clientSecret = "171f77f8648e44c89c9e9a347f912d21";
// These next two lines have to match. A client clientSpotifierHostName makes the
// node listen on http://esp8266.local. Spotify needs an exact match of the redirect url
String espotifierNodeName = "esp8266";
String redirectUri = "http%3A%2F%2F" + espotifierNodeName + ".local%2Fcallback";
//String redirectUri = "http%3A%2F%2Fhttpbin.org%2Fanything";
