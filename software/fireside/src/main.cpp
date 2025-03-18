#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <espnow.h>
#include <string.h>
#include "touchSensor.hpp"
#include "macAddr.h"

// GPIOs
// #define LED_BUILTIN D0  // defined within Arduino.h
#define HEATING_COIL D3
#define NEOPIXEL D4
#define CATHODE D8
#define ANODE D5

// Time Constants
#define ONE_SEC 1000
#define FIVE_MIN 20000

// NeoPixel Constants
#define NUMPIXEL_COUNT 8
#define R_HEX 255
#define G_HEX 169
#define B_HEX 87
#define W_HEX 255

#define PULSE_RATE 1
#define MAX_PULSE_BRIGHTNESS 255

touchSensor touch(CATHODE, ANODE, DEVICE_THRESHOLD);

uint8_t buddyAddress[sizeof(MAC_ADDR_LIST)] = {0};  // Ensure the size matches LIST

// Heating Element
bool isHeating = false;           // Also refers to the state of the partner's device
unsigned long heat_timestamp = 0;

// Light Element Flag
bool isPulse = false;

// Cap Touch Flag
bool isTouched = false;

// Message Structure
typedef struct msg_type{
  bool isMoving;
}msg_t;

// NeoPixel
Adafruit_NeoPixel baseLight = Adafruit_NeoPixel(NUMPIXEL_COUNT, NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Callback Functions
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t len);

// Lighting Effect Functions
void pulseLight(void);

/**
 * @brief Callback function that will be called when data is sent
 * 
 * @param mac_addr MAC address of the recipient
 * @param sendStatus Status of the data send
 * @return void
 */
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  // Serial.print("Msg Sent to: ");
  // for (int i = 0; i < 6; i++) {
  //   Serial.print(mac_addr[i], HEX);
  //   if (i < 5){
  //     Serial.print(":");
  //   }
  // }
  // Serial.println();

  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

/**
 * @brief Callback function that will be called when data is received
 * 
 * @param mac_addr MAC address of the sender
 * @param incomingData Data received
 * @param len Length of the data received
 * @return void
 */
void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t len) {

  msg_t newMsg;
  memcpy(&newMsg, data, sizeof(newMsg));
  // Serial.print("Msg Received: ");
  // Serial.println(len);
  // Serial.print("isMoving: ");
  // Serial.println(newMsg.isMoving);
  // Serial.println();

  // Begin the heating, and note that the partner is active
  heat_timestamp = millis();
  isHeating = true;

  // Turn on Heating
  digitalWrite(HEATING_COIL, HIGH);

  // Light up the Neopixel
  isPulse = true;
}

/**
 * @brief Function to pulse the light
 */
void pulseLight(void)
{

  static uint8_t brightness = 0;
  static bool glowUp;

  // Check if glow up or down needs to change
  if(brightness >= MAX_PULSE_BRIGHTNESS){
    glowUp = false;
  }
  else if(brightness <= 0){
    glowUp = true;
  }

  // Update the brightness
  if(glowUp){
    brightness += PULSE_RATE;
  }
  else{
    brightness -= PULSE_RATE;
  }

  Serial.print("Brightness: ");
  Serial.println(brightness);

  // Set the brightness
  for(uint8_t i = 0 ; i < NUMPIXEL_COUNT; i++){
    baseLight.setPixelColor(i, baseLight.Color(R_HEX, G_HEX, B_HEX, W_HEX));
  }
  baseLight.setBrightness(brightness);
  baseLight.show();  
}

void setup() {

  // Configure board LED pin for output
  pinMode(LED_BUILTIN, OUTPUT);

  // Blink to test power on: Something seems fishy on startup
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize Serial Monitor
  Serial.begin(9600);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  delay(1000);

  Serial.println("Starting...");

  // Configure Heating Coil MOSFET pin for output
  pinMode(HEATING_COIL, OUTPUT);

  // Set GPIOs to OFF
  digitalWrite(LED_BUILTIN, LOW);  // Start LED off as low
  digitalWrite(HEATING_COIL, LOW);  // Start Heating Coil off as low

  // Initialize the NeoPixel
  baseLight.begin();
  for(uint8_t i = 0 ; i < NUMPIXEL_COUNT; i++){
    baseLight.setPixelColor(i, baseLight.Color(R_HEX, G_HEX, B_HEX, W_HEX));
  }
  baseLight.setBrightness(0); // Set the brightness to 0
  baseLight.show();

  // Import the buddy address from the header file
  memcpy(buddyAddress, MAC_ADDR_LIST, sizeof(MAC_ADDR_LIST));

  // Announce the buddy address
  Serial.println("This is my buddy: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(buddyAddress[i], HEX);
    if (i < 5){
      Serial.print(":");
    }
  }

  // Set the device as a WIFI Station
  WiFi.mode(WIFI_STA);
  Serial.println("Set as WiFi Station");

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW Initialized");

  // Set device as a combo
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  Serial.println("Set as Combo");

  // Register callback functions
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Registered Callbacks");

  // Add broadcast address
  esp_now_add_peer(buddyAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  Serial.println("Added Buddy");

  delay(1000);
}

void loop() {

  static unsigned long poll_timestamp = millis();

  // Update at Loop Speed
  if(isPulse){
    pulseLight();
  }
  else{
    baseLight.setBrightness(0);
    baseLight.show();
  }

  if(millis() - poll_timestamp > ONE_SEC){

    if(touch.checkTouch()){
      Serial.println("Touch Detected.");

      // Pulse the Light
      isPulse = true;

      // Send a message 
      msg_t newMsg;
      newMsg.isMoving = true;
      Serial.println("Message Sending.");
      esp_now_send(buddyAddress, (uint8_t *) &newMsg, sizeof(newMsg));
    }
    else{
      Serial.println("No Touch Detected.");

      // Turn off the pulse
      isPulse = false;
    }

    poll_timestamp = millis();
  }

  // Safety Condition to prevent heating over 5 min
  if(isHeating){

    // Check if we should reset
    if(millis() - heat_timestamp > FIVE_MIN){
      Serial.println("Heating Complete");
      digitalWrite(HEATING_COIL, LOW);

      isHeating = false;
      isPulse = false;
    }
  }
}