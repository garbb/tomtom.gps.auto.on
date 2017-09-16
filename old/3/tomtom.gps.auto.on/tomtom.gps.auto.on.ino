#include "LowPower.h"

// The TomTom Mkii Bluetooth GPS receiver does not automatically turn on when power is applied via its charging port.
// The power button must be held for approximately 1 second to turn it on.
// This program is intented to be used with an ardunio with an I/O pin connected to one of the leads of the power button
// to simulate a power button press and turn the device on as soon as power is applied to the charging port and also turn
// the device off when power is removed from the charging port.

const int ledpin = 13;       // built-in led pin
const int pwrbuttonpin = 4; //[BLUE] pin to be connected to tomtom gps power button "sense" lead
const int inputpowerpin = 2; //[RED] pin to be connected to positive voltage of power adaptor jack (NEED external pulldown)
const int gpspowerpin = 5;   //[YELLOW] pin to be connected to GPS circuit to detect when GPS is powered on

// delay for simulated power button hold-down (ms)
const unsigned long pwrbuttondelay = 1100;

byte inputpowerpin_state = LOW;           // inputpowerpin state
byte inputpowerpin_state_last = LOW;      // last inputpowerpin state
byte inputpowerpin_state_stab = LOW;      // stable (debounced) inputpowerpin state
int inputpowerpin_state_stab_last = -1;   // last stable (debounced) inputpowerpin state (for detecting state change)
                                              // (set as neither LOW nor HIGH so that it will run change code at least once)
// debounce vars
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 1000;

// time we should start waiting from to determine when we can sleep
unsigned long sleepWaitDelayStart = 0;

// function to "press" power button
void press_power_button() {
  // short pwrbuttonpin to HIGH
  // (simulate button press)
  digitalWrite(pwrbuttonpin, HIGH);
  pinMode(pwrbuttonpin, OUTPUT);
  
  // digitalWrite(ledpin, HIGH);

  // wait
  delay(pwrbuttondelay);
  
  // "disconnect" pwrbuttonpin (input mode = high impediance)
  // (simulate button release)
  pinMode(pwrbuttonpin, INPUT);
  digitalWrite(pwrbuttonpin, LOW);
  
  // digitalWrite(ledpin, LOW);

  // reset sleep wait
  sleepWaitDelayStart = millis();
}

// need dummy function for interrupt binding (we are just using interrupt for low power mode)
void wakeup() {
}

// go into low power sleep mode until interrupt happens (change in state of input power pin)
void sleepUntilPwrChange() {
  digitalWrite(ledpin, LOW);

  // setup interrupt, then sleep until interrupt triggers
  attachInterrupt(digitalPinToInterrupt(inputpowerpin), wakeup, CHANGE);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  
  // detach interrupt as soon as we wake up b/c we don't need it again until we sleep the next time
  detachInterrupt(digitalPinToInterrupt(inputpowerpin));
  
  // reset sleep wait (need enough time to do debouncing on input power pin signal and press power button) 
  // if we don't reset sleep timer here then it will just immediately sleep on next loop() cycle without doing anything...
  sleepWaitDelayStart = millis();
  digitalWrite(ledpin, HIGH);
}

void setup() {
  // turn on built-in led
  pinMode(ledpin, OUTPUT); 
  digitalWrite(ledpin, HIGH);

  // set pwrbuttonpin pin low (to gnd?)
  pinMode(pwrbuttonpin, INPUT);
  digitalWrite(pwrbuttonpin, LOW);
}

void loop() {

  // read powerbuttonpin
  inputpowerpin_state = digitalRead(inputpowerpin);
  
  // every time inputpowerpin changes, reset debounce timer
  if (inputpowerpin_state != inputpowerpin_state_last) {
    lastDebounceTime = millis();
  }
  
  // if inputpowerpin state has not changed in (debounceDelay)ms then accept current state
  if((millis() - lastDebounceTime) > debounceDelay) {
    // store new stable state
    inputpowerpin_state_stab = inputpowerpin_state;
  }

  // if input power pin and gps power state is mis-matched then we need to "press" button to toggle gps power state
  if (inputpowerpin_state_stab != inputpowerpin_state_stab_last) {
    if (inputpowerpin_state_stab != digitalRead(gpspowerpin)) {
      press_power_button();
    }
  }

  if ((millis() - sleepWaitDelayStart) > ((pwrbuttondelay + debounceDelay) * 2)) {
      // sleep until inputpowerpin changes state
      sleepUntilPwrChange();
  }

  // store last states for debouncing and stable state
  inputpowerpin_state_stab_last = inputpowerpin_state_stab;
  inputpowerpin_state_last = inputpowerpin_state;
  
}
