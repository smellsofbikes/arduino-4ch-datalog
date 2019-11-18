#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>

const int chipSelect = 10;
uint32_t sampletime = 500000;
uint8_t debug = 0;
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;

uint16_t button_detect(uint16_t button_analog_value);

void setup() {
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "";
  char buttonState[][4] = {"Rght", "Up  ", "Down", "Left", "SEL ", "?   "};
  uint16_t val;
  bool recording = 0;

  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  float sps = 1000000/sampletime;
  lcd.setCursor(0,0);
  lcd.print(sps);
  lcd.print("M/S");

  char namebuffer[] = "log00.txt";        // buffer for sdcard name
  for (uint8_t i = 0; i < 100; i++)       // neat bit of code from adafruit to make unique names rather than overwriting
  {
    namebuffer[3] = i/10 + '0';
    namebuffer[4] = i%10 + '0';
    if (! SD.exists(namebuffer)) 
    {
      Serial.print("Datalog name: "); 
      Serial.println(namebuffer);
      break; 
    }
  }

  while(1)
  {
    if (recording)
    {
      for (int analogPin = 2; analogPin < 6; analogPin++) 
      {
        int sensor = analogRead(analogPin);
        dataString += String(sensor);
        if (analogPin < 5) 
        {
          dataString += ",";
        }
      }
      File dataFile = SD.open(namebuffer, FILE_WRITE);
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(dataString);
      }
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening datalog.txt");
      }
      dataString = "";
    }

    val = button_detect(analogRead(A0));
    Serial.println(val);
    if (val == 4)
    {
      if (recording == 0)
      {
        recording = 1;
        lcd.setCursor(10,0);
        lcd.print("Recd");
      }
      else
      {
        recording = 0;
        lcd.setCursor(10,0);
        lcd.print("Stby");
      }
    }
  }
}

uint16_t button_detect(uint16_t button_analog_value)
{
  //Serial.print("A0: ");
  //Serial.println(button_analog_value);
  if (button_analog_value < 60)
      return 0;
    else if ((button_analog_value > 60) && (button_analog_value < 145))
      return 1;
    else if ((button_analog_value > 145) && (button_analog_value < 350))
      return 2;
    else if ((button_analog_value > 350) && (button_analog_value < 500))
      return 3;
    else if ((button_analog_value > 500) && (button_analog_value < 800))
      return 4;
    else
      return 5;
}
