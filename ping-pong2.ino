#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <Adafruit_SleepyDog.h>

#include "arduino_secrets.h"

char serverAddress[]     = "192.168.10.18";
int port                 = 3000;

int channel              = 0;
int threshold            = 300;
unsigned long interval   = 200;
unsigned long idle_time  = 7000;
unsigned long reset_time = 3600000;
int watchdog_time        = 5000;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int status                     = WL_IDLE_STATUS;
int volume                     = 0;
unsigned long current_time     = 0;
unsigned long last_detect_time = 0;
int volume_offset              = 512;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  int countdownMS = Watchdog.enable(watchdog_time);
}

//void(* resetFunc) (void) = 0;

void loop() {
  // put your main code here, to run repeatedly:
  current_time = millis();
  digitalWrite(LED_BUILTIN, LOW);

  if(current_time < reset_time) { Watchdog.reset(); }

  volume = analogRead(channel) - volume_offset;

  //Serial.print(current_time);
  //Serial.print(" : ");
  //Serial.println(volume);

  if(abs(volume) > threshold) {
    if(current_time - last_detect_time > idle_time){
      digitalWrite(LED_BUILTIN, HIGH);
      client.get("/");
      int statusCode = client.responseStatusCode();
      String response = client.responseBody();

      last_detect_time = current_time;
    }
  }

  delay(interval);
}