// Very bad code demonstrating basic usage of the ESP8266 development board
// I don't think there's a name for the board. "It's the white one."
// To use, put your network name and password into the ssid and pass locations below
// it will generate some debugging output on the serial monitor at 9600 baud
// to enable more serial debugging, #define sdebug
// When it connects, it will tell you its IP address on the serial monitor.
// use a web browser to go to that IP address and play with the available controls.
#include <Adafruit_NeoPixel.h>

//#include <Dht11.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
//char pass[] = "fuzzyraccoon898";
int keyIndex = 0;                                          // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
WiFiServer server(80);
//#define REDLED 15
//#define GREENLED 13
//#define BLUELED 12
#define RGBPIN D8
//#define relayPin D1
const long interval = 2000;
const int relayPin = D1;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, RGBPIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  pinMode(relayPin, OUTPUT);            // relay
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);                                                 //Initialize serial and wait for port to open:
  WiFi.mode(WIFI_AP);
  // WiFi.softAP("MiFi 127.256.512", "grmhm5nh");                      // Provide the (SSID, password);
  // WiFi.softAP("BTHub4-R35P", "76ced27f74");// Provide the (SSID, password);
  WiFi.softAP("FlappyBox", "");
  server.begin();
  printWifiStatus();                                                  // you're connected now, so print out the status:
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());                                        // print your WiFi shield's IP address:
  IPAddress ipLocal = WiFi.localIP();
  IPAddress ip = WiFi.softAPIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("Local IP Address: ");
  Serial.println(ipLocal);
  long rssi = WiFi.RSSI();                                                // print the received signal strength:
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void fab()
{
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        pixels.setPixelColor(0, pixels.Color(i * 255, j * 255, k * 255)); // Moderately bright green color.
        pixels.show(); // This sends the updated pixel color to the hardware.
        delay(200); // Delay for a period of time (in milliseconds).
      }
    }
  }
  pixels.setPixelColor(0, pixels.Color(0, 0, 0)); // Moderately bright green color.
  pixels.show();
}

