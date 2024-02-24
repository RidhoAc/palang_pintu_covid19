#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MLX90614.h>
#include <Servo.h>
Servo myservo;

LiquidCrystal_I2C lcd(0x27 ,16,2);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const int trigPin1 = 10;
const int echoPin1 = 11;
const int trigPin2 = 8;
const int echoPin2 = 9;
const int LEDkuning = 7;
const int pinPIR = 3;
const int pinBuzzer = 2;
const int LEDmerah = 4;

int jumlahPengunjung = 0;
float durasi1;
float durasi2;
float jarakMasuk;
float jarakKeluar;
float suhu;
float x; //kalibrasi sensor suhu MLX90614
float y; //kalibrasi sensor ultrasonik masuk (Punya Ridho)
float z; //kalibrasi sensor ultrasonik keluar (Punya Ratna)
int statusPIR = 0;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(pinPIR, INPUT);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  Serial.begin(9600);
  myservo.attach(5);
  myservo.write(0);
  lcd.init();
  lcd.backlight();
  mlx.begin();
  startMillis = millis();
}

void readUltra1(){
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  durasi1 = pulseIn(echoPin1, HIGH);
  jarakMasuk = (durasi1/2)/29,1;
  y = 1.0524*jarakMasuk - 0.6597;
}

void readUltra2(){
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  durasi2 = pulseIn(echoPin2, HIGH);
  jarakKeluar = (durasi2/2)/29,1;
  z = 1.0554*jarakKeluar - 1.044;
}

void readSuhu(){
  suhu = mlx.readObjectTempC();
  x = 0.9684*suhu + 1.4024;
  currentMillis = millis();
  if (currentMillis - startMillis >= period){
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("Suhu = ");
    lcd.print(x);
    lcd.print(char(0xdf));
    lcd.print("C");
    startMillis = currentMillis;
  }
}

void loop() {
  readUltra1();
  readUltra2();
  readSuhu();
  statusPIR = digitalRead(pinPIR);
  
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("PENGUNJUNG ");
  lcd.print(jumlahPengunjung);

  if (statusPIR == 1){
    if (x < 40){
      digitalWrite(LEDmerah, LOW);
      digitalWrite(pinBuzzer, LOW);
      myservo.write(0);
      delay(1000);
      myservo.write(90);
      delay(7000);
      myservo.write(0);
    }else {
      digitalWrite(LEDmerah, HIGH);
      digitalWrite(pinBuzzer, HIGH);
    }
  }else {
    digitalWrite(LEDmerah, LOW);
    digitalWrite(pinBuzzer, LOW);}
  if (y < 8){
    jumlahPengunjung = jumlahPengunjung + 1;
    lcd.setCursor(0,1);
    lcd.print("PENGUNJUNG ");
    lcd.print(jumlahPengunjung);
  }
  if (z < 8){
    jumlahPengunjung = jumlahPengunjung - 1;
    lcd.setCursor(0,1);
    lcd.print("PENGUNJUNG ");
    lcd.print(jumlahPengunjung);
  }
  if (jumlahPengunjung >= 15){
    jumlahPengunjung = 15;
    digitalWrite(LEDkuning, HIGH);
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("PENGUNJUNG FULL");
  }else {digitalWrite(LEDkuning, LOW);}
  if (jumlahPengunjung < 0){
    jumlahPengunjung = 0;
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("PENGUNJUNG ");
    lcd.print(jumlahPengunjung);
    lcd.print("0");
  }
  delay(500);
  Serial.print(x);
  Serial.print("*");
  Serial.print("C");
  Serial.print(" : ");
  Serial.print(y);
  Serial.print(" : ");
  Serial.print(z);
  Serial.print(" : ");
  Serial.println(statusPIR);
}
