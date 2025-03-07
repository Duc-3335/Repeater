#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
// Cấu hình RF24
RF24 radio(9, 10);  // CE, CSN
const uint64_t pipe1 = 0xF0F0F0F0A2;  // Địa chỉ pipe1 (từ TX trực tiếp)
const uint64_t pipe2 = 0xF0F0F0F0A3;  // Địa chỉ pipe2 (từ Repeater)
//servo
Servo sv1;
Servo sv2;
Servo sv3;
// Điều khiển LED và Servo
int ledPin = 6;      // LED trạng thái

// Biến kiểm tra trùng lặp gói tin
uint16_t lastCounter = 0;
uint8_t lastParity = 0;

// Cấu trúc gói tin
struct Payload {
  uint8_t parity;        // Bit kiểm tra Parity
  uint16_t counter;   // Bộ đếm gói tin
  byte data;       
};
Payload payload;

bool isValidPacket(Payload& payload);
void updateLastPacket(Payload& payload);
unsigned int sv1_d = 0;
unsigned int sv2_d = 0;
void setup() {
  Serial.begin(9600);
  sv1.attach(3);
  sv2.attach(5);
  // Khởi tạo RF24
  if (!radio.begin()) {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  }

  // Mở cả hai pipe để lắng nghe
  radio.openReadingPipe(1, pipe1);//1034834473121ULL
  radio.openReadingPipe(2, pipe2);//1034834473122ULL
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
  if(sv1_d < 180) {
    sv1.write(sv1_d++);
  }
  else {
    sv1_d = 0;
    sv1.write(sv1_d);
  }
  if(sv2_d < 180) {
    sv1.write(sv2_d++);
  }
  else {
    sv2_d = 0;
    sv1.write(sv2_d);
  }
  uint8_t pipeNo;
  if (radio.available(&pipeNo)) { // Lấy thông tin pipe

    radio.read(&payload, sizeof(payload));  // Đọc dữ liệu từ RF

    if (isValidPacket(payload)) {
      updateLastPacket(payload);
      // Xử lý gói tin theo pipe
      if (pipeNo == 2) {
        Serial.println("Dữ liệu từ Pipe 2 (Repeater)");
      } else if (pipeNo == 1) {
        Serial.println("Dữ liệu từ Pipe 1 (TX trực tiếp)");
      }
      // Điều khiển LED
      if (payload.data != 0) {
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
