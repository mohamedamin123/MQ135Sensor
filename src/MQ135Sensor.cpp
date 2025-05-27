#include "MQ135Sensor.h"

MQ135Sensor::MQ135Sensor(int analogPin, float loadResistance, float vSupply) {
  _analogPin = analogPin;
  _loadResistance = loadResistance;
  _vSupply = vSupply;
  _baseRawValue = 0;
  _r0 = 0;
  _numSamples = 10;
  pinMode(_analogPin, INPUT);
}

// Calibre la valeur base (analogique et Rs) en air propre
void MQ135Sensor::calibrate(int samples, int delayMs) {
  _baseRawValue = readAverage(samples, delayMs);
  _r0 = (_vSupply * _loadResistance / sensorVoltage(_baseRawValue)) - _loadResistance;
}

// Lecture brute moyenne
float MQ135Sensor::readRaw() {
  return readAverage(_numSamples, 20);
}

// Indice simple qualité air = différence brute avec base
float MQ135Sensor::getAirQualityIndex() {
  float current = readRaw();
  return current - _baseRawValue;
}

// Calcul de la tension capteur à partir de la valeur analogique
float MQ135Sensor::sensorVoltage(float analogValue) {
  // ADC 10-bit par défaut (0-1023), adapter si 12 bits (ESP32)
  // Exemple pour ESP32 ADC 12 bits (0-4095)
  float adcMax = 4095.0; // changer à 1023 si Arduino UNO
  return (analogValue / adcMax) * _vSupply;
}

// Calcul Rs : résistance du capteur à l'instant t (en kilo-ohms)
float MQ135Sensor::getRs() {
  float analogVal = readRaw();
  float vSensor = sensorVoltage(analogVal);
  float rs = (_vSupply * _loadResistance / vSensor) - _loadResistance;
  return rs;
}

float MQ135Sensor::getR0() {
  return _r0;
}

// Estimation ppm CO2 selon courbe datasheet (Rs/R0 vs ppm)
// Formule log-log : log10(ppm) = m * log10(Rs/R0) + b
// Valeurs approximatives issues du datasheet MQ135 (à calibrer)
float MQ135Sensor::getPPM_CO2() {
  float rs_ro_ratio = getRs() / _r0;
  float m = -0.42;  // pente approximative
  float b = 1.92;   // intercept approximative
  float log_ppm = m * log10(rs_ro_ratio) + b;
  float ppm = pow(10, log_ppm);
  if (ppm < 0) ppm = 0;
  return ppm;
}

// Estimation ppm NH3 selon courbe datasheet MQ135
float MQ135Sensor::getPPM_NH3() {
  float rs_ro_ratio = getRs() / _r0;
  float m = -0.38;  // pente approximative
  float b = 1.65;   // intercept approximative
  float log_ppm = m * log10(rs_ro_ratio) + b;
  float ppm = pow(10, log_ppm);
  if (ppm < 0) ppm = 0;
  return ppm;
}

// Lecture moyenne (private)
float MQ135Sensor::readAverage(int samples, int delayMs) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(_analogPin);
    delay(delayMs);
  }
  return (float)sum / samples;
}
