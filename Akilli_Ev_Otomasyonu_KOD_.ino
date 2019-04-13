#include <Servo.h>
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.
#define DHT11PIN 2 // DHT11PIN olarak Dijital 2"yi belirliyoruz.
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f,16,2); 
dht11 DHT11;
Servo camServo;
Servo perdeServo;
Servo kapiServo;

int fan=10;
int lamba=6;
int ldr=A0;
int buzzer=4;
int hirsizGeldi=5;
int buttonState = 0;
int alarm;
int alarmEtkin=0;
int a,k,e,d=0;
int sicaklik;
int nem;

unsigned int ldrDeger=0 ;


int maxSicaklik=28;
int minSicaklik=25;
int maxNem=80;
int minNem=50;

char gelenVeri;
int cam=0;
signed int posCam=0;
signed int posPerde=0;
signed int posKapi=0;

void setup()
{
  kapiServo.attach(7);
  posKapi=0;
  kapiServo.write(posKapi);
  perdeServo.attach(8);
  posPerde=0;
  perdeServo.write(posPerde);
  camServo.attach(9);
  posCam=0;
  camServo.write(posCam);
  
  alarm=0;
  
  
  pinMode(fan, OUTPUT);
  pinMode(lamba, OUTPUT);
  pinMode(ldr, INPUT);
  pinMode(hirsizGeldi,INPUT);
  pinMode(buzzer,OUTPUT);
  
  lcd.init();   
  lcd.backlight();
  Serial.begin(9600);
  digitalWrite(fan, HIGH);
  lcd.print("SMART HOME");
}

void loop()
{
  Serial.println();
  int chk = DHT11.read(DHT11PIN);

 
  Serial.print(gelenVeri);

  

  bluetoothKontrol();
  lcdYazdir();
//  sicaklikKontrolu();
//  camSilme();
//  isikKontrolu();
  hirsizKontrolu();
  ldrKontrolu();
} 

  
  
      
  
  
void bluetoothKontrol()
  {
    gelenVeri = Serial.read();

    if(gelenVeri=='A')a=1;
    while(a==1)
      {      
        posKapi=120;
        kapiServo.write(posKapi);  
        gelenVeri = Serial.read();
        if(gelenVeri=='P')a=0;
      }

    if(gelenVeri=='E')e=1;
    while(e==1)
      {        
        posKapi=0;
        kapiServo.write(posKapi);
        gelenVeri = Serial.read();
        if(gelenVeri=='P')e=0;   
      }

    if(gelenVeri=='K')k=1;
    while(k==1)
      {        
        alarmEtkin=1;   
        gelenVeri = Serial.read();
        if(gelenVeri=='P')k=0;
      }

    if(gelenVeri=='D')d=1;
    while(d==1)
      {        
        alarmEtkin=0; 
        gelenVeri = Serial.read();
        if(gelenVeri=='P')d=0;   
      }
  }  

////////////////////////////////////LCD YAZDIRMA//////////////////////////////////////
  
void lcdYazdir()
{
  int nem=DHT11.humidity;
  int sicaklik=DHT11.temperature;
  lcd.clear();
  lcd.print("NemDegeri(%): "); 
  lcd.println((float)DHT11.humidity, 2); 
  lcd.println("   ");
  lcd.setCursor(0,1);
  lcd.print("Sicaklik (C): ");
  lcd.println((float)DHT11.temperature, 2);
  delay(500);
  if(sicaklik>maxSicaklik)
  {
    digitalWrite(fan, LOW);
  }
  if(sicaklik<minSicaklik)
  {
    digitalWrite(fan, HIGH);
  }
  if(nem>maxNem)cam=1;
  if(cam==1)
  {
    for (posCam = 0; posCam <= 90; posCam += 1) 
    { 
      camServo.write(posCam);              
      delay(15);                          
    }
    for (posCam = 90; posCam >= 0; posCam -= 1) 
    { 
      camServo.write(posCam);              
      delay(15);                      
    }
    delay(1000);
    if(nem<minNem)cam=0;
  }
}

////////////////////////////////////SICAKLIK İLE FAN KONTROLÜ////////////////////////////////////// 
//void sicaklikKontrolu()
//{  
//  if(sicaklik>maxSicaklik)
//  {
//    digitalWrite(fan, LOW);
//  }
//  if(sicaklik<minSicaklik)
//  {
//    digitalWrite(fan, HIGH);
//  }
//}

////////////////////////////////////NEM İLE CAM SİLME//////////////////////////////////////
//void camSilme()
//{
//  if(nem>maxNem)cam=1;
//  if(cam==1)
//  {
//    for (posCam = 0; posCam <= 90; posCam += 1) 
//    { 
//      camServo.write(posCam);              
//      delay(15);                          
//    }
//    for (posCam = 90; posCam >= 0; posCam -= 1) 
//    { 
//      camServo.write(posCam);              
//      delay(15);                      
//    }
//    delay(1000);
//    if(nem<minNem)cam=0;
//  }
//}

////////////////////////////////////LDR İLE PERDE KALDIR ve IŞIK AÇ//////////////////////////////////////
void ldrKontrolu()
{
  ldrDeger = analogRead(ldr);
  if(ldrDeger>200) 
  {
    posPerde=90;
    perdeServo.write(posPerde); 
    digitalWrite(lamba, HIGH);
  }
  if(ldrDeger<50) 
  {
    posPerde=0;
    perdeServo.write(posPerde);
    digitalWrite(lamba, LOW); 
  }
}

////////////////////////////////////LDR İLE IŞIĞI KAPAT AÇ//////////////////////////////////////
//void isikKontrolu()
//{  
//  ldrDegerLamba = analogRead(ldrLamba);
//  if(ldrDegerLamba>200) 
//  {
//    digitalWrite(lamba, HIGH);
//  }
//  if(ldrDegerLamba<50) 
//  {
//    digitalWrite(lamba, LOW);
//  }   
//}

////////////////////////////////////HIRSIZI KONTROL ET//////////////////////////////////////
void hirsizKontrolu()
{
  buttonState = digitalRead(hirsizGeldi);
  if(buttonState==HIGH)
  {
    alarm=1;
    while((alarm==1)&&(alarmEtkin==1))
    { 
      digitalWrite(buzzer,HIGH);
      delay(500);    
      digitalWrite(buzzer,LOW);
      delay(500);
      gelenVeri = Serial.read();
      if(gelenVeri=='D')d=1;
        while(d==1)
        {        
          alarmEtkin=0; 
          alarm=0;
          gelenVeri = Serial.read();
          if(gelenVeri=='P')d=0;   
        }
      }
      
  } 
}
