#include <MIDI.h>

int timeRunning;

#define NUM_PADS    5
#define SWITCH_PIN  12

//low pass filter definition
class LPFilter {
  public:
    float lastval = 0;
    float val = 0;

    float update(float input, float freq) {
      val = input * (1.0 - freq);
      val = val + (lastval * freq);
      lastval = val;
      return val;
    }

    void force(float input){
      val = input;
      lastval = input;
    }
};

class Drumpad {
  public:
    //what port do we listen to?
    int micPort = 0;

    //micVal this frame
    int micVal = 0;

    //mic val last frame
    int _micVal = 0;

    //running millis
    long timeRunning = 0;

    //last hit time
    long lastHit = 100;

    //how long before next hit could happen
    int timeThreshold = 100;

    //how hard should theØ sensor be hit?
    int intensityThreshold = 50;
    float intensityThresholdMin = 0.0;

    int sensitivity = 50;
    
    //measure how hard the sensor will fly
    int hitMaxIntensity = 0;

    //velocity
    int velocity = 0;

    //resulting running average from filter
    float runningAverage;

    //how heavy to smooth the input? closer to 1.0 is smoother
    float smoothVal = 0.99;

    //how hard was the last hit?
    int hitVal = 0;

    LPFilter averageFilter;

    int checkMic() {
      micVal = analogRead(micPort);
//      Serial.print(micVal);
//      Serial.print(",");

      //the minumum value needed for a note trigger is the average plus a little bit
      intensityThreshold = runningAverage + sensitivity;
      intensityThresholdMin = runningAverage - sensitivity;
      timeRunning = millis();

//      Serial.println("micPort: " + String(micPort) + "micVal: " + String(micVal) + " intensityThreshold " + String(intensityThreshold) + " intensityThresholdMin " + String(intensityThresholdMin) + " timeSinceLastHit:" + String(lastHit));
      //detect mic hit
      if (micVal > intensityThreshold) {
        //check if it was not too soon after last hit
        if (timeRunning - lastHit > timeThreshold) {
//          Serial.println("Real hit! Time since last = " + String(timeRunning - lastHit) + " at pad " + String(micPort));
          
          //update last hit time
          lastHit = timeRunning;

          //reset intensity and peak reached
          hitMaxIntensity = micVal;
          return hitMaxIntensity;
        }
      } 
      return 0;

      //update last mic val
      _micVal = micVal;
    }

    void setMicPort(int port) {
      micPort = port;
      pinMode(micPort, INPUT);
      //      averageFilter = new LPFilter();
      //

    }

    void updateRunningAverage() {
      runningAverage = averageFilter.update(micVal, 0.85);

    }
};


Drumpad drumpads[NUM_PADS];


void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLDOWN);

  Serial.begin(9600);
//  Serial.println("Hi");

  initPixels();
//  delay(100);
  for (int i = 0; i < NUM_PADS;  i++) {
    drumpads[i].setMicPort(23 - i);
//    Serial.println("Set drumpad to port " + String(23 - i));
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(SWITCH_PIN) == HIGH){
    timeRunning = millis();
  
    if (timeRunning % 20 == 0) {
      //    pixelHit();
      updatePixels();
    }
  
    for (int i = 0; i < NUM_PADS;  i++) {
      int hitVal = drumpads[i].checkMic();
      if (hitVal > 0) { 
//        Serial.println("hit at " + String(i) + " vol" + String(hitVal));
        sendHit(i + 60, hitVal);
        pixelHit(i);
      }
      //Serial.println(String(i) + " hit at value " + String(hitVal));
      if (timeRunning % 20 == 0) {
        drumpads[i].updateRunningAverage();
      }
    }

//    Serial.println();
  } else {
    delay(500);
  }
}

void sendHit(int note, int velocity) {
  velocity = map(velocity, 0, 1023, 10, 127);
  velocity = velocity * velocity;
  velocity = 60;
  int channel = 1;
  usbMIDI.sendNoteOn(note, velocity, channel);
  delay(10);
  usbMIDI.sendNoteOff(note, velocity, channel);
  int hitVal = 400;
}
