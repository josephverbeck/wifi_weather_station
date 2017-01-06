#include <SPI.h>
#include <WiFi101.h>
#include <DHT.h>

#define DHTPIN 2 //DHT connection pin
#define DHTTYPE DHT22 //DHT 22 sensor

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "SSID";
char pass[] = "PASSWORD";

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  WiFi.setPins(8,7,4);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  status = WiFi.begin(ssid, pass);
  dht.begin();
  server.begin();
  IPAddress ip = WiFi.localIP();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 30");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          float h = dht.readHumidity();
          // Read temperature as Fahrenheit (isFahrenheit = true)
          float t = dht.readTemperature(true);
          client.print("Humidity: ");
          client.print(h);
          client.print(" %\t");
          client.print("Temperature: ");
          client.print(t);
          
          client.println("<br />");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
 
}
