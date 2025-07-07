
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


unsigned long prevByteMillis = 0;
unsigned long curMillis;


const int recPin = 3;
long baudRate;
long rate=10000;

String dataIn;
String dt[10];
int i;
boolean parsing=false;

int x, minX;

void setup() 
{   pinMode(recPin, INPUT);
    digitalWrite(recPin, HIGH);
    baudRate = detRate(recPin);  
    Serial.begin(baudRate);
    delay(100);
    Serial.flush();  
    dataIn="";  
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
    }    
}
 
void loop() 
{     
     
      curMillis = millis();    
      display.clearDisplay();
      display.setTextSize(2);            
      display.setCursor(12,12);
      display.setTextColor(WHITE);        // Draw white text    
     if(Serial.available()>0)
     {
           char inChar = (char)Serial.read();
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
                display.setTextSize(2);             // Normal 1:1 pixel scale
                display.setTextColor(WHITE);        // Draw white text
                display.setCursor(12,10);             // Start at top-left corner
                display.print("NO DATA");

                display.setTextSize(1);  
                display.setCursor(2,28);             // Start at top-left corner
                display.print("Or Change Baudrate");
                display.display();
                display.clearDisplay();
            
       }
      
     

       

   
    
      
}
 
void parsingData()
{
      int j=0;
      dt[j]="";
      //proses parsing data
      for(i=1;i<dataIn.length();i++)
      {
             //pengecekan tiap karakter dengan karakter (#) dan (,)
             if ((dataIn[i] == '$') || (dataIn[i] == ','))
             {
                   //increment variabel j, digunakan untuk merubah index array penampung
                   j++;
                  dt[j]=""; //inisialisasi variabel array dt[j]
             }
             else
             {
                  //proses tampung data saat pengecekan karakter selesai.
                  dt[j] = dt[j] + dataIn[i];
             }
      }
          if(dt[0] == "GPRMC"){
          float lat = dt[3].toFloat();
          static char buff[10];
          dtostrf( lat,7,3,buff);
          Serial.print(buff);
          display.print(buff);
          display.display();
          display.clearDisplay();
      }
      

//      if(dt[0] == "SDDPT"){
//          display.setTextSize(2);   
//           
//          display.setCursor(0,12);
//          display.print("STNC:"); 
//
//          display.setCursor(60,12);
//          display.print(dt[0]);
//
//          display.setCursor(0,28);  
//          display.print("DPTH:");
//
//           
//          display.setCursor(60,28);  
//          display.print(dt[1]);
//
//          
//          display.display();
//          delay(100);
//          display.clearDisplay();
//      }
//
//      else if(dt[0] == "HCHDT"){
//          display.setTextSize(2);   
//           
//          display.setCursor(0,12);
//          display.print("STNC:"); 
//
//          display.setCursor(60,12);
//          display.print(dt[0]);
//
//          display.setCursor(0,28);  
//          display.print("HDG :");
//
//           
//          display.setCursor(60,28);  
//          display.print(dt[1]);
//
//          
//          display.display();
//          delay(100);
//          display.clearDisplay();
//      }
//
//      else if(dt[0] == "GPRMC"){
//
//        
//          float lat ;
//          float long_;
//          char buffer1[10];
//          char buffer2[10];
//          lat = dt[3].toFloat();
//          long_ = dt[5].toFloat();
//
//          dtostrf( lat , 7, 3, buffer1);
//          
//          dtostrf( long_ , 7,3, buffer2);
//       
//          display.setTextSize(1);   
//           
//          display.setCursor(0,8);
//          display.print("STNC:"); 
//
//          display.setCursor(40,8);
//          display.print(dt[0]);
//
//          display.setCursor(0,26);  
//          display.print("LAT :");
//
//           
//          display.setCursor(40,26);  
//          display.print(buffer1);
//
//          display.setCursor(0,44);  
//          display.print("LONG:");
//
//          display.setCursor(40,44);  
//
//          
//          display.print(buffer2);
//
//          display.display();
//          delay(100);
//          display.clearDisplay();
//      }
//

      



           
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
   
   for (int i = 0; i < 5; i++)
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
