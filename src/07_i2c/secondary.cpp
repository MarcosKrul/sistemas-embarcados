/*
  LIBS
*/
#include <Wire.h>

#define DEBUG 0
#define D_ID 1
#define SENSOR_TMP_PIN A0

/*
  GLOBALS OBJECTS/VARS
*/
int analog_read = 0;

/*
  PROTOTYPES
*/
void handler();

void setup()
{
  Serial.begin(9600);

  pinMode(SENSOR_TMP_PIN, INPUT);

  Wire.begin(D_ID);
  Wire.onRequest(handler);
}

void loop()
{
  delay(100);
}

void handler()
{
  analog_read = analogRead(SENSOR_TMP_PIN);

#if DEBUG
  Serial.print("Secundario: ");
  Serial.println(analog_read);
#endif

  Wire.write((byte *)&analog_read, 2);
}