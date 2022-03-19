#define DEBUG 1
#define QNT_MOTORS 2
#define ULTRASONIC_TRIGGER_PIN 7
#define ULTRASONIC_ECHO_PIN 6
#define ULTRASONIC_ADJUSTMENT_FACTOR 0.01723
#define CN_MIN_ULTRASONIC_VALUE 20

#define CONST_MAX_ANALOG_VALUE 255
#define CONST_LEFT_SIDE_CHAR 'E'
#define CONST_RIGHT_SIDE_CHAR 'D'
#define CONST_CN_MAX_DISTANCE 50.0
#define CONST_CN_BLOCKED_TIME_IN_MS 1000
#define CONST_CN_WALKING_TIME_IN_MS 1000

/*
	CLASS
*/
class Ultrasonic
{

private:
	byte pin_echo;
	byte pin_trigger;

public:
	Ultrasonic(byte trigger, byte echo)
	{
		this->pin_trigger = trigger;
		this->pin_echo = echo;
	}

	float getCurrentValue()
	{
		pinMode(this->pin_trigger, OUTPUT);
		digitalWrite(this->pin_trigger, LOW);
		delayMicroseconds(2);
		digitalWrite(this->pin_trigger, HIGH);
		delayMicroseconds(10);
		digitalWrite(this->pin_trigger, LOW);
		pinMode(this->pin_echo, INPUT);
		return ULTRASONIC_ADJUSTMENT_FACTOR * pulseIn(this->pin_echo, HIGH);
	}
};

class Motor
{

private:
	char side;
	byte speed;
	byte pin_gnd;
	byte pin_vcc;
	byte pin_speed_control;

public:
	Motor(byte vcc, byte gnd, byte speed_control, char side)
	{
		this->side = side;
		this->pin_gnd = gnd;
		this->pin_vcc = vcc;
		this->pin_speed_control = speed_control;

		this->speed = CONST_MAX_ANALOG_VALUE;

		pinMode(this->pin_gnd, OUTPUT);
		pinMode(this->pin_vcc, OUTPUT);
		pinMode(this->pin_speed_control, OUTPUT);
	}

	byte getPinVcc() { return this->pin_vcc; }

	byte getPinGnd() { return this->pin_gnd; }

	byte getPinSpeedControl() { return this->pin_speed_control; }

	byte getSpeed() { return this->speed; }

	char getSide() { return this->side; }

	void setSpeed(float percentage)
	{
		this->speed = CONST_MAX_ANALOG_VALUE * percentage;
	}
};

class McQueenV4
{

private:
	Motor *motors;
	bool handling;
	int decision;
	unsigned long last_millis_stopped;
	unsigned long last_millis_walking;

	void random_decision_side()
	{
		long r = random(0, 2); // aleatorio entre [0,1], mapeado para [-1,1]
		this->decision = r ? -1 : 1;
	}

	void write(int index, byte vcc, byte gnd)
	{
		if (index < 0 || index >= QNT_MOTORS)
		{
#if DEBUG
			Serial.println("WARNING McQueenV4::write -> invalid index value");
#endif
			return;
		}

		digitalWrite(this->motors[index].getPinVcc(), vcc);
		digitalWrite(this->motors[index].getPinGnd(), gnd);
		analogWrite(this->motors[index].getPinSpeedControl(), this->motors[index].getSpeed());
	}

	void walk(int x_axis_direction, int y_axis_direction)
	{
		byte vcc = y_axis_direction > 0 || y_axis_direction == 0 ? HIGH : LOW;
		byte gnd = !vcc;

		bool change_speed = x_axis_direction != 0;
		char side = x_axis_direction > 0 ? CONST_RIGHT_SIDE_CHAR : CONST_LEFT_SIDE_CHAR;

		for (int i = 0; i < QNT_MOTORS; i++)
		{
			if (change_speed && this->motors[i].getSide() == side)
				this->motors[i].setSpeed(y_axis_direction == 0 ? 0 : 0.6);
			else
				this->motors[i].setSpeed(1);

			this->write(i, vcc, gnd);
		}
	}

	void turnOff()
	{
		for (int i = 0; i < QNT_MOTORS; i++)
			this->write(i, LOW, LOW);
	}

	void turnOn() { this->walk(0, 1); }
	void turnBack() { this->walk(0, -1); }

public:
	McQueenV4(Motor *motors)
	{
		this->handling = false;
		this->motors = motors;
	}

	void drive_manul_mode()
	{ /* TO-DO */
	}

	void drive_automatic_mode(long distance)
	{
		if (!this->handling)
			this->turnOn();

		if (distance <= CONST_CN_MAX_DISTANCE)
		{
			if (!this->handling)
			{
				this->last_millis_stopped = millis();
				this->last_millis_walking = this->last_millis_stopped + CONST_CN_BLOCKED_TIME_IN_MS;
				this->random_decision_side();
				this->turnOff();
			}
			this->handling = true;
		}

		if (this->handling && (millis() - this->last_millis_stopped) >= CONST_CN_BLOCKED_TIME_IN_MS)
		{
			this->walk(this->decision, -1);

			if ((millis() - this->last_millis_walking) >= CONST_CN_WALKING_TIME_IN_MS)
			{
				this->turnOff();

				if ((millis() - this->last_millis_stopped) >= 2 * CONST_CN_BLOCKED_TIME_IN_MS + CONST_CN_WALKING_TIME_IN_MS)
					this->handling = false;
			}
		}
	}
};

/*
	GLOBALS OBJECTS/VARS
*/
Ultrasonic ultrasonic = Ultrasonic(
		ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);
McQueenV4 mc = McQueenV4(new Motor[QNT_MOTORS]{
		Motor(12, 13, 11, CONST_LEFT_SIDE_CHAR),
		Motor(8, 10, 9, CONST_RIGHT_SIDE_CHAR)});

void setup()
{
	Serial.begin(9600);
	randomSeed(analogRead(A0));
}

void loop()
{

	float distance_in_cm = ultrasonic.getCurrentValue();

#if DEBUG
	Serial.print("LOG loop -> distance_in_cm == ");
	Serial.print(distance_in_cm);
	Serial.println("cm");
#endif

	mc.drive_automatic_mode(distance_in_cm);

	delay(50);
}
