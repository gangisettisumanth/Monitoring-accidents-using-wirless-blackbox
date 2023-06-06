//BLACK BOX//
#include <LiquidCrystal.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
SoftwareSerial GPS_SoftSerial(4,5);/* (Rx, Tx) */
TinyGPSPlus gps; 


#define gas_sen A0
#define mems_sen A1
#define vibration_sen A5
#define eyeblink_sen 2
#define fire_sen 3 
#define buzzer 6
#define motor 7

volatile float minutes, seconds;
volatile int degree, secs, mins;

double lat_val, lng_val, alt_m_val;
uint8_t hr_val, min_val, sec_val;
bool loc_valid, alt_valid, time_valid;

int vib_count=0,eye_blink_count=0;

int gas_sen_val,vibration_sen_val,mems_sen_val,eyeblink_sen_val,fire_sen_val;
const int rs =13,en =12,d4 =11,d5 =10, d6 =9,d7 =8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            

void gps_read();
void smartDelay(unsigned long ms);
void DegMinSec( double tot_val);
void adc_read();
void digital_read();
void gsm_write(String x);

void setup() {
Serial.begin(9600);
GPS_SoftSerial.begin(9600);
 
lcd.begin(16,2);
 
pinMode(gas_sen,INPUT);  
pinMode(vibration_sen,INPUT);
pinMode(mems_sen,INPUT);
pinMode(eyeblink_sen,INPUT);
pinMode(fire_sen,INPUT);
pinMode(buzzer,OUTPUT);
pinMode(motor,OUTPUT);
}

void loop() { 
 delay(500);
 adc_read();
 digital_read(); 
 gps_read();
  lcd.clear();
 lcd.setCursor(0,0);
// lcd.print("v:");
// lcd.print(vib_count);
  lcd.print("Lat:");
  lcd.print(lat_val, 6);
  lcd.setCursor(0,1);
//  lcd.print("VV:");
//  lcd.print(vibration_sen_val);
  lcd.print("Lon:");
  lcd.print(lng_val,6);
 if (fire_sen_val==1)
 {
  gsm_write("Fire_Detected");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fire_Detected");
 }
 if (vibration_sen_val==0)
 {
  vib_count++;
 }
 if(vib_count==2)
 {
  gsm_write("Accident_Detected");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Accident_Detected");
  vib_count=0;
 }
 if (gas_sen_val>=400)
 {
  gsm_write("Smoke_Detected");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Smoke_Detected");
 }
 if (mems_sen_val>300)
 {
  gsm_write("Vehicle_flipped");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Vehicle_flipped");
 }
  if (eyeblink_sen_val==1)
 {
  eye_blink_count++;
 }
 if(eye_blink_count==3)
 {
  digitalWrite(buzzer,HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Drowsiness_detected");
 }
  else if(eye_blink_count==5)
  {
   gsm_write("Drowsiness_detected  Vehicle_Stopped` "); 
   digitalWrite(buzzer,LOW);
   digitalWrite(motor,HIGH); 
   eye_blink_count=0;
   lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Drowsiness_detected");
  lcd.setCursor(0,1);
  lcd.print("Vehicle_stopped");
  }
}

void adc_read()
{
 gas_sen_val=analogRead(gas_sen);
 mems_sen_val=analogRead(mems_sen);
 vibration_sen_val=analogRead(vibration_sen); 
}
void digital_read()
{
 
fire_sen_val=digitalRead(fire_sen);
eyeblink_sen_val=digitalRead(eyeblink_sen);  
 
}
void gsm_write(String x )

{
  String a=x;
 delay(2000);
 Serial.print("at+cmgs=\"+919052142468\"\r"); // number to receive msg
 delay(1000);
 Serial.println(a);
 Serial.print("LATITUDE:");
 delay(100);
 Serial.println(lat_val, 6);
  Serial.print("LONGITUDE:");
 delay(100);
 Serial.print(lng_val,6);
 delay(100);
 Serial.print((char)26);
 delay(1000);
}

void gps_read()
{
        smartDelay(1000); 
        unsigned long start;

        lat_val = gps.location.lat(); 
        loc_valid = gps.location.isValid(); 
        lng_val = gps.location.lng(); 
        alt_m_val = gps.altitude.meters(); 
        alt_valid = gps.altitude.isValid();
        hr_val = gps.time.hour();
        min_val = gps.time.minute(); 
        sec_val = gps.time.second(); 
        time_valid = gps.time.isValid(); 
        if (!loc_valid)
        {          
          Serial.print("Latitude : ");
          Serial.println("*****");
          Serial.print("Longitude : ");
          Serial.println("*****");
        }
        else
        {
          DegMinSec(lat_val);
          Serial.print("Latitude in Decimal Degrees : ");
          Serial.println(lat_val,6);
//          lcd.setCursor(0,0);
//          lcd.print("lat:");
//          lcd.print(lat_val, 6);
          DegMinSec(lng_val); /* Convert the decimal degree value into degrees minutes seconds form */
          Serial.print("Longitude in Decimal Degrees : ");
          Serial.println(lng_val,6);
        }
        if (!alt_valid)
        {
         // Serial.print("Altitude : ");
          //Serial.println("*****");
        }
        else
        {
         // Serial.print("Altitude : ");
         // Serial.println(alt_m_val, 6);    
        }
        if (!time_valid)
        {
         // Serial.print("Time : ");
          //Serial.println("*****");
        }
        else
        {
          char time_string[32];
          sprintf(time_string, "Time : %02d/%02d/%02d \n", hr_val, min_val, sec_val);
          //Serial.print(time_string);    
        } 
}
void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPS_SoftSerial.available())  /* Encode data read from GPS while data is available on serial port */
      gps.encode(GPS_SoftSerial.read());
/* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
  } while (millis() - start < ms);
}

void DegMinSec( double tot_val)   /* Convert data in decimal degrees into degrees minutes seconds form */
{  
  degree = (int)tot_val;
  minutes = tot_val - degree;
  seconds = 60 * minutes;
  minutes = (int)seconds;
  mins = (int)minutes;
  seconds = seconds - minutes;
  seconds = 60 * seconds;
  secs = (int)seconds;
}
