#include <EEPROM.h>

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
  LCD RS to PIN 10
  LCD E to PIN 11
  LCD D4 to PIN 12
  LCD D5 to PIN 13
  LCD D6 to PIN 14
  LCD D7 to PIN 15
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
int digito = 0, opc, uid, posicion, contador, dato, reporte[8], member;
int IN1 = 16;
int IN2 = 17;
int IN3 = 18;
int IN4 = 19;
int steps = 0;
int n_steps = (4 * 1.4222222222);
char codigo[4], password[] = "2021", opcMenuDisenador[] = "1234567", opcMenuMember[] = "123", optsMotor[] = "469";
bool passD = false, optMenuDisenador = false, pwMember = false, optMenuMember = false, newpin = false, sa = false, ja = false, ma = false, pinadmin = false, motor = false;
short pos_libre;
const char* nombre[3] = {"Santiago Ruiz", "Miguel Salamanca", "Juan Avellaneda"};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  //rtc.adjust(DateTime(__DATE__, __TIME__));
  
  lcd.begin(16, 2);
  //char pw_SA[4] = "1234", pw_MA[4] = "4321", pw_JA[4] = "1324";
  //EEPROM.put(0, pw_SA);
  //EEPROM.put(5, pw_MA);
  //EEPROM.put(9, pw_JA);

  // LIMPIAR MEMORIA EXTERNA
  /*
  for (int i = 0; i<280; i++) {
    EepromRTC.write(i,0);
  }
  byte a = EepromRTC.read(1);
  Serial.print("Dato byte: ");Serial.println(a);
  contador = 0;
  posicion = 0;
  */
  // INITIALIZE PIN MOTOR
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
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
    digito++;
    delay(200);
    if (digito == 1) {
      optMenuDisenador = false;
      if (opc == opcMenuDisenador[0]) {
        // Password SA
        getPasswordMember();
        member = 0;
        delay(10);
      }

      if (opc == opcMenuDisenador[1]) {
        // Password MA
        getPasswordMember();
        member = 1; 
        delay(10);
      }

      if (opc == opcMenuDisenador[2]) {
        // Password JA
        getPasswordMember();
        member = 2;
        delay(10);
      }

      if (opc == opcMenuDisenador[3]) {
        // Cambiar fecha
      }

      if (opc == opcMenuDisenador[4]) {
        // ver reportes
        getPasswordAdmin();
        delay(10);
      }

      if (opc == opcMenuDisenador[5]) {
        // Motor
        menuMotor();
      }

      if (opc == opcMenuDisenador[6]) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Bye...");
        delay(1000);
        getPassword();
      }
      
    }
  }
}
/*==========================================
 * PASSWORD MEMBER
============================================*/
void passwMember() {
  char tecla = keypad.getKey();
  if (tecla != NO_KEY) {
    codigo[digito] = tecla;
    lcd.setCursor(digito+5, 2);
    lcd.print('*');
    digito++;
    if (digito == 4) {
      pwMember = false;
      // obtener y verificar password santiago
      if (opc == opcMenuDisenador[0]) {
        char res_pin_SA[4];
        EEPROM.get(0, res_pin_SA);
        if (codigo[0] == res_pin_SA[0] && codigo[1] == res_pin_SA[1] && codigo[2] == res_pin_SA[2] && codigo[3] == res_pin_SA[3]) {
          lcd.clear();
          lcd.print("Correct!");
          delay(1000);
          uid = 29;
          sa = true;
          showMenuMember();
        } else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Incorrect!");
          delay(1000);
          getPasswordMember();
        }
      }
       // obtener y verificar password miguel
      if (opc == opcMenuDisenador[1]) {
        char res_pin_MA[4];
        EEPROM.get(5, res_pin_MA);
        if (codigo[0] == res_pin_MA[0] && codigo[1] == res_pin_MA[1] && codigo[2] == res_pin_MA[2] && codigo[3] == res_pin_MA[3]) {
          lcd.clear();
          lcd.print("Correct!");
          delay(1000);
          uid = 30;
          ma = true;
          showMenuMember();
        } else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Incorrect!");
          delay(1000);
          getPasswordMember();
        }
      }
       // obtener y verificar password juan
      if (opc == opcMenuDisenador[2]) {
        char res_pin_JA[4];
        EEPROM.get(9, res_pin_JA);
        if (codigo[0] == res_pin_JA[0] && codigo[1] == res_pin_JA[1] && codigo[2] == res_pin_JA[2] && codigo[3] == res_pin_JA[3]) {
          lcd.clear();
          lcd.print("Correct!");
          delay(1000);
          uid = 6;
          ja = true;
          showMenuMember();
        } else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Incorrect!");
          delay(1000);
          getPasswordMember();
        }
      }
    }
  }
}
/*==========================================
 * MENU MEMEBR
============================================*/
void menuMemberOpc() {
  char tecla = keypad.getKey();
  if (tecla != NO_KEY) {
    opc = tecla;
    lcd.setCursor(digito+5, 2);
    lcd.print(tecla);
    digito++;
    delay(200);
    if (digito == 1) {
      optMenuMember = false;
      if (opc == opcMenuMember[0]) {
        // Nuevo pin
        getNewPin();
        delay(10);
      }

      if (opc == opcMenuMember[1]) {
        // Crear reporte
        lcd.clear();
        lcd.setCursor(0,0);
        buscar_pos_libre();
        if (pos_libre == true) {
          Serial.println(contador);
          DateTime now = rtc.now();
          reporte[0] = contador;
          reporte[1] = now.day();
          reporte[2] = now.month();
          reporte[3] = now.year();
          reporte[4] = now.hour();
          reporte[5] = now.minute();
          reporte[6] = now.second();
          reporte[7] = uid;

          for (int i = 0; i < 8; i++) {   
            EepromRTC.write(posicion,(byte *)reporte[i]);               
            delay(5);
            posicion = posicion + 1;
          }
            
          lcd.print("Creando reporte"); 
          delay(500);
          lcd.clear();
        } else {
          if (posicion <= 240) {
            lcd.print("Memoria llena");
            delay(1000);
          }
        }
        delay(100);
        showMenuMember();
      }

      if (opc == opcMenuMember[2]) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Bye...");
        delay(1000);
        sa = false;
        ma = false;
        ja = false;
        menuDisenador();
        
      }
    }
  }
}

