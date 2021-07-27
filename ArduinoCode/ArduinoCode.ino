//Including Libraries
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include "RTClib.h"
#include <SPI.h>
#include "logos.h"

//Declaring Functions
void data_collection();
void clock_set();
void led_status();
void temperature();
void serial_communication();
void web_server();

//Macros Definition
#define wifi_ssid ""    //SSID
#define wifi_key ""   ///Password
#define led LED_BUILTIN
#define led1 12        //D6
#define led2 13        //D7
#define button 14      //D5
#define buzzer 15      //D8
#define firebase_URL ""   //URL
#define firebase_key ""   //Key
RTC_DS3231 rtc;
WiFiClient client;
WiFiServer server (80);
Adafruit_SSD1306 display(128, 64);

//Global variables
String daysOfTheWeek[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
String Month_names[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};
String status_led[2] = {"off", "on"};
int count=1;

//*******************************************SETUP*******************************************

void setup() 
{ 
  Serial.begin(9600); 
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);
  rtc.begin();
  server.begin();
//Setting Up Display (OLED)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  {
    Serial.println(F("OLED connection Failed"));
    for(;;);
  }
  display.clearDisplay();
  display.drawBitmap(2, 4, NUST_logo, 128, 64, 1);
  display.display();
 
  // wifi Connection
  
  WiFi.begin(wifi_ssid, wifi_key); 
  Serial.print("connecting"); 
  while (WiFi.status() != WL_CONNECTED) 
  { 
    Serial.print("."); 
    delay(500); 
  }
  Serial.println(); 
  Serial.print("connected: "); 
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,10);
  display.println("Presented");
  display.setCursor(44,40);
  display.println("to");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.print("Teacher");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(28,38);
  display.print("SurName");
  display.display();
  Firebase.begin(firebase_URL, firebase_key); 
  delay(500);
  Firebase.setFloat("number", 40.0);  
  if (Firebase.failed()) 
  { 
      Serial.print("setting /number failed:"); 
      Serial.println(Firebase.error());   
      return; 
  } 
  //firebase variables
  Firebase.setInt("value1",0);
  Firebase.setInt("value2",0);
  Firebase.setInt("min",0);
  
}  

 //*******************************************LOOP*******************************************
void loop() 
{
  DateTime now = rtc.now();
  // set value and handling error
  if (digitalRead(button)== HIGH)
  temperature();
  if (Serial.available() > 0)                 // If data is available on serial port 
  {
    serial_communication();
    led_status();
  }
  web_server();
  if (Firebase.getInt("count1")!=Firebase.getBool("Led 1") || Firebase.getInt("count2")!=Firebase.getBool("Led 2"))
  led_status();
  if (Firebase.getInt("min")!=now.minute())
  {
    clock_set();
    data_collection();
  }
  if (Serial.available() > 0)
  {
    serial_communication();
    led_status();
  }
  web_server();
  if (Firebase.getInt("count1")!=Firebase.getBool("Led 1") || Firebase.getInt("count2")!=Firebase.getBool("Led 2"))
  led_status();
}

 //**********************************Function Definitions*********************************

