// Define token //
#define BLYNK_TEMPLATE_ID "TMPL6kbGOx8q6"
#define BLYNK_TEMPLATE_NAME "Air monitoring system"
#define BLYNK_AUTH_TOKEN "qJpNVg5rkgjsQq9a9NxWqm1z46vD1n9D"

#include "DHT.h" // Include DHT sensor library
#include <BlynkSimpleEsp32.h> //Include Blynk library

#define DHTPIN 18 // DHT sensor pin
#define DHTTYPE DHT22 // Define DHT22

#define MQ2 32 // MQ-2 Gas sensor pin
#define MQ7 33 // MQ-7 Gas sensor pin
#define BUZZER 23 // Buzzer pin

#define MQ2_THRESHOLD 400 // Threshold for MQ-2 sensor
#define MQ7_THRESHOLD 3000 // Threshold for MQ-7 sensor

#define green_led 16 // choose the pin for the Green LED 
#define red_led 17 // choose the pin for the Red Led

#define FAN1 21 // Fan 1 pin
#define FAN2 12 // Fan 2 pin

// define virtual for blynk connection
#define VIRTUAL_PIN_TEMP V6
#define VIRTUAL_PIN_HUMIDITY V5
#define VIRTUAL_PIN_MQ7 V4
#define VIRTUAL_PIN_MQ2 V3

// Define the WiFi network SSID and password.
const char* ssid = "";
const char* password = "";

//  create authentication variable
char auth[] = BLYNK_AUTH_TOKEN;

// Create DHT sensor object with defined pin and type
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  dht.begin(); // Initialize DHT sensor
  Serial.begin(9600);

  // Configure the gas sensor pins as input
  pinMode(MQ2, INPUT);
  pinMode(MQ7, INPUT);

  // Configure the buzzer pin as output
  pinMode(BUZZER, OUTPUT);
  
  // Configure the LED pins as output
  pinMode(green_led, OUTPUT);   // declare Green LED as output   
  pinMode(red_led, OUTPUT);     // declare Red LED as output   

  // Configure the fan pins as output
  pinMode(FAN1, OUTPUT); // declare Fan 1 as output
  pinMode(FAN2, OUTPUT); // declare Fan 2 as output

  // Set analog read resolution to 12 bits (0-4095)
  analogReadResolution(12);

  // Connect to the WiFi network.
  WiFi.begin(ssid, password);
  
  // Wait until the connection is established.
  while (WiFi.status() != WL_CONNECTED) {
    // Print a message to the serial port.
    Serial.println("Connecting to WiFi...");
    // Wait 500 milliseconds before checking again.
    delay(500);
  }
  Serial.println("WiFi connected");

  //initialize WIFI to Blynk
  Blynk.begin(auth, ssid, password);
}

void loop() {
  delay(5000); // Wait for 5 seconds before each read operation
  Serial.println("--------------------");

  // Read humidity and temperature from the DHT sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Check if any reading operation failed
  if (isnan(h) || isnan(t)) {
    // If a reading operation failed, print an error message
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Print the humidity and temperature values
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println("% ");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println("°C");
  }

  // Get reading from gas sensors 
  int mq2_value = analogRead(MQ2);
  int mq7_value = analogRead(MQ7);

  // Print the MQ-2 sensor value
  Serial.print("MQ-2 Value: ");
  Serial.println(mq2_value);

  // Print the MQ-7 sensor value
  Serial.print("MQ-7 Value: ");
  Serial.println(mq7_value);

  // Check if the gas levels exceed the thresholds
  if (mq2_value > MQ2_THRESHOLD || mq7_value > MQ7_THRESHOLD) {
    digitalWrite(BUZZER, HIGH); // Turn on the buzzer
    digitalWrite(red_led, HIGH); // Turn LED on
    digitalWrite(green_led, LOW); // Turn LED off
    digitalWrite(FAN1, HIGH); // Turn on Fan 1
    digitalWrite(FAN2, HIGH); // Turn on Fan 2
    Serial.println("DANGER! GAS LEAK");
  } else {
    digitalWrite(BUZZER, LOW); // Turn off the buzzer
    digitalWrite(red_led, LOW); // Turn LED off
    digitalWrite(green_led, HIGH); // Turn LED on
    digitalWrite(FAN1, LOW); // Turn off Fan 1
    digitalWrite(FAN2, LOW); // Turn off Fan 2
    Serial.println("No Leak");
  }

  Serial.println("--------------------");

  // Transmit datastream to Blynk server //
  Blynk.virtualWrite(VIRTUAL_PIN_TEMP, t);
  Blynk.virtualWrite(VIRTUAL_PIN_HUMIDITY, h);
  Blynk.virtualWrite(VIRTUAL_PIN_MQ7, mq7_value);
  Blynk.virtualWrite(VIRTUAL_PIN_MQ2, mq2_value);

  //Sending warning event to Bylnk web and App
  if (temperature > temp_threshold)
  {
   Blynk.logEvent("tempTrigger");
  }

   if (humidity > humid_threshold)
  {
   Blynk.logEvent("humiditytrigger");
  }

   if (mq7.readSensor() > MQ7_THRESHOLD)
  {
   Blynk.logEvent("mq7_trigger");
  }

   if (mq2.readSensor() > MQ2_THRESHOLD)
  {
   Blynk.logEvent("mq2_trigger");
  }
}
