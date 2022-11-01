#include <PJONSoftwareBitBang.h>

PJONSoftwareBitBang bus(45);
int led_addresses[] = {5, 6, 7};
int btn_addresses[] = {9, 10, 11};

void light(int led, bool status);
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info);


void setup() {
  Serial.begin(115200);

  for (int i=0 ; i<3 ; i++) {
    pinMode(led_addresses[i], OUTPUT);
    light(i, false);
    pinMode(btn_addresses[i], INPUT_PULLUP);
  }

  bus.set_receiver(receiver_function);
  bus.strategy.set_pin(A5);
  bus.begin();
}


// ----- hardware functions -----

void light(int led, bool status) {
  digitalWrite(led_addresses[led], status ? HIGH : LOW);
}

bool is_pushed(int button) {
  return digitalRead(btn_addresses[button]) == LOW;
}


// ----- Game -----

int current_led=-1;

void cancel_button() {
  if (current_led != -1)
    light(current_led, false);
  current_led = -1;
}

void set_button(int idx) {
  cancel_button();
  current_led = idx;
  light(current_led, true);
}

void resolve_button() {
  if (current_led == -1)
    return;
  
  if (is_pushed(current_led)) {
    cancel_button();
    bus.send(44, "R", 1);
  }
}


// ----- Interaction functions -----

char err_msg[256];
void reply_error(char * msg, size_t length) {
  err_msg[0] = 'E';
  err_msg[1] = ' ';
  for (int i=0 ; i<length ; i++)
    err_msg[i+2] = msg[i];

  if(!bus.update())
    bus.reply(err_msg, length+2);
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  for (uint16_t i=0 ; i<length ; i++)
    Serial.print((char)payload[i]);
  Serial.println();

  /* Light a certain idx */
  if((char)payload[0] == 'L') {
    // Payload large enougth ?
    if (length < 2) {
      reply_error("No value for L cmd", 18);
      return;
    }
    // Btn value 
    int8_t idx = (char)payload[1] - '0';
    Serial.println(idx);
    if (idx<0 or idx > 2) {
      reply_error("Btn value not in range", 22);
      return;
    }

    // Set the value
    set_button(idx);
  }
  else if ((char)payload[0] == 'C') {
    cancel_button();
  }
};



void loop() {
  bus.receive(10);
  resolve_button();
  bus.update();
}
