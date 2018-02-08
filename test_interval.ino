/*
Code reads in neural recordings at a precise frequency using interrupts.
Meanwhile, data is accumulated in a data buffer.
Data is separated into spikes and noise via a simple threshold.
The threshold can be controlled in two ways.

Manual Control:
Potentiometer

Automatic Control:
From the data buffer, a threshold is calculated using histograms via the functions
get_bins, gaussian_smooth, find_threshold

The data buffer is updated every recording.
*/

#include <Neuro_Sig.h>

IntervalTimer myTimer;
elapsedMillis sinceThreshold;

Neuro_Sig fly(23, 20000, -5, 5); //pin=23, fs=20000, min=-5, max=5
//Neuro_Sig fly(23, 1000, -5, 5); //pin=23, fs=20000, min=-5, max=5
int pin = fly.get_pin();
int interval_us = 1000000/fly.get_fs();

int thresholdInputPin = A0;
//int thresholdOutputPin = A1;
//int signalCopyPin = A2;
int spikePin = 0;

float thresholdValue = 0;
float newData;
bool state;

void setup() 
{
  Serial.begin(9600);
  pinMode(thresholdInputPin, INPUT);
  //pinMode(thresholdOutputPin, OUTPUT);
  //pinMode(signalCopyPin, OUTPUT);
  pinMode(spikePin, OUTPUT);
  myTimer.begin(read_data, interval_us); //read data every 'interval_us' microseconds
  interrupts();
}

/* function reads in data value and adds to buffer */
void read_data()
{
  newData = 3.3*analogRead(pin)/1024;
  fly.append_buffer(newData);
  //analogWrite(signalCopyPin, newData);
  //Serial.println(newData);
}

void loop() 
{  
  while (micros() >= 5000*interval_us) //if data buffer initialised (i.e full)
  {
    if (fly.detect_spike())
    {
      digitalWrite(spikePin, HIGH); //toggle output after every spike  
      delay(1);   
      digitalWrite(spikePin, LOW); //toggle output after every spike      
      fly.set_isi_rate();
    }
    
    if (sinceThreshold >= 100) //repeat every 100ms
    {
      sinceThreshold = sinceThreshold - 100;
      
      thresholdValue = 3.3*analogRead(thresholdInputPin)/1024; //convert 0-1023 potentiometer to 0-3.3V
      fly.set_threshold(thresholdValue);      

      //Serial.println(millis());
      //Serial.println(thresholdValue);
      fly.get_bins();
      //fly.gaussian_smooth(20, 4); //sigma=20, iterations=4
      //fly.find_threshold();
      
    }
  }
}
