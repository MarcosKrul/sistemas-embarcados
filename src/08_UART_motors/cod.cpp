/*
	LIBS
*/
#include <Keypad.h>

#define DEGUB 0
#define QNT_MOTORS 4
#define MOTORS_IDS "ABCD"

/*
	CLASS
*/
class Motor
{

private:
	char ID_keypad;
	byte pin_gnd;
	byte pin_vcc;

public:
	Motor(byte vcc, byte gnd, char id)
	{
		this->pin_gnd = gnd;
		this->pin_vcc = vcc;
		this->ID_keypad = id;

		pinMode(this->pin_gnd, OUTPUT);
		pinMode(this->pin_vcc, OUTPUT);
	}

	byte getPinVcc()
	{
		return this->pin_vcc;
	}

	byte getPinGnd()
	{
		return this->pin_gnd;
	}
};

class McQueenV4
{
private:
	Motor *motors;
	String motors_ids;
	byte target_motor_index;

	bool hasTargetMotor()
	{
		return (
				this->target_motor_index != -1 &&
				this->target_motor_index >= 0 &&
				this->target_motor_index < QNT_MOTORS);
	}

	void clearTargetMotor()
	{
		this->target_motor_index = -1;
	}

	void setTargetMotor(char id)
	{
		byte aux = this->motors_ids.indexOf(id);

		if (aux < 0 || aux >= QNT_MOTORS)
		{
			Serial.println("Error McQueen.setActivatedMotor: received invalid id value");
			return;
		}

		this->target_motor_index = aux;
	}

	void write(byte vcc, int vcc_state, byte gnd, int gnd_state)
	{
		digitalWrite(vcc, vcc_state);
		digitalWrite(gnd, gnd_state);
	}

	void line(int vcc_state, int gnd_state)
	{
		if (this->hasTargetMotor())
			return this->write(
					this->motors[this->target_motor_index].getPinVcc(), vcc_state,
					this->motors[this->target_motor_index].getPinGnd(), gnd_state);

		for (int i = 0; i < QNT_MOTORS; i++)
			this->write(
					this->motors[i].getPinVcc(), vcc_state,
					this->motors[i].getPinGnd(), gnd_state);
	}

	void corner(byte break_index)
	{
		for (int i = 0; i < QNT_MOTORS; i++)
			this->write(
					this->motors[i].getPinVcc(), i == break_index ? LOW : HIGH,
					this->motors[i].getPinGnd(), LOW);
		this->clearTargetMotor();
	}

	void turnOn() { this->line(HIGH, LOW); }
	void turnOff() { this->line(LOW, LOW); }
	void turnBack() { this->line(LOW, HIGH); }
	void turnRight() { this->corner(3); }
	void turnLeft() { this->corner(2); }

public:
	McQueenV4(Motor *motors)
	{
		this->motors = motors;
		this->target_motor_index = -1;
		this->motors_ids = MOTORS_IDS;
	}

	void drive(char key)
	{
		switch (key)
		{
		case '2':
			return this->turnOn();
		case '8':
			return this->turnBack();
		case '4':
			return this->turnLeft();
		case '6':
			return this->turnRight();
		case '5':
			return this->turnOff();
		case '*':
			return this->clearTargetMotor();
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
			return this->setTargetMotor(key);
		}
	}
};

/*
	GLOBALS OBJECTS/VARS
*/
char key_pressed;
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLUMNS = 4;
byte KEYPAD_ROW_PINS[KEYPAD_ROWS] = {9, 8, 7, 6};
byte KEYPAD_COL_PINS[KEYPAD_COLUMNS] = {5, 4, 3, 2};
const char KEYPAD_KEYS[KEYPAD_ROWS][KEYPAD_COLUMNS] = {
		{'1', '2', '3', 'A'},
		{'4', '5', '6', 'B'},
		{'7', '8', '9', 'C'},
		{'*', '0', '#', 'D'}};
Keypad keypad = Keypad(
		makeKeymap(KEYPAD_KEYS),
		KEYPAD_ROW_PINS, KEYPAD_COL_PINS,
		KEYPAD_ROWS, KEYPAD_COLUMNS);
McQueenV4 mc = McQueenV4(new Motor[QNT_MOTORS]{
		Motor(12, 13, 'A'),
		Motor(11, 10, 'B'),
		Motor(A0, A1, 'C'),
		Motor(A2, A3, 'D')});

/*
	PROTOTYPES
*/
void uart_receive();

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	key_pressed = keypad.getKey();
	uart_receive();

	if (key_pressed != NO_KEY)
	{

#if DEBUG
		Serial.print("key_pressed: ");
		Serial.println(key_pressed);
#endif

		mc.drive(key_pressed);
	}

	delay(50);
}

void uart_receive()
{
	if (Serial.available())
		key_pressed = Serial.read();
}