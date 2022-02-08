#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "SimpleTimer.h"
#include <DHT.h>
#include <DHT_U.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
#define BLYNK_TEMPLATE_ID "your_blynk_template_id_here"
#define BLYNK_DEVICE_NAME "Connected Greenhouse"
char auth[] = " "; //Enter the Auth code which was send by Blink

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = " ";  //Enter your WIFI Name
char pass[] = " ";  //Enter your WIFI Password

#define DHTTYPE DHT11
#define DHTPIN 2        // Digital pin 4
#define LDR A0
#define LED 5   // composante diode électroluminescente sur la pin 9
#define trigPin D6 // Trigger Pin of Ultrasonic Sensor
#define echoPin D5 // Echo Pin of Ultrasonic Sensor
#define buzzer 4  // D2
boolean notifiedOn = true;
boolean notifiedOff = true;
boolean notifiedClose = true;
long duration;
int distance;

DHT dht(DHTPIN, DHTTYPE);
SimpleTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensors() {
  // DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // LDR
  int LDRvalue = analogRead(LDR);
  /*allume la LED */
  if (LDRvalue > 800) {
    digitalWrite(LED, HIGH);
    if ( notifiedOn) {
      Blynk.notify("It's too dark. The LED is turned on !");
      notifiedOn = false;
      notifiedOff = true;
    }
  }
  /* désactiver la LED */
  if (LDRvalue < 800) {
    digitalWrite(LED, LOW);
    if (notifiedOff) {
      Blynk.notify("Yaay... Sun is up. The LED is turned off !");
      notifiedOff = false;
      notifiedOn = true;
    }
  }

  // Ultrasonic Sensor
  /* Clears the trigPin condition */
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Send notification of approx
  if (distance < 10) {
    tone(buzzer, 1000, 900);
    if ( notifiedClose == true) {
      Blynk.notify("Someone is in the approximity of your greenhouse !!");
       notifiedClose = false;
    }
  }
  else if (distance > 50) {
    notifiedClose = true;
  }

  // You can send any value at any time
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);  //V5 is for Humidity
  Blynk.virtualWrite(V6, t);  //V6 is for Temperature
  Blynk.virtualWrite(V7, distance);  //V7 is for distance
  Blynk.virtualWrite(V8, LDRvalue);   // V8 is for LDR value
}

void setup() {
  Serial.begin(9600); // See the connection status in Serial Monitor
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Blynk.begin(auth, ssid, pass); //setting Blynk param

  dht.begin(); //setting DHT11 param

  pinMode(LDR, INPUT);  //setting LDR param
  pinMode(LED, OUTPUT);

  pinMode(trigPin, OUTPUT);  //setting Ultrasonic sensor param
  pinMode(echoPin, INPUT);

  pinMode(buzzer, OUTPUT);  // setting buzzer stuff

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensors);
}

void loop() {
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer
}

int microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
