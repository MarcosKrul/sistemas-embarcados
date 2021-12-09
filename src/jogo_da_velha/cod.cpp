
/*===Primeiro CI 74HC595===*/
#define CLK_1 4
#define LATCH_1 3
#define DATA_1 2
/*=========================*/

/*===Segundo CI 74HC595===*/
#define CLK_2 7
#define LATCH_2 6
#define DATA_2 5
/*=========================*/

/*==Último LED RGB==*/
#define L_8_B A0
#define L_8_G A1
/*==================*/

/*===Entrada dos botões===*/
#define IN_0 12
#define IN_1 13
#define IN_2 A2
#define IN_3 A3
#define IN_4 A4
#define IN_5 A5
#define IN_6 8
#define IN_7 9
#define IN_8 10
#define RESET 11
/*=========================*/

/*========Protótipo das funções========*/
byte rowColumnToValue(byte, byte);
bool ganhou_linha(byte, byte);
bool ganhou_coluna(byte, byte);
bool ganhou_dig_principal(byte);
bool ganhou_dig_secundaria(byte);
void reset_leds();
void piscar_reset();
void ligar_led(byte);
void verificar_ganhador();
void carregarLedsLigados();
void piscar(byte, byte, byte, byte);
void aux_piscar(byte, byte, byte, byte);
void valueToRowColumn(byte, byte *, byte *);
/*=====================================*/

