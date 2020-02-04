#include <Wire.h>
#include <SPI.h>
#include <SD.h>
//#include "IRremote.h"
//#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//if you need to read altitude,you need to know the sea level pressure
#define SEALEVELPRESSURE_HPA (1013.25)

//This Macro definition decide whether you use I2C or SPI
//When USEIIC is 1 means use I2C interface, When it is 0,use SPI interface
#define USEIIC 1

//int receiver = 9; // Signal Pin of IR receiver to Arduino Digital Pin 11
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  3;      // the number of the LED pin
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long prev_read_Millis = 0;
unsigned long prev_button_Millis = 0;
const unsigned long read_period = 60000;  //the value is a number of milliseconds
const unsigned long button_period = 300;  //the value is a number of milliseconds
int buttonPushCounter = 1;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
File myfile;
const int pin_sd = 10;

#if(USEIIC)
	Adafruit_BME280 bme;
#else
	#define SPI_SCK 13
	#define SPI_MISO 12
	#define SPI_MOSI 11
	#define SPI_CS 10
	Adafruit_BME280 bme(SPI_CS, SPI_MOSI, SPI_MISO, SPI_SCK);
#endif

unsigned long delayTime;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.print("Initializing SD card...");
    if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
    }
    Serial.println("initialization done.");
    bool rslt;
    rslt = bme.begin();  
    if (!rslt) {
        Serial.println("Init Fail,Please Check your address or the wire you connected!!!");
        while (1);
    }
	
    Serial.println("Init Success");
    Serial.println("Temperature           Pressure             Humidity");
    
    // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);
    startMillis = 0;  //initial start time
     
}

void loop() {
  
  currentMillis = millis();
  check_button();
  led_status();     
  get_values(); 
    
}

void get_values() {
  if ((currentMillis - prev_read_Millis >= read_period) && (buttonPushCounter % 2 == 0)) {
      myfile = SD.open("values.txt",FILE_WRITE);
      myfile.print(bme.readTemperature()*9/5 + 32);
      myfile.print(",");
      myfile.print(bme.readPressure()/100.0F);
      myfile.print(",");
      myfile.println(bme.readHumidity());
      //Serial.print("Data Written");
      myfile.close();
      prev_read_Millis += read_period;
  }
}

void check_button() {
  if (currentMillis - prev_button_Millis >= button_period) {
    buttonState = digitalRead(buttonPin);
    if (buttonState != lastButtonState) {
      if (buttonState == HIGH) {
        buttonPushCounter++;
      } else {
      }
    delay(50);
  }
      prev_button_Millis += button_period;
  }
}

void led_status() {
  if (buttonPushCounter % 2 == 0) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
}
