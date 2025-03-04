/**
 * @file IMU.cpp
 * 
 * @author Vineeth Kirandumkara
 * @brief IMU class source file
 * @date 2021-07-10
 */

#include "IMU.hpp"

#define PERCENT_DIFF 0.10

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
    static float axArr[ROLLING_AVG_SIZE] = {0}, ayArr[ROLLING_AVG_SIZE] = {0}, azArr[ROLLING_AVG_SIZE] = {0};
    static float axSum = 0, aySum = 0, azSum = 0;

    int16_t ax, ay, az;
    int16_t gx, gy, gz; // Dummies, not really needed
    /* Read raw accel/gyro data from the module. Other methods commented*/
    this->mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    //mpu.getAcceleration(&ax, &ay, &az);
    //mpu.getRotation(&gx, &gy, &gz);

    // Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
    // Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
    // Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
    // Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
    // Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
    // Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));

    // Check if reading and avg are different
    float tempAvg = 0;
    float tempPercent = 0;
    float tempReading = 0;

    for(uint8_t axis = X; axis <= Z; axis++){
        switch(axis){
            case X:
                tempAvg = axSum / ROLLING_AVG_SIZE;
                tempReading = ax;
                Serial.print("X: ");
                Serial.print(ax);
                Serial.print(" ");
                break;
            case Y:
                tempAvg = aySum / ROLLING_AVG_SIZE;
                tempReading = ay;
                Serial.print("Y: ");
                Serial.print(ay);
                Serial.print(" ");
                break;
            case Z:
                tempAvg = azSum / ROLLING_AVG_SIZE;
                tempReading = az;
                Serial.print("Z: ");
                Serial.print(az);
                Serial.print(" ");
                break;
        }

        Serial.print("Avg: ");
        Serial.println(tempAvg);
        tempPercent = (PERCENT_DIFF * tempAvg);

        // if( tempReading > (tempAvg * (1 + PERCENT_DIFF)) ){
        //     Serial.println("Above the avg Threshold");
        //     Serial.println("avg + percent < reading:");
        //     Serial.print(tempAvg);
        //     Serial.print(" + ");
        //     Serial.print(tempPercent);
        //     Serial.print(" = ");
        //     Serial.print(tempAvg + tempPercent);
        //     Serial.print(" > ");
        //     Serial.println(tempReading);
        // }

        // if( tempReading < (tempAvg * (1 - PERCENT_DIFF)) ){
        //     Serial.println("Below the avg Threshold");
        //     Serial.println("avg - percent > reading:");
        //     Serial.print(tempAvg);
        //     Serial.print(" - ");
        //     Serial.print(tempPercent);
        //     Serial.print(" = ");
        //     Serial.print(tempAvg - tempPercent);
        //     Serial.print(" < ");
        //     Serial.println(tempReading);
        // }

        if(( tempReading < (tempAvg * (1 + PERCENT_DIFF)) ) || ( tempReading > (tempAvg * (1 - PERCENT_DIFF)) )){
            Serial.println("Motion detected!");
            //return false;
        }

        // if( (tempAvg + tempPercent) < tempReading || (tempAvg - tempPercent) > tempReading){
        //     if(axArr[iterator] != 0){
        //         Serial.println("avg + percent < reading:");
        //         Serial.print(tempAvg);
        //         Serial.print(" + ");
        //         Serial.print(tempPercent);
        //         Serial.print(" = ");
        //         Serial.print(tempAvg + tempPercent);
        //         Serial.print(" < ");
        //         Serial.println(tempReading);

        //         Serial.println("avg - percent > reading:");
        //         Serial.print(tempAvg);
        //         Serial.print(" - ");
        //         Serial.print(tempPercent);
        //         Serial.print(" = ");
        //         Serial.print(tempAvg - tempPercent);
        //         Serial.print(" > ");
        //         Serial.println(tempReading);

        //         Serial.println("Motion detected!");
        //         //return false;
        //     }
        // }
    }

    // Update the Sums
    axSum = axSum - axArr[iterator] + ax;
    aySum = aySum - ayArr[iterator] + ay;
    azSum = azSum - azArr[iterator] + az;

    axArr[iterator] = ax;
    ayArr[iterator] = ay;
    azArr[iterator] = az;

    // Check to reset the iterator
    if(iterator == ROLLING_AVG_SIZE - 1){
        iterator = 0;
    }
    else{
        iterator++;
    }
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
