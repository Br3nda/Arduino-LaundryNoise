// based on https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels

#include <VirtualWire.h>

const int led_pin = 11;
const int transmit_pin = 1;
//const int receive_pin = 2;
const int sensor_pin = 8;
const int sensor_analogue_pin = A1;
const String sensor_name = "laundry";
const bool debug_mode = false;

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;


void setup()
{
  delay(1000);
  
  if (debug_mode) {
    Serial.begin(115200);	// Debugging only
    Serial.println("Starting receiver");
  }
    // Initialise the IO and ISR
    vw_set_tx_pin(transmit_pin);
    vw_setup(500);       // Bits per sec
    
    pinMode(led_pin, OUTPUT);
    pinMode(sensor_pin, INPUT);
    pinMode(sensor_analogue_pin, INPUT);
    
}

void loop()
{
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
 
   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(sensor_analogue_pin);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
 
  if (debug_mode) {
    Serial.println(volts);
  }
   
  String message;
  message = "R>" + sensor_name + ":noise=" + (String) volts;
  sendMessage(message);
  
  if (volts > 0.4) {
    message = "R>" + sensor_name + ":washingmachine=On";
    sendMessage(message);
  }
  else {
    message = "R>" + sensor_name + ":washingmachine=Off";
    sendMessage(message);
  }
  delay(15 * 1000);
  
}

void sendMessage(String message) { 
  
  if (debug_mode) {
    Serial.println(message);
    delay(500);
    return;
  }
  int buf_len = message.length() +1;
  
  char buf[buf_len];
  message.toCharArray(buf, buf_len);
  
  digitalWrite(led_pin, HIGH); // Flash a light to show transmitting
  vw_send((uint8_t *)buf, buf_len);
  vw_wait_tx(); // Wait until the whole message is gone
  digitalWrite(led_pin, LOW);
  delay(2000);
 
}
