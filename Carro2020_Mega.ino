#define pinDetecMotorLigado A0
#define btn 37
const byte portasOut[] = {45, 43, 41, 39, 33, 31}; //45 biometrico, 43 destravar porta, 41 ignicao, 39 partida, 33 travar porta, 31 partidaB
String nome[] = {"", ""};
byte passo = 0;

#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522::MIFARE_Key key; 
MFRC522 mfrc522(53, 49);   
int statuss = 0;
int out = 0;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);


void setup(){ 

  for(byte i=0; i<6; i++){
    pinMode(portasOut[i], OUTPUT);
    }
  pinMode(btn, INPUT_PULLUP);
  pinMode (pinDetecMotorLigado, INPUT);
  digitalWrite (portasOut[0], LOW);
  digitalWrite (portasOut[1], HIGH);
  digitalWrite (portasOut[2], LOW);
  digitalWrite (portasOut[3], HIGH);
  digitalWrite (portasOut[4], HIGH);
  digitalWrite (portasOut[5], HIGH);
  
  Serial.begin(57600);   
  Serial1.begin(57600); 
  SPI.begin();      
  mfrc522.PCD_Init();    
  finger.begin(57600);
  lcd.init(); 

  wdt_enable(WDTO_8S); // WatchDog (WDTO_8S, WDTO_4S, WDTO_2S, WDTO_1S, WDTO_500MS, WDTO_250MS,WDTO_120MS, WDTO_60MS, WDTO_30MS e WDTO_15MS)
}

void loop(){

  wdt_reset();

  switch(passo){
    case 0:
      RFID1();
      break;
    case 1:
      Biometria();
      break;
    case 2:
      Ignicao();
      break;
    case 3:
      RFID2();
      break;
    }
}


void RFID1(){
  
  if ( ! mfrc522.PICC_IsNewCardPresent()){
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }

  String content = "";
  
  for (byte i = 0; i < mfrc522.uid.size; i++){
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  content.toUpperCase();
  
  if ((content == "A63FF921")){    
      statuss = 1;  
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(2, 0); 
      lcd.print("CONFIRME SUA");
      lcd.setCursor(0, 1);
      lcd.print("BIOMETRIA WALDIR"); 
      nome[0] = "Waldir";
      digitalWrite(portasOut[1], LOW);
      delay(1000);
      digitalWrite(portasOut[1], HIGH);
      passo++;
  } 
  
  else if ((content == "2967845A")){    
      statuss = 1;  
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(2, 0); 
      lcd.print("CONFIRME SUA");
      lcd.setCursor(0, 1);
      lcd.print("BIOMETRIA LEILI"); 
      nome[0] = "Leiliane";
      digitalWrite(portasOut[1], LOW);
      delay(1000);
      digitalWrite(portasOut[1], HIGH);
      passo++;
  }
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}


byte Biometria() {

  static int tempo = 0;
  digitalWrite(portasOut[0], HIGH);

  delay(1);
  tempo++;

  if(tempo >= 18000){
    lcd.noBacklight();
    lcd.clear();
    tempo = 0;
    digitalWrite(portasOut[0], LOW);
    passo = 3;
    }
  
  getFingerprintID();
  
  byte p = finger.getImage();

  if(p != FINGERPRINT_OK) return -1 ;

  p = finger.image2Tz();
  if(p != FINGERPRINT_OK) return -1 ;

  p = finger.fingerFastSearch();
  if(p != FINGERPRINT_OK) return -1;  
  
  if((finger.fingerID == 2) || (finger.fingerID == 3) || (finger.fingerID == 4) || (finger.fingerID == 5) || (finger.fingerID == 6) || (finger.fingerID == 7) || (finger.fingerID == 8)){
    nome[1] = "Waldir";
    passo++;
  }

  if((finger.fingerID == 9) || (finger.fingerID == 10) || (finger.fingerID == 11) || (finger.fingerID == 12) || (finger.fingerID == 13) || (finger.fingerID == 14) || (finger.fingerID == 15) || (finger.fingerID == 16)){
    nome[1] = "Leiliane";
    passo++;
  }
}


byte getFingerprintID() {
  
  byte p = finger.getImage();
  if(p) return p;

  p = finger.image2Tz();
  if(p) return p;

  p = finger.fingerSearch();
  if(p == FINGERPRINT_NOTFOUND) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("BIOMETRIA NAO");
    lcd.setCursor(3, 1);
    lcd.print("CADASTRADA");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TENTE NOVAMENTE");
    return p;
  }

  return finger.fingerID;
}


