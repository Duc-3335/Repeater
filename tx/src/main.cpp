#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
uint16_t counter = 0;
bool parity = 0;

RF24 radio(9, 10);  // CE, CSN
const uint64_t pipe1 = 0xF0F0F0F0A1;

int nutnhan = 5;
boolean gtnutnhan = 0;
int bientro = A0;
int gtbientro;
int gtbiendoi;

struct Payload {
  bool parity;
  uint16_t counter;
  byte mang[2];  //Mảng có 2 phần tử
};

void setup() {
  Serial.begin(9600);

  if (!radio.begin()) {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  }
  radio.openWritingPipe(pipe1);
  radio.setPALevel(RF24_PA_MAX);  //Cài bộ khuyết địa công suất ở mức MIN
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();  //Cài đặt module là TX
  if (!radio.available()) {
    Serial.println("Chưa kết nối được với RX...!!");
    Serial.println("CHỜ KẾT NỐI.......");
  }

  pinMode(nutnhan, INPUT_PULLUP);
  pinMode(bientro, INPUT);
}

void loop() {
  Payload payload;
  payload.parity = parity;
  payload.counter = counter;
  gtbientro = analogRead(bientro);
  gtbiendoi = map(gtbientro, 0, 1023, 0, 180);
  payload.mang[0] = gtbiendoi;  //Ghi gtbiendoi vào phần tử 0

  static bool lastState = HIGH;
  bool currentState = digitalRead(nutnhan);
  unsigned long currentTime = millis();

  if (currentState != lastState) {
    lastDebounceTime = currentTime;
  }

  if ((currentTime - lastDebounceTime) > debounceDelay) {
    if (currentState == LOW) {
      payload.mang[1] = 1;  // Nút nhấn
      Serial.print("nhan");
    } else {
      payload.mang[1] = 0;  // Nút nhả
      Serial.print("tha ");
    }
  }

  lastState = currentState;

  bool ok = radio.write(&payload, sizeof(payload));
  Serial.println(ok);
  counter = (counter + 1) % 65536;
  if (counter == 0) {
    parity = !parity;
  }
  delay(10);
}