/**
 * @file Touch.cpp
 * 
 * @author Vineeth Kirandumkara
 * @brief Touch class source file
 * @date 2025-03-18
 */

#include "touchSensor.hpp"

#define ROLLING_AVG_SIZE 10
#define PERCENT_DIFF 0.5
#define TOUCH_THRESHOLD 200

touchSensor::touchSensor(uint8_t cathode, uint8_t anode, uint8_t deviceThreshold)
{
    // Update the capSense object
    capSensor = new CapacitiveSensor(cathode, anode);

    // Turn off autocalibrate
    capSensor->set_CS_AutocaL_Millis(0xFFFFFFFF);

    // Set the threshold
    touchThreshold = deviceThreshold;
}

bool touchSensor::checkTouch(void)
{

    // // Serial.println("Checking Touch");
    // // Rolling Average
    // static long avgArr[ROLLING_AVG_SIZE] = {0};
    // static long sum = 0;
    // static uint8_t iterator = 0;

    // Grab a reading
    long reading = this->capSensor->capacitiveSensor(30);
    Serial.print("Reading: ");
    Serial.println(reading);

    return (reading > this->touchThreshold);

    // Serial.print("arr val: ");
    // Serial.println(avgArr[iterator]);

    // // Check if the first 10 readings have already occured
    // if(avgArr[iterator] != 0){
        
    //     // Grab the Average value
    //     long avg = sum / ROLLING_AVG_SIZE;

    //     Serial.print("Reading: ");
    //     Serial.print(reading);
    //     Serial.print(" | Avg: ");
    //     Serial.println(avg);

    //     // Reading has spiked, there has been a touch!
    //     if(reading > avg * (1 + PERCENT_DIFF) ){
    //         Serial.println("Touch Detected!");
    //         // Serial.print(reading);
    //         // Serial.print(" | avg: ");
    //         // Serial.println(avg);
    //         return true;
    //     }
    // }

    // // No Touch Detected
    // // Remove the oldest value
    // sum -= avgArr[iterator];

    // // Add the new value
    // avgArr[iterator] = reading;

    // // Update the sum
    // sum += reading;

    // // Update the iterator
    // iterator = (iterator + 1) % ROLLING_AVG_SIZE;
}

void touchSensor::scrapReadings(void)
{
    while(capSensor->capacitiveSensor(30) < 5){
        //no nothing
    }
}
