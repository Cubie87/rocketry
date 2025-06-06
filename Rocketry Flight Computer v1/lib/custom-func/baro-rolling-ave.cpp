//Parameters
const int numReadings = 24;
float readings [numReadings];
int averageArrayIndex  = 0;
float total  = 0;

float baroSmooth(float baroPressure){ /* function smooth */
    ////Perform average on sensor readings
    float average;
    // subtract the oldest reading
    total = total - readings[averageArrayIndex];
    // read the sensor
    readings[averageArrayIndex] = baroPressure; // replace with barometric pressure -> bmp.getTemperatureAndPressure(Temp,Pres);
    // add newest value to total
    total = total + readings[averageArrayIndex];
    // advance index pointer
    averageArrayIndex = averageArrayIndex + 1;
    if (averageArrayIndex >= numReadings) {
      averageArrayIndex = 0;
    }
    // calculate average
    average = total / numReadings;
    // return average
    return average;
}