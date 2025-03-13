#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <espnow.h>
#include <string.h>
#include "IMU.hpp"
#include "macAddr.h"

#define DEVICE_B

#define ONE_SEC 1000
#define FIVE_MIN 100000
#define LED_BUILTIN D0
#define HEATING_COIL D1
#define NEOPIXEL D2
#define NUMPIXEL_COUNT 8
#define R_HEX 255
#define G_HEX 197
#define B_HEX 143

#ifdef defined DEVICE_A
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

  if(newMsg.isMoving){
    if(amIMoving){
      // Turn on the Heating Element
      digitalWrite(HEATING_COIL, HIGH);
      isHeating = true;
      heat_timestamp = millis();
    }
    else{
      // Turn on the Lighting Element
      baseLight.fill(baseLight.Color(R_HEX, G_HEX, B_HEX), 0, NUMPIXEL_COUNT);
      baseLight.show();
    }
  }
}

void setup() {

    // Configure board LED pin for output
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(HEATING_COIL, OUTPUT);

    // Set GPIOs to OFF
    digitalWrite(LED_BUILTIN, LOW);  // Start LED off as low
    digitalWrite(HEATING_COIL, LOW);  // Start Heating Coil off as low

    // Initialize the NeoPixel
    baseLight.begin();
    baseLight.clear();
    baseLight.show();

    // Initialize Serial Monitor
    Serial.begin(9600);
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Starting...");

    #ifdef defined DEVICE_A
      Serial.println("I am Device A");
    #elif defined DEVICE_B
      Serial.println("I am Device B");
    #endif

    #ifdef defined DEVICE_A
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

  Serial.println(".");

  switch(systemState){
    case polling:
        // Check the IMU every 1 sec
        #ifdef defined DEVICE_A
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
          }
        #endif
      break;
    case msg_send:
        // Send Message to Buddy
        #ifdef defined DEVICE_A
          msg_t newMsg;
          newMsg.isMoving = true;
          Serial.println("Message Sending.");
          esp_now_send(buddyAddress, (uint8_t *) &newMsg, sizeof(newMsg));
          systemState = wait;

          digitalWrite(LED_BUILTIN, HIGH);
        #endif
      break;
    case wait:
        // Give it 5 min before trying to read the IMU again
        #ifdef defined DEVICE_A
          if(millis() - wait_timestamp > FIVE_MIN){
            Serial.println("Wait Complete");
            systemState = polling;
            digitalWrite(LED_BUILTIN, LOW);
          }
        #endif
      break;
    default:
      break;
  }

  // Turn off heating after 5 min?
  if(isHeating){
    if(millis() - heat_timestamp > FIVE_MIN){
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(HEATING_COIL, LOW);
      isHeating = false;
      baseLight.clear();
      baseLight.show();
    }
  }
























  

  // // Reset after 5 min
  // if(millis() - sentTime > FIVE_MIN){
  //   // Begin taking readings again

  // }

  // if(millis() - recvTime > FIVE_MIN){
  //   // Turn off the Heating element
  // }

  // // Reset the LED after 5 minutes
  // if(digitalRead(LED_BUILTIN) == HIGH){
  //   Serial.println("Turning off LED...");
  //   if(millis() - recvTime > FIVE_MIN){
  //     digitalWrite(HEATING_COIL, LOW);
  //     digitalWrite(LED_BUILTIN, LOW);
  //     Serial.println("It's been 5 min since our buddy moved...");
  //   }
  // }

  // #ifdef defined DEVICE_A
  //   // Check IMU for movement
  //   Serial.println("Checking IMU...");
  //   if(millis() - sentTime > FIVE_MIN){ // We only want to send another message if 5 Min has elapsed
  //     Serial.println("5 Min has elapsed since last message");
  //     if(imu.update() == false){
  //       msg_t newMsg;
  //       newMsg.isMoving = true;
  //       esp_now_send(buddyAddress, (uint8_t *) &newMsg, sizeof(newMsg));

  //     }
  //   }
  // #endif
  delay(500);
}