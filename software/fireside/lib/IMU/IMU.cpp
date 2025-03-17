/**
 * @file IMU.cpp
 * 
 * @author Vineeth Kirandumkara
 * @brief IMU class source file
 * @date 2021-07-10
 */

#include "IMU.hpp"

#define PERCENT_DIFF 0.5

bool IMU::initialize(void)
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
        setOffsets();
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
    
    /* Read raw accel/gyro data from the module. Other methods commented*/
    this->mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    if(axArr[iterator] != 0){
        int16_t axAvg = axSum / ROLLING_AVG_SIZE;
        int16_t ayAvg = aySum / ROLLING_AVG_SIZE;
        int16_t azAvg = azSum / ROLLING_AVG_SIZE;

        // Usually a positive value is detected in the X-axis when the sensor is moved
        if((ax < axAvg * (1 + PERCENT_DIFF)) || (ax > axAvg * (1 - PERCENT_DIFF))){
            Serial.println("Movement Detected in X-axis");
            Serial.print("ax: ");
            Serial.print(ax);
            Serial.print(" | axAvg: ");
            Serial.println(axAvg);
            return false;
        }

        // Usually a negative value is detected in the Y and Z-axis when the sensor is moved
        if((ay < ayAvg * (1 + PERCENT_DIFF)) || (ay > ayAvg * (1 - PERCENT_DIFF))){
            Serial.println("Movement Detected in Y-axis");
            Serial.print("ay: ");
            Serial.print(ay);
            Serial.print(" | ayAvg: ");
            Serial.println(ayAvg);
            return false;
        }

        if((az > azAvg * (1 + PERCENT_DIFF)) || (az < azAvg * (1 - PERCENT_DIFF))){
            Serial.println("Movement Detected in Z-axis");
            Serial.print("az: ");
            Serial.print(az);
            Serial.print(" | azAvg: ");
            Serial.println(azAvg);
            return false;
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

    return true;
}

void IMU::setOffsets(void)
{
    /* Use the code below to change accel/gyro offset values. Use MPU6050_Zero to obtain the recommended offsets */ 
    Serial.println("Updating internal sensor offsets...\n");
    this->mpu.setXAccelOffset(0); //Set your accelerometer offset for axis X
    this->mpu.setYAccelOffset(0); //Set your accelerometer offset for axis Y
    this->mpu.setZAccelOffset(0); //Set your accelerometer offset for axis Z
    this->mpu.setXGyroOffset(0);  //Set your gyro offset for axis X
    this->mpu.setYGyroOffset(0);  //Set your gyro offset for axis Y
    this->mpu.setZGyroOffset(0);  //Set your gyro offset for axis Z
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