void Ignicao(){

  static unsigned long temporizador[] = {0, 0};
  static boolean controle[] = {0, 0, 0, 0};
  static byte quantasVezesMotorLigado = 0;
  boolean onOff = digitalRead(btn);
  int detec = analogRead(pinDetecMotorLigado);
  digitalWrite(portasOut[0], LOW);
  
  if((nome[0] == "Waldir") && (nome[1] == "Waldir")){
    digitalWrite(portasOut[2], HIGH);
    digitalWrite(portasOut[5], LOW);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("BEM VINDO");
    lcd.setCursor(5, 1);
    lcd.print("WALDIR");
    delay(4000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PRECIONE O BOTAO");
    lcd.setCursor(0, 1);
    lcd.print("PARA DAR PARTIDA");
    nome[1] = "";
  } 

  else if((nome[0] == "Leiliane") && (nome[1] == "Leiliane")){
    digitalWrite(portasOut[2], HIGH);
    digitalWrite(portasOut[5], LOW);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("BEM VINDA");
    lcd.setCursor(4, 1);
    lcd.print("LEILIANE");
    delay(4000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PRECIONE O BOTAO");
    lcd.setCursor(0, 1);
    lcd.print("PARA DAR PARTIDA");
    nome[1] = "";
  } 

  else if((nome[0] == "Waldir") && (nome[1] == "Leiliane")){
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("BIOMETRIA NAO");
    lcd.setCursor(2, 1);
    lcd.print("RECONHECIDA");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TENTE NOVAMENTE");
    passo = 1;
    nome[1] = "";
  } 

  else if((nome[0] == "Leiliane") && (nome[1] == "Waldir")){
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("BIOMETRIA NAO");
    lcd.setCursor(2, 1);
    lcd.print("RECONHECIDA");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TENTE NOVAMENTE");
    passo = 1;
    nome[1] = "";
  } 

  if((!onOff) && (detec <= 100)){ //Liga motor de partida
    digitalWrite(portasOut[3], LOW);
    digitalWrite(portasOut[5], HIGH);
    } else {
       digitalWrite(portasOut[3], HIGH);
       digitalWrite(portasOut[5], LOW);
    }

  if((detec < 100) && (controle[0]) ){
    temporizador[1] = millis();
    controle[0] = false;
    }

  if((detec > 600) && (!controle[1])){
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("MOTOR LIGADO");
    lcd.setCursor(0, 1);
    lcd.print("******EBA*******");
    delay(5000);
    lcd.clear();
    lcd.noBacklight();
    digitalWrite(portasOut[4], LOW);
    delay(1000);
    digitalWrite(portasOut[4], HIGH);
    temporizador[1] = millis();
    temporizador[0] = millis();
    quantasVezesMotorLigado++;
    controle[2] = true;
    controle[0] = true;
    controle[1] = true;
    }

  if((detec < 100) && ((millis() - temporizador[1]) > 1000)){
    controle[1] = false;
    }

  if((detec < 100) && (quantasVezesMotorLigado > 0) && (controle[2]) && ((millis() - temporizador[1]) > 1000)){
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("MOTOR APAGOU");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PRECIONE O BOTAO");
    lcd.setCursor(0, 1);
    lcd.print("PARA DAR PARTIDA");
    controle[2] = false;
    }
    
  if((!onOff) && (detec > 600) && ((millis() - temporizador[0]) > 3000)){ //desliga o carro
    nome[0] = "";
    controle[0] = false;
    controle[1] = false;
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("IGNICAO");
    lcd.setCursor(3,1);
    lcd.print("DESLIGADA");
    quantasVezesMotorLigado = 0;
    digitalWrite(portasOut[2], LOW);
    digitalWrite(portasOut[5], HIGH);
    digitalWrite(portasOut[1], LOW);
    delay(1000);
    digitalWrite(portasOut[1], HIGH);
    delay(4000);
    lcd.clear();
    lcd.noBacklight();
    passo++;
  }
}


void RFID2(){

  if ( ! mfrc522.PICC_IsNewCardPresent()){
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }

  String content = "";
  
  for (byte i = 0; i < mfrc522.uid.size; i++){
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  content.toUpperCase();
  
  if ((content == "A63FF921") || (content == "2967845A")){    
      digitalWrite (portasOut[4], LOW);
      delay(1000);
      digitalWrite (portasOut[4], HIGH);
      passo = 0;
  }
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
