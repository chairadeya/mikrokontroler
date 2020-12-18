#include <Servo.h> 
#include <SD.h>
#include <SPI.h>
#include <DS3231.h>
Servo myservo; 
DS3231  rtc(SDA, SCL);
File myFile;

int pinCS = 10;         
int LDR1 = A1;          //sambungkan ldr1 ke pin A1
int LDR2 = A2;          //sambungkan ldr2 ke pin A2         
int servopin=9;         //sambungkan servo ke pin 9
const int sensor = A0;
const int LedGreen = 8;
const int LedRed = 7 ;
const int pompa = 6;
int error = 5;              //inisiasi nilai error = 5
int initial_position = 90;  //inisiasi nilai awal servo 90deg

void setup(){ 
  Serial.begin(9600);
  pinMode(pinCS, OUTPUT);
  pinMode(LDR1, INPUT);   
  pinMode(LDR2, INPUT);
  pinMode(pompa, OUTPUT);
  pinMode(LedGreen, OUTPUT);  
  pinMode(LedRed, OUTPUT);
  myservo.attach(servopin);  
  myservo.write(initial_position);   //servo dipindah ke posisi 90deg

  rtc.begin();
  //set date & time (pertama kali set saja)
  /*rtc.setDate(18, 12, 2020);   //mensetting tanggal 18 desember 2020
  rtc.setTime(14, 00, 00);     //menset jam 14:00:00
  rtc.setDOW(5);     //menset hari "jumat"*/
  
  // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  
  // Create/Open file 
  //myFile = SD.open("autogarden.txt", FILE_WRITE);
  
  delay(2000);
  
} 
   
void loop(){ 
  //Solar Tracker
  int R1 = analogRead(LDR1); // read  LDR 1
  int R2 = analogRead(LDR2); // read  LDR 2
  int diff1= abs(R1 - R2);   // selesih nilai R1-R2
  int diff2= abs(R2 - R1);   // selesih nilai R2-R1
  if((diff1 <= error) || (diff2 <= error)) { } //jika kedua selisih <= error,nothing happen
  else {    
    if(R1 > R2){
      initial_position = --initial_position;}
    if(R1 < R2){
      initial_position = ++initial_position;}
  }
  
  myservo.write(initial_position); 
  delay(20);
  
  //Autogarden System
  int value = analogRead(map(sensor, 0, 1023, 100, 0));
  //int value = digitalRead(sensor);
  if ((value) < 60){
    digitalWrite(pompa, HIGH);
    digitalWrite(LedRed, HIGH);
    digitalWrite(LedGreen, LOW);}
  else {
    digitalWrite(pompa, LOW);
    digitalWrite(LedRed, LOW);
    digitalWrite(LedGreen, HIGH);}
  delay(10);
  
  int pump = digitalRead(pompa);
  Serial.println("value ldr: " + String(R1) + "--" + String (R2));
  Serial.println("posisi motor: " + String(initial_position));
  Serial.println("value SM: " + String(value));
  Serial.println("value pump: " + String(pump));

  //rtc + sdcard
  
  myFile = SD.open("autogarden.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    // Write to file
    myFile.print(rtc.getDOWStr());
    myFile.print(" ");
    myFile.print(rtc.getDateStr());
    myFile.print("--");
    myFile.print(rtc.getTimeStr());
    myFile.print(",");
    myFile.print(R1);
    myFile.print(",");
    myFile.print("R2");
    myFile.print(",");
    myFile.print(initial_position);
    myFile.print(",");
    myFile.print(value);
    myFile.print(",");
    myFile.println(pump);
    myFile.close(); // close the file
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening autogarden.txt");
  }
  delay(3000);
  // Reading the file  
  /*
  myFile = SD.open("autogarden.txt");
  if (myFile) {
    Serial.println("Read:");
    // Reading the whole file
    while (myFile.available()) {
      Serial.write(myFile.read());
   }
    myFile.close();
  }
  else {
    Serial.println("error reading test.txt");
  }*/
}
