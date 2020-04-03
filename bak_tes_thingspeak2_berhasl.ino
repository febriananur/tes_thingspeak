//#include <Wire.h>
//#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <SoftwareSerial.h> // memasukan library sofwareserial
SoftwareSerial wifi(10,11); // RX, TX

#define WiFiSSID "FoxFi31"
#define WiFiPassword "zero221211_00"
#define DestinationIP "184.106.153.149" // ip web thingspeak.com
//#define TS_Key "" // api key dari thingspeak.com buat chanel untuk mendapatkanya
String GET = "GET /update?api_key=XTTB07P6ODDQNQMJ&field";
#define triger 6
#define echo 7

boolean connected=false;

unsigned int durasi, jarak;

char c;
String cmd;

// berikut adalah fungsi program sensor jarak

void sensor_jarak(){

digitalWrite(triger, LOW);
  delayMicroseconds(2);
  digitalWrite(triger, HIGH);
  delayMicroseconds(10);
  digitalWrite(triger, LOW);
  delayMicroseconds(2);

durasi = pulseIn(echo, HIGH);
jarak = durasi/58.2;

delay(300);
}

void setup()
{
//lcd.begin();

wifi.begin(9600);
wifi.setTimeout(5000);
Serial.begin(9600);
Serial.println("ESP8266 Client Demo");
delay(1000);
//  periksa apakah modul ESP8266 aktif
wifi.println("AT+RST");
delay(1000);
if(wifi.find("WIFI GOT IP"))
{
Serial.println("Modul siap");
}
else
{
Serial.println("Tidak ada respon dari modul");
while(1);
}
delay(1000);
//setelah modul siap, kita coba koneksi sebanyak 5 kali

for(int i=0;i<5;i++)
{
connect_to_WiFi();
if (connected)
{
break;
}
}
if (!connected){
while(1);
}
delay(5000);

// set the single connection mode
wifi.println("AT+CIPMUX=0");
delay(1000);

pinMode(triger, OUTPUT);
pinMode(echo, INPUT);
}

unsigned long lastTime=0;

void loop()
{
  sensor_jarak(); // memanggil fungsi program sensor jarak
  if (millis() - lastTime >=500){
//lcd.clear();
//
//lcd.setCursor(0,0);
//lcd.print ("jarak: ");
//lcd.print(jarak);
//lcd.setCursor(0,1);
//lcd.print("OTW thingspeak");
  lastTime = millis();
  }
  kirim_thingspeak();
}


//===============================================================
void kirim_thingspeak()
{
  cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DestinationIP;
  cmd += "\",80";
//   
//cmd = "AT+CIPSTART=”TCP”,""";
//cmd += DestinationIP;
//cmd += "","80";
wifi.println(cmd);
Serial.println(cmd);
if(wifi.find("Error"))
{
Serial.println("Koneksi error.");
return;
}

//cmd = "GET /update?key=";
cmd = GET;
cmd +="1=";
cmd += jarak;
//cmd += "rn";    
cmd += "\r\n\r\n";// jangan lupa, setiap perintah selalu diakhiri dengan CR+LF
wifi.print("AT+CIPSEND=");
wifi.println(cmd.length());
if(wifi.find(">"))
{
Serial.print(">");
}
else
{
wifi.println("AT+CIPCLOSE");
Serial.println("koneksi timeout");
delay(1000);
return;
}
wifi.print(cmd);
delay(2000);

while (wifi.available())
{
 c = wifi.read();
Serial.write(c);
if(c=='r') Serial.print('n');
}
Serial.println("——end");
delay(10000);
}


//======================================================
void connect_to_WiFi()
{
wifi.println("AT+CWMODE=1");
String cmd="AT+CWJAP=";
cmd+=WiFiSSID;
cmd+="","";
cmd+=WiFiPassword;
cmd+="";
wifi.println(cmd);
Serial.println(cmd);
if(wifi.find("OK"))
{
Serial.println("Sukses, terkoneksi ke WiFi.");
connected= true;
}
else
{
Serial.println("Tidak dapat terkoneksi ke WiFi. ");
connected= false;
}
}
