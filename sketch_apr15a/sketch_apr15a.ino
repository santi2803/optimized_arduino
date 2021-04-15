#include <AT24CX.h>

#include <Wire.h>

#include <RTClib.h>

#include <LiquidCrystal.h>

#include <Key.h>
#include <Keypad.h>

RTC_DS1307 rtc;
AT24C32 EepromRTC;

/*===========================
Conection LCD to Arduino
=============================*/

/*
  LCD RS to PIN 22
  LCD E to PIN 23
  LCD D4 to PIN 24
  LCD D5 to PIN 25
  LCD D6 to PIN 26
  LCD D7 to PIN 27
*/
LiquidCrystal lcd(10, 11, 12, 13, 14, 15);

/*
  LCD RS to PIN A0
  LCD E to PIN A1
  LCD D4 to PIN A2
  LCD D5 to PIN A3
  LCD D6 to PIN 12
  LCD D7 to PIN 13
  LiquidCrystal lcd(A0, A1, A2, A3, 12, 13);
*/

/*==========================
 * Configuration keyboard
============================*/

/*
*/
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},  
};
/*
  Filas y columnas a las que van conectado el teclado
*/
byte rowPins[rows] = {9,8,7,6};
byte colPins[cols] = {5,4,3,2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

/*=============================
 * Global Variables
====================================*/
int digito = 0, opc;
char codigo[4], password[] = "2020";
bool passD = false, optMenuDisenador = false;
const char* nombre[3] = {"Santiago Ruiz", "Miguel Salamanca", "Juan Avellaneda"};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(__DATE__, __TIME__));
  
  lcd.begin(16, 2);
  
  bienvenida();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

/*==================================================
 * RELOJ
====================================================*/
void showDateTime() {
  DateTime now = rtc.now();
  Serial.print(now.year());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Date: ");
  lcd.print(now.day());
  lcd.print('/');
  lcd.print(now.month());
  lcd.print('/');
  lcd.print(now.year());
    
  lcd.setCursor(0,1);
  lcd.print("Hour: ");
  lcd.print(now.hour());
  lcd.print(':');
  lcd.print(now.minute());
  lcd.print(':');
  lcd.print(now.second());
  delay(1000);
}

/*==================================================
 * FUNCION TECLADO PARA OBTENER CONTRASEÑA DISEÑADOR
====================================================*/

void passw() {
  char tecla = keypad.getKey();
  if (tecla != NO_KEY) {
    codigo[digito] = tecla;
    lcd.setCursor(digito+5, 2);
    lcd.print('*');
    digito++;
    if (digito == 4) {
      passD = false;
      if (codigo[0] == password[0] && codigo[1] == password[1] && codigo[2] == password[2] && codigo[3] == password[3]) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Correct!");
        delay(1000);
        menuDisenador();
      } else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Incorrect!");
        delay(1000);
        getPassword();
      }
    }
  }
}
/*==================================================
 * FUNCION TECLADO PARA OBTENER OPCION MENU DISEÑADOR
====================================================*/
void menuDisenadorOpt() {
  char tecla = keypad.getKey();
  if (tecla != NO_KEY) {
    opc = tecla;
    lcd.setCursor(digito + 5,2);
    lcd.print(tecla);
    delay(500);
    if (digito == 0) {
      
    }
  }
}
/*==========================================
 * BIENVENIDA
============================================*/
void bienvenida() {
  lcd.setCursor(0,0);
  lcd.print("Bienvenidos");
  lcd.setCursor(0,1);
  lcd.print("Arquitectura");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  // print the number of seconds since reset:

  lcd.print("Grupo 7");
  lcd.setCursor(0,1);
  lcd.print(nombre[0]);
  delay(2000);
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print(nombre[1]);
  lcd.setCursor(0,1);
  lcd.print(nombre[2]);
  delay(2000);
  getPassword();
}
/*==========================================
 * FUNCION PARA OBTENER CONTRASEÑA DISEÑADOR
============================================*/
void getPassword() {
  digito = 0;
  codigo[0]="";
  codigo[1]="";
  codigo[2]="";
  codigo[3]="";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ingrese pin");
  passD = true;
  while(passD) {
    passw();
  }
}
/*==========================================
 * MOSTRAR MENU DEL DISEÑADOR
============================================*/
void menuDisenador() {
  for (int i = 0; i<=11; i++) {
    showDateTime();
  }
  delay(10);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Bienvenido al");
  lcd.setCursor(0,1);
  lcd.print("menu disenador");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("1)SA    2)MA");
  lcd.setCursor(0,1);
  lcd.print("3)JA    4)ch h/d");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("5)V RE   6)Salir");
  delay(2000);
}

void getOptionsMenuDisenador() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ingrese opcion");
  optMenuDisenador = true;
  while(optMenuDisenador) {
    
  }
}