void relayState(char relaySelection) {
  //while(Serial.available())
  char statRelay = relaySelection;
  Wire.beginTransmission(2);
  Wire.write(statRelay);
  Wire.endTransmission();
  Wire.flush();
  Serial.println("Relays buzz on ...." + statRelay);
  statRelay = -1;
}
void loop() {
  char linebuf[255], getvalue[255], *lineptr;
  int linecount;
  WiFiClient client = server.available();                           // listen for incoming clients
  if (client) {
#ifdef sdebug
    Serial.println("new client");
#endif
    boolean currentLineIsBlank = true;                            // an http request ends with a blank line
    lineptr = linebuf;
    linecount = 0;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
#ifdef sdebug
        Serial.write(c);
#endif
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");                                  // the connection will be closed after completion of the response
          client.print( "Refresh: 20\r\n");        // refresh the page automatically every 20 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          if (memcmp(getvalue, "/RED", 4) == 0)
          {
            //int x;
            for (int repeat = 1; repeat < 5; repeat++) {
              for (int x = 0; x < 1024; x += 64)
              {
                analogWrite(LED_BUILTIN, x);
                delay(20);
              }
              for (int x = 1024; x >= 0; x -= 64)
              {
                analogWrite(LED_BUILTIN, x);
                delay(20);
              }
            }
          }
          if (memcmp(getvalue, "/GREEN", 6) == 0)
          {
            int newval = digitalRead(LED_BUILTIN);
            //  int newval = !digitalRead(LED_BUILTIN);
            if (newval == 1) {
              newval = 0;
            }
            else if (newval == 0)
            {
              newval = 1;
            }
            Serial.print("New green value = ");
            Serial.println(newval);
            digitalWrite(LED_BUILTIN, newval);
            Wire.write('1');
          }
          if (memcmp(getvalue, "/BLUE", 5) == 0)
          {
            /*int newval = !digitalRead(BLUELED);
              digitalWrite(BLUELED,newval);
              //Wire.write('0');*/
            Serial.print("Blue called");
            client.print("Blue called");
          }
          if (memcmp(getvalue, "/WHT", 4) == 0)
          { /*
              int newval = !digitalRead(14);
              Serial.print("New white value = "); Serial.println(newval);
              digitalWrite(14,newval);*/
            Serial.print("white called");
            client.print("white called");
          }
          if (memcmp(getvalue, "/RELAY", 6) == 0)
          {
            //    int newval = !digitalRead(relayPin);
            //     digitalWrite(relayPin,newval);

            digitalWrite(relayPin, HIGH); // turn on relay with voltage HIGH
            delay(interval);              // pause
            digitalWrite(relayPin, LOW);  // turn off relay with voltage LOW
            delay(interval);              // pause

            Serial.print("relay called");
            client.print("relay called");
          }
          if (memcmp(getvalue, "/BUZ", 4) == 0)
          { /*
              int newval = !digitalRead(5);
              digitalWrite(5,newval);*/
            Serial.print("buz called");
            client.print("buz called");
          }
          if (memcmp(getvalue, "/FAB", 4) == 0)
          {
            fab();
            Serial.print("fab called");
            client.print("fab called");
          }

          if (memcmp(getvalue, "/RELON", 6) == 0)
          {

            digitalWrite(relayPin, HIGH);
            Serial.print("Relay called on");
            client.print("Relays cyclying");
          }
          if (memcmp(getvalue, "/RELOFF", 7) == 0)
          {
            digitalWrite(relayPin, LOW);
            Serial.print("Relay called off");
            client.print("Relays off");
          }
          if (memcmp(getvalue, "/MUSE", 5) == 0)
          {
            char relSel = '8';
            relayState(relSel);
            Serial.print("Relay 1 Muse thing");
            client.println("Relays Muse");
          }

          if (memcmp(getvalue, "/REL1", 5) == 0)
          {
            char relSel = '2';
            relayState(relSel);
            Serial.println("relsel 2   -----   Relay called");
            client.println("Relay 1");
          }
          if (memcmp(getvalue, "/REL2", 5) == 0)
          {
            char relSel = '3';
            relayState(relSel);
            Serial.println("relsel 3   -----   Relay called");
            client.println("Relay 2");
          }
          if (memcmp(getvalue, "/REL3", 5) == 0)
          {
            char relSel = '4';
            relayState(relSel);
            Serial.println("relsel 4   -----   Relay called");
            client.println("Relay 3");
          }
          if (memcmp(getvalue, "/REL4", 5) == 0)
          {
            char relSel = '5';
            relayState(relSel);
            Serial.println("relsel 5   -----   Relay called");
            client.println("Relay 4");
          }
          // print form
          client.println("<a href=\"/RED\">Green fader</a><br><br>");
          client.println("<a href=\"/GREEN\">toggle GREEN (Builtin Led)</a><br><br>");
          client.println("<a href=\"/BLUE\">toggle BLUE</a><br><br>");
          client.println("<a href=\"/WHT\">toggle WHITE</a><br><br>");
          client.println("<a href=\"/BUZ\">toggle BUZZER</a><br><br>");
          client.println("<a href=\"/RELAY\">toggle RELAY/green</a><br><br>");
          client.println("<a href=\"/FAB\">FAB</a><br><br>");
          client.println("<a href=\"/RELON\">Relays on</a><br><br>");
          client.println("<a href=\"/RELOFF\">Relays off</a><br><br>");
          client.println("<a href=\"/MUSE\">Muse thingy</a><br><br>");
          client.println("<a href=\"/REL1\">Relay 1</a><br><br>");
          client.println("<a href=\"/REL2\">Relay 2</a><br><br>");
          client.println("<a href=\"/REL3\">Relay 3</a><br><br>");
          client.println("<a href=\"/REL4\">Relay 4</a><br><br>");
          long rssi = WiFi.RSSI();
          client.print("signal strength (RSSI):");
          client.print(rssi);
          client.println(" dBm");
          /*#if 1
                    // output the value of each analog input pin
                    int sensorReading = digitalRead(5);
                    client.println("input 5: ");
                    client.print(sensorReading);
                    //Wire.write('1');
                    client.println("<br />");
            #endif*/

          client.println("<br></html>");
          break;
        }
        if (c == '\n') {                                                // you're starting a new line
          currentLineIsBlank = true;
          if (memcmp(linebuf, "GET ", 4) == 0)
          {
            strcpy(getvalue, &(linebuf[4]));
#ifdef sdebug
            Serial.print("GET buffer = ");
            Serial.println(getvalue);
#endif
          }
          lineptr = linebuf;
          linecount = 0;
        }
        else if (c != '\r') {                                             // you've gotten a character on the current line
          currentLineIsBlank = false;
          if (linecount < sizeof(linebuf))
          {
            *lineptr++ = c;
            linecount++;
          }
        }
      }
    }
    delay(10);                                                           // give the web browser time to receive the data
    client.stop();                                                      // close the connection:
#ifdef sdebug
    Serial.println("client disconnected");
#endif
  }
}
