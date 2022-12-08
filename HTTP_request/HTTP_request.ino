v #include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

/* Set these to your desired credentials. */
const char *ssid = "Virus Detected";  //ENTER YOUR WIFI SETTINGS
const char *password = "!Mariedenise05+";

//Link to read data from https://jsonplaceholder.typicode.com/comments?postId=7
//Web/Server address to read/write from 
const char *host = "pillcasetimer-firestoreapi-production.up.railway.app";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "1E CB E3 98 65 D9 7C C3 CC 20 2D 49 15 06 76 B9 9F 66 89 48";
//=======================================================================
//                    Power on setup
//=======================================================================


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(28800);
  Serial.println("Local Time Set");
  Serial.println("----------------------------------------------------");
}

void requestDataAndParse(int hour, int minute){\
  Serial.println("<-------------------- DATA COLLECTION --------------------->");
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(host);

//  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  Serial.println("Using fingerprint...");
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.println("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 10)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==10) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
  }
  
  String Link;
  //GET Data
  Link = "/admin_thesis/schedules";

  Serial.print("requesting URL: ");
  Serial.println(host+Link);

  httpsClient.print(String("GET ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +               
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
                  
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  Serial.println("reply was:");
  Serial.println("==========");
  String line;
  while(httpsClient.available()){        
    line = httpsClient.readStringUntil('\n');  //Read Line by Line
    Serial.println(line); //Print response
  }
  Serial.println("==========");
  Serial.println("closing connection");
  Serial.println("----------------------------------------------------");
  Serial.println("----------------------------------------------------");
  
  // parse incoming data

  String cHour, cMinute;
  if(hour < 10){
    cHour = "0" + String( hour );
  }else{
    String cHour = String( hour );
  }
  if(minute < 10){
    cHour = "0" + String( minute );
  }else{
    cMinute = String( minute );
  }

  #if 0
    String responseBodyString( line );

    int lengthString = responseBodyString.length();
    Serial.print( F("JSON String response length: ") );  // always one char shorter than the char[]
    Serial.println(lengthString);
  #endif

  int length = line.length(); // sizeof(responseBody);
  Serial.print( F("JSON response length: ") );  // always one char longer than the string?
  Serial.println(length);

  // Change this to a 0 to focus on the response memory usage, not JSON.
  #if 1
    StaticJsonDocument<500> jsonBuffer;
    //JsonObject& json = jsonBuffer.parseObject(responseBody);
    auto error = deserializeJson(jsonBuffer, line);

    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }
    Serial.println( F("SUCCESS! ") );

    // extract the values
    JsonArray array = jsonBuffer.as<JsonArray>();
    for(JsonVariant v : array) {
        String alarmTime = v["alarmTime"];
        const char* _alarmTime = v["alarmTime"];
        int container = v["container"];
      
        // Print values.
        Serial.println(alarmTime);
        Serial.println(container);

        if(alarmTime.substring(0, 2) == cHour)
        {
           if(alarmTime.substring(3, 5) == cMinute){
              Serial.print("KKKdrop");
              Serial.println(container);
           }
        }
    }
    Serial.println(cHour);
    Serial.println(cMinute);
  #endif

  Serial.println("----------------------------------------------------");
  Serial.println("----------------------------------------------------");
}

void updateTime() {
  
  timeClient.update();
 
  time_t epochTime = timeClient.getEpochTime();
 
  String formattedTime = timeClient.getFormattedTime();
  Serial.println("");
  Serial.println("<------ TIME START ------->");
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);  
 
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
 
//  Serial.print("Time: ");
//  Serial.print(currentHour);
//  Serial.print(currentMinute);
//  Serial.print(currentSecond);

  Serial.println("");
  // collect data 
  if( currentSecond > 42){
    requestDataAndParse(currentHour - 12, currentMinute + 1);  
  }
  String stringOne = "Content-Type: text/html";
  // you can also look for a substring in the middle of a string:
  if (stringOne.substring(14,18) == "text") {
    Serial.println("yes");
  }

  
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {

  updateTime();
  
  delay(1000);  //GET Data at every 15 seconds
}
//=======================================================================
