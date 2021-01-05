

#include <WiFiNINA.h>
#include "DHT.h"

#define DHTPIN 2    // what pin we're connected to, pin1 is 5th pin from end

// Uncomment whatever DHT sensor type you're using!
//#define DHTTYPE DHT11  // DHT 11
//#define DHTTYPE DHT21  // DHT 21
#define DHTTYPE DHT22  // DHT 22

DHT dht(DHTPIN,DHTTYPE);
WiFiClient client;

const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "v7DCFF3CD14D7A91"; //device ID on Pushingbox for our Scenario

const char* MY_SSID = "HomeNetwork";
const char* MY_PWD =  "H0m3N3tw0rk";


int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(MY_SSID);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(MY_SSID, MY_PWD);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
  dht.begin();
}

void loop() {

   // Wait between measurements.
  

  //prefer to use float, but package size or float conversion isnt working
  //will revise in future with a string fuction or float conversion function

  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();


  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float HeatIndex = dht.computeHeatIndex(temperature, humidity, false);

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C ");
  Serial.print(" Heat index: ");
  Serial.print(HeatIndex);
  Serial.print(" *C ");
  

Serial.println("\nSending Data to Server..."); 
  // if you get a connection, report back via serial:
  //Instantiate WiFi object, can scope from here or Globally

    //API service using WiFi Client through PushingBox then relayed to Google
    if (client.connect(WEBSITE, 80))
      { 
         client.print("GET /pushingbox?devid=" + devid
       + "&temperature=" + (String) temperature
       + "&humidity="      + (String) humidity
       + "&HeatIndex="      + (String) HeatIndex
         );

      // HTTP 1.1 provides a persistent connection, allowing batched requests
      // or pipelined to an output buffer
      client.println(" HTTP/1.1"); 
      client.print("Host: ");
      client.println(WEBSITE);
      client.println("User-Agent: MKR1000/1.0");
      //for MKR1000, unlike esp8266, do not close connection
      client.println();
      Serial.println("\nData Sent"); 
      //client.flush();
     // client.stop();
      delay(864000);
      }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
