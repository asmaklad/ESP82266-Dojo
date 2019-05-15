void setup() {
  Serial.begin(115200);
  Serial.println("Started..");
  // detect D5,D6 and D7 joystick button
  int interruptPinD5 = D5;
  int interruptPinD6 = D6;
  int interruptPinD7 = D7;
  //INPUT, OUTPUT, or INPUT_PULLUP
  pinMode(interruptPinD5, INPUT_PULLUP);
  pinMode(interruptPinD6, INPUT_PULLUP);
  pinMode(interruptPinD7, INPUT_PULLUP);
  //CHANGE, RISING and FALLING
  attachInterrupt(digitalPinToInterrupt(interruptPinD5), handleInterruptD5, FALLING);
  attachInterrupt(digitalPinToInterrupt(interruptPinD6), handleInterruptD6, FALLING);
  attachInterrupt(digitalPinToInterrupt(interruptPinD7), handleInterruptD7, FALLING);
  ///
}

void loop() {
  Serial.println("looping");
  delay(6000);
}

void handleInterruptD5() {
  Serial.print("D5:press:");
  Serial.println(D5);
}

void handleInterruptD6() {
  Serial.print("D6:up:");
  Serial.println(D6);
}

void handleInterruptD7() {
  Serial.print("D7:down:");
  Serial.println(D7);
}
