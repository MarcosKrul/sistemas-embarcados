/*
	LIBS
*/
#include <LiquidCrystal.h>

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

#define CELSIUS2FAHRENHEIT(celsius) ((celsius * 9 / 5) + 32)
#define CONVERT_TMP(value) ((value * SENSOR_TMP_VOLTAGE / (CONST_8BITS - 1) * 1000 - SENSOR_TMP_OFFSET) / SENSOR_TMP_SCALE)

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
	PROTOTYPES
*/
byte calc_length_char(const void *);
void centralized_display(byte, const void *, byte (*)(const void *));

/*
	GLOBALS OBJECTS/VARS
*/
float temp = 0;
long last_millis = millis();

byte custom_degrees_char[8] = {
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
  Serial.begin(9600);

  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.createChar(0, custom_degrees_char);

  //lcd.setCursor(0,0);
  //lcd.print("Temperatura");
  centralized_display(0, "Temperatura", calc_length_char);

  pinMode(SENSOR_TMP_PIN, INPUT);
}

void loop()
{
  if ((millis() - last_millis) > 15)
  {

    temp = CONVERT_TMP(analogRead(SENSOR_TMP_PIN));

    lcd.setCursor(0, 1);

    lcd.print(temp, 1);
    lcd.write((byte)0);
    lcd.print("C ");

    lcd.print(CELSIUS2FAHRENHEIT(temp), 1);
    lcd.write((byte)0);
    lcd.print("F ");

    //char* buffer = (char*) malloc(16*sizeof(char));
    //char buffer[100];
    //snprintf_P(buffer, sizeof(buffer), PSTR("%s %d:%02d"), "teste", 10, 20);
    //Serial.println(buffer);

    last_millis = millis();
  }
}

void centralized_display(byte row, const void *content, byte (*length)(const void *))
{
  byte column = (LCD_COLUMNS - length(content)) / 2;

  lcd.setCursor(column, row);
  lcd.print((char *)content);
}

byte calc_length_char(const void *content)
{
  String str = (const char *)content;
  return str.length();
}

byte cal_length_values()
{
}
