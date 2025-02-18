#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Cấu hình RF24
RF24 radio(9, 10);  // CE, CSN
const uint64_t pipe1 = 0xF0F0F0F0A1;  // Địa chỉ pipe1 (từ TX trực tiếp)
const uint64_t pipe2 = 0xF0F0F0F0A2;  // Địa chỉ pipe2 (từ Repeater)

// Điều khiển LED và Servo
int ledPin = 6;      // LED trạng thái

// Biến kiểm tra trùng lặp gói tin
uint16_t lastCounter = 0;
bool lastParity = 0;

// Cấu trúc gói tin
struct Payload {
  uint8_t parity;        // Bit kiểm tra Parity
  uint16_t counter;   // Bộ đếm gói tin
  byte mang[2];       // Mảng có 2 phần tử: [0] - Servo, [1] - Nút nhấn
};
bool isValidPacket(Payload& payload);
void updateLastPacket(Payload& payload);

void setup() {
  Serial.begin(9600);
  
  // Khởi tạo RF24
  if (!radio.begin()) {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  }

  // Mở cả hai pipe để lắng nghe
  radio.openReadingPipe(2, pipe1);
  radio.openReadingPipe(3, pipe2);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();  // Bắt đầu lắng nghe

  // Khởi tạo LED và Servo
  pinMode(ledPin, OUTPUT);


  Serial.println("Receiver đang chờ dữ liệu từ cả pipe1 và pipe2...");
}

void loop() {
  delay(20);
  Payload payload;
  uint8_t pipeNo;
  if (radio.available(&pipeNo)) { // Lấy thông tin pipe

    radio.read(&payload, sizeof(payload));  // Đọc dữ liệu từ RF

    if (isValidPacket(payload)) {
      updateLastPacket(payload);
      // Xử lý gói tin theo pipe
      if (pipeNo == 3) {
        Serial.println("Dữ liệu từ Pipe 2 (Repeater)");
      } else if (pipeNo == 2) {
        Serial.println("Dữ liệu từ Pipe 1 (TX trực tiếp)");
      }
      // Điều khiển LED
      if (payload.mang[1] == 1) {
        digitalWrite(ledPin, HIGH);  // Bật LED
      } else {
        digitalWrite(ledPin, LOW);   // Tắt LED
      }
    } else {
      Serial.println("Gói tin bị trùng lặp, bỏ qua...");
    }
  }
}

// Hàm kiểm tra tính hợp lệ của gói tin
bool isValidPacket(Payload& payload) {
  return ((payload.counter > lastCounter && payload.parity == lastParity) ||
          (payload.counter < lastCounter && payload.parity != lastParity));
}

// Cập nhật giá trị Parity và Counter sau mỗi gói tin hợp lệ
void updateLastPacket(Payload& payload) {
  lastCounter = payload.counter;
  lastParity = payload.parity;
}
