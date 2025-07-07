
// #include <SPI.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <Adafruit_SH110X.h>
// #include <Fonts/TomThumb.h>
// #include <Fonts/FreeSerif9pt7b.h>
// #include <Fonts/FreeMono9pt7b.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Fonts/TomThumb.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <HardwareSerial.h>
#include <ezButton.h>
//
//
//#define I2C_SDA 22
//#define I2C_SCL 21
//
//// #define I2C_SDA 22
//// #define I2C_SCL 21
//#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//
//
//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//
//// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
////Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Declaration for an SH1106 display connected to I2C (SCL=D1, SDA=D2 pins)
#define SCREEN_ADDRESS 0x3C //< See datasheet for Address; 0x3C for 128x64
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SH1106G oled = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1, 400000, 100000);




unsigned long prevByteMillis = 0;
unsigned long curMillis;


const int recPin = 5;
long baudRate;
long rate=10000;

String dataIn;
String dt[10];
int i;
boolean parsing=false;

int x, minX;

void setup() 
{   
    
    Wire.begin(I2C_SDA, I2C_SCL);
    display.begin(i2c_Address, true);
    display.clearDisplay();
    display.drawRect(1, 1, display.width() - 2, display.height() - 2, SH110X_WHITE);
    display.setTextColor(SH110X_WHITE);
    //display.setFont(&FreeMono9pt7b);
    display.setTextSize(2);
    display.setCursor(44,10);             // Start at top-left corner
    display.print("NMEA");
    display.setCursor(36,28); 
    display.print("BUFFER");
    display.setTextSize(1);
    display.setCursor(30,46);
    display.print("M2N.Instument");
    display.display();


    pinMode(recPin, INPUT);
    digitalWrite(recPin, HIGH);
    baudRate = detRate(recPin);  
    Serial2.begin(baudRate);
    Serial.begin(115200);
    delay(100);
    Serial2.flush();  
    dataIn="";  
}

 
void loop() 
{     
     curMillis = millis();    
     display.clearDisplay();
     display.setTextColor(SH110X_WHITE);        // Draw SH110X_WHITE text   
     display.drawRect(1, 1, display.width() - 2, display.height() - 2, SH110X_WHITE);
     if(Serial2.available()>0)
     {
           char inChar = (char)Serial2.read();
           dataIn += inChar;
           if (inChar == '\n') {parsing = true;}
     }
     if(parsing)
     {     
           parsingData();
           parsing=false;
           dataIn="";
           prevByteMillis = curMillis; 
     }

     if (curMillis - prevByteMillis >=5000) {
               display.clearDisplay();
               display.drawRect(1, 1, display.width() - 2, display.height() - 2, SH110X_WHITE);
               display.setTextSize(2);             // Normal 1:1 pixel scale
               display.setTextColor(SH110X_WHITE);        // Draw SH110X_WHITE text
               display.setCursor(22,6);             // Start at top-left corner
               display.print("NO DATA");
               display.setTextSize(1.5);
               display.setCursor(24,30);             // Start at top-left corner
               display.print("Or Press Reset");
               display.setCursor(10,46);             // Start at top-left corner
               display.print("to Change Baudrate");
               display.display();
               display.clearDisplay();
           
      }
            
}
 
