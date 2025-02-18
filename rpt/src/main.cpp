#include <Arduino.h>

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
//----------------------------------
RF24 radio(5, 4);
//----------------------------------
const uint64_t pipe1 = 0xF0F0F0F0A1;
const uint64_t pipe2 = 0xF0F0F0F0A2;

uint16_t lastCounter = 0;
bool lastParity = 0;
//----------------------------------
struct Payload {
  uint8_t parity;
  uint16_t counter;
  byte mang[2];  //Mảng có 2 phần tử;
};

bool isValidPacket(Payload& payload);
void updateLastPacket(Payload& payload);

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe1);
  radio.openWritingPipe(pipe2);
  radio.setChannel(80);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
}
//==========================================================================
void loop() {
  delay(15);
  Payload payload;
  radio.startListening();
  while (!radio.available()) digitalWrite(2, LOW);
  radio.read(&payload, sizeof(payload));
  Serial.print("Counter: "); Serial.print(payload.counter); Serial.print(' ');
  Serial.print("Parity: "); Serial.print(payload.parity); Serial.print(' ');
  Serial.print(payload.mang[1]);
  //--------------------------------------------------------------
  delay(15);
  digitalWrite(2, HIGH);
  if (isValidPacket(payload)) {
    updateLastPacket(payload);
    radio.stopListening();
    bool ok = radio.write(&payload, sizeof(payload));
    Serial.println(ok);
  } else {
    Serial.println("Duplicate packet ignored");
  }
  //--------------------------------------------------------------
}
bool isValidPacket(Payload& payload) {
  return ((payload.counter > lastCounter && payload.parity == lastParity) ||
          (payload.counter < lastCounter && payload.parity != lastParity));
}
void updateLastPacket(Payload& payload) {
  lastCounter = payload.counter;
  lastParity = payload.parity;
}