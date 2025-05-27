#include "MQ135Sensor.h"

enum GasType { GAS_CO2, GAS_NH3 };

MQ135Sensor::MQ135Sensor(int analogPin, float loadResistance, float vSupply) {
  _analogPin = analogPin;
  _loadResistance = loadResistance;
  _vSupply = vSupply;
  _baseRawValue = 0;
  _r0 = 0;
  _numSamples = 10;
  pinMode(_analogPin, INPUT);
}

void MQ135Sensor::calibrate(int samples, int delayMs) {
  _baseRawValue = readAverage(samples, delayMs);
  _r0 = (_vSupply * _loadResistance / sensorVoltage(_baseRawValue)) - _loadResistance;
}

float MQ135Sensor::readRaw() {
  return readAverage(_numSamples, 20);
}

float MQ135Sensor::getAirQualityIndex() {
  float current = readRaw();
  return current - _baseRawValue;
}

float MQ135Sensor::sensorVoltage(float analogValue) {
  float adcMax = 4095.0; // Adapter selon ADC (ESP32 ou Arduino UNO)
  return (analogValue / adcMax) * _vSupply;
}

float MQ135Sensor::getRs() {
  float analogVal = readRaw();
  float vSensor = sensorVoltage(analogVal);
  float rs = (_vSupply * _loadResistance / vSensor) - _loadResistance;
  return rs;
}

float MQ135Sensor::getR0() {
  return _r0;
}

float MQ135Sensor::getPPM_CO2() {
  float rs_ro_ratio = getRs() / _r0;
  float m = -0.42;
  float b = 1.92;
  float log_ppm = m * log10(rs_ro_ratio) + b;
  float ppm = pow(10, log_ppm);
  return (ppm < 0) ? 0 : ppm;
}

float MQ135Sensor::getPPM_NH3() {
  float rs_ro_ratio = getRs() / _r0;
  float m = -0.38;
  float b = 1.65;
  float log_ppm = m * log10(rs_ro_ratio) + b;
  float ppm = pow(10, log_ppm);
  return (ppm < 0) ? 0 : ppm;
}

float MQ135Sensor::readAverage(int samples, int delayMs) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(_analogPin);
    delay(delayMs);
  }
  return (float)sum / samples;
}

// --- Nouvelle méthode : retourne la concentration selon le gaz demandé ---
float MQ135Sensor::getGasConcentration(GasType gas) {
  switch (gas) {
    case GAS_CO2:
      return getPPM_CO2();
    case GAS_NH3:
      return getPPM_NH3();
    default:
      return 0;
  }
}

// --- Nouvelle méthode : détecte si concentration dépasse seuil, affiche message ---
bool MQ135Sensor::detectGas(GasType gas, float threshold) {
  float concentration = getGasConcentration(gas);
  if (concentration >= threshold) {
    switch (gas) {
      case GAS_CO2:
        Serial.print("Attention : concentration de CO2 élevée détectée : ");
        Serial.print(concentration);
        Serial.println(" ppm");
        break;
      case GAS_NH3:
        Serial.print("Attention : concentration de NH3 élevée détectée : ");
        Serial.print(concentration);
        Serial.println(" ppm");
        break;
      default:
        Serial.println("Gaz inconnu détecté");
    }
    return true;
  }
  return false;
}
