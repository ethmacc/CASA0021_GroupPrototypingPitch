#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu6050;

void setup() {
  Serial.begin(9600);
  // while (!Serial)
  //   delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Starting...");

  while(1);

  // // Try to initialize!
  // if (!mpu6050.begin()) {
  //   Serial.println("Failed to find MPU6050 chip");
  //   while (1) {
  //     delay(10);
  //   }
  // }
  // Serial.println("MPU6050 Found!");

  // //setupt motion detection
  // mpu6050.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  // mpu6050.setMotionDetectionThreshold(1);
  // mpu6050.setMotionDetectionDuration(20);
  // mpu6050.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  // mpu6050.setInterruptPinPolarity(true);
  // mpu6050.setMotionInterrupt(true);

  // Serial.println("");
  // delay(100);
}

void loop() {
  if(mpu6050.getMotionInterruptStatus()) {
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu6050.getEvent(&a, &g, &temp);

    /* Print out the values */
    Serial.print("AccelX:");
    Serial.print(a.acceleration.x);
    Serial.print(",");
    Serial.print("AccelY:");
    Serial.print(a.acceleration.y);
    Serial.print(",");
    Serial.print("AccelZ:");
    Serial.print(a.acceleration.z);
    Serial.print(", ");
    Serial.print("GyroX:");
    Serial.print(g.gyro.x);
    Serial.print(",");
    Serial.print("GyroY:");
    Serial.print(g.gyro.y);
    Serial.print(",");
    Serial.print("GyroZ:");
    Serial.print(g.gyro.z);
    Serial.println("");
  }

  delay(10);
}