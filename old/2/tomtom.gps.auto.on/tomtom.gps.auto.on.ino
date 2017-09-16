#include "LowPower.h"

// The TomTom Mkii Bluetooth GPS receiver does not automatically turn on when power is applied via its charging port.
// The power button must be held for approximately 1 second to turn it on.
// This program is intented to be used with an ardunio with an I/O pin connected to one of the leads of the power button
// to simulate a power button press and turn the device on as soon as the arduino powers on.

const int ledpin = 13;       // built-in led pin
const int pwrbuttonpin = 10; //[BLUE] pin to be connected to tomtom gps power button
const int inputpowerpin = 2; //[RED] pin to be connected to positive voltage of power adaptor jack (NEED external pulldown)
const int gpspowerpin = 9;   //[YELLOW] pin to be connected to GPS circuit to detect when GPS is powered on

// delay for simulated power button hold-down (ms)
const int pwrbuttondelay = 1300;

// how many power button presses we need to do
volatile unsigned int presses = 0;

volatile byte ledstate = LOW;       // built-in led state
byte inputpowerpin_state = LOW;      // inputpowerpin state
byte inputpowerpin_state_last = LOW; // last inputpowerpin state
byte inputpowerpin_state_stab = LOW;
byte inputpowerpin_state_stab_last = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 100;

void press_power_button() {
  // short pwrbuttonpin to HIGH
  // (simulate button press)
  digitalWrite(pwrbuttonpin, HIGH);
  pinMode(pwrbuttonpin, OUTPUT);
  
  digitalWrite(ledpin, HIGH);

  // wait
  delay(pwrbuttondelay);
  
  // "disconnect" pwrbuttonpin (input mode = high impediance)
  // (simulate button release)
  pinMode(pwrbuttonpin, INPUT);
  digitalWrite(pwrbuttonpin, LOW);
  
  digitalWrite(ledpin, LOW);

}

void power_state_check() {
  if (digitalRead(inputpowerpin) != digitalRead(gpspowerpin)) {
    presses++;
  }
  ledstate = !ledstate;
  Serial.print(ledstate);
  Serial.println(" change");
}

void setup() {
  Serial.begin(9600);

  pinMode(ledpin, OUTPUT); 

  // set pwrbuttonpin pin low (to gnd?)
  pinMode(pwrbuttonpin, INPUT);
  digitalWrite(pwrbuttonpin, LOW);

  // probably only using interrupt for lowerpower wakeup
  // attachInterrupt(digitalPinToInterrupt(inputpowerpin), power_state_check, CHANGE);

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

  if (inputpowerpin_state_stab != inputpowerpin_state_stab_last) {
    // ledstate = !ledstate;
    if (inputpowerpin_state_stab != digitalRead(gpspowerpin)) {
      press_power_button();
    }
  }

  // store last states for debouncing and stable state
  inputpowerpin_state_stab_last = inputpowerpin_state_stab;
  inputpowerpin_state_last = inputpowerpin_state;
  

}
