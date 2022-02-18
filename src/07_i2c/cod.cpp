/*
  LIBS
*/
#include <LiquidCrystal.h>
#include <Wire.h>

#define D_ID_TMP 1

#define SENSOR_TMP_VOLTAGE 5.0
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
  PROTOTYPES
*/
inline byte get_column_to_centralized_display(byte);
void display_tmp_value(float, const char *);

/*
  GLOBALS OBJECTS/VARS
*/
const char *title = "Termometro";
const int num_bytes = sizeof(int);
float temp_celsius = 0;
float temp_fahernheit = 0;
int tmp_sensor_value = 0;
long last_millis = millis();

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
  Serial.begin(9600);

  Wire.begin();

  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.createChar(0, custom_degrees_char);

  lcd.setCursor(get_column_to_centralized_display(LENGTH(title)), 0);
  lcd.print(title);
}

void loop()
{
  if ((millis() - last_millis) > 15)
  {

    Wire.requestFrom(D_ID_TMP, num_bytes);

    for (int i = 0; i < num_bytes && Wire.available(); i++)
    {
      tmp_sensor_value += Wire.read() << (i * 8);
    }

    temp_celsius = (tmp_sensor_value * SENSOR_TMP_VOLTAGE / (CONST_8BITS - 1) * 1000 - SENSOR_TMP_OFFSET) / SENSOR_TMP_SCALE;
    temp_fahernheit = CELSIUS2FAHRENHEIT(temp_celsius);

    lcd.setCursor(0, 1);

    // exibir temperatura em celsius
    display_tmp_value(temp_celsius, "C ");

    // exibir temperatura em fahernheit
    display_tmp_value(temp_fahernheit, "F ");

    tmp_sensor_value = 0;
    last_millis = millis();
  }
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
