/*!
 * @file readData.ino
 * @brief Download this demo to test simple read from bmp280, connect sensor through IIC interface.
 * @n  Data will print on your serial monitor
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Frank](jiehan.guo@dfrobot.com)
 * @version  V1.0
 * @date  2019-03-12
 * @url https://github.com/DFRobot/DFRobot_BMP280
 */

#include "BMP280.h"
#include "Wire.h"

typedef DFRobot_BMP280_IIC    BMP;    // ******** use abbreviations instead of full names ********

BMP   bmp(&Wire, BMP::eSdoLow);

#define SEA_LEVEL_PRESSURE    1015.0f   // sea level pressure

void setup(){
  Serial.begin(115200);
  bmp.reset();
  Serial.println("bmp read data test");
  while(bmp.begin() != BMP::eStatusOK) {
    Serial.println("bmp begin faild");
    delay(2000);
  }
  Serial.println("bmp begin success");
  delay(1000);
}



void loop(){
  float   temp = bmp.getTemperature();
  uint32_t    press = bmp.getPressure();
  float   alti = bmp.calAltitude(SEA_LEVEL_PRESSURE, press);

  Serial.println();
  Serial.println("================");
  Serial.print("temperature (C): "); Serial.println(temp);
  Serial.print("pressure (Pa):         "); Serial.println(press);
  Serial.print("altitude (AGL meter):      "); Serial.println(alti);
  Serial.println("================");

  delay(100);
}