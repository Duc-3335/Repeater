#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// #include <Arduino_FreeRTOS.h>
// #include <task.h>

uint16_t counter = 0;
uint8_t parity = 0;

RF24 radio(9, 10);  // CE, CSN
const uint64_t pipe1 = 0xF0F0F0F0A1;//rpt
const uint64_t pipe2 = 0xF0F0F0F0A2;//rx
int nutnhan = 5;

struct Payload {
  uint8_t parity;
  uint16_t counter;
  byte data;  //Mảng có 2 phần tử
};
Payload payload;

void writeData();
// void transmit_1(void *parameter);
// void transmit_2(void *parameter);

void setup() {
  Serial.begin(9600);

  if (!radio.begin()) {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  }

  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();  //Cài đặt module là TX
  if (!radio.available()) {
    Serial.println("Chưa kết nối được với RX...!!");
    Serial.println("CHỜ KẾT NỐI.......");
  }

  pinMode(nutnhan, INPUT_PULLUP);

  // xTaskCreate(transmit_1, "Transmit 1", 10000, NULL, 1, NULL);
  // xTaskCreate(transmit_2, "Transmit 2", 10000, NULL, 1, NULL);
}

void loop() {
  radio.openWritingPipe(pipe1);
  writeData();
  delay(15);
  radio.openWritingPipe(pipe2);
  writeData();
  delay(15);
}

// void transmit_1(void *parameter) {
//   while (1) {
    
//     vTaskDelay(15 / portTICK_PERIOD_MS);
//   }
  
// }

// void transmit_2(void *parameter) {
//   while (radio.available()) {
//     writeData();
//     vTaskDelay(15 / portTICK_PERIOD_MS);
//   }
  
// }
void writeData() {
  payload.parity = parity;
  payload.counter = counter;
  payload.data = digitalRead(nutnhan);

  bool ok = radio.write(&payload, sizeof(payload));
  Serial.println(ok);
  counter = (counter + 1) % 65536;
  if (counter == 0) {
    parity = !parity;
  }
  Serial.print("Counter: ");
  Serial.print(payload.counter);
  Serial.print(", Parity: ");
  Serial.print(payload.parity);
  Serial.print(", Data: ");
  Serial.print(payload.data);
  Serial.print(", Send: ");
  Serial.println(ok ? "OK" : "FAILED");
}