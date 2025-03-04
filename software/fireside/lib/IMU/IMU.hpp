/**
 * @file IMU.hpp
 * 
 * @author Vineeth Kirandumkara
 * @brief IMU class header file
 * @date 2021-07-10
 */

#include "I2Cdev.h"
#include "MPU6050.h"

#define ROLLING_AVG_SIZE 10

class IMU{

    private:

        typedef enum axisType{
            X,
            Y,
            Z
        }axis_t;

        /* MPU6050 default I2C address is 0x68*/
        MPU6050 mpu;

        void setOffsets(void);

    public:

        bool initialize(void);

        bool update(void);
};