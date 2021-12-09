/*
	LIBS
*/
#include <Servo.h>

/*
	DEFINITIONS
*/
#define DEBUG 0

#define IN_PB_INCR 8
#define IN_PB_DECR 7
#define OUT_LEDRGB_RED 4
#define OUT_LEDRGB_GREEN 2
#define OUT_LEDRGB_PWM_BLUE 3

#define IN_ANALOG_POT A0
#define OUT_PWM_LED_RED 6

#define CONST_MAX_10B 1023
#define CONST_MAX_8B 255
#define CONST_MAX_ANGLE_SERVO 180

#define MICRO_SERVO 5

#define STEP 1

/*
	GLOBAL
*/
int led_out_value = 0;

int angle = 0;
Servo microservo;

int analog_pot;

/*
    PROTOTYPES
*/
void limit_control(int, int);
void servo_control(bool);

void setup()
{
  Serial.begin(9600);

  microservo.attach(MICRO_SERVO);
  microservo.write(led_out_value);

  pinMode(IN_PB_INCR, INPUT);
  pinMode(IN_PB_DECR, INPUT);
  pinMode(OUT_LEDRGB_RED, OUTPUT);
  pinMode(OUT_LEDRGB_GREEN, OUTPUT);
  pinMode(OUT_LEDRGB_PWM_BLUE, OUTPUT);
  pinMode(IN_ANALOG_POT, INPUT);
  pinMode(MICRO_SERVO, OUTPUT);
  pinMode(OUT_PWM_LED_RED, OUTPUT);
}

void loop()
{
  // controle do botao de incremento
  if (digitalRead(IN_PB_INCR) == HIGH)
  {
    if (led_out_value < CONST_MAX_8B)
      servo_control(true);
    else
      limit_control(OUT_LEDRGB_GREEN, 1000);
  }

  // controle do botao de decremento
  if (digitalRead(IN_PB_DECR) == HIGH)
  {
    if (led_out_value > 0)
      servo_control(false);
    else
      limit_control(OUT_LEDRGB_RED, 500);
  }

  // led de controle
  analogWrite(OUT_LEDRGB_PWM_BLUE, led_out_value);

  // potenciometro
  analog_pot = map(
    analogRead(IN_ANALOG_POT),
    0, CONST_MAX_10B,
    0, CONST_MAX_8B
  );
  analogWrite(OUT_PWM_LED_RED, analog_pot);

#if DEBUG
  Serial.print("Valor - ");
  Serial.print(led_out_value);
  Serial.print(" - angulo - ");
  Serial.println(angle);
#endif

  delay(10);
}

void limit_control(int led, int interval)
{
  analogWrite(OUT_LEDRGB_PWM_BLUE, 0);
  digitalWrite(led, HIGH);
  delay(interval);
  digitalWrite(led, LOW);
  delay(interval);
}

void servo_control(bool incr)
{
  led_out_value = incr
    ? led_out_value + STEP
    : led_out_value - STEP;

  angle = map(
    led_out_value,
    0, CONST_MAX_8B,
    0, CONST_MAX_ANGLE_SERVO
  );

  microservo.write(angle);
}
