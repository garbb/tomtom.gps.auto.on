// The TomTom Mkii Bluetooth GPS receiver does not automatically turn on when power is applied via its charging port.
// The power button must be held for approximately 1 second to turn it on.
// This program is intented to be used with an ardunio with an I/O pin connected to one of the leads of the power button
// to simulate a power button press and turn the device on as soon as the arduino powers on.

// pin to be connected to tomtom gps power button
#define pwrbuttonpin 10
// delay for simulated power button hold-down
#define pwrbuttondelay 1200
// built-in led pin
#define ledpin 13

void setup() {
  //pinMode(ledpin, OUTPUT); 
  
  // set pwrbuttonpin pin HIGH (to 5v)
  digitalWrite(pwrbuttonpin, HIGH);

  // short pwrbuttonpin to ground
  // (simulate button press)
  pinMode(pwrbuttonpin, OUTPUT);
  //digitalWrite(ledpin, HIGH);

  // wait
  delay(pwrbuttondelay);
  
  // disconnect pwrbuttonpin (input mode = high impediance)
  // (simulate button release)
  pinMode(pwrbuttonpin, INPUT);
  //digitalWrite(ledpin, LOW);
}

void loop() {
  /*
  delay(pwrbuttondelay);
  
  // short pwrbuttonpin to ground
  // (simulate button press)
  pinMode(pwrbuttonpin, OUTPUT);
  digitalWrite(ledpin, HIGH);

  delay(pwrbuttondelay);
  
  // disconnect pwrbuttonpin (input mode = high impediance)
  // (simulate button release)
  pinMode(pwrbuttonpin, INPUT);
  digitalWrite(ledpin, LOW);
  */
}
