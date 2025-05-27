#include <MQ135Sensor.h>

MQ135Sensor mq135(34, 10.0, 5.0); // analogPin=34, RL=10k, Vcc=5V

void setup() {
  Serial.begin(115200);
  Serial.println("Calibration du capteur MQ-135...");
  mq135.calibrate();
  Serial.print("R0 (resistance air propre) = ");
  Serial.println(mq135.getR0());
}

void loop() {
  float rs = mq135.getRs();
  float ppmCO2 = mq135.getPPM_CO2();
  float ppmNH3 = mq135.getPPM_NH3();

  Serial.print("Rs = ");
  Serial.print(rs);
  Serial.print(" kOhms, ");

  Serial.print("CO2 estimé = ");
  Serial.print(ppmCO2);
  Serial.print(" ppm, ");

  Serial.print("NH3 estimé = ");
  Serial.print(ppmNH3);
  Serial.println(" ppm");

  delay(3000);
}
