/*
	LIBS
*/
#include <LiquidCrystal.h>

/*
	DEFINITIONS
*/
#define DEBUG 0
#define QNT_LEDS 8

#define OUT_LEDS_ENABLED 3
#define IN_LEDS_ENABLED 2
#define IN_ANALOG_POT A0
#define OUT_PWM_LED_RGB 11

#define LCD_ENABLED 10
#define LCD_RS 7
#define LCD_DB4 8
#define LCD_DB5 9
#define LCD_DB6 12
#define LCD_DB7 13
#define LCD_ROWS 2
#define LCD_COLUMNS 16

#define CI_SHT_CLK 6
#define CI_SHT_LATCH 5
#define CI_SHT_DATA 4

/*
	GLOBAL
*/
int disabled;
int analog_value;
int converted_value;

byte title_1_length;
byte title_2_length;
const char *title_1 = "Valor lido: ";
const char *title_2 = "Convertido: ";

const byte LEDS_DEFINITION[] = {
    0b00000000, // index == 0
    0b10000000, // index == 1
    0b11000000, // index == 2
    0b11100000, // index == 3
    0b11110000, // index == 4
    0b11111000, // index == 5
    0b11111100, // index == 6
    0b11111110, // index == 7
    0b11111111  // index == 8
};

/*
    PROTOTYPES
*/
byte length(const char *);
void display(int, int, int);
void set_leds();

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

void setup()
{
  Serial.begin(9600);

  title_1_length = length(title_1);
  title_2_length = length(title_2);

  lcd.begin(LCD_COLUMNS, LCD_ROWS);

  pinMode(OUT_LEDS_ENABLED, OUTPUT);
  pinMode(IN_LEDS_ENABLED, INPUT);
  pinMode(IN_ANALOG_POT, INPUT);
  pinMode(OUT_PWM_LED_RGB, OUTPUT);

  lcd.print(title_1);
  lcd.setCursor(0, 1);
  lcd.print(title_2);
}

void loop()
{
  // leitura do interruptor e definicao do led
  disabled = digitalRead(IN_LEDS_ENABLED);
  digitalWrite(OUT_LEDS_ENABLED, !disabled);

  // leitura e conversao do potenciometro
  analog_value = analogRead(IN_ANALOG_POT);
  converted_value = map(analog_value, 0, 1023, 0, 255);

  // led RGB
  analogWrite(OUT_PWM_LED_RGB, disabled ? 0 : converted_value);

  // set dos leds
  set_leds();

  // display dos valores no lcd
  display(analog_value, 0, title_1_length);
  display(converted_value, 1, title_2_length);

#if DEBUG
  Serial.println(analog_value);
  Serial.println(converted_value);
#endif

  delay(100);
}

byte length(const char *string)
{
  byte l = 0;
  int aux = 0;
  while (string[aux++] != '\0')
    l++;
  return l;
}

void display(int value, int row, int column)
{
  lcd.setCursor(column, row);
  lcd.print(value);
  lcd.print("   ");
}

void set_leds()
{
  int index = disabled
    ? 0
    : ((converted_value + 1) * QNT_LEDS / 255);

  if (index < 0 || index > QNT_LEDS)
    index = 0;

  shiftOut(CI_SHT_DATA, CI_SHT_CLK, LSBFIRST, LEDS_DEFINITION[index]);
  digitalWrite(CI_SHT_LATCH, LOW);
  digitalWrite(CI_SHT_LATCH, HIGH);
  digitalWrite(CI_SHT_LATCH, LOW);

#if DEBUG
  Serial.println(index);
  Serial.println(LEDS_DEFINITION[index], BIN);
#endif
}
