import serial
import time

# Cấu hình cổng Serial
ser = serial.Serial('COM8', 9600, timeout=1)  # Thay 'COM3' bằng cổng Serial của bạn

# Đợi một chút để cổng Serial sẵn sàng
time.sleep(2)

# Mở file để ghi dữ liệu
with open('ReadRPT.txt', 'w', encoding='utf-8') as file:
    try:
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').rstrip()
                print(line)
                file.write(line)  # Ghi dòng vào file
    except KeyboardInterrupt:
        print("Đã dừng đọc dữ liệu từ Serial.")
    finally:
        ser.close()