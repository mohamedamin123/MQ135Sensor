#ifndef MQ135SENSOR_H
#define MQ135SENSOR_H

#include "Arduino.h"

class MQ135Sensor {
  public:
    MQ135Sensor(int analogPin, float loadResistance = 10.0, float vSupply = 5.0);
    void calibrate(int samples = 50, int delayMs = 100);
    float readRaw();
    float getAirQualityIndex();

    // Nouvelles méthodes pour estimation Rs et ppm gaz
    float getRs();             // Résistance instantanée du capteur
    float getR0();             // Résistance base (air propre)
    float getPPM_CO2();        // Estimation CO2 en ppm
    float getPPM_NH3();        // Estimation NH3 en ppm

  private:
    int _analogPin;
    float _baseRawValue;
    float _loadResistance;    // Résistance de charge (en kilo-ohms), typiquement 10k
    float _vSupply;           // Tension alimentation (ex : 5V)
    float _r0;                // Résistance base calibrée (air propre)
    int _numSamples;

    float readAverage(int samples, int delayMs);
    float sensorVoltage(float analogValue);
};

#endif