/*===========================================
 * NUEVO PIN
=============================================*/
void newPin() {
  char tecla = keypad.getKey();
  if (tecla != NO_KEY) {
    codigo[digito] = tecla;
    lcd.setCursor(digito+5, 2);
    lcd.print('*');
    digito++;
    if (digito == 4) {
      newpin = false;
      // ACTUALIZAR PASSWORD SANTIAGO
      if (sa) {
        char new_pin_SA[4];
        for (int i = 0; i < 4; i++) {
          new_pin_SA[i] = codigo[i];
        }
        EEPROM.put(0, new_pin_SA);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Guardando...");
        delay(500);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("PW actualizada");
        delay(1000);
        showMenuMember();
      }
      // ACTUALIZAR PASSWORD MIGUEL
      if (ma) {
        char new_pin_MA[4];
        for (int i = 0; i < 4; i++) {
          new_pin_MA[i] = codigo[i];
        }
        EEPROM.put(5, new_pin_MA);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Guardando...");
        delay(500);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("PW actualizada");
        delay(1000);
        showMenuMember();
      }
      // ACTUALIZAR PASSWORD JUAN
      if (ja) {
        char new_pin_JA[4];
        for (int i = 0; i < 4; i++) {
          new_pin_JA[i] = codigo[i];
        }
        EEPROM.put(9, new_pin_JA);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Guardando...");
        delay(500);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("PW actualizada");
        delay(1000);
        showMenuMember();
      } 
    }
  }
}
/*=============================================
 * PIN ADMIN AND VERIFY PASSWORD
===============================================*/
void pinAdmin() {
  char tecla = keypad.getKey();
  if (tecla != NO_KEY) {
    codigo[digito] = tecla;
    lcd.setCursor(digito+5, 2);
    lcd.print('*');
    digito++;
    if (digito == 4) {
      pinadmin = false;
      if (opc == opcMenuDisenador[4]) {
        char res_pin_SA[4];
        EEPROM.get(0, res_pin_SA);
        if (codigo[0] == res_pin_SA[0] && codigo[1] == res_pin_SA[1] && codigo[2] == res_pin_SA[2] && codigo[3] == res_pin_SA[3]) {
          lcd.clear();
          lcd.print("Correct!");
          delay(1000);
          getReports();
        } else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Incorrect!");
          delay(1000);
          getPasswordAdmin();
        }
      }
    }
  }
}
/*================================================
 * GET OPT MOTOR AND SEND MOVE
==================================================*/
void motorOpt() {
    char tecla = keypad.getKey();
    if (tecla != NO_KEY) {
      opc = tecla;
      digito++;
      delay(50);
      if (digito == 1) {
        motor = false;
        // MOVE TO LEFT
        if (opc == optsMotor[0]) {
          // Left
          while (steps<n_steps) {
            step_left();
            steps = steps + 1;
          }
          delay(100);
          off();
          menuMotor();
        }
        // MOVE TO RIGHT
        if (opc == optsMotor[1]) {
          // Right
          while (steps<n_steps) {
            step_right();
            steps = steps + 1;
          }
          off();
          menuMotor();
        }
        // EXIT
        if (opc == optsMotor[2]) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Bye...");
          delay(500);
          menuDisenador();
        }
        
        
      }
    }
}

