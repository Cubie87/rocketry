// Include Libraries
#include "Wire.h" // I2C Bus
#include "BMP280.h" // Barometer and Temperature Sensor
#include "MPU9250_WE.h" // Inertial Measurement Unit

// Definitions for BMP280 Barometer and Temperature Sensor
typedef DFRobot_BMP280_IIC BMP;
BMP bmp(&Wire, BMP::eSdoLow);
//float g_GROUND_ALT;

// Definitions for MPU9250 Inertial Measurement Unit
MPU9250_WE mpu = MPU9250_WE(0x68); // I2c Address of MPU


void setup(){
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
    mpu.setAccDLPF(MPU9250_DLPF_6); // Level 6 limit is 5Hz with 67ms delay. Level 0 is 460Hz with 2ms delay.
    mpu.setGyrRange(MPU9250_GYRO_RANGE_2000); // Gyro Settings
    mpu.enableGyrDLPF();
    mpu.setGyrDLPF(MPU9250_DLPF_6); // Level 6 limit is 5Hz with 33ms delay. Level 0 is 250Hz with 1ms delay.

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
    Print out over serial to computer
    */
    Serial.println();
    Serial.println("================");
    Serial.print("Baro Temp (C):   "); Serial.println(bmptemp);
    Serial.print("Pres (Pa):       "); Serial.println(press);
    Serial.print("Alt (AGL meter): "); Serial.println(alti);

    Serial.print("Accel (g) x, y, z: "); Serial.print(gValue.x); Serial.print("  "); Serial.print(gValue.y); Serial.print("  "); Serial.print(gValue.z);
    Serial.println();
    Serial.print("Resultant g:       "); Serial.println(resultantG);
    Serial.print("Gyro (deg/s) x, y, z: "); Serial.print(gyr.x); Serial.print("  "); Serial.print(gyr.y); Serial.print("  "); Serial.print(gyr.z);
    Serial.println();
    Serial.print("MPU Temp (C): "); Serial.println(mputemp);

    Serial.println("================");

    

    delay(5000);
}