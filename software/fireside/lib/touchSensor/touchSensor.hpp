/**
 * @file Touch.hpp
 * 
 * @author Vineeth Kirandumkara
 * @brief Touch class header file
 * @date 2025-03-18
 */

#ifndef TOUCH_LIB
#define TOUCH_LIB

#include <CapacitiveSensor.h>

// #define ROLLING_AVG_SIZE 10

class touchSensor {
    
    private:
        // Pointer to Capacitive Sensor Object
        CapacitiveSensor *capSensor;

    public:

        /**
         * @brief Construct a new touch Sensor object
         * 
         * @param CATHODE 
         * @param ANODE 
         */
        touchSensor(uint8_t cathode, uint8_t anode);

        /**
         * @brief Check if the touch sensor is touched
         * 
         * @return true
         */
        bool checkTouch(void);
};

#endif