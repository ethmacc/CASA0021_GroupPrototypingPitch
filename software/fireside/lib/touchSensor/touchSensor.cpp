/**
 * @file Touch.cpp
 * 
 * @author Vineeth Kirandumkara
 * @brief Touch class source file
 * @date 2025-03-18
 */

#include "touchSensor.hpp"

#define ROLLING_AVG_SIZE 10

touchSensor::touchSensor(uint8_t cathode, uint8_t anode)
{
    // Update the capSense object
    capSensor = new CapacitiveSensor(cathode, anode);

    // Turn off autocalibrate
    capSensor->set_CS_AutocaL_Millis(0xFFFFFFFF);
}

bool touchSensor::checkTouch(void)
{
    // Rolling Average
    static long avgArr[ROLLING_AVG_SIZE] = {0};
    static long sum = 0;
    static uint8_t iterator = 0;

    // Grab a reading
    long reading = capSensor->capacitiveSensor(30);

    // Check if the first 10 readings have already occured
    if(avgArr[iterator] != 0){

        // Grab the Average value
        long avg = sum / ROLLING_AVG_SIZE;

        // Reading has spiked, there has been a touch!
        if(reading > avg){
            Serial.print("Touch Detected! | reading: ");
            Serial.print(reading);
            Serial.print(" | avg: ");
            Serial.println(avg);
            return true;
        }

        // No Touch Detected
        // Remove the oldest value
        sum -= avgArr[iterator];

        // Add the new value
        avgArr[iterator] = reading;

        // Update the sum
        sum += reading;

        // Update the iterator
        iterator = (iterator + 1) % ROLLING_AVG_SIZE;

    }

    return false;
}