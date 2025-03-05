#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <string.h>
#include "IMU.hpp"
#include "macAddr.h"

#define DEVICE_B

#define FIVE_MIN 100000
#define HEATING_COIL D8 // GPIO15

#ifdef DEVICE_A
  IMU imu;
#endif

uint8_t buddyAddress[sizeof(MAC_ADDR_LIST)] = {0};  // Ensure the size matches LIST

typedef struct msg_type{
  bool isMoving;
}msg_t;

unsigned long sentTime = millis(), recvTime = millis();

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
  Serial.print("Sending message to: ");
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
    sentTime = millis();
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
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("isMoving: ");
  Serial.println(newMsg.isMoving);
  Serial.println();

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(HEATING_COIL, HIGH);
  Serial.println("Our Buddy is Moving!");
  recvTime = millis();
}

void setup() {

    /*Configure board LED pin for output*/ 
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(HEATING_COIL, OUTPUT);
    digitalWrite(HEATING_COIL, LOW);  // Start Heating Coil off as low

    // Blink to test power on: Something seems fishy on startup
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(9600);
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Starting...");

    #ifdef DEVICE_A
      /*--Start I2C interface--*/
      Wire.begin();
      
      /*--Initialize IMU--*/
      if(imu.initialize()){
          Serial.println("IMU initialized successfully");
      }
      else{
          Serial.println("IMU initialization failed");
      }
    #endif

    // Import the buddy address from the header file
    memcpy(buddyAddress, MAC_ADDR_LIST, sizeof(MAC_ADDR_LIST));  // Copy the contents

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
  Serial.println(".");

  // Reset the LED after 5 minutes
  if(digitalRead(LED_BUILTIN) == HIGH){
    Serial.println("Turning off LED...");
    if(millis() - recvTime > FIVE_MIN){
      digitalWrite(HEATING_COIL, LOW);
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("It's been 5 min since our buddy moved...");
    }
  }

  #ifdef DEVICE_A
    // Check IMU for movement
    Serial.println("Checking IMU...");
    if(millis() - sentTime > FIVE_MIN){ // We only want to send another message if 5 Min has elapsed
      Serial.println("5 Min has elapsed since last message");
      if(imu.update() == false){
        msg_t newMsg;
        newMsg.isMoving = true;
        esp_now_send(buddyAddress, (uint8_t *) &newMsg, sizeof(newMsg));
      }
    }
  #endif
  delay(500);
}