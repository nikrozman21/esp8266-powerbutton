#include <ESP8266WiFi.h>

// Variables
const char* ssid = "HomeAP";
const char* password = "passw0rd";
const char* espName = "ESP-1"; // Change to your liking
const int outputPin = D8;
const int analogInPin = A0;
int ledOnTreshold = 1000;

// Static values
String header;
double voltage = 0;
String buttonState = "off";
String ledState = "off";

// Web server
WiFiServer server(80);
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(D8, OUTPUT);
  Serial.println("Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();

  // Read voltage
  voltage = analogRead(analogInPin);
  delay(5);
  if (voltage > ledOnTreshold){
    ledState = "on";
  } else {
    ledState = "off";
  }

  // Serve client
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();         
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Turn pin on/off
            if (header.indexOf("POST /reset") >= 0) {
              Serial.println("Power reset!");
              buttonState = "on";
              digitalWrite(D8, HIGH);
              delay(10000);
              digitalWrite(D8, LOW);
              buttonState = "off";
            } else if (header.indexOf("POST /pin/on") >= 0) {
              Serial.println("Power button pressed!");
              buttonState = "on";
              digitalWrite(D8, HIGH);
            } else if (header.indexOf("POST /pin/off") >= 0) {
              Serial.println("Power button de-pressed!");
              buttonState = "off";
              digitalWrite(D8, LOW);
            } else if (header.indexOf("POST /short") >= 0) {
              Serial.println("Short button press!");
              buttonState = "on";
              digitalWrite(D8, HIGH);
              delay(200);
              digitalWrite(D8, LOW);
              buttonState = "off";
            }

            // Check what website to serve
            if (header.indexOf("GET / HTTP") >= 0 || header.indexOf("POST /") >= 0) {
            // HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"5; URL=/\">");   
            client.println("<body><h1>" + String(espName) + "Power button</h1>");
            
            // Current state 
            client.println("<p>Button state: " + buttonState + "</p>");
            client.println("<p>LED state: " + ledState + "</p>");
            client.println("<p>--------[ DEBUG ]--------</p>");
            client.println("<p> LED voltage: " + String((int)voltage) + "</p>");

            client.println("<p><form action=\"/reset\" method=\"post\"><button type=\"submit\" class=\"button\">SEND RESET</button></form></a></p>");
            client.println("<p><form action=\"/short\" method=\"post\"><button type=\"submit\" class=\"button\">SEND SHORT</button></form></a></p>");
            if (buttonState=="off") {
              client.println("<p><form action=\"/pin/on\" method=\"post\"><button type=\"submit\" class=\"button\">ON</button></form></a></p>");
            } else {
              client.println("<p><form action=\"/pin/off\" method=\"post\"><button type=\"submit\" class=\"button button2\">OFF</button></form></a></p>");
            } 
            client.println("</body></html>");
            // CSS
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            // Get just voltage - live update yet to be implemented
            } else if (header.indexOf("GET /update") >= 0) {
              client.println(String((int)voltage));
            }

            // End HTTP header
            client.println();
            
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
