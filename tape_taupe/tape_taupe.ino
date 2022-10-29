// Low level functions

int leds[] = {0, 1, 2, 3, 4, 5, 6, 7};
int buttons[] = {8, 9, 10, 11, 12, 13, A0, A1};

void light(int led, bool status) {
  digitalWrite(leds[led], status ? LOW : HIGH);
  //Serial.print(led);Serial.print(' ');
  //Serial.print(leds[led]);Serial.print(' ');
  //Serial.print(status ? LOW : HIGH);Serial.println();
}

bool pushed (int button) {
  return digitalRead(buttons[button]) == LOW;
}

void setup() {
  //Serial.begin(115200);
  for (int i=0 ; i<8 ; i++) {
    pinMode(leds[i], OUTPUT);
    light(i, false);
    pinMode(buttons[i], INPUT_PULLUP);
  }

  randomSeed(analogRead(A2));
}

// High level functions

void animate(int start) {
  light(start, false);
  for (int offset=1 ; offset<=4 ; offset++) {
    light((start+offset)%8, true);
    light((start-offset+8)%8, true);

    delay(100);

    light((start+offset)%8, false);
    light((start-offset+8)%8, false);
  }
}

void button_resolve(int bidx) {
  light(bidx, true);

  while(!pushed(bidx))
    delay(1);

  animate(bidx);
}

void loop() {
  int bidx = (int)random(8);
  button_resolve(bidx);
}
