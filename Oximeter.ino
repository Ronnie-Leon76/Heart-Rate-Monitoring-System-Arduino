/*
 In this Project, we are building an IoT heart rate monitoring system using Arduino Mega, KY-039 heartbeat sensor and other components.
 The KY-039 heartbeat sensor does not provide a digital number through I2C. It provides an analog value from 0 - 1023 indicating how much
 infra red light the light sensor receives, or actually how much something is shading the light sensor. The higher value, the less infra red
 In short, one is supposed to place your finger between the IR LED and the light transistor of the sensor.
 Your heartbeat dilates the blood vessels in your finger, which will filter the IR creating a pulsating signal.
*/
#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//To get a smoother output, take an average of say 4 last readings from the sensor
//Define a constant stating how many readings are needed
#define samp_siz 4 //readings to be read
/*Taking a look at the graph generated on the serial plotter, the values on the y-axis keep on changing thus it is wise to take 5 consecutive
 * rising values to denote that you are at a bottom heading up. Note the time and print the corresponding BPM
*/
//rise_threshold is the number of consecutive rise that denote the value is heading up and we should note the time and print the BPM
#define rise_threshold 4

float reads[samp_siz], sum;//reads is an array holding the number of readings
long int now, x;//help us to change the index of the reads array
float last, reader, start;//last denotes the last reading, reader is the sensor value read and start is the initial value
float first, second, third, before, print_value;//denotes 1st, 2nd, 3rd readings and before readings
bool rising;//boolean indicating whether the values i.e BPM is rising
int rise_count;//no. of consecutive rising values to help us establish the point of BPM
int n;//helps us iterate through the do..while loop
long int last_beat;

//have a push button that will be pressed to start reading the value
const int pushButton = 10;

// Pulse Monitor Test Script
int sensorPin = 0;


byte customChar1[8] = {0b00000,0b00000,0b00011,0b00111,0b01111,0b01111,0b01111,0b01111};
byte customChar2[8] = {0b00000,0b11000,0b11100,0b11110,0b11111,0b11111,0b11111,0b11111};
byte customChar3[8] = {0b00000,0b00011,0b00111,0b01111,0b11111,0b11111,0b11111,0b11111};
byte customChar4[8] = {0b00000,0b10000,0b11000,0b11100,0b11110,0b11110,0b11110,0b11110};
byte customChar5[8] = {0b00111,0b00011,0b00001,0b00000,0b00000,0b00000,0b00000,0b00000};
byte customChar6[8] = {0b11111,0b11111,0b11111,0b11111,0b01111,0b00111,0b00011,0b00001};
byte customChar7[8] = {0b11111,0b11111,0b11111,0b11111,0b11110,0b11100,0b11000,0b10000};
byte customChar8[8] = {0b11100,0b11000,0b10000,0b00000,0b00000,0b00000,0b00000,0b00000};



void setup() {
    Serial.begin(9600);
    pinMode(pushButton, INPUT);
    lcd.begin(16,2);
    lcd.createChar(1, customChar1);
    lcd.createChar(2, customChar2);
    lcd.createChar(3, customChar3);
    lcd.createChar(4, customChar4);
    lcd.createChar(5, customChar5);
    lcd.createChar(6, customChar6);
    lcd.createChar(7, customChar7);
    lcd.createChar(8, customChar8);

}

void loop ()
{
    lcd.setCursor(0,0);
    lcd.print("Place The Finger");
    lcd.setCursor(0, 1);
    lcd.print("And Press button");
    if(digitalRead(pushButton) == HIGH){
      lcd.clear();
      for (int i = 0; i < samp_siz; i++)
        reads[i] = 0;
      sum = 0;
      x = 0;
  
      while(1)
      {
        // calculate an average of the sensor
        // during a 20 ms period (this will eliminate
        // the 50 Hz noise caused by electric light in the room
        n = 0;
        start = millis();
        reader = 0.;
        //Remember the syntax of the do..while loop
        //So, long as now is less than the start time + 20 it will continue reading the value from the sensor
        do
        {
          reader += analogRead (sensorPin);
          n++;
          now = millis();
        }
        while (now < start + 20);  
        reader /= n;  // we got an average
        
        // Add the newest measurement to an array
        // and subtract the oldest measurement from the array
        // to maintain a sum of last measurements
        sum -= reads[x];
        sum += reader;
        reads[x] = reader;
        last = sum / samp_siz;
        // now last holds the average of the values in the array
  
        // check for a rising curve (= a heart beat)
        if (last > before)
        {
          rise_count++;
          if (!rising && rise_count > rise_threshold)
          {
            // Ok, we have detected a rising curve, which implies a heartbeat.
            // Record the time since last beat, keep track of the two previous
            // times (first, second, third) to get a weighed average.
            // The rising flag prevents us from detecting the same rise more than once.
            rising = true;
            first = millis() - last_beat;
            last_beat = millis();
  
            // Calculate the weighed average of heartbeat rate
            // according to the three last beats
            print_value = 60000. / (0.4 * first + 0.3 * second + 0.3 * third);

            lcd.setCursor(0, 0);
            lcd.setCursor(2, 0);
            lcd.write(byte(1));
            lcd.setCursor(3, 0);
            lcd.write(byte(2));
            lcd.setCursor(4, 0);
            lcd.write(byte(3));
            lcd.setCursor(5, 0);
            lcd.write(byte(4));

            lcd.setCursor(2, 1);
            lcd.write(byte(5));
            lcd.setCursor(3, 1);
            lcd.write(byte(6));
            lcd.setCursor(4, 1);
            lcd.write(byte(7));
            lcd.setCursor(5, 1);
            lcd.write(byte(8));
            lcd.setCursor(7, 1);
            lcd.print(print_value);
            lcd.print(" BPM");
            
            Serial.print(print_value);
            Serial.print('\n');
            
            third = second;
            second = first;
            
          }
        }
        else
        {
          // Ok, the curve is falling
          rising = false;
          rise_count = 0;
        }
        before = last;
        
        
        x++;
        x %= samp_siz;
        delay(1000);
  
      }
      delay(1000);
      }
      else{}
      

    
}
