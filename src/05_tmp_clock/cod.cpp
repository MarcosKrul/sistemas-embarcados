/*
	LIBS
*/
#include <LiquidCrystal.h>

#define BTN_ALTER_BEHAVIOR 8
#define BTN_ALTER_CLOCK_FORMAT 9
#define LED_FEEDBACK2USER 10

#define SENSOR_TMP_VOLTAGE 5.0
#define SENSOR_TMP_PIN A0
#define SENSOR_TMP_OFFSET 500
#define SENSOR_TMP_SCALE 10

#define LCD_ENABLED 6
#define LCD_RS 7
#define LCD_DB4 5
#define LCD_DB5 4
#define LCD_DB6 3
#define LCD_DB7 2
#define LCD_ROWS 2
#define LCD_COLUMNS 16

#define CONST_8BITS 1024
#define CONST_DELAY_DEBOUNCE 50

#define LENGTH(value) (String(value).length())
#define CELSIUS2FAHRENHEIT(celsius) ((celsius * 9 / 5) + 32)

/*
	LCD INIT
*/
LiquidCrystal lcd(
    LCD_RS,
    LCD_ENABLED,
    LCD_DB4,
    LCD_DB5,
    LCD_DB6,
    LCD_DB7);

/*
	STRUCT
*/
struct datetime_t
{
  byte hour;
  byte minute;
  byte second;
};

struct debounce_t
{
  unsigned long last_time;
  byte state;
  byte last_state;
  byte delay;
  byte pin;
  bool key;
};

/*
	PROTOTYPES
*/
int config_timer(int);
inline byte get_column_to_centralized_display(byte);
void display_tmp_value(float, const char *);
void tmp_control();
void clock_control(bool);
void debounce(struct debounce_t *, int);
void update_date_time(struct datetime_t *);
void print_title(const char *);
void shift_and_print(byte, bool);

/*
	GLOBALS OBJECTS/VARS
*/
int counter = 0;
float temp_celsius = 0;
float temp_fahernheit = 0;

struct debounce_t debounce_btn_behavior = {
    .last_time = 0,
    .state = LOW,
    .last_state = LOW,
    .delay = CONST_DELAY_DEBOUNCE,
    .pin = BTN_ALTER_BEHAVIOR,
    .key = true};

struct debounce_t debounce_btn_clock_format = {
    .last_time = 0,
    .state = LOW,
    .last_state = LOW,
    .delay = CONST_DELAY_DEBOUNCE,
    .pin = BTN_ALTER_CLOCK_FORMAT,
    .key = true};

struct datetime_t clock = {
    .hour = 16,
    .minute = 59,
    .second = 58};

byte custom_degrees_char[] = {
    0b00000110,
    0b00001001,
    0b00001001,
    0b00000110,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
};

void setup()
{
  //Serial.begin(9600);

  counter = config_timer(1);

  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.createChar(0, custom_degrees_char);

  pinMode(SENSOR_TMP_PIN, INPUT);
  pinMode(BTN_ALTER_BEHAVIOR, INPUT);
  pinMode(BTN_ALTER_CLOCK_FORMAT, INPUT);
  pinMode(LED_FEEDBACK2USER, OUTPUT);
}

void loop()
{
  debounce(&debounce_btn_behavior, LOW);
  debounce(&debounce_btn_clock_format, LOW);

  if (debounce_btn_behavior.key)
  {
    print_title("Termometro");
    tmp_control();
  }
  else
  {
    print_title("Relogio");
    clock_control(debounce_btn_clock_format.key);
  }

  digitalWrite(LED_FEEDBACK2USER, debounce_btn_behavior.key);

  delay(50);
}

ISR(TIMER1_OVF_vect)
{
  TCNT1 = counter;
  clock.second++;
}

int config_timer(int frequency)
{
  TCCR1A = 0x00;
  TCCR1B = 0x04;
  TIMSK1 |= (1 << TOIE1);
  TCNT1 = 65536 - (16000000 / 256) / frequency;
  return TCNT1;
}

inline byte get_column_to_centralized_display(byte length)
{
  byte value = (LCD_COLUMNS - length);
  return (value % 2 == 0 ? value : value + 1) / 2;
}

void display_tmp_value(float value, const char *title)
{
  lcd.print(value, 1);
  lcd.write((byte)0);
  lcd.print(title);
}

void print_title(const char *title)
{
  lcd.setCursor(get_column_to_centralized_display(LENGTH(title)), 0);
  lcd.print(title);
}

void tmp_control()
{
  temp_celsius = (analogRead(SENSOR_TMP_PIN) * SENSOR_TMP_VOLTAGE / (CONST_8BITS - 1) * 1000 - SENSOR_TMP_OFFSET) / SENSOR_TMP_SCALE;
  temp_fahernheit = CELSIUS2FAHRENHEIT(temp_celsius);

  lcd.setCursor(0, 1);

  // exibir temperatura em celsius
  display_tmp_value(temp_celsius, "C ");

  // exibir temperatura em fahernheit
  display_tmp_value(temp_fahernheit, "F ");
}

void clock_control(bool patternAmPm)
{
  update_date_time(&clock);

  lcd.setCursor(get_column_to_centralized_display(LENGTH("hh:mm:ss") + (patternAmPm ? 2 : 0)), 1);

  shift_and_print(patternAmPm ? ((clock.hour + 11) % 12 + 1) : clock.hour, true);
  shift_and_print(clock.minute, true);
  shift_and_print(clock.second, false);

  if (patternAmPm)
    lcd.print(clock.hour >= 12 ? "pm" : "am");
}

void shift_and_print(byte value, bool colon)
{
  lcd.print(value < 10 ? "0" : "");
  lcd.print(value);
  if (colon)
    lcd.print(":");
}

void debounce(struct debounce_t *d, int pattern)
{
  int value = digitalRead(d->pin);

  if (value != d->last_state)
    d->last_time = millis();

  if ((millis() - d->last_state) > d->delay && value != d->state)
  {
    d->state = value;
    if (d->state == pattern)
    {
      d->key = !d->key;
      lcd.clear();
    }
  }

  d->last_state = value;
}

void update_date_time(struct datetime_t *date)
{
  if (date->second >= 60)
  {
    date->second = 0;
    date->minute = date->minute + 1;
  }

  if (date->minute >= 60)
  {
    date->minute = 0;
    date->hour = date->hour + 1;
  }

  if (date->hour == 24)
    date->hour = 0;
}
