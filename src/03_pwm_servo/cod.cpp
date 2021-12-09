/*
	LIBS
*/
#include <Servo.h>

#define STEP_S0 1
#define STEP_S1 2
#define STEP_S2 5
#define STEP_S3 10

#define OUT_SERVO_S0 5
#define OUT_SERVO_S1 6
#define OUT_SERVO_S2 9
#define OUT_SERVO_S3 10

#define CONST_MIN_ANGLE 0
#define CONST_MAX_ANGLE 180

/*
	CLASSES
*/
class ServoMotor
{

private:
	byte step;
	byte angle;
	Servo servo;

	void Invert()
	{
		this->step = (-1) * this->step;
	}

public:
	ServoMotor(byte step)
	{
		this->angle = 0;
		this->step = step;
	}

	void Setup(int pin)
	{
		this->servo.attach(pin);
		this->servo.write(this->angle);
	}

	void Control()
	{
		this->servo.write(this->angle);
		this->angle += this->step;

		if (this->angle >= CONST_MAX_ANGLE || this->angle <= CONST_MIN_ANGLE)
			this->Invert();
	}
};

/*
	GLOBALS OBJECTS/VARS
*/
ServoMotor servo0(STEP_S0);
ServoMotor servo1(STEP_S1);
ServoMotor servo2(STEP_S2);
ServoMotor servo3(STEP_S3);

long last_millis = millis();

void setup()
{
	Serial.begin(9600);

	pinMode(OUT_SERVO_S0, OUTPUT);
	pinMode(OUT_SERVO_S1, OUTPUT);
	pinMode(OUT_SERVO_S2, OUTPUT);
	pinMode(OUT_SERVO_S3, OUTPUT);

	servo0.Setup(OUT_SERVO_S0);
	servo1.Setup(OUT_SERVO_S1);
	servo2.Setup(OUT_SERVO_S2);
	servo3.Setup(OUT_SERVO_S3);
}

void loop()
{
	if ((millis() - last_millis) > 60)
	{
		servo0.Control();
		servo1.Control();
		servo2.Control();
		servo3.Control();

		last_millis = millis();
	}
}