void parsingData()
{
      int j=0;
      dt[j]="";
      for(i=1;i<dataIn.length();i++)
      {
             if ((dataIn[i] == '$') || (dataIn[i] == ','))
             {
                  j++;
                  dt[j]="";
             }
             else
             {
                  dt[j] = dt[j] + dataIn[i];
             }
      }

      if(dt[0] == "GPRMC"){ 
            float lat,lat1,long_, long_1;
            static char buff[10], buff1[10];
            String lat_d ="";
            String long_d="";
            lat = dt[3].toFloat();
            if(dt[4]=="S"){
              lat1= -(lat/100);
            }
            else if(dt[4]=="N"){
              lat1= (lat/100);
              
            }
            dtostrf(lat1, 4,4,buff);
            lat_d += buff;

            long_ = dt[5].toFloat();
            long_1= long_/100;
            dtostrf(long_1, 4,4,buff1);
            long_d += buff1;
            display.setTextSize(2);            
            display.setCursor(34,4);
            display.print(dt[0]);            
            display.setCursor(24,24);
            display.print(lat_d);       
            display.setCursor(20,44);
            display.print(long_d);
            display.display();
            display.clearDisplay();
            
       } 

       else if ( dt[0].charAt(2)=='H' && dt[0].charAt(3)=='D' && dt[0].charAt(4)== 'T'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[1]);
            display.display();
            display.clearDisplay();
            
        
       }
       else if ( dt[0].charAt(2)=='H' && dt[0].charAt(3)=='D' && dt[0].charAt(4)== 'M'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[1]);
            display.display();
            display.clearDisplay();
            
        
       }
       else if ( dt[0].charAt(2)=='M' && dt[0].charAt(3)=='T' && dt[0].charAt(4)== 'W'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[1]);
            display.display();
            display.clearDisplay();
       }
       else if ( dt[0].charAt(2)=='D' && dt[0].charAt(3)=='P' && dt[0].charAt(4)== 'T'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[1]);
            display.display();
            display.clearDisplay();
       }
       else if ( dt[0].charAt(2)=='D' && dt[0].charAt(3)=='B' && dt[0].charAt(4)== 'T'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[3]);
            display.display();
            display.clearDisplay();
       }
       
       else if ( dt[0].charAt(2)=='V' && dt[0].charAt(3)=='H' && dt[0].charAt(4)== 'W'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[5]);
            display.display();
            display.clearDisplay();
       }
       else if ( dt[0].charAt(2)=='M' && dt[0].charAt(3)=='W' && dt[0].charAt(4)== 'D'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[1]);
            display.display();
            display.clearDisplay();
       }
       else if ( dt[0].charAt(2)=='M' && dt[0].charAt(3)=='W' && dt[0].charAt(4)== 'V'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[1]);
            display.display();
            display.clearDisplay();
       }
       else if ( dt[0].charAt(2)=='V' && dt[0].charAt(3)=='W' && dt[0].charAt(4)== 'R'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[1]);
            display.display();
            display.clearDisplay();
       }
       else if ( dt[0].charAt(2)=='H' && dt[0].charAt(3)=='D' && dt[0].charAt(4)== 'G'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[1]);
            display.display();
            display.clearDisplay();
       }

       else if ( dt[0].charAt(2)=='V' && dt[0].charAt(3)=='B' && dt[0].charAt(4)== 'W'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setCursor(20,38);
            display.print(dt[1]);
            display.display();
            display.clearDisplay();
       }

       else if ( dt[0].charAt(2)=='V' && dt[0].charAt(3)=='D' && dt[0].charAt(4)== 'M'){

            display.setTextSize(3);            
            display.setCursor(20,10);
            display.print(dt[0]);
            display.setTextSize(2);  
            display.setCursor(52,38);
            display.print("AIS");
            display.display();
            display.clearDisplay();
       }




                
}

long detRate(int recpin)  // function to return valid received baud rate
                          // Note that the serial monitor has no 600 baud option and 300 baud
                          // doesn't seem to work with version 22 hardware serial library
  {
    
    
    /*
    long baud, rate = 10000, x;
  for (int i = 0; i < 10; i++) {
      x = pulseIn(recpin,LOW);   // measure the next zero bit width
      rate = x < rate ? x : rate;

    
    */
   long baud,  x;
   
   for (int m = 0; m < 5; m++)
     {
      while(digitalRead(recpin) == 1){} // wait for low bit to start
      x = pulseIn(recpin,LOW);   // measure the next zero bit width
      rate = x < rate ? x : rate;
     }
  // long rate = pulseIn(recpin,LOW);   // measure zero bit width from character 'U'
     if (rate < 12)
      baud = 115200;
      else if (rate < 20)
      baud = 57600;
      else if (rate < 29)
      baud = 38400;
      else if (rate < 40)
      baud = 28800;
      else if (rate < 60)
      baud = 19200;
      else if (rate < 80)
      baud = 14400;
      else if (rate < 150)
      baud = 9600;
      else if (rate < 300)
      baud = 4800;
      else if (rate < 600)
      baud = 2400;
      else if (rate < 1200)
      baud = 1200;
      else 
      baud = 0;  
   return baud; 
  }

  
