// Include Libraries
#include "Wire.h" // I2C Bus
#include "BMP280.h" // Barometer and Temperature Sensor
#include "MPU9250_WE.h" // Inertial Measurement Unit

#include "baro-rolling-ave.h"

// Definitions for BMP280 Barometer and Temperature Sensor
typedef DFRobot_BMP280_IIC BMP;
BMP bmp(&Wire, BMP::eSdoLow);

// Definitions for MPU9250 Inertial Measurement Unit
MPU9250_WE mpu = MPU9250_WE(0x68); // I2c Address of MPU

// System State Variables
int systemState = 0;
float groundAlt = 0.0;
float maxAltThusFar = 0.0;

void setup(){
    // Move Data from SPI Flash to microSD Card. Halt if erroring.


    Serial.begin(115200); // Start Serial monitoring over USB to host computer

    Wire.begin(); // Start I2C Bus


    // Setup for BMP280
    bmp.reset();
    while(bmp.begin() != BMP::eStatusOK) {
        Serial.println("BMP init faild");
        delay(2000);
    }
    Serial.println("BMP init success");

    
    // Setup for MPU9250
    while(mpu.init() == false){ // failed init returns false
        Serial.println("MPU init failed");
        delay(2000);
    }
    Serial.println("MPU init success");
    mpu.autoOffsets(); // calibration.
    mpu.setSampleRateDivider(5);
    mpu.setAccRange(MPU9250_ACC_RANGE_16G); // Accel Settings
    mpu.enableAccDLPF(true);
    mpu.setAccDLPF(MPU9250_DLPF_1); // Level 6 limit is 5Hz with 67ms delay. Level 0 is 460Hz with 2ms delay.
    mpu.setGyrRange(MPU9250_GYRO_RANGE_2000); // Gyro Settings
    mpu.enableGyrDLPF();
    mpu.setGyrDLPF(MPU9250_DLPF_1); // Level 6 limit is 5Hz with 33ms delay. Level 0 is 250Hz with 1ms delay.

    delay(5000);
}



void loop(){
    /*
    Read sensor data
    */
    // Barometer
    float bmptemp = bmp.getTemperature();
    float press = bmp.getPressure();
    float alti = bmp.calAltitude(press); // calculate altitude AGL
    // IMU
    xyzFloat gValue = mpu.getGValues();
    xyzFloat gyr = mpu.getGyrValues();
    xyzFloat magValue = mpu.getMagValues();
    float mputemp = mpu.getTemperature();
    float resultantG = mpu.getResultantG(gValue);



    /*
    Do Maths
    */
    float rollingAltAverage = baroSmooth(alti);
    if(rollingAltAverage > maxAltThusFar){
        maxAltThusFar = rollingAltAverage;
    }


    /*
    Finite State Machine
    */
    switch (systemState){
        case 0: // Pre Launch
            if(rollingAltAverage > groundAlt + 20.0f){
                // if we are above 20m AGL, we have launched.
                systemState = 2;
            }
            break;
        case 2: // Post Launch
            if(rollingAltAverage < maxAltThusFar - 20.0f){
                // if we are 20m below our max recorded alt, we have reached apogee.
                systemState = 6;
                // yeetChutes(){}
            }
            break;
        case 6: // Post Apogee
            /* code */
            break;
        case 9: // Landed
            /* code */
            break;
        default:
            // something fucked up somewhere
            break;
    }



    /*
    Save Data to SPI Flash
    */


    /*
    Print out over serial to computer
    */
    Serial.println();
    Serial.println("================");
    Serial.print("Baro Temp (C):   "); Serial.println(bmptemp);
    Serial.print("Pres (Pa):       "); Serial.println(press);
    Serial.print("Alt (AGL meter): "); Serial.println(alti);
    Serial.print("Alt (rolling):   "); Serial.println(rollingAltAverage);

    Serial.print("Accel (g) x, y, z: "); Serial.print(gValue.x); Serial.print("  "); Serial.print(gValue.y); Serial.print("  "); Serial.print(gValue.z);
    Serial.println();
    Serial.print("Resultant g:       "); Serial.println(resultantG);
    Serial.print("Gyro (deg/s) x, y, z: "); Serial.print(gyr.x); Serial.print("  "); Serial.print(gyr.y); Serial.print("  "); Serial.print(gyr.z);
    Serial.println();
    Serial.print("MPU Temp (C): "); Serial.println(mputemp);

    Serial.println("================");


    delay(200);
}