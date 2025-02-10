import serial
import time


def send_binary_file(port, baudrate, binary_path):
    print("Hello, World")

    ser = serial.Serial(
        port     = "/dev/ttyACM0",
        baudrate = 115200,
        parity   = serial.PARITY_NONE,
        stopbits = serial.STOPBITS_ONE,
        bytesize = serial.EIGHTBITS
    )

    time.sleep(2)
    try:
        with open(binary_path, 'rb') as file:
            binary_data = file.read()
        
        ser.write(binary_data)
        print(f"{len(binary_data)} byte veri gönderildi.")

    except Expection as e:
        print(f"Error : {e}")

    finally:
        ser.close()

port = "/dev/ttyACM0"
baudrate = 115200
binary_path = "/home/bugraalp/personalFiles/embedded/STM32/STM32CubeIDE/workspace_1.14.0/L476RG_Project/L476RG_Application/build/L476RG_Application.bin"

send_binary_file(port, baudrate, binary_path)
