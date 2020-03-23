#include "time.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "WiFiClientSecure.h"

#define WLAN_SSID       "Chronos"
#define WLAN_PASS       "testzahl098765"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883
#define AIO_USERNAME    "Baalarios"
#define AIO_KEY "aa186a125b8d48ec8609a7762ad7ebfd"

WiFiClientSecure client;
const char *fingerprint = "77:00:54:2D:DA:E7:D8:03:27:31:23:99:EB:27:DB:CB:A5:4C:57:18";

const byte sensorPin = 21;
int pirState = LOW; // we start, assuming no motion detected
int count = 0;
bool room_activity = false;

long sensor_maximum_time = 60000; // 1 minute
long wait_time = 3000; //3 seconds
long start_time;
long sensor_time;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish activity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/activity");
Adafruit_MQTT_Publish mail = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/mail");
Adafruit_MQTT_Publish room = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/room");

bool DEBUG = false;

void setup() {
  
  start_time = millis();
  pinMode(sensorPin, INPUT);
  Serial.begin(115200);
  Serial.print("Starting Setup");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(2000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  verifyFingerprint();
}

void loop() {
  MQTT_connect();
  sensor_check();
  delay(wait_time);
 
}

void sensor_check()
{
    if(digitalRead(sensorPin) == HIGH)
    {
      count = count + 1;
      if( count > 3 )
      {
        // we have just turned on
        Serial.println("Motion detected!");
        // We only want to print on the output change, not state
        activity.publish(1); 
        if(!room_activity)
        {
          mail.publish("Room is occupied");
          room.publish(1);
        }
        room_activity = true;
        sensor_time = millis();
      }
    }
    else 
    {
        // we have just turned of
        Serial.println("Motion ended!");
        // We only want to print on the output change, not state
        activity.publish(0); 
        count = 0;
    }
    
    if(millis() - sensor_time > sensor_maximum_time)
    {
      if(room_activity)
      {
        mail.publish("Room is free");
        room.publish(0);
      }
      room_activity = false;
    }
}
 


void verifyFingerprint() {

  const char* host = AIO_SERVER;

  Serial.print("Connecting to ");
  Serial.println(host);

  if (! client.connect(host, AIO_SERVERPORT)) {
    Serial.println("Connection failed. Halting execution.");
    while(1);
  }

  if (client.verify(fingerprint, host)) {
  } else {
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("MQTT Connected!");
}
