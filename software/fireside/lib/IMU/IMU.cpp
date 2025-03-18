/**
 * @file IMU.cpp
 * 
 * @author Vineeth Kirandumkara
 * @brief IMU class source file
 * @date 2021-07-10
 */

#include "IMU.hpp"

#define PERCENT_DIFF 0.5

bool IMU::initialize(int X_Acc_OFFSET, int Y_Acc_OFFSET, int Z_Acc_OFFSET, int X_Gyr_OFFSET, int Y_Gyr_OFFSET, int Z_Gyr_OFFSET)
{
    /*Initialize device and check connection*/ 
    Serial.println("Initializing MPU...");
    this->mpu.initialize();

    Serial.println("Testing MPU6050 connection...");
    if(this->mpu.testConnection() ==  false){
        Serial.println("MPU6050 connection failed");
        return false;
    }
    else{
        Serial.println("MPU6050 connection successful");
        setOffsets(X_Acc_OFFSET, Y_Acc_OFFSET, Z_Acc_OFFSET, X_Gyr_OFFSET, Y_Gyr_OFFSET, Z_Gyr_OFFSET);
        return true;
    }
}

bool IMU::update(void)
{
    static int iterator = 0;
    static int16_t axArr[ROLLING_AVG_SIZE] = {0}, ayArr[ROLLING_AVG_SIZE] = {0}, azArr[ROLLING_AVG_SIZE] = {0};
    static int32_t axSum = 0, aySum = 0, azSum = 0;

    int16_t ax, ay, az;
    int16_t gx, gy, gz; // Dummies, not really needed

    bool retVal = true;
    
    /* Read raw accel/gyro data from the module. Other methods commented*/
    this->mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Convert all values to positive
    // ax = abs(ax);
    // ay = abs(ay);
    // az = abs(az);

    if(axArr[iterator] != 0){
        int16_t axAvg = axSum / ROLLING_AVG_SIZE;
        int16_t ayAvg = aySum / ROLLING_AVG_SIZE;
        int16_t azAvg = azSum / ROLLING_AVG_SIZE;

        // Usually a positive value is detected in the X-axis when the sensor is moved
        if((ax > axAvg * (1 + PERCENT_DIFF)) || (ax < axAvg * (1 - PERCENT_DIFF))){
            Serial.println("Movement Detected in X-axis");
            Serial.print("ax: ");
            Serial.print(ax);
            Serial.print(" | axAvg: ");
            Serial.println(axAvg);
            // return false;
            retVal = false;
        }

        // Usually a negative value is detected in the Y and Z-axis when the sensor is moved
        if((ay > ayAvg * (1 + PERCENT_DIFF)) || (ay < ayAvg * (1 - PERCENT_DIFF))){
            Serial.println("Movement Detected in Y-axis");
            Serial.print("ay: ");
            Serial.print(ay);
            Serial.print(" | ayAvg: ");
            Serial.println(ayAvg);
            // return false;
            retVal = false;
        }

        if((az > azAvg * (1 + PERCENT_DIFF)) || (az < azAvg * (1 - PERCENT_DIFF))){
            Serial.println("Movement Detected in Z-axis");
            Serial.print("az: ");
            Serial.print(az);
            Serial.print(" | azAvg: ");
            Serial.println(azAvg);
            // return false;
            retVal = false;
        }

    }

    // Remove oldest value
    axSum -= axArr[iterator];
    aySum -= ayArr[iterator];
    azSum -= azArr[iterator];

    // Add new value
    axArr[iterator] = ax;
    ayArr[iterator] = ay;
    azArr[iterator] = az;

    // Update the sum
    axSum += ax;
    aySum += ay;
    azSum += az;

    // Update the iterator
    iterator = (iterator + 1) % ROLLING_AVG_SIZE;

    return retVal;
}

void IMU::setOffsets(int X_Acc_OFFSET, int Y_Acc_OFFSET, int Z_Acc_OFFSET, int X_Gyr_OFFSET, int Y_Gyr_OFFSET, int Z_Gyr_OFFSET)
{
    /* Use the code below to change accel/gyro offset values. Use MPU6050_Zero to obtain the recommended offsets */ 
    Serial.println("Updating internal sensor offsets...\n");
    this->mpu.setXAccelOffset(X_Acc_OFFSET); //Set your accelerometer offset for axis X
    this->mpu.setYAccelOffset(Y_Acc_OFFSET); //Set your accelerometer offset for axis Y
    this->mpu.setZAccelOffset(Z_Acc_OFFSET); //Set your accelerometer offset for axis Z
    this->mpu.setXGyroOffset(X_Gyr_OFFSET);  //Set your gyro offset for axis X
    this->mpu.setYGyroOffset(Y_Gyr_OFFSET);  //Set your gyro offset for axis Y
    this->mpu.setZGyroOffset(Z_Gyr_OFFSET);  //Set your gyro offset for axis Z
    /*Print the defined offsets*/
    Serial.print("\t");
    Serial.print(this->mpu.getXAccelOffset());
    Serial.print("\t");
    Serial.print(this->mpu.getYAccelOffset()); 
    Serial.print("\t");
    Serial.print(this->mpu.getZAccelOffset());
    Serial.print("\t");
    Serial.print(this->mpu.getXGyroOffset()); 
    Serial.print("\t");
    Serial.print(this->mpu.getYGyroOffset());
    Serial.print("\t");
    Serial.print(this->mpu.getZGyroOffset());
    Serial.print("\n");
}
