/*
  LIBS
*/
#include <Adafruit_NeoPixel.h>

#define DEBUG 0
#define CONST_RED 0xFF0000
#define CONST_GREEN 0x00FF00
#define CONST_YELLOW 0xFFFF00
#define BUZZER_PIN 5
#define NEOPIXEL_PIN 2
#define NEOPIXEL_QNT_LEDS 6
#define ULTRASONIC_PIN 4
#define ULTRASONIC_ADJUSTMENT_FACTOR 0.01723
#define ULTRASONIC_BUZZER_FREQUENCY 250
#define ULTRASONIC_BUZZER_DURATION 800
#define CN_MAX_ULTRASONIC_VALUE 200
#define CN_MIN_ULTRASONIC_VALUE 30

/*
  CLASS
*/
class NeoPixel
{

private:
  int index;
  Adafruit_NeoPixel pixels;

public:
  NeoPixel(byte pin, byte qnt_leds)
  {
    this->index = -1;
    this->pixels = Adafruit_NeoPixel(
        qnt_leds, pin,
        NEO_GRB + NEO_KHZ800);
  }

  void setup()
  {
    this->pixels.begin();
  }

  int getIndexToBuzzer()
  {
    return this->index == -1
               ? 0
               : (NEOPIXEL_QNT_LEDS - this->index);
  }

  void setIndex(int index)
  {
    this->index = index >= 0 && index <= NEOPIXEL_QNT_LEDS
                      ? index
                      : -1;

#if DEBUG
    Serial.print("LOG NeoPixel.setIndex -> index == ");
    Serial.println(this->index);
#endif
  }

  void setAndShowColor()
  {
    if (this->index == -1)
    {
#if DEBUG
      Serial.print("WARNING NeoPixel.setAndShowColor -> invalid index value");
      Serial.println(this->index);
#endif

      return;
    }

    this->pixels.clear();

    uint32_t color;
    if (this->index <= 1)
      color = CONST_RED;
    else if (this->index <= 3)
      color = CONST_YELLOW;
    else
      color = CONST_GREEN;

    this->pixels.setPixelColor(this->index, color);
    this->pixels.show();
  }
};

/*
  GLOBALS OBJECTS/VARS
*/
bool playing = false;
unsigned long last_millis = millis();
NeoPixel neopixel = NeoPixel(NEOPIXEL_PIN, NEOPIXEL_QNT_LEDS);

/*
  PROTOTYPES
*/
void alert(unsigned int, unsigned long);
long readUltrasonicDistance(byte, byte);

void setup()
{
  Serial.begin(9600);
  neopixel.setup();
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop()
{

  float distance_in_cm = ULTRASONIC_ADJUSTMENT_FACTOR * readUltrasonicDistance(
                                                            ULTRASONIC_PIN, ULTRASONIC_PIN);

#if DEBUG
  Serial.print("LOG loop -> distance_in_cm == ");
  Serial.print(distance_in_cm);
  Serial.println("cm");
#endif

  neopixel.setIndex(map(
      distance_in_cm,
      CN_MIN_ULTRASONIC_VALUE, CN_MAX_ULTRASONIC_VALUE,
      0, NEOPIXEL_QNT_LEDS));

  neopixel.setAndShowColor();

  int index = neopixel.getIndexToBuzzer();
  alert(ULTRASONIC_BUZZER_FREQUENCY * index, ULTRASONIC_BUZZER_DURATION / index);

  delay(100);
}

long readUltrasonicDistance(byte triggerPin, byte echoPin)
{
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

void alert(unsigned int frequency, unsigned long duration)
{
  tone(BUZZER_PIN, frequency, duration);

  if ((millis() - last_millis) > duration * 1.3)
  {
    noTone(BUZZER_PIN);
    last_millis = millis();
  }

  delay(50);
}