/*===========================================
 * TERMINAN FUNCIONES TECLADO 
=============================================*/

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
  lcd.print("5)V RE   6)Motor");
  lcd.setCursor(0,1);
  lcd.print("7).Salir");
  delay(2000);
  getOptionsMenuDisenador();
}
/*==========================================
 * OBTENER OPCIONES MENU DISENADOR
============================================*/
void getOptionsMenuDisenador() {
  digito = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ingrese opcion");
  optMenuDisenador = true;
  while(optMenuDisenador) {
    menuDisenadorOpt();
  }
}
/*==========================================
 * OBTENER PASSWORD MEMBER
============================================*/
void getPasswordMember() {
  const char* members[] = {"santiago", "miguel", "juan"};
  digito = 0;
  codigo[0]="";
  codigo[1]="";
  codigo[2]="";
  codigo[3]="";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pin");
  lcd.print(" ");
  lcd.print(members[member]);
  pwMember = true;
  while(pwMember) {
    passwMember();
  }
}
/*==========================================
 * MOSTRAR MENU MIEMBRO
============================================*/
void showMenuMember() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("1).Cambiar PW");
  lcd.setCursor(0,1);
  lcd.print("2). RE  3).Salir");
  delay(2000);
  getOptionMember();
}
/*==========================================
 * OBTENER OBCIONES MENU MIEMBRO
============================================*/
void getOptionMember() {
  digito = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ingrese opcion");
  optMenuMember = true;
  while (optMenuMember) {
    menuMemberOpc();
  }
}
/*==========================================
 * OBTENER NUEVO PIN
============================================*/
void getNewPin() {
  digito = 0;
  codigo[0]="";
  codigo[1]="";
  codigo[2]="";
  codigo[3]="";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Nuevo pin");
  newpin = true;
  while (newpin) {
    newPin();
  }
}

/*==============================================
 * OBTENER REPORTES
================================================*/
void getReports() {
  const char* opts[] = {"REPORTE", "Day", "Month", "Year", "Hour", "Minute", "Second", "Id"};
  int add = 0;
  int opt = 0;
  byte value = EepromRTC.read(add);
  while(value != 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(opt[opts]);
    lcd.print(" ");
    lcd.print(value);
    delay(1000);

    lcd.clear();

    add++;
    opt++;
    value = EepromRTC.read(add);     
    if (add == 8 || add == 16 || add == 24 || add == 32 || add == 40 || add == 48 || add == 56 || add == 64 || add == 72 || add == 80 || add == 88 || add == 96 || add == 104) {
      opt = 0;
    }
  }
  delay(100);
  lcd.print("Fin...");
  delay(1000);
  menuDisenador();
}
/*==============================================
 * BUSCAR POSICION LIBRE
================================================*/
void buscar_pos_libre() {
  posicion = 0;
  pos_libre = false;
  contador = 0;
  do {
      
    dato = EepromRTC.read(posicion);
      
    if (dato == 0) {
      pos_libre = true;
    } else {
      posicion += 8;      
    }
    contador++;
  } while (pos_libre == false);
}
/*==============================================
 * OBTENER PASSWORD ADMIN
================================================*/
void getPasswordAdmin() {
  digito = 0;
  codigo[0]="";
  codigo[1]="";
  codigo[2]="";
  codigo[3]="";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pin admin");
  pinadmin = true;
  while (pinadmin) {
    pinAdmin();
  }
}
/*=======================================================
 * MUESTRA MENU DEL MOTOR Y LLAMA METODO PARA CAPTURAR TECLAS
=========================================================*/
void menuMotor() {
  digito = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("4)Left  6)Right");
  lcd.setCursor(0,1);
  lcd.print("9).Exit");
  motor = true;
  while(motor) {
    motorOpt();
  }
}

/*=======================================================
 * PASOS A LA DERECHA
=========================================================*/
void step_right() {
  digitalWrite(IN1, HIGH);  // paso 1 
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(20);
  
  digitalWrite(IN1, LOW); // paso 2
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(20);
  
  digitalWrite(IN1, LOW); // paso 3
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  delay(20);
  
  digitalWrite(IN1, HIGH); // paso 4
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(20);
}
/*=======================================================
 * PASOS A LA INZQUIERDA
=========================================================*/
void step_left() {
  digitalWrite(IN1, LOW);  // paso 1 
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  delay(20);
  
  digitalWrite(IN1, LOW); // paso 2
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(20);
  
  digitalWrite(IN1, HIGH); // paso 3
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(20);
  
  digitalWrite(IN1, HIGH); // paso 4
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(20);
}
/*=======================================================
 * APAGA MOTOR
=========================================================*/
void off() {
  steps = 0;
  digitalWrite(IN1, LOW); // paso 4
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(20);
}
