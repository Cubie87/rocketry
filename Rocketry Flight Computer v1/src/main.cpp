// Include Libraries
#include "Wire.h" // I2C Bus
#include "BMP280.h" // Barometer and Temperature Sensor
#include "MPU9250_WE.h" // Inertial Measurement Unit
#include "SD.h" // microSD Card
#include "SPI.h" // SPI Bus for SPI Flash and microSD
#include "SerialFlash.h"

#include "custom-functions.h"



// Definitions for BMP280 Barometer and Temperature Sensor
typedef DFRobot_BMP280_IIC BMP;
BMP bmp(&Wire, BMP::eSdoLow);

// Definitions for MPU9250 Inertial Measurement Unit
MPU9250_WE mpu = MPU9250_WE(0x68); // I2C Address of MPU

// Definitions for SD Card
const int sdChipSelect = 22; // SPI CS Pin for SD Card
File sdLogFile; // Fat32 file variable
String sdLogFileName;

// Definitions for SPI Flash
const int flashChipSelect = 17; // digital pin for flash chip CS pin

// System State Variables
int systemState = 0; // Finite State Machine
float baroGroundAlt = 0.0; // Barometric Ground Altitude
float maxAltThusFar = 0.0; // Rolling Max Alt




void setup(){
    Serial.begin(115200); // Start Serial monitoring over USB to host computer

    delay(500);
    Wire.begin(); // Start I2C Bus
    Wire.setClock(400000); // Set I2C to Fast (400kHz)



    // Setup for BMP280
    bmp.reset();
    while(bmp.begin() != BMP::eStatusOK){
        Serial.println("BMP init faild");
        delay(2000);
    }
    Serial.println("BMP init success");
    baroGroundAlt = bmp.calAltitude(bmp.getPressure()); // Set "Ground" Barometric Altitude
    maxAltThusFar = baroGroundAlt; // Ensure that negative barometric altitudes cannot cause errors.
    


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


    
    // Setup for SD Card
    while(!SD.begin(sdChipSelect)){ // initialise SD card
        Serial.println("SD Card init failed");
        delay(2000);
    }
    Serial.println("SD Card init success");
    // determine a suitable sd card filename.
    // the oldest file is 0, the youngest file is the highest number
    int i = 0;
    sdLogFileName = String(String(i, HEX) + ".csv");
    while(SD.exists(sdLogFileName)){ // do not overwrite previous files.
        i++;
        sdLogFileName = String(String(i, HEX) + ".csv");
    }


    
    // Setup for SD Card
    while(!SerialFlash.begin(flashChipSelect)){
        Serial.println(F("SPI Flash init failed"));
        delay(2000);
    }
    Serial.println("SPI Flash init success");
    // Move all data from SPI Flash to microSD Card. Halt if erroring.
    SerialFlash.opendir();
    while(1){
        char filename[64];
        uint32_t filesize;
        if(SerialFlash.readdir(filename, sizeof(filename), filesize)){ // if data exists on SPI Flash
            Serial.println("Data found on SPI Flash!");
            Serial.println("Copying...");
            SerialFlashFile serialFile = SerialFlash.open(filename); // Get file from SPI Flash
            sdLogFile = SD.open(sdLogFileName, FILE_WRITE); // make file on SD Card
            
            unsigned long n = filesize;
            char buffer[256];
            while (n > 0) {
                unsigned long rd = n;
                if (rd > sizeof(buffer)) rd = sizeof(buffer);
                sdLogFile.write(serialFile.read(buffer, rd));
                n = n - rd;
            }
            sdLogFile.close(); // Close SD Card
            // Index logfile
            i++;
            sdLogFileName = String(String(i, HEX) + ".csv");
        }
        else {
            break; // no more files
        }
    }


    // Write SD Card Preamble to Log File.
    // This should be moved to SPI Flash...
    sdLogFile = SD.open(sdLogFileName, FILE_WRITE); // touch file
    // write some basic file headers to the file.
    sdLogFile.println("Millis, BMP T(C), Pressure (hPa), MPU T(C), Lx, Ly, Lz, gyrX, gyrY, gyrZ");
    sdLogFile.close(); // remember to close the file and flush changes!

    // serial debugging. Possibility to turn this into telemetry in the future
    Serial.println("Data preamble written.");
    Serial.print("Filename:");
    Serial.println(sdLogFileName);
    
    delay(5000);
}






void loop(){
    /*
    Read sensor data
    */
    // Barometer
    float bmptemp = bmp.getTemperature();
    float baroPressure = bmp.getPressure();
    float aglAltitude = bmp.calAltitude(baroPressure) - baroGroundAlt; // calculate altitude AGL
    // IMU
    xyzFloat imuAccelG = mpu.getGValues();
    xyzFloat gyr = mpu.getGyrValues();
    xyzFloat magValue = mpu.getMagValues();
    float mputemp = mpu.getTemperature();
    float resultantG = mpu.getResultantG(imuAccelG);



    /*
    Do Maths
    */
    // Rolling average for barometric altitude
    float rollingAltAverage = baroSmooth(aglAltitude);
    if(rollingAltAverage > maxAltThusFar){
        maxAltThusFar = rollingAltAverage;
    }


    /*
    Finite State Machine
    */
    switch(systemState){
        case 0: // Pre Launch
            if(rollingAltAverage > baroGroundAlt + 20.0f){
                // if we are above 20m AGL, we have launched.
                systemState = 2;
            }
            break;
        case 2: // Post Launch
            if(rollingAltAverage < maxAltThusFar - 20.0f){
                // if we are 20m below our max recorded alt, we have reached apogee.
                systemState = 6;
                // yeetChutes();
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
            // log error message, send downlink
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
    Serial.print("Pres (Pa):       "); Serial.println(baroPressure);
    Serial.print("Alt (AGL meter): "); Serial.println(aglAltitude);
    Serial.print("Alt (rolling):   "); Serial.println(rollingAltAverage);

    Serial.print("Accel (g) x, y, z: "); Serial.print(imuAccelG.x); Serial.print("  "); Serial.print(imuAccelG.y); Serial.print("  "); Serial.print(imuAccelG.z);
    Serial.println();
    Serial.print("Resultant g:       "); Serial.println(resultantG);
    Serial.print("Gyro (deg/s) x, y, z: "); Serial.print(gyr.x); Serial.print("  "); Serial.print(gyr.y); Serial.print("  "); Serial.print(gyr.z);
    Serial.println();
    Serial.print("MPU Temp (C): "); Serial.println(mputemp);

    Serial.println("================");


    delay(200);
}