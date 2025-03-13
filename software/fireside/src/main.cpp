#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <espnow.h>
#include <string.h>
#include "IMU.hpp"
#include "macAddr.h"

// #define DEVICE_A 1
#define DEVICE_B 1

// GPIOs
// #define LED_BUILTIN D0  // defined within Arduino.h
// #define SCL D1
// #define SDA D2
#define HEATING_COIL D3
#define NEOPIXEL D4

// Time Constants
#define ONE_SEC 1000
#define FIVE_MIN 10000

// NeoPixel Constants
#define NUMPIXEL_COUNT 8
#define R_HEX 255
#define G_HEX 197
#define B_HEX 143

#ifdef DEVICE_A
  IMU imu;
#endif

uint8_t buddyAddress[sizeof(MAC_ADDR_LIST)] = {0};  // Ensure the size matches LIST

// Heating Element
bool isHeating = false;
unsigned long heat_timestamp;

// Message Structure
typedef struct msg_type{
  bool isMoving;
}msg_t;

// System States
enum systemState_t {
  polling,
  msg_send,
  wait,
};

// Movement Flags
bool amIMoving = false;
bool hasBuddyMoved = false;

// NeoPixel
Adafruit_NeoPixel baseLight = Adafruit_NeoPixel(NUMPIXEL_COUNT, NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Callback functions
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t len);

/**
 * @brief Callback function that will be called when data is sent
 * 
 * @param mac_addr MAC address of the recipient
 * @param sendStatus Status of the data send
 * @return void
 */
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  Serial.print("Msg Sent to: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac_addr[i], HEX);
    if (i < 5){
      Serial.print(":");
    }
  }
  Serial.println();

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
  Serial.print("Msg Received: ");
  Serial.println(len);
  Serial.print("isMoving: ");
  Serial.println(newMsg.isMoving);
  Serial.println();

  // Save Buddies State
  hasBuddyMoved = newMsg.isMoving;

  amIMoving = hasBuddyMoved;  // TODO: REMOVE THIS FOR THE FINAL IMPLEMENTATION
  // THIS LINE IS SIMPLY TO TEST ON THE BREADBOARD, SINCE I ONLY HAVE 1 IMU
}

void setup() {

      /*Configure board LED pin for output*/ 
      pinMode(LED_BUILTIN, OUTPUT);
  
      // Blink to test power on: Something seems fishy on startup
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);

  // Initialize Serial Monitor
  Serial.begin(9600);
  while (!Serial)
      delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Starting...");

    // Configure board LED pin for output
    //pinMode(LED_BUILTIN, OUTPUT);
    pinMode(HEATING_COIL, OUTPUT);

    // Set GPIOs to OFF
    digitalWrite(LED_BUILTIN, LOW);  // Start LED off as low
    digitalWrite(HEATING_COIL, LOW);  // Start Heating Coil off as low

    // Initialize the NeoPixel
    baseLight.begin();
    baseLight.clear();
    baseLight.show();

    #ifdef DEVICE_A
      Serial.println("I am Device A");
    #elif DEVICE_B
      Serial.println("I am Device B");
    #endif

    #ifdef DEVICE_A
      // Start I2C interface
      Wire.begin();
      
      // Initialize IMU
      if(imu.initialize()){
          Serial.println("IMU initialized successfully");
      }
      else{
          Serial.println("IMU initialization failed");
      }
    #endif

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

  static systemState_t systemState = polling;
  static unsigned long poll_timestamp = millis();
  static unsigned long wait_timestamp = millis();

  // Serial.println(".");

  switch(systemState){
    case polling:
        // Check the IMU every 1 sec
        #ifdef DEVICE_A
          if(millis() - poll_timestamp > ONE_SEC){
            if(imu.update() == false){
              Serial.println("Movement Detected.");
              amIMoving = true;
              systemState = msg_send; // Change the systemState
            }
            else{
              Serial.println("No Movement Detected.");
              amIMoving = false;
            }
            poll_timestamp = millis();  // Reset the timestamp
          }
        #endif
      break;
    case msg_send:
        // Send Message to Buddy
        #ifdef DEVICE_A
          msg_t newMsg;
          newMsg.isMoving = true;
          Serial.println("Message Sending.");
          esp_now_send(buddyAddress, (uint8_t *) &newMsg, sizeof(newMsg));
          systemState = wait;
          wait_timestamp = millis();
          digitalWrite(LED_BUILTIN, HIGH);
        #endif
      break;
    case wait:
        // Give it 5 min before trying to read the IMU again
        #ifdef DEVICE_A
          if(millis() - wait_timestamp > FIVE_MIN){
            Serial.println("Wait Complete");
            systemState = polling;
            digitalWrite(LED_BUILTIN, LOW);

            // Clear Lights after 5 Min ???
            baseLight.clear();
            baseLight.show();
            amIMoving = false;
          }
        #endif
      break;
    default:
      break;
  }

  #ifdef DEVICE_B
    // Turn off heating after 5 min?
    if(isHeating){
      if(millis() - heat_timestamp > FIVE_MIN){
        Serial.println("Heating Complete");
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(HEATING_COIL, LOW);
        isHeating = false;
        amIMoving = false;  // TODO: Look into removing this once the second IMU is added
        hasBuddyMoved = false; // TODO: Look into removing this once the second IMU is added

        // Clear Lights 5 min after Heating is registered
        baseLight.clear();
        baseLight.show();
        Serial.println("Lights Cleared?");
      }
    }
  #endif

  // If either myself, or my buddy has moved
  if(amIMoving || hasBuddyMoved){
    // Turn on the Lighting Element
    for(uint8_t i = 0 ; i < NUMPIXEL_COUNT; i++){
      baseLight.setPixelColor(i, baseLight.Color(R_HEX, G_HEX, B_HEX));
      baseLight.show();
    }
  }

  // If both myself, and my buddy have moved
  if((amIMoving && hasBuddyMoved) && !isHeating){
    // Turn on the Heating Element
    Serial.println("Heating Element On");
    digitalWrite(HEATING_COIL, HIGH);
    isHeating = true;
    heat_timestamp = millis();
  }
}