#include <PJONSoftwareBitBang.h>
bool verbose = false;

PJONSoftwareBitBang bus(44);

void send_rand_btn();
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info);

void setup() {
  Serial.begin(115200);
  srand(analogRead(A3));

  //bus.set_error(error_handler);
  bus.set_receiver(receiver_function);
  bus.strategy.set_pin(A5);
  bus.begin();

  send_rand_btn();
}


void send_rand_btn() {
  uint8_t l_msg[] = {'L', 0};
  l_msg[1] = '0' + (rand() % 3);
  bus.send(45, l_msg, 2);
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if (verbose) {
    for (uint16_t i=0 ; i<length ; i++)
      Serial.print((char)payload[i]);
    Serial.println();
  }

  if (payload[0] == 'R') {
    delay(250);
    send_rand_btn();
  }
};

bool isVerbose(char * msg, uint16_t size) {
  if (size >= 7)
    return (msg[0] == 'v') and (msg[1] == 'e') and (msg[2] == 'r') and (msg[3] == 'b') and (msg[4] == 'o') and (msg[5] == 's') and (msg[6] == 'e');
  return false;
}

/* Valid serial commands:
 * - L <idx> : Light the idx led and wait for a button response
 * - C : Cancel the last light sent.
 */

int8_t msg[256];
size_t msg_size = 0;

void send_serial() {
  while (Serial.available() > 0) {
    // read the incoming byte:
    int8_t incomingByte = Serial.read();
    msg[msg_size++] = incomingByte;

    // Send on newline
    if (incomingByte == (int8_t)'\n') {
      if (isVerbose(msg, msg_size))
        verbose = !verbose;
      else
        bus.send(45, msg, msg_size);
      msg_size = 0;
    }
  }
}


void loop() {
  bus.receive(10);
  send_serial();
  bus.update();
}