//Retrieving and checking data in Firebase
void data_collection()
{
  DateTime now = rtc.now();
  int counter;
  int button_value=0;
  int i=now.month();
  int j=now.day();
  int k=now.hour();
  int l=now.minute();
  led_status();
  String main= (String)i+"/"+(String)j+"/"+(String)k+"/"+(String)l;
  if(Firebase.getInt(main + "/Value")==1)
  {
      counter=0;
      while(digitalRead(button)!= HIGH && counter!=5)
      { 
        tone(buzzer,4000);
        delay(500); 
        display.clearDisplay();
        display.drawBitmap(0, 0, alarm_clock, 128, 64, 1);
        display.display();
        noTone(buzzer);
        delay(500);
        tone(buzzer, 2000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(32,0);
        display.println("Title:");
        display.println("__________");
        display.setTextSize(1);
        display.println(Firebase.getString(main + "/Note Name"));
        display.display();
        delay(2000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(32,0);
        display.println("Note:");
        display.println("__________");
        display.setTextSize(1);
        display.println(Firebase.getString(main + "/Note"));
        display.display();
        if (Firebase.getInt("count1")!=Firebase.getBool("Led 1") || Firebase.getInt("count2")!=Firebase.getBool("Led 2"))
        led_status();
        delay(100);
        noTone(buzzer);
        delay(500);
        counter++;
      }
      Firebase.setInt(main + "/Value", 0);
    }
    else if (Firebase.getInt(main + "/Value")==0)
    {
      if (Firebase.getInt("count1")!=Firebase.getBool("Led 1") || Firebase.getInt("count2")!=Firebase.getBool("Led 2"))
      led_status();
      clock_set();
    }
    else  
    {
      if (Firebase.getInt("count1")!=Firebase.getBool("Led 1") || Firebase.getInt("count2")!=Firebase.getBool("Led 2"))
      led_status();
      clock_set();
      led_status();
    }
}

//Frequently updating clock
void clock_set()
{
  DateTime now = rtc.now();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.drawBitmap(0, 0, frame, 128, 64, 1);
  display.setCursor(25,0);
  display.print("Clock");
  display.setTextSize(3);
  display.setCursor(0, 35);
  display.print(now.hour());
  display.print(":");
  display.print(now.minute()); 
  display.setTextSize(1);
  display.setCursor(110, 35);
  display.print(daysOfTheWeek[now.dayOfTheWeek()]);
  display.setCursor(110, 45);
  display.print(now.day());
  display.setCursor(110, 55);
  display.print(Month_names[now.month()-1]);
  display.display();
  if (Firebase.getInt("count1")!=Firebase.getBool("Led 1") || Firebase.getInt("count2")!=Firebase.getBool("Led 2"))
  led_status();
}

//Display Temprature
void temperature()
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(29, 10);
    display.println("Temp: ");
    display.setCursor(20, 32);
    display.print(rtc.getTemperature());
    display.println(" C");
    display.display();
  }

//Checking and updating led status
void led_status()
{
  if (Firebase.getInt("count1")!=Firebase.getBool("Led 1"))
  {
    tone(buzzer,5000);
    delay(200);
    noTone(buzzer);
    if (Firebase.getBool("Led 1")==1)
    digitalWrite(led1, HIGH);
    else if (Firebase.getBool("Led 1")==0)
    digitalWrite(led1, LOW);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(35,05);
    display.println("LED 1 ");
    display.setCursor(50,33);
    display.println(status_led[Firebase.getBool("Led 1")]);
    display.display();
  }
  Firebase.setInt("count1",Firebase.getBool("Led 1")); 
  if (Firebase.getInt("count2")!=Firebase.getBool("Led 2"))
  {
    tone(buzzer,5000);
    delay(200);
    noTone(buzzer);
    if (Firebase.getBool("Led 2")==1)
    digitalWrite(led2, HIGH);
    else if (Firebase.getBool("Led 2")==0)
    digitalWrite(led2, LOW);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(35,05);
    display.println("LED 2 ");
    display.setCursor(50,33);
    display.println(status_led[Firebase.getBool("Led 2")]);
    display.display();
  }
  Firebase.setInt("count2",Firebase.getBool("Led 2")); 
}

void serial_communication()
{
  String data_received; 
  data_received = Serial.readStringUntil('\n');  /* Data received from bluetooth */
  if (data_received.equals("led1_on"))
  {
    digitalWrite(led1, HIGH);
    Serial.write("LED 1 turned ON\n"); 
    Firebase.setBool("Led 1", 1);       
  }
  else if (data_received.equals("led1_off"))
  {
    digitalWrite(led1, LOW);
    Serial.write("LED 1 turned OFF\n");
    Firebase.setBool("Led 1", 0);
  }
  if (data_received.equals("led2_on"))
  {
    digitalWrite(led2, HIGH);
    Serial.write("LED 2 turned ON\n");
    Firebase.setBool("Led 2", 1);        
  }
  else if (data_received.equals("led2_off"))
  {
    digitalWrite(led2, LOW);
    Serial.write("LED 2 turned OFF\n");
    Firebase.setBool("Led 2", 0);
  }
}

void web_server()
{
  client = server.available();  //Gets a client that is connected to the server and has data available for reading.    
  if (client == 1)
  {
    String command = client.readStringUntil('\n');
    command.trim();
    if (command == "GET /led1on HTTP/1.1")
    {
      digitalWrite(led1, HIGH);
      Firebase.setBool("LED 1", 1);
    }
    else if (command == "GET /led1off HTTP/1.1")
    {
      digitalWrite(led1, LOW);
      Firebase.setBool("LED 1", 0);
    }
    if (command == "GET /led2on HTTP/1.1")
    {
      digitalWrite(led2, HIGH);
      Firebase.setBool("LED 2", 1);
    }
    else if (command == "GET /led2off HTTP/1.1")
    {
      digitalWrite(led2, LOW);
      Firebase.setBool("LED 2", 0);
    }
    Serial.println(command);
  }
}
