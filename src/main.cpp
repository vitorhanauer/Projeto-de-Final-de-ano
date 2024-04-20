#include <Arduino.h>
#include "debounce.h"
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#define sdaRfid 53
#define rst 5

RtcDateTime DT;
ThreeWire myWire(14, 16, 15);
RtcDS1302<ThreeWire> Rtc(myWire);
MFRC522 rfid(sdaRfid, rst);
LiquidCrystal_I2C lcd(0x27, 16, 2);

deb::dadosBotao fechar;
deb debounce;
Teclado teclado;

char num[5] = {"____"};

char datestring[20];
bool sw;
unsigned long marca;
const byte luzes = 36;
const byte ledF = 32, ledS = A0, ledA = 28, fecharPorta = 22, pir = 6,alarme = 24;
byte coluna[4] = {7, 8, 9, A4};
byte linhas[4] = {10, 11, 12, 13};
byte estado = 0,tentativas = 3;
int senha;
unsigned long marca2;
bool tag = false;
bool validacao;
bool a;

int lerTeclado()
{
  char a = teclado.varredura();
  if (a != 0)
  {
    if (isDigit(a))
    {
      num[0] = num[1];
      num[1] = num[2];
      num[2] = num[3];
      num[3] = a;


      lcd.setCursor(0, 1);
      lcd.print("Senha: ");
      lcd.print(num);
      lcd.print("       ");
      Serial.print("Valor: ");
      Serial.println(num);
    }

    if (a == '*')
    {
      num[3] = num[2];
      num[2] = num[1];
      num[1] = num[0];
      num[0] = '_';
      lcd.setCursor(0,1);
      lcd.print("Senha: ");
      lcd.print(num);
      Serial.print("Valor: ");
      Serial.println(num);
    }

    if (a == '#')
    {
      for (byte i = 0; i < 4; i++)
      {
        if (num[i] == '_')
          num[i] = '0';
      }

      int numFinal = atoi(num);
      return numFinal;
    }
  }
  return -1;
}
bool cartao()
{
  digitalWrite(sdaRfid,1);
  if(!rfid.PICC_IsNewCardPresent()){
    
    digitalWrite(sdaRfid,0);
    return false;
  }

  if(!rfid.PICC_ReadCardSerial()){
    
    digitalWrite(sdaRfid,0);
    return false;
  }
 
  Serial.print("UID da tag :");
  String conteudo = "";
  byte letra;
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    conteudo.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mensagem : ");
  conteudo.toUpperCase();

  if (conteudo.substring(1) == "A0 8F B6 20")
  {
    digitalWrite(sdaRfid,0);
    return true;
  }

  if (conteudo.substring(1) == "73 FE 3D B7")
  {
    digitalWrite(sdaRfid,0);
    return true;
  }

  digitalWrite(sdaRfid,0);
  return false;
}

void setup()
{

  Serial.begin(9600);
  Rtc.Begin();
  SPI.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(10);
  teclado.iniTeclado();

  pinMode(alarme, OUTPUT);
  pinMode(luzes, OUTPUT);
  pinMode(pir, INPUT);
  pinMode(ledA, OUTPUT);
  pinMode(ledF, OUTPUT);
  pinMode(ledS, OUTPUT);

  digitalWrite(alarme,0);

  pinMode(sdaRfid,OUTPUT);
  pinMode(rst, OUTPUT);

  digitalWrite(rst,0);
  digitalWrite(sdaRfid,1);

  rfid.PCD_Init();
  delay(15);
  digitalWrite(sdaRfid,0);

  for (byte i; i < 4; i++)
  {
    pinMode(linhas[i], OUTPUT);
    pinMode(coluna[i], INPUT_PULLUP);
  }
  DT = RtcDateTime(__DATE__,__TIME__);
  Serial.println(DT.Hour());
  marca2 = millis();
  sw = true;
}

void loop()
{
  switch (estado)
  {
  case 0:
    if (sw)
    {
      a = false;
      digitalWrite(rst,1);
      delay(15);
      digitalWrite(rst,0);
      delay(15);
      rfid.PCD_Init();
      delay(15);
      digitalWrite(ledF, 1);
      digitalWrite(ledA, 0);
      digitalWrite(ledS, 0);
      lcd.clear();
      lcd.home();
      lcd.print("Digite a senha");
      Serial.println("Digite a senha");
      sw = false;
    }

    if(cartao()){
      Serial.println("DESSOAR");
      digitalWrite(alarme,0);
    }

    if ((digitalRead(coluna[3]) == 1 || tentativas == 0 || digitalRead(pir)) && !a){
      Serial.println("SOAR");
      a = true;
      digitalWrite(alarme, 1);
    }
    senha = lerTeclado();

    if (senha == 6969){
      if(DT.Hour()>8 && DT.Hour()<=23){
        for(byte i = 0; i<4; i++){
        num[i] = '_';
      }
      estado = 1;
      sw = true;
      digitalWrite(alarme,0);

      }else{
        Serial.println("SOAR");
        digitalWrite(alarme,1);
      }
   
    }else if(senha != 6969 && senha !=-1 && tentativas !=0){
      tentativas--;
      lcd.clear();
      lcd.home();
      lcd.print("Senha errada");
      lcd.setCursor(0,1);
      lcd.print("Tentativas: ");
      lcd.print(tentativas);
      }

    break;
    case 1:
    if (sw)
    {
      a = false;
      digitalWrite(rst,1);
      delay(15);
      digitalWrite(rst,0);
      delay(15);
      rfid.PCD_Init();
      delay(15);
      digitalWrite(ledF, 1);
      digitalWrite(ledA, 0);
      digitalWrite(ledS, 0);
      lcd.clear();
      lcd.home();
      lcd.print("proxime o cartao");
      Serial.println("Aproxime o cartÃ£o");
      sw = false;
    }
   
    if ((digitalRead(coluna[3]) == 1 || digitalRead(pir)) && !a){
      Serial.println("SOAR");
      a = true;
      digitalWrite(alarme, 1);
    }

    if (cartao())
    {
      Serial.println("DESSOAR");
      digitalWrite(alarme, 0);
      digitalWrite(ledS, 1);
      estado = 2;
      marca = millis();
      sw = true;
    }
    break;
  case 2:
    if (sw)
    {
      digitalWrite(rst,1);
      delay(15);
      digitalWrite(rst,0);
      delay(15);
      rfid.PCD_Init();
      delay(15);
      digitalWrite(luzes,1);
      lcd.clear();
      lcd.home();
      lcd.print("CofreDestrancado");
      sw = false;
      digitalWrite(ledS, 1);
      digitalWrite(ledA, 1);
      digitalWrite(ledF, 0);
      digitalWrite(alarme, 0);
    }
   
    if ((millis() - marca) > 10000)
    {
      if (digitalRead(coluna[3]) == 0 && digitalRead(ledS) == 1)
      {
        sw = true;
        estado = 0;
        tentativas = 3;
        digitalWrite(luzes,0);
      }
     
      digitalWrite(ledS, 0);
    }
    if (teclado.varredura() == '#')
    {
      digitalWrite(alarme, 0);
      digitalWrite(ledS, 1);
      marca = millis();
    }
    break;
  }
}