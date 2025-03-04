#include <Arduino.h>
#include "IMU.hpp"

IMU imu;

void setup() {
    Serial.begin(9600);
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Starting...");

    /*--Start I2C interface--*/
    Wire.begin();

    /*--Initialize IMU--*/
    if(imu.initialize()){
        Serial.println("IMU initialized successfully");
    }
    else{
        Serial.println("IMU initialization failed");
    }
  
    /*Configure board LED pin for output*/ 
    pinMode(LED_BUILTIN, OUTPUT);
    delay(1000);
}

void loop() {

  // Check IMU for movement
  if(imu.update()){
    digitalWrite(LED_BUILTIN, LOW);
  }
  else{
    //Serial.println("Motion detected");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  delay(500);
}