/*========Variáveis e constantes========*/
int valueIn;
bool player1;
bool ganhou;
byte value1 = 0, value2 = 0;
byte matriz[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
const byte blue[4] = {0b10000000, 0b00100000, 0b00001000, 0b00000010};
const byte green[4] = {0b01000000, 0b00010000, 0b00000100, 0b00000001};
/*======================================*/

void setup()
{
  player1 = true;
  ganhou = false;
  Serial.begin(9600);
  pinMode(IN_0, INPUT);
  pinMode(IN_1, INPUT);
  pinMode(IN_2, INPUT);
  pinMode(IN_3, INPUT);
  pinMode(IN_4, INPUT);
  pinMode(IN_5, INPUT);
  pinMode(IN_6, INPUT);
  pinMode(IN_7, INPUT);
  pinMode(IN_8, INPUT);
  pinMode(RESET, INPUT);
  pinMode(L_8_B, OUTPUT);
  pinMode(L_8_G, OUTPUT);
  pinMode(CLK_1, OUTPUT);
  pinMode(CLK_2, OUTPUT);
  pinMode(LATCH_1, OUTPUT);
  pinMode(LATCH_2, OUTPUT);
  pinMode(DATA_1, OUTPUT);
  pinMode(DATA_2, OUTPUT);
}

void loop()
{
  if ((valueIn = digitalRead(RESET)) == HIGH)
  {
    reset_leds();
    delay(500);
  }
  if ((valueIn = digitalRead(IN_0)) == HIGH && !ganhou)
  {
    ligar_led(0);
    delay(500);
  }
  if ((valueIn = digitalRead(IN_1)) == HIGH && !ganhou)
  {
    ligar_led(1);
    delay(500);
  }
  if ((valueIn = digitalRead(IN_2)) == HIGH && !ganhou)
  {
    ligar_led(2);
    delay(500);
  }
  if ((valueIn = digitalRead(IN_3)) == HIGH && !ganhou)
  {
    ligar_led(3);
    delay(500);
  }
  if ((valueIn = digitalRead(IN_4)) == HIGH && !ganhou)
  {
    ligar_led(4);
    delay(500);
  }
  if ((valueIn = digitalRead(IN_5)) == HIGH && !ganhou)
  {
    ligar_led(5);
    delay(500);
  }
  if ((valueIn = digitalRead(IN_6)) == HIGH && !ganhou)
  {
    ligar_led(6);
    delay(500);
  }
  if ((valueIn = digitalRead(IN_7)) == HIGH && !ganhou)
  {
    ligar_led(7);
    delay(500);
  }
  if ((valueIn = digitalRead(IN_8)) == HIGH && !ganhou)
  {
    ligar_led(8);
    delay(500);
  }
  delay(100);
}

byte rowColumnToValue(byte linha, byte coluna)
{
  byte soma = 0;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (i == linha && j == coluna)
        return soma;
      else
        soma++;
}

void valueToRowColumn(byte value, byte *linha, byte *coluna)
{
  switch (value)
  {
  case 0:
    *linha = 0;
    *coluna = 0;
    break;
  case 1:
    *linha = 0;
    *coluna = 1;
    break;
  case 2:
    *linha = 0;
    *coluna = 2;
    break;
  case 3:
    *linha = 1;
    *coluna = 0;
    break;
  case 4:
    *linha = 1;
    *coluna = 1;
    break;
  case 5:
    *linha = 1;
    *coluna = 2;
    break;
  case 6:
    *linha = 2;
    *coluna = 0;
    break;
  case 7:
    *linha = 2;
    *coluna = 1;
    break;
  case 8:
    *linha = 2;
    *coluna = 2;
    break;
  }
}

void carregarLedsLigados()
{
  /* carrega os dados para o primeiro ci 74hc595 */
  for (int j = 0; j < 3; j++)
  {
    if (player1 && matriz[0][j] == 1)
      value1 = value1 | blue[j];
    else if (!player1 && matriz[0][j] == 2)
      value1 = value1 | green[j];
  }
  if (player1 && matriz[1][0] == 1)
    value1 = value1 | blue[3];
  else if (!player1 && matriz[1][0] == 2)
    value1 = value1 | green[3];
  /* ------------------------------------------- */
  /* carrega os dados para o segundo ci 74hc595  */
  for (int j = 1; j < 3; j++)
  {
    if (player1 && matriz[1][j] == 1)
      value2 = value2 | blue[j - 1];
    else if (!player1 && matriz[1][j] == 2)
      value2 = value2 | green[j - 1];
  }
  for (int j = 0; j < 2; j++)
  {
    if (player1 && matriz[2][j] == 1)
      value2 = value2 | blue[j + 2];
    else if (!player1 && matriz[2][j] == 2)
      value2 = value2 | green[j + 2];
  }
  /* ------------------------------------------- */
}

void ligar_led(byte led)
{
  byte linha, coluna;
  valueToRowColumn(led, &linha, &coluna);
  if (matriz[linha][coluna] != 0)
    return;
  if (player1)
    matriz[linha][coluna] = 1;
  else
    matriz[linha][coluna] = 2;
  if (led == 8)
  {
    if (player1)
      analogWrite(L_8_B, 255);
    else
      analogWrite(L_8_G, 255);
    goto EXIT;
  }
  carregarLedsLigados();
  delay(100);
  shiftOut(DATA_1, CLK_1, LSBFIRST, value1);
  digitalWrite(LATCH_1, LOW);
  digitalWrite(LATCH_1, HIGH);
  digitalWrite(LATCH_1, LOW);
  shiftOut(DATA_2, CLK_2, LSBFIRST, value2);
  digitalWrite(LATCH_2, LOW);
  digitalWrite(LATCH_2, HIGH);
  digitalWrite(LATCH_2, LOW);
EXIT:
  verificar_ganhador();
  player1 = !player1;
}

void reset_leds()
{
  memset(matriz, 0, 9);
  player1 = true;
  ganhou = false;
  value1 = 0b00000000;
  value2 = 0b00000000;
  piscar_reset();
  piscar_reset();
  shiftOut(DATA_1, CLK_1, LSBFIRST, value1);
  digitalWrite(LATCH_1, LOW);
  digitalWrite(LATCH_1, HIGH);
  digitalWrite(LATCH_1, LOW);
  shiftOut(DATA_2, CLK_2, LSBFIRST, value2);
  digitalWrite(LATCH_2, LOW);
  digitalWrite(LATCH_2, HIGH);
  digitalWrite(LATCH_2, LOW);
  analogWrite(L_8_B, 0);
  analogWrite(L_8_G, 0);
}

void piscar(byte ci1, byte ci2, byte blue, byte green)
{
  aux_piscar(0b00000000, 0b00000000, 0, 0);
  delay(200);
  aux_piscar(ci1, ci2, blue, green);
  delay(200);
  aux_piscar(0b00000000, 0b00000000, 0, 0);
  delay(200);
  aux_piscar(ci1, ci2, blue, green);
  delay(200);
}

void aux_piscar(byte ci1, byte ci2, byte blue, byte green)
{
  shiftOut(DATA_1, CLK_1, LSBFIRST, ci1);
  digitalWrite(LATCH_1, LOW);
  digitalWrite(LATCH_1, HIGH);
  digitalWrite(LATCH_1, LOW);
  shiftOut(DATA_2, CLK_2, LSBFIRST, ci2);
  digitalWrite(LATCH_2, LOW);
  digitalWrite(LATCH_2, HIGH);
  digitalWrite(LATCH_2, LOW);
  analogWrite(L_8_B, blue);
  analogWrite(L_8_G, green);
}

void piscar_reset()
{
  aux_piscar(0b01010101, 0b01010101, 0, 255);
  delay(200);
  aux_piscar(0b10101010, 0b10101010, 255, 0);
  delay(200);
  aux_piscar(0b01010101, 0b01010101, 0, 255);
  delay(200);
}

bool ganhou_linha(byte linha, byte player)
{
  byte soma = 0;
  for (int i = 0; i < 3; i++)
    if (matriz[linha][i] == player)
      soma++;
  return (soma == 3);
}

bool ganhou_coluna(byte coluna, byte player)
{
  byte soma = 0;
  for (int i = 0; i < 3; i++)
    if (matriz[i][coluna] == player)
      soma++;
  return (soma == 3);
}

bool ganhou_dig_principal(byte player)
{
  byte soma = 0;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (i == j && matriz[i][j] == player)
        soma++;
  return (soma == 3);
}

bool ganhou_dig_secundaria(byte player)
{
  byte soma = 0;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if ((i + j) == 2 && matriz[i][j] == player)
        soma++;
  return (soma == 3);
}

void verificar_ganhador()
{
  byte aux1, aux2;
  byte led_8_b = matriz[2][2] == 1 ? 255 : 0;
  byte led_8_g = matriz[2][2] == 2 ? 255 : 0;
  if (ganhou_linha(0, 1))
  {
    ganhou = true;
    aux1 = 0b00000011 & value1;
    aux1 = aux1 | 0b10101000;
    piscar(aux1, value2, led_8_b, led_8_g);
  }
  if (ganhou_linha(0, 2))
  {
    ganhou = true;
    aux1 = 0b00000011 & value1;
    aux1 = aux1 | 0b01010100;
    piscar(aux1, value2, led_8_b, led_8_g);
  }
  if (ganhou_linha(1, 1))
  {
    ganhou = true;
    aux1 = 0b11111100 & value1;
    aux1 = aux1 | 0b00000010;
    aux2 = 0b00001111 & value2;
    aux2 = aux2 | 0b10100000;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_linha(1, 2))
  {
    ganhou = true;
    aux1 = 0b11111100 & value1;
    aux2 = 0b00001111 & value2;
    aux1 = aux1 | 0b00000001;
    aux2 = aux2 | 0b01010000;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_linha(2, 1))
  {
    ganhou = true;
    aux1 = 0b11110000 & value2;
    aux1 = aux1 | 0b00001010;
    piscar(value1, value2, led_8_b, led_8_g);
  }
  if (ganhou_linha(2, 2))
  {
    ganhou = true;
    aux1 = 0b11110000 & value2;
    aux1 = aux1 | 0b00000101;
    piscar(value1, value2, led_8_b, led_8_g);
  }
  if (ganhou_coluna(0, 1))
  {
    ganhou = true;
    aux1 = 0b00111100 & value1;
    aux2 = 0b11110011 & value2;
    aux1 = aux1 | 0b10000010;
    aux2 = aux2 | 0b00001000;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_coluna(0, 2))
  {
    ganhou = true;
    aux1 = 0b00111100 & value1;
    aux2 = 0b11110011 & value2;
    aux1 = aux1 | 0b01000001;
    aux2 = aux2 | 0b00000100;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_coluna(1, 1))
  {
    ganhou = true;
    aux1 = 0b11001111 & value1;
    aux2 = 0b00111100 & value2;
    aux1 = aux1 | 0b00100000;
    aux2 = aux2 | 0b10000010;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_coluna(1, 2))
  {
    ganhou = true;
    aux1 = 0b11001111 & value1;
    aux2 = 0b00111100 & value2;
    aux1 = aux1 | 0b00010000;
    aux2 = aux2 | 0b01000001;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_coluna(2, 1))
  {
    ganhou = true;
    aux1 = 0b11110011 & value1;
    aux2 = 0b11001111 & value2;
    aux1 = aux1 | 0b00001000;
    aux2 = aux2 | 0b00100000;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_coluna(2, 2))
  {
    ganhou = true;
    aux1 = 0b11110011 & value1;
    aux2 = 0b11001111 & value2;
    aux1 = aux1 | 0b00000100;
    aux2 = aux2 | 0b00010000;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_dig_principal(1))
  {
    ganhou = true;
    aux1 = 0b00111111 & value1;
    aux2 = 0b00111111 & value2;
    aux1 = aux1 | 0b10000000;
    aux2 = aux2 | 0b10000000;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_dig_principal(2))
  {
    ganhou = true;
    aux1 = 0b00111111 & value1;
    aux2 = 0b00111111 & value2;
    aux1 = aux1 | 0b01000000;
    aux2 = aux2 | 0b01000000;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_dig_secundaria(1))
  {
    ganhou = true;
    aux1 = 0b11110011 & value1;
    aux2 = 0b00110011 & value2;
    aux1 = aux1 | 0b00001000;
    aux2 = aux2 | 0b10001000;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
  if (ganhou_dig_secundaria(2))
  {
    ganhou = true;
    aux1 = 0b11110011 & value1;
    aux2 = 0b00110011 & value2;
    aux1 = aux1 | 0b00000100;
    aux2 = aux2 | 0b01000100;
    piscar(aux1, aux2, led_8_b, led_8_g);
  }
}
