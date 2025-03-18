/**
 * @file IMU.hpp
 * 
 * @author Vineeth Kirandumkara
 * @brief IMU class header file
 * @date 2025-03-11
 */

#include "I2Cdev.h"
#include "MPU6050.h"

#define ROLLING_AVG_SIZE 10

class IMU{

    private:

        /* MPU6050 default I2C address is 0x68*/
        MPU6050 mpu;

        void setOffsets(int X_Acc_OFFSET, int Y_Acc_OFFSET, int Z_Acc_OFFSET, int X_Gyr_OFFSET, int Y_Gyr_OFFSET, int Z_Gyr_OFFSET);

    public:

        bool initialize(int X_Acc_OFFSET, int Y_Acc_OFFSET, int Z_Acc_OFFSET, int X_Gyr_OFFSET, int Y_Gyr_OFFSET, int Z_Gyr_OFFSET);

        bool update(void);
};