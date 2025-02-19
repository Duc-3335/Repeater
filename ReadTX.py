import serial
import time

# Cấu hình cổng Serial
ser = serial.Serial('COM4', 9600, timeout=1)  # Thay 'COM4' bằng cổng Serial của bạn

# Đợi một chút để cổng Serial sẵn sàng
time.sleep(2)

# Mở file để ghi dữ liệu với mã hóa UTF-8
with open('E:/LAB/Repeater/ReadTX.txt', 'w', encoding='utf-8') as file:
    try:
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').rstrip()
                print(line)
                file.write(line + '\n')  # Ghi dòng vào file và thêm '\n'
    except KeyboardInterrupt:
        print("Đã dừng đọc dữ liệu từ Serial.")
    finally:
        ser.